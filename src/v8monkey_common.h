#ifndef V8MONKEY_COMMON_H
#define V8MONKEY_COMMON_H

#include "test.h"


#ifdef DEBUG
  #define ASSERT(condition, location, message) do {\
      if (!(condition)) {\
        v8::V8Monkey::V8MonkeyCommon::TriggerFatalError(location, message);\
      }\
    } while (0)
#else
  #define ASSERT(condition, location, message) ((void) 0)
#endif


namespace v8 {
  namespace V8Monkey {
    class EXPORT_FOR_TESTING_ONLY V8MonkeyCommon {
      public:
        // Used by static destructor in init to ensure main thread's JSRuntime and JSContext are disposed of before
        // SpiderMonkey
        static void ForceRTCXDisposal();

        // Uh-oh. Something's gone awry.
        static void TriggerFatalError(const char* location, const char* message);

        // Ensure SpiderMonkey is initted independent of the API's calls to V8::Initialize
        static void EnsureSpiderMonkey();

      private:
        // This is simply a collection of helper methods
        V8MonkeyCommon();
    };
  }
}


#endif
