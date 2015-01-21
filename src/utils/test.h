#ifndef V8MONKEY_TEST_H
#define V8MONKEY_TEST_H

// FatalErrorCallbackFn
#include "v8.h"


// Export extra symbols when building the test library
#ifdef V8MONKEY_INTERNAL_TEST
  #define EXPORT_FOR_TESTING_ONLY  __attribute__ ((visibility("default")))
#else
  #define EXPORT_FOR_TESTING_ONLY
#endif


/*
 * In internals testing builds, define a utility class with a grabbag of methods handy for poking around inside our
 * internals, and additional utility functions.
 *
 */

#ifdef V8MONKEY_INTERNAL_TEST

namespace v8 {
  namespace V8Monkey {
    class EXPORT_FOR_TESTING_ONLY TestUtils {
      public:

        // XXX Is this still needed?
        /*
         *
         * Returns true if an attempt (not necessarily succesful) has been made to initialize the engine
         *
         */
        static bool IsV8Initialized();

        /*
         * Many API functions implicitly init V8 but first confirm it's not dead. They test this by setting up a fatal
         * error handler and invoking the implicitly initting function. There are a couple of key problems: calling
         * TriggerFatalError, though killing V8, invokes the fatal error handler itself, defeating the point of the
         * test, and calling SetFatalErrorHandler implicitly inits V8, which is again something the test needs to
         * control.
         *
         * This function circumvents API conventions, and installs an error handler without init, and kills V8 without
         * triggering the handler.
         *
         */
        static void SetHandlerAndKill(FatalErrorCallback fn);

        /*
         * The following RAII classes clean up our trail of destruction:
         *
         * AutoIsolateCleanup exits and disposes of any isolates entered by this thread
         * AutoTestCleanup will, in addition to the above, dispose of V8.
         *
         * AutoTestCleanup must only be used on the main thread.
         *
         */

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


/*
 * Macro to exit all extant isolates for this thread and dispose V8. This depends on isolate.h, but I leave it up to
 * each file to include it (to avoid circular dependencies)
 *
 */

#define EXIT_ALL_ISOLATES_AND_DISPOSE \
  do {\
    while (v8::V8Monkey::InternalIsolate::IsEntered(v8::V8Monkey::InternalIsolate::GetCurrent())) {\
      v8::Isolate::GetCurrent()->Exit(); \
    } \
  } while (0);


/*
 * Define tests that check for implicit V8 initialization, and default isolate entry (i.e. check that an API call
 * exhibits the behaviour defined by the V8 internal function EnsureInitializedForIsolate)
 *
 */

#define ISOLATE_INIT_TESTS(stem, val1, val2, val3, body) \
  V8MONKEY_TEST(stem##val1, "Inits V8") {\
    v8::V8Monkey::TestUtils::AutoTestCleanup ac; \
    body\
    V8MONKEY_CHECK(v8::V8Monkey::TestUtils::IsV8Initialized(), "V8 was implicitly initialized");\
  }\
  \
  \
  V8MONKEY_TEST(stem##val2, "Implicitly enters default isolate if necessary") {\
    v8::V8Monkey::TestUtils::AutoTestCleanup ac;\
  \
    body\
    V8MONKEY_CHECK(v8::V8Monkey::InternalIsolate::IsEntered(v8::V8Monkey::InternalIsolate::FromIsolate(v8::Isolate::GetCurrent())), "Correctly entered default isolate");\
  }\
  \
  \
  V8MONKEY_TEST(stem##val3, "Doesn't change isolate if already entered") {\
    v8::V8Monkey::TestUtils::AutoTestCleanup ac;\
    \
    v8::Isolate* i = v8::Isolate::New();\
    i->Enter();\
    \
    body\
    V8MONKEY_CHECK(v8::V8Monkey::InternalIsolate::IsEntered(v8::V8Monkey::InternalIsolate::FromIsolate(i)), "Correctly stayed in isolate");\
  }
#endif


#endif
