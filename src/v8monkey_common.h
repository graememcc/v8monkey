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

        // Intended to be called only by the static initializer to ensure that the primitive singletons (i.e. true,
        // false etc) exist.
        static void InitPrimitiveSingletons();

        // Intended to be called only by the static initializer's destructor to clean up singleton primitives
        static void TearDownPrimitiveSingletons();

        // Used by static destructor in init to enforce an ordering constraint: the main thread's JSRuntime and JSContext
        // must be disposed of before SpiderMonkey itself is disposed
        static void ForceMainThreadRTCXDisposal();

        // Uh-oh. Something's gone awry.
        static void TriggerFatalError(const char* location, const char* message);

        // Returns true if V8 is dead, but also triggers a fatal error handler call, reporting it
        static bool CheckDeath(const char* method);

      private:
        // This is simply a collection of helper methods
        V8MonkeyCommon();
    };
  }
}


#endif
