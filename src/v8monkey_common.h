#ifndef V8MONKEY_COMMON_H
#define V8MONKEY_COMMON_H

#include "platform.h"


namespace v8 {
  namespace V8Monkey {
    using namespace v8::V8Platform;


    typedef struct V8Monkey_TLSKeys {
      TLSKey* threadIDKey;
    } V8Monkey_TLSKeys;


    class V8MonkeyCommon {
      public:
        static V8Monkey_TLSKeys TLSKeys;
    };
  }
}


#endif
