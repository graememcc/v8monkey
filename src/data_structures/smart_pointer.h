#ifndef V8MONKEY_SMARTPOINTER_H
#define V8MONKEY_SMARTPOINTER_H

#include "test.h"


namespace v8 {
namespace V8Monkey {

template <class T>
class EXPORT_FOR_TESTING_ONLY SmartPtr {
  public:
    SmartPtr() : ptr(NULL) {}

    SmartPtr(T* p) : ptr(p) {
      if (ptr) {
        ptr->AddRef();
      }
    }

    SmartPtr(const SmartPtr<T> &rhs) : ptr(rhs.ptr) {
      if (ptr) {
        ptr->AddRef();
      }
    }

    ~SmartPtr() {
      if (ptr) {
        ptr->Release();
      }
    }

    inline bool operator== (const SmartPtr<T> &rhs) {
      return ptr == rhs.ptr;
    }

    inline bool operator!= (const SmartPtr<T> &rhs) {
      return ptr != rhs.ptr;
    }

    inline bool operator== (const T* &rhs) {
      return ptr == rhs;
    }

    inline bool operator!= (const T* &rhs) {
      return ptr != rhs;
    }

    SmartPtr<T>& operator= (const SmartPtr<T> &rhs) {
      if (this == &rhs || ptr == rhs.ptr) {
        return *this;
      }

      if (ptr) {
        ptr->Release();
      }

      ptr = rhs.ptr;

      if (ptr) {
        ptr->AddRef();
      }

      return *this;
    }

    SmartPtr<T>& operator= (T* rhs) {
      if (rhs == ptr) {
        return *this;
      }

      if (ptr) {
        ptr->Release();
      }

      ptr = rhs;

      if (ptr) {
        ptr->AddRef();
      }

      return *this;
    }

    T& operator* () {
      return *ptr;
    }

    T* operator-> () {
      return ptr;
    }

  private:
    T* ptr;
};


}
}

#endif
