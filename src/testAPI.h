#ifndef V8MONKEY_TESTAPI_H
#define V8MONKEY_TESTAPI_H


#include "v8.h"


namespace v8 {
  class APIEXPORT V8Test {
    public:
      static bool IsInternal();
  };
}

#endif
