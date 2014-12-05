#ifndef V8MONKEY_ISOLATE_H
#define V8MONKEY_ISOLATE_H 

#include "test.h"

namespace v8 {
  namespace V8Monkey {
    class EXPORT_FOR_TESTING_ONLY InternalIsolate {
      public:
        static bool IsDefaultIsolate(InternalIsolate* i);
        static InternalIsolate* GetDefaultIsolate();

        #ifdef V8MONKEY_INTERNAL_TEST
          // Has the current thread entered the given isolate?
          static bool IsEntered(InternalIsolate* i);
        #endif

      private:
        static InternalIsolate* defaultIsolate;
    };
  }
}


#endif
