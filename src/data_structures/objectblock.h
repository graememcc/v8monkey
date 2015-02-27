#ifndef V8MONKEY_OBJECTBLOCK_H
#define V8MONKEY_OBJECTBLOCK_H

// find_if
#include <algorithm>

// iterator
#include <iterator>

// memory
#include <memory>

// vector
#include <vector>

// V8MONKEY_ASSERT
#include "utils/V8MonkeyCommon.h"


/*
 * V8 HandleScopes and HandleScopeImplementers cooperate to manage a linked list of slabs of continuous memory
 * containing internal object double pointers. We adopt a similar approach, with minor differences.
 *
 * XXX Check the comment about V8 Persistent handles once implemented.
 * In V8, those double pointers point to objects stored elsewhere, with the pointers managed by the moving
 * garbage-collector. Of course, we don't have a garbage-collector; our objects are reference-counted.
 * Our slabs are thus slabs of std::shared_ptrs to the underlying objects,which relieves the objects of some of the
 * reference-counting administrivia (though not completely, due to the requirements of V8 Persistent handles).
 *
 */


namespace v8 {
  namespace DataStructures {

    template <class T, class LimitType = std::shared_ptr<T>, unsigned int SlabSize = 1024u>
    class ObjectBlock {
      private:
        using SlotContents = std::shared_ptr<T>;
        using Slot = SlotContents*;
        using Slab = std::vector<SlotContents>;
        using SlabPtr = std::unique_ptr<Slab>;
        using Slabs = std::vector<SlabPtr>;

      public:
        using ValueType = SlotContents;
        using AddressType = ValueType*;

        static const unsigned int slabSize {SlabSize};

        ObjectBlock() : slabs() {}

        ~ObjectBlock() {}

        ObjectBlock(ObjectBlock<T, LimitType*, SlabSize>&& other) : slabs(std::move(other.slabs)) {}

        ObjectBlock<T, LimitType*, SlabSize>& operator=(ObjectBlock<T>&& other) {
          // XXX Review this assert
          V8MONKEY_ASSERT(other != *this, "Do you really mean to assign to self?");

          if (other != *this) {
            slabs = std::move(other.slabs);
          }
        }

        /*
         * Return the number of occupied slots in this ObjectBlock
         *
         */

        unsigned long NumberOfItems() const {
          if (slabs.empty()) {
            return 0;
          }

          return slabSize * (slabs.size() - 1) + slabs.back()->size();
        }

        // XXX Need to make a note about dereferencing these pointers once we've worked through it.
        /*
         * Informational structure returned by a call to Add. The pointers returned point to the first free slot in
         * the current slab, and the limit of the current slab (i.e. one element past the last valid slot) respectively.
         *
         * The limit member is intended only for V8 compatability. Delete accepts next values returned by Add.
         *
         */

        struct Limits {
          LimitType* objectAddress;
          LimitType* next;
          LimitType* limit;
        };


        /*
         * Adds a new element to the ObjectBlock, allocating a new block of slab storage if necessary. Returns a Limits
         * structure for V8 compatability, pointing to the next free slot and the limit of the current block. Our
         * clients should only require this information for deletion.
         *
         */

        V8_INLINE Limits Add(T* data);

        /*
         * Takes a pointer to the slot that should be the first empty slot after this deletion operation completes.
         * Deletes each pointer from there to the end of the ObjectBlock
         *
         * The pointer supplied should either be nullptr, in which case the entire contents are deleted, the last next
         * pointer returned by an Add call (in which case this call is a no-op), or a pointer to a currently-filled
         * slot in a currently-allocated slab. The address doesn't necessarily have to be one that was explicitly
         * returned by a previous Add call, but computing other addresses is likely to be error-prone.
         *
         */

        V8_INLINE void Delete(LimitType* desiredEnd);

        // Note: we expect iteration to be performed in a tight loop, so provide a custom iteration function rather
        // than defining custom iterators and begin/end methods. This avoids potential problems with invalidating
        // iterators when values are added/removed.

