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
        // Intended to be called only by the static initializer to ensure that all required TLS keys exist
        static void InitTLSKeys();

        // Intended to be called only by the static initializer to ensure that the default isolate exists, and that
        // the thread running static initializers has that isolate pointer in TLS. Must be called after InitTLSKeys.
        static void EnsureDefaultIsolate();

        // Ensure TLS destructor for default isolate doesn't run after main thread exit
        static void ClearOutMainThreadIsolateTLS();

        // Used by static destructor in init to ensure main thread's JSRuntime and JSContext are disposed of before
        // SpiderMonkey
        static void ForceRTCXDisposal();

        // Uh-oh. Something's gone awry.
        static void TriggerFatalError(const char* location, const char* message);

        // Ensure SpiderMonkey is initted independent of the API's calls to V8::Initialize
        // XXX Is this still needed?
        static void EnsureSpiderMonkey();

        // Returns true if V8 is dead, but also triggers a fatal error handler call, reporting it
        static bool CheckDeath(const char* method);

      private:
        // This is simply a collection of helper methods
        V8MonkeyCommon();
    };
  }
}


#endif
