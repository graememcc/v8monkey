#ifndef V8MONKEY_HANDLESCOPE_H
#define V8MONKEY_HANDLESCOPE_H

#include "types/base_types.h"
#include "test.h"

namespace v8 {
  namespace V8Monkey {
    class V8MonkeyObject;

    class HandleScopeData {
     public:
      V8MonkeyObject** next;

      V8MonkeyObject** limit;

      // XXX Needed?
      int level;

      inline void Initialize() {
        next = limit = NULL;
        level = 0;
      }
    };
  }
}
#endif