        /*
         * Calls the given function with a shared pointer to the given raw type for each entry in the ObjectBlock.
         *
         */

        V8_INLINE void Iterate(void (*iterationFunction)(ValueType)) const;

        /*
         * Calls the given function with a shared pointer to the given raw type and the supplied data for each entry in
         * the ObjectBlock.
         *
         */

        V8_INLINE void Iterate(void (*iterationFunction)(ValueType, void*), void* data) const;

        ObjectBlock(const ObjectBlock<T>& other) = delete;
        ObjectBlock<T>& operator=(const ObjectBlock<T>& other) = delete;

      private:
        Slabs slabs {};
    };


    template <class T, class LimitType, unsigned int SlabSize>
    typename ObjectBlock<T, LimitType, SlabSize>::Limits ObjectBlock<T, LimitType, SlabSize>::Add(T* data) {
      if (slabs.empty() || slabs.back()->size() == slabSize) {
        // Allocate a new block
        // XXX Can new return nullptr?
        Slab* slab {new Slab};
        slab->reserve(slabSize);
        slabs.emplace_back(slab);
      }

      V8MONKEY_ASSERT(!slabs.empty(), "Slabs cannot be empty here!");
      Slab* slab {slabs.back().get()};
      V8MONKEY_ASSERT(slab->size() != slabSize, "Slab cannot be full here!");
      slab->emplace_back(data);
      Slot slabStart {slab->data()};

      return Limits {reinterpret_cast<LimitType*>(slabStart + slab->size() - 1),
                     reinterpret_cast<LimitType*>(slabStart + slab->size()),
                     reinterpret_cast<LimitType*>(slabStart + slabSize)};
    }


    template <class T, class LimitType, unsigned int SlabSize>
    void ObjectBlock<T, LimitType, SlabSize>::Delete(LimitType* desiredPtr) {
      // Fast path
      if (!desiredPtr) {
        slabs.clear();
        return;
      }

      Slot desiredEnd {reinterpret_cast<Slot>(desiredPtr)};

      V8MONKEY_ASSERT(!slabs.empty(), "Attempting to delete from an empty ObjectBlock");

      // Note we search backwards
      auto iterEnd = slabs.rend();
      auto slabIter = std::find_if(slabs.rbegin(), iterEnd, [&desiredEnd](const SlabPtr& slab) {
        Slot slabStart {slab.get()->data()};
        return desiredEnd >= slabStart && desiredEnd <= slabStart + slabSize;
      });

      V8MONKEY_ASSERT(slabIter != iterEnd, "Desired slot doesn't exist");
      slabs.erase(slabIter.base(), slabs.end());

      // All that remains is to delete within the slab. The slab iterator shouldn't be invalidated by erase, as it
      // lies before the deletion range
      Slab* slots {(*slabIter).get()};
      V8MONKEY_ASSERT(static_cast<typename Slab::size_type>(desiredEnd - slots->data()) <= slots->size(),
                      "desiredEnd should be in the current bounds of the slab");

      // erase would be semantically clearer, but then we have the (minor) hassle of converting the reverse iterator
      // to a standard iterator. The assert above confirms this call won't increase the size or capacity.
      slots->resize(static_cast<typename Slab::size_type>(desiredEnd - slots->data()));
    }


    template <class T, class LimitType, unsigned int SlabSize>
    void ObjectBlock<T, LimitType, SlabSize>::Iterate(void (*iterationFunction)(ValueType)) const {
      for (auto const& slab : slabs) {
        for (SlotContents value : *(slab.get())) {
          iterationFunction(value);
        }
      }
    }


    template <class T, class LimitType, unsigned int SlabSize>
    void ObjectBlock<T, LimitType, SlabSize>::Iterate(void (*iterationFunction)(ValueType, void*), void* data) const {
      for (auto const& slab : slabs) {
        for (SlotContents value : *(slab.get())) {
          iterationFunction(value, data);
        }
      }
    }


  }
}

#endif
