#include <pthread.h>

#include "V8MonkeyTest.h"
#include "platform.h"


namespace {
  // Support function for thread running test
  void* thread_run_main(void* arg) {
    return NULL;
  }


  // Support function for thread result test
  #define THREAD_RESULT 100
  void* thread_join_main(void* arg) {
    return reinterpret_cast<void*>(THREAD_RESULT);
  }



  // Support function for thread result test
  #define THREAD_ARG 7
  void* thread_arg_main(void* arg) {
    return arg;
  }


  // Support function for thread noparam test
  void* thread_noarg_main(void* arg) {
    return arg;
  }


  // Helper variable for the one shot test: ensure this is zeroed before starting the test
  int oneShotVal = 0;


  // One shot function
  void oneShot() {
    oneShotVal++;
  }
}


using namespace v8::V8Platform;


// XXX This should probably be checked at configuration time, rather than getting this far
// XXX And if not, we at least want to factor out the platform specific bit
V8MONKEY_TEST(Plat001, "Platform-specific TLS key fits in a pointer") {
  V8MONKEY_CHECK(Platform::GetTLSKeySize() <= sizeof(TLSKey*), "Opaque type fits in a pointer");
}


V8MONKEY_TEST(Plat002, "TLS Key get initially returns null") {
  TLSKey* key = Platform::CreateTLSKey();
  V8MONKEY_CHECK(Platform::GetTLSData(key) == NULL, "Key value is initially null");
  Platform::DeleteTLSKey(key);
}


V8MONKEY_TEST(Plat003, "TLS Key get returns correct value") {
  TLSKey* key = Platform::CreateTLSKey();
  intptr_t value = 42;
  Platform::StoreTLSData(key, reinterpret_cast<void*>(value));
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(Platform::GetTLSData(key)) == value, "Retrieved key is correct");
  Platform::DeleteTLSKey(key);
}


V8MONKEY_TEST(Plat004, "HasRan correct after thread runs") {
  Thread t(thread_run_main);
  t.Run(NULL);
  t.Join();
  V8MONKEY_CHECK(t.HasRan(), "HasRan reported true");
}


V8MONKEY_TEST(Plat005, "Thread joining returns correct value") {
  Thread t(thread_join_main);
  t.Run(NULL);
  void* result = t.Join();
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(result) == THREAD_RESULT, "Value returned by thread join is correct");
}


V8MONKEY_TEST(Plat006, "Thread argument passing works correctly") {
  Thread t(thread_arg_main);
  t.Run(reinterpret_cast<void*>(THREAD_ARG));
  void* result = t.Join();
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(result) == THREAD_ARG, "Value returned shows arg was passed correctly");
}


V8MONKEY_TEST(Plat007, "Run called with no arguments calls thread function with NULL") {
  Thread t(thread_noarg_main);
  t.Run();
  void* result = t.Join();
  V8MONKEY_CHECK(result == NULL, "Run was called with null pointer");
}


V8MONKEY_TEST(Plat008, "OneShot called only once") {
  oneShotVal = 0;
  OneShot o(oneShot);
  o.Run();
  o.Run();
  V8MONKEY_CHECK(oneShotVal == 1, "One shot function only ran once");
}
