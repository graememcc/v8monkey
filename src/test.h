#ifndef V8MONKEY_TEST_H
#define V8MONKEY_TEST_H

// Export extra symbols when building the test library
#ifdef V8MONKEY_INTERNAL_TEST
  #define EXPORT_FOR_TESTING_ONLY  __attribute__ ((visibility("default")))
#else
  #define EXPORT_FOR_TESTING_ONLY
#endif


#endif
