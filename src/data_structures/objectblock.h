#ifndef V8MONKEY_OBJECTBLOCK_H
#define V8MONKEY_OBJECTBLOCK_H


// ptrdiff_t
#include <cstddef>


/*
 * V8 allocates slabs of contiguous memory to handle the handles owned by the handlescopes for a given isolate. For
 * the motivation behind this, see the comment in src/runtime/handlescope.cpp.
 *
 * We essentially steal the similar implementation from V8, with some minor adaptations.
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
    // The caller should supply the first invalid address for the current block (or nullptr if this is the first block),
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
    static void Delete(T** currentLimit, T** currentTop, T** desiredTop, void (*deletionFunction)(T*) = nullptr);

    // Returns the number of objects in the list of blocks, excluding the objects in this block (as the call has no
    // knowledge of how many slots the caller has used in the current block).
    static unsigned long NumberOfItems(T** limit) {
      if (!limit) {
        return 0;
      }

      T** top = limit - RealBlockSize;
      return *(reinterpret_cast<unsigned long*>(top + 1)) * BlockSize;
    }

    // Iterates over every object contained in the list of blocks, and calls the given function with each one in
    // turn.
    static void Iterate(T** currentLimit, T** currentTop, void (*iterationFunction)(T*));

    // Iterates over every object contained in the list of blocks, and calls the given function with each one in
    // turn, passing the supplied data
    static void Iterate(T** currentLimit, T** currentTop, void (*iterationFunction)(T*, void*), void* data);

    // The number of slots in the block for users
    static const int BlockSize = 1022;

  private:
    static const int RealBlockSize = BlockSize + 2;
};


template<class T>
typename ObjectBlock<T>::Limits ObjectBlock<T>::Extend(T** currentLimit) {
  // If we're extending then we need new storage
  T** nextBlock = new T*[RealBlockSize];

  // We store metadata in the first two slots: in slot 0, we store a pointer to the previous block (the allocated
  // blocks form a linked list) and in slot 1, we store the number of blocks prior to this one.
  if (!currentLimit) {
    nextBlock[0] = nullptr;
    nextBlock[1] = reinterpret_cast<T*>(0);
  } else {
    T** top = currentLimit - RealBlockSize;
    nextBlock[0] = reinterpret_cast<T*>(top);
    nextBlock[1] = reinterpret_cast<T*>(*(reinterpret_cast<unsigned long*>(top + 1)) + 1);
  }

  return Limits{nextBlock + 2, nextBlock + RealBlockSize};
}


template<class T>
void ObjectBlock<T>::Delete(T** currentLimit, T** currentTop, T** desiredTop, void (*deletionFunction)(T*)) {
  // If there is nothing to do, then do nothing!
  if (!currentLimit || !currentTop) {
    return;
  }

  // Where does the current block start?
  T** top = currentLimit - RealBlockSize;
  T** limit = currentLimit;

  if (!desiredTop || !(top < desiredTop && desiredTop <= limit)) {
    // We are either deleting all allocated blocks, or the desired top lies outwith this block. We need to delete to
    // the beginning of the current block, and then delete zero or more full blocks until we have either deleted
    // everything, or we have found the block in which the desiredTop lies

    while (top && (!desiredTop || !(top < desiredTop && desiredTop <= limit))) {
      // The first time, through this loop, we will be deleting a partial block. Later iterations delete full blocks
      // only
      ptrdiff_t deletionIndex = (limit == currentLimit ? currentTop : limit) - top - 1;

      for (ptrdiff_t i = deletionIndex; i >= 2; i--) {
        if (deletionFunction) {
          deletionFunction(*(top + i));
        } else {
          delete *(top + i);
        }
      }

      T** nextTop = reinterpret_cast<T**>(*top);

      // Delete the block storage
      delete[] top;

      top = nextTop;
      limit = top + RealBlockSize;
    }
  }

  if (!top || desiredTop == limit) {
    return;
  }

  // The desiredTop lies within this block. We need to delete backwards to that location. Note that we might not have
  // executed the loop above if desiredTop lies in the bounds of the first block; in that case we start deleting from
  // the slot before currentTop, otherwise we start deleting from the end
  ptrdiff_t deletionIndex = (top < currentTop && currentTop <= limit ? currentTop : limit) - desiredTop - 1;
  for (ptrdiff_t i = deletionIndex; i >= 0; i--) {
    if (deletionFunction) {
      deletionFunction(*(desiredTop + i));
    } else {
      delete *(desiredTop + i);
    }
  }
}


template<class T>
void ObjectBlock<T>::Iterate(T** currentLimit, T** currentTop, void (*iterationFunction)(T*)) {
  if (!currentLimit || !currentTop) {
    return;
  }

  T** top = currentLimit - RealBlockSize;
  T** limit = currentLimit;

  while (top) {
    // The first time through, we may have to iterate over a partial block. After that, it's full blocks all the way
    ptrdiff_t index = (limit == currentLimit ? currentTop : limit) - top - 1;

    for (ptrdiff_t i = index; i >= 2; i--) {
      iterationFunction(*(top + i));
    }

    top = reinterpret_cast<T**>(*top);
    limit = top + RealBlockSize;
  }
}


template<class T>
void ObjectBlock<T>::Iterate(T** currentLimit, T** currentTop, void (*iterationFunction)(T*, void*), void* data) {
  if (!currentLimit || !currentTop) {
    return;
  }

  T** top = currentLimit - RealBlockSize;
  T** limit = currentLimit;

  while (top) {
    // The first time through, we may have to iterate over a partial block. After that, it's full blocks all the way
    ptrdiff_t index = (limit == currentLimit ? currentTop : limit) - top - 1;

    for (ptrdiff_t i = index; i >= 2; i--) {
      iterationFunction(*(top + i), data);
    }

    top = reinterpret_cast<T**>(*top);
    limit = top + RealBlockSize;
  }
}

}
}

#endif
