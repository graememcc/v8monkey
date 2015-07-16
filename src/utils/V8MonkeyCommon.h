/*
#ifndef V8MONKEY_COMMON_H
#define V8MONKEY_COMMON_H

// EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"

namespace v8 {
  namespace V8Monkey {
//    class EXPORT_FOR_TESTING_ONLY V8MonkeyCommon {
//      public:
//        // Intended to be called only by the static initializer to ensure that the primitive singletons (i.e. true,
//        // false etc) exist.
//        static void InitPrimitiveSingletons();
//
//        // Intended to be called only by the static initializer's destructor to clean up singleton primitives
//        static void TearDownPrimitiveSingletons();
//
//        // Used by static destructor in init to enforce an ordering constraint: the main thread's JSRuntime and JSContext
//        // must be disposed of before SpiderMonkey itself is disposed
//        static void ForceMainThreadRTCXDisposal();
//
        // Uh-oh. Something's gone awry.
        EXPORT_FOR_TESTING_ONLY void TriggerFatalError(const char* location, const char* message);

        // Trigger an error that should not be catchable by a user-supplied handler
        // Note: we need to EXPORT_FOR_TESTING_ONLY so that files that tests that only require header files don't fail to link
        // if the header under test contains asserts
        EXPORT_FOR_TESTING_ONLY void Abort(const char* location, const char* message, bool isAssert = false);

//        // Returns true if V8 is dead, but also triggers a fatal error handler call, reporting it
//        static bool CheckDeath(const char* method);
//
//      private:
//        // This is simply a collection of helper methods
//        V8MonkeyCommon();
//    };
  }
}


#ifdef DEBUG
  #define xstr(s) str(s)
  #define str(s) #s
  #define V8MONKEY_ASSERT(condition, message) do {\
      if (!(condition)) {\
        ::v8::V8Monkey::Abort(__FILE__ ":" xstr(__LINE__), "Assertion failure: " message, true); \
      }\
    } while (0)
#else
  #define V8MONKEY_ASSERT(condition, message) ((void) 0)
#endif


#endif
*/
