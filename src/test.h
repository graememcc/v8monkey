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
namespace v8 {
  namespace V8Monkey {
    class EXPORT_FOR_TESTING_ONLY TestUtils {
      public:
        // Fool V8 into thinking it's had a meltdown
        static void TriggerFatalError();
      private:
        // This is just a utility class, so should not be constructible
        TestUtils();
    };
  }
}
#endif


#endif
