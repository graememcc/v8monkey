#ifndef V8MONKEY_OBJECTBLOCK_H
#define V8MONKEY_OBJECTBLOCK_H

#include "test.h"


/*
 * V8 allocates slabs of contiguous memory to handle the handles owned by the handlescopes for a given isolate. The
 * handlescope manages the pointers into this memory, and twiddles them to unstack handlescopes as they fall out of
 * scope. We blatanly steal this idea for our own handlescopes, but with some required changes.
 *
 * The basic problem is that life is uncomplicated at that level of abstraction in V8: it's OK to throw away those
 * pointers, as they're really double-pointers to memory owned by the garbage-collector, who will free the resource
 * when it discovers it unrooted.
 *
 * We don't have a garbage-collector in V8Monkey, but we will be wrapping SpiderMonkey objects that will be wrapped
 * in our own objects to present a V8-like interface. While we can rely on the SpiderMonkey GCThings being cleaned up
 * after we unroot them, we still need to manage the the lifetime of the wrapped objects. This suggests that when
 * moving the pointers in the HandleScope memory, we must delete them also.
 *
 * What complicates matters is that Handles can be assigned from other Handles; it is quite reasonable to have two
 * Handles pointing to the same internal object in this memory, possibly even in different scopes. Obviously this
 * could all too easily result in dangling pointers, so it looks as if we too need to indirectly manage our internal
 * objects through a double pointer-in fact a reference-counting smart pointer.
 *
 */


namespace v8 {
namespace V8Monkey {

template<class T>
class ObjectBlock {
  public:
    struct Limits {
      T** top;
      T** limit;
    };

    // Allocate a new block, and return a struct pointing to the first slot, and the address after the last valid slot.
    // The caller should supply the first invalid address for the current block (or NULL if this is the first block),
    // as the blocks will chain in a linked list for freeing memory. The caller must ensure that the given limit is
    // valid.
    static Limits Extend(T** currentLimit);


    // Delete from the given slot to the desired slot. currentLimit should be the address of the memory after the last
    // valid slot (this address is returned in the limit field of the struct returned from Add). The currentTop
    // parameter should point to the first free slot in the current block (this may equal the first invalid slot if the
    // block is full). desiredTop should be the address of what will become the first free slot in the remaining
    // blocks. The caller is responsible for ensuring that this value lies in the memory of the blocks already
    // allocated (although the address immediately after the last slot of any block is also acceptable).
    // Optionally takes a function pointer. If supplied this function will be called to teardown deleted elements
    // instead of deleting them.
    static void Delete(T** currentLimit, T** currentTop, T** desiredTop, void (*deletionFunction)(T*) = NULL);

    // Returns the number of objects in the list of blocks, excluding the objects in this block (as the call has no
    // knowledge of how many slots the caller has used in the current block).
    static unsigned long NumberOfItems(T** limit) {
      if (!limit) {
        return 0;
      }

      T** top = limit - BlockSize;
      return *(reinterpret_cast<unsigned long*>(top + 1)) * (BlockSize - 2);
    }

    // Iterates over every object contained in the list of blocks, and calls the given function with each one in
    // turn.
    static void Iterate(T** currentLimit, T** currentTop, void (*iterationFunction)(T*));

  private:
    static const int BlockSize = 1022;

  #ifdef V8MONKEY_INTERNAL_TEST
    public:
      static const int EffectiveBlockSize = BlockSize - 2;
  #endif
};


template<class T>
typename ObjectBlock<T>::Limits ObjectBlock<T>::Extend(T** currentLimit) {
  // If we're extending then we need new storage
  T** nextBlock = new T*[BlockSize];
  // Fill in the metadata in the first two slots
  if (!currentLimit) {
    nextBlock[0] = nullptr;
    nextBlock[1] = reinterpret_cast<T*>(0);
  } else {
    T** top = currentLimit - BlockSize;
    nextBlock[0] = reinterpret_cast<T*>(top);
    nextBlock[1] = reinterpret_cast<T*>(*(reinterpret_cast<unsigned long*>(top + 1)) + 1);
  }

  return Limits{nextBlock + 2, nextBlock + BlockSize};
}

template<class T>
void ObjectBlock<T>::Delete(T** currentLimit, T** currentTop, T** desiredTop, void (*deletionFunction)(T*)) {
  // If there's nothing to do, then do nothing
  if (!currentLimit || !currentTop) {
    return;
  }

  T** top = currentLimit - BlockSize;
  T** limit = currentLimit;

  if (!desiredTop || !(top < desiredTop && desiredTop < limit)) {
    // The desired top isn't in the bounds of the current block. We should first delete the contents of this block.
    // After that we may have 0 or more full blocks which also don't contain our desired position. They too will be
    // for the chop.

    // Watch out for the client returning something bogus, and ensure we don't deref null
    while (top && (!desiredTop || !(top < desiredTop && desiredTop < limit))) {
      // We need to delete back to the beginning of this block. Most of the time that will involve deleting
      // in the block, but on our first time through, we may only be deleting a partial block.
      int deletionIndex = top < currentTop && currentTop < limit ? currentTop - top : BlockSize;
      for (int i = deletionIndex - 1; i >= 2; i--) {
        if (deletionFunction == NULL) {
          delete *(top + i);
        } else {
          deletionFunction(*(top + i));
        }
      }

      // Compute the next top
      T** nextTop = reinterpret_cast<T**>(*top);
      delete[] top;

      top = nextTop;
      limit = top + BlockSize;
    }
  }

  if (!top) {
    return;
  }

  // Now, we are in the block containing the desired slot. There are two scenarios:
  //   - currentTop lies outside this block: we need to delete back from the limit to the desiredTop
  //   - currentTop lies in this block: we need to delete from currentTop to desiredTop
  int deletionCount = (top < currentTop && currentTop < limit ? currentTop : limit) - desiredTop - 1;
  for (int i = deletionCount; i >= 0; i--) {
    if (!deletionFunction) {
      delete *(desiredTop + i);
    } else {
      deletionFunction(*(desiredTop + i));
    }
  }
}


template<class T>
void ObjectBlock<T>::Iterate(T** currentLimit, T** currentTop, void (*iterationFunction)(T*)) {
  if (!currentLimit || !currentTop)
    return;


  T** from = currentTop - 1;
  T** top = currentLimit - BlockSize;

  do {
    for (T** ptr = from; ptr >= top + 2; ptr--) {
      iterationFunction(*ptr);
    }

    top = reinterpret_cast<T**>(*top);
    from = top + BlockSize - 1;
  } while (top);
}
}
}


#endif
