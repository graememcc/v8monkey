// intptr_t
#include <cinttypes>

// memcpy
#include <cstring>

// Class under test
#include "platform/platform.h"

// Unit-testing support
#include "V8MonkeyTest.h"


// XXX I think we need to fix some of these casts


namespace {
  bool destructorCalled {false};

  intptr_t testData {42};

  void tlsDestructor(void* data) {
    intptr_t realData;
    std::memcpy(reinterpret_cast<char*>(&realData), reinterpret_cast<char*>(&data), sizeof(intptr_t));
    destructorCalled = realData == testData;
  }


  // Tests to see if a destructor is called
  void* thread_tls_destruct(void* arg) {
    v8::V8Platform::TLSKey<intptr_t>* key {};
    std::memcpy(reinterpret_cast<char*>(&key), reinterpret_cast<char*>(&arg),
                sizeof(v8::V8Platform::TLSKey<intptr_t>*));
    key->Set(testData);
    return nullptr;
  }


  // Support function for thread running test
  void* thread_run_main(void*) {
    return nullptr;
  }


  // Support function for thread result test
  int threadResult {1000};
  void* thread_join_main(void*) {
    return reinterpret_cast<void*>(threadResult);
  }



  // Support function for thread result test
  int threadArg {7};
  void* thread_arg_main(void* arg) {
    return arg;
  }


  // Support function for thread noparam test
  void* thread_noarg_main(void* arg) {
    return arg;
  }


  // Helper variable for the one shot test: ensure this is zeroed before starting the test
  int oneShotVal {0};


  // One shot function
  void oneShot() {
    oneShotVal++;
  }
}


using namespace v8::V8Platform;


V8MONKEY_TEST(Plat001, "TLS Key get initially returns null") {
  TLSKey<void*> key1 {};
  V8MONKEY_CHECK(key1.Get() == nullptr, "Key value is initially null");
  TLSKey<int> key2 {};
  V8MONKEY_CHECK(key2.Get() == 0, "Key value is initially null");
}


V8MONKEY_TEST(Plat002, "TLS Key get returns correct value (1)") {
  TLSKey<intptr_t> key {};
  intptr_t value {42};
  key.Set(value);
  V8MONKEY_CHECK(key.Get() == value, "Retrieved key is correct");
}


V8MONKEY_TEST(Plat003, "TLS Key get returns correct value (2)") {
  // Test with something smaller than a pointer
  TLSKey<char> key {};
  char value {'\7'};
  key.Set(value);
  V8MONKEY_CHECK(key.Get() == value, "Retrieved key is correct");
}


V8MONKEY_TEST(Plat004, "Key creation with destructor works correctly") {
  destructorCalled = false;
  Thread t {thread_tls_destruct};

  TLSKey<intptr_t> key {tlsDestructor};
  t.Run(&key);
  t.Join();
  V8MONKEY_CHECK(destructorCalled, "Destructor for TLS key was called");
}


V8MONKEY_TEST(Plat005, "HasRan correct after thread runs") {
  Thread t {thread_run_main};
  t.Run(nullptr);
  t.Join();
  V8MONKEY_CHECK(t.HasExecuted(), "HasRan reported true");
}


V8MONKEY_TEST(Plat006, "Thread joining returns correct value") {
  Thread t {thread_join_main};
  t.Run(nullptr);
  void* result {t.Join()};
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(result) == threadResult, "Value returned by thread join is correct");
}


V8MONKEY_TEST(Plat007, "Thread argument passing works correctly") {
  Thread t {thread_arg_main};
  t.Run(reinterpret_cast<void*>(threadArg));
  void* result {t.Join()};
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(result) == threadArg, "Value returned shows arg was passed correctly");
}


V8MONKEY_TEST(Plat008, "Run called with no arguments calls thread function with nullptr") {
  Thread t {thread_noarg_main};
  t.Run();
  void* result {t.Join()};
  V8MONKEY_CHECK(result == nullptr, "Run was called with null pointer");
}


V8MONKEY_TEST(Plat009, "OneShot called only once") {
  oneShotVal = 0;
  OneShot o(oneShot);
  o.Run();
  o.Run();
  V8MONKEY_CHECK(oneShotVal == 1, "One shot function only ran once");
}
