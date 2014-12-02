#ifndef V8MONKEY_TESTAPI_H
#define V8MONKEY_TESTAPI_H


#include "v8.h"


namespace v8 {
  class APIEXPORT V8Test {
    public:
      static bool IsInternal();
      #ifdef V8MONKEY_INTERNAL_TEST
        static bool IsInternal2() {return false;};
      #endif
  };
}

#endif
