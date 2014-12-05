#ifndef V8MONKEY_COMMON_H
#define V8MONKEY_COMMON_H

#include "test.h"


namespace v8 {
  namespace V8Monkey {
    class EXPORT_FOR_TESTING_ONLY V8MonkeyCommon {
      public:
        // Uh-oh. Something's gone awry.
        static void TriggerFatalError();

      private:
        // This is simply a collection of helper methods
        V8MonkeyCommon();
    };
  }
}


#endif
