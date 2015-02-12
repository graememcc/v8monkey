// intptr_t
#include <cinttypes>

// memcpy
#include <cstring>

// Class under test
#include "platform.h"

// Unit-testing support
#include "V8MonkeyTest.h"


// XXX I think we need to fix some of these casts


namespace {
  bool destructorCalled {false};

  intptr_t testData {42};

  void tlsDestructor(void* data) {
    intptr_t realData;
    static_assert(sizeof(intptr_t) <= sizeof(void*), "Somebody hasn't read their standard");
    std::memcpy(reinterpret_cast<char*>(&realData), reinterpret_cast<char*>(&data), sizeof(intptr_t));

    destructorCalled = realData == testData;
  }


  // Tests to see if a destructor is called
  void* thread_tls_destruct(void* arg) {
    v8::V8Platform::TLSKey* key {reinterpret_cast<v8::V8Platform::TLSKey*>(arg)};

    void* p = nullptr;
    static_assert(sizeof(intptr_t) <= sizeof(void*), "Somebody hasn't read their standard");
    std::memcpy(reinterpret_cast<char*>(&p), reinterpret_cast<char*>(&testData), sizeof(intptr_t));

    v8::V8Platform::Platform::StoreTLSData(key, p);
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
  TLSKey* key {Platform::CreateTLSKey()};
  V8MONKEY_CHECK(Platform::GetTLSData(key) == nullptr, "Key value is initially null");
  Platform::DeleteTLSKey(key);
}


V8MONKEY_TEST(Plat002, "TLS Key get returns correct value") {
  TLSKey* key {Platform::CreateTLSKey()};
  intptr_t value {42};
  Platform::StoreTLSData(key, reinterpret_cast<void*>(value));
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(Platform::GetTLSData(key)) == value, "Retrieved key is correct");
  Platform::DeleteTLSKey(key);
}


V8MONKEY_TEST(Plat003, "Key creation with destructor works correctly") {
  TLSKey* key {Platform::CreateTLSKey(tlsDestructor)};

  destructorCalled = false;
  Thread t {thread_tls_destruct};
  t.Run(key);
  t.Join();
  V8MONKEY_CHECK(destructorCalled, "Destructor for TLS key was called");
}


V8MONKEY_TEST(Plat004, "HasRan correct after thread runs") {
  Thread t {thread_run_main};
  t.Run(nullptr);
  t.Join();
  V8MONKEY_CHECK(t.HasRan(), "HasRan reported true");
}


V8MONKEY_TEST(Plat005, "Thread joining returns correct value") {
  Thread t {thread_join_main};
  t.Run(nullptr);
  void* result {t.Join()};
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(result) == threadResult, "Value returned by thread join is correct");
}


V8MONKEY_TEST(Plat006, "Thread argument passing works correctly") {
  Thread t {thread_arg_main};
  t.Run(reinterpret_cast<void*>(threadArg));
  void* result {t.Join()};
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(result) == threadArg, "Value returned shows arg was passed correctly");
}


V8MONKEY_TEST(Plat007, "Run called with no arguments calls thread function with nullptr") {
  Thread t {thread_noarg_main};
  t.Run();
  void* result {t.Join()};
  V8MONKEY_CHECK(result == nullptr, "Run was called with null pointer");
}


V8MONKEY_TEST(Plat008, "OneShot called only once") {
  oneShotVal = 0;
  OneShot o(oneShot);
  o.Run();
  o.Run();
  V8MONKEY_CHECK(oneShotVal == 1, "One shot function only ran once");
}
