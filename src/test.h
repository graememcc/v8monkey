#include <v8.h>


#ifndef V8MONKEY_TEST_H
#define V8MONKEY_TEST_H

// Export extra symbols when building the test library
#ifdef V8MONKEY_INTERNAL_TEST
  #define EXPORT_FOR_TESTING_ONLY  __attribute__ ((visibility("default")))
#else
  #define EXPORT_FOR_TESTING_ONLY
#endif


// Define a utility class with a grabbag of methods handy for internal testing
#ifdef V8MONKEY_INTERNAL_TEST
#include "runtime/isolate.h"

namespace v8 {
  namespace V8Monkey {
    class EXPORT_FOR_TESTING_ONLY TestUtils {
      public:
        // Returns true if an attempt (not necessarily succesful) has been made to initialize the engine
        static bool IsV8Initialized();
        static void SetHandlerAndKill(FatalErrorCallback fn);

        // The following RAII classes clean up our trail of destruction.
        // AutoIsolateCleanup exits and disposes of any isolates entered by this thread
        // AutoTestCleanup in addition will dispose of V8
        //
        // The AutoTestCleanup class must only be used on the main thread

        class AutoIsolateCleanup {
          public:
            AutoIsolateCleanup() {}

            ~AutoIsolateCleanup();
        };

        class AutoTestCleanup {
          public:
            AutoTestCleanup();

            ~AutoTestCleanup();
        };

      private:
        // This is just a utility class, so should not be constructible
        TestUtils();
    };
  }
}


// Macro to exit all extant isolates for this thread and dispose V8. This depends on isolate.h, but I leave it up to
// each file to include it (to ensure I update the dependencies in the Makefile)
#define EXIT_ALL_ISOLATES_AND_DISPOSE \
  do {\
    while (v8::V8Monkey::InternalIsolate::IsEntered(v8::V8Monkey::InternalIsolate::GetCurrent())) {\
      v8::Isolate::GetCurrent()->Exit(); \
    } \
  } while (0);


// Define tests that check for implicit V8 initialization
#define IMPLICIT_INIT_TESTS(stem, val1, val2, body) \
  namespace {  \
    int stem##FatalErrorCheck = 0; \
    void stem##FatalErrorHandler(const char* location, const char* message) { \
      stem##FatalErrorCheck = 1; \
    } \
  } \
  V8MONKEY_TEST(stem##val1, "Bails when V8 is dead") {\
    v8::V8Monkey::TestUtils::AutoTestCleanup ac; \
    v8::V8Monkey::TestUtils::SetHandlerAndKill(stem##FatalErrorHandler); \
    stem##FatalErrorCheck = 0;\
    body\
    V8MONKEY_CHECK(stem##FatalErrorCheck != 0, "Errored out due to dead V8"); \
  } \
\
  V8MONKEY_TEST(stem##val2, "Inits V8") {\
    v8::V8Monkey::TestUtils::AutoTestCleanup ac; \
    body\
    V8MONKEY_CHECK(v8::V8Monkey::TestUtils::IsV8Initialized(), "V8 was implicitly initialized");\
  }
#endif


#endif
