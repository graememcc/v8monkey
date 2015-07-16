/*
#ifndef V8MONKEY_AUTOLOCK_H
#define V8MONKEY_AUTOLOCK_H

// Mutex
#include "platform/platform.h"


namespace v8 {
  namespace V8Monkey {
*/

    /*
     * A stack-allocated RAII class which will acquire a mutex, and release it when falling out of scope
     *
     */


/*
    using namespace v8::V8Platform;


    class AutoLock {
      public:
        AutoLock(Mutex* m) : mutex(m) {
          mutex->Lock();
        }


        AutoLock(Mutex& m) : mutex(&m) {
          mutex->Lock();
        }

        ~AutoLock() {
          mutex->Unlock();
        }


      private:
        void* operator new(size_t);
        void* operator new[](size_t);
        void operator delete(void *);
        void operator delete[](void *);

        // XXX Delete copy constructor

        Mutex* mutex;
    };
  }
}


#endif
*/
