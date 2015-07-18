// intptr_t
#include <cinttypes>

// memcpy
#include <cstring>

// Class under test
#include "platform/platform.h"

// Unit-testing support
#include "V8MonkeyTest.h"


namespace {
  bool destructorCalled {false};

  int testData {42};

  extern "C"
  void tlsDestructor(void* data) {
    int* realData {reinterpret_cast<int*>(data)};
    destructorCalled = realData == &testData;
  }


  // To test if a TLS destructor is called, a TLS key is created. In this function, we set the key (the spawning thread
  // supplies the address of the key). In the destructor function, we will confirm that the value passed to the
  // destructor function is the same value.
  extern "C"
  void* tlsDestructorTest(void* arg) {
    v8::V8Platform::TLSKey<int>* key = reinterpret_cast<v8::V8Platform::TLSKey<int>*>(arg);
    key->Set(&testData);
    return nullptr;
  }


  // A trivial function to be invoked by a thread, used when testing whether a Thread object correctly reports whether
  // it has been executed.
  extern "C"
  void* hasRan(void*) {
    return nullptr;
  }


  // To test whether Run reports success correctly, we use this function which sets a boolean to note that it has been
  // executed. Clearly, if the code has executed, then the call to Run should have reported success.
  bool threadExecuted {false};
  extern "C"
  void* threadRunTest(void*) {
    threadExecuted = true;
    return nullptr;
  }


  // To test whether Join returns values correctly, we take the address of the value below and return it. The joining
  // thread checks to see if it receives this address.
  int threadResult {1000};
  extern "C"
  void* threadJoinTest(void*) {
    return reinterpret_cast<void*>(&threadResult);
  }


  // To test whether arguments are supplied to the thread function correctly, this function returns whatever it was
  // given, which the spawning thread can check.
  int threadArg {7};
  extern "C"
  void* threadArgTest(void* arg) {
    return arg;
  }


  // To test that one-shots are only executed once, we zero a variable and twice attempt to invoke a function which
  // increments it, and check the final value.
  int oneShotVal {0};
  extern "C"
  void oneShot() {
    oneShotVal++;
  }
}


using namespace v8::V8Platform;


V8MONKEY_TEST(Plat001, "TLS Key get initially returns null") {
  TLSKey<void> key1 {};
  V8MONKEY_CHECK(key1.Get() == nullptr, "Key value is initially null");
  TLSKey<int> key2 {};
  V8MONKEY_CHECK(key2.Get() == 0, "Key value is initially null");
}


V8MONKEY_TEST(Plat002, "TLS Key get returns correct value (1)") {
  TLSKey<int> key {};
  int value {42};
  key.Set(&value);
  V8MONKEY_CHECK(key.Get() == &value, "Retrieved key is correct");
}


V8MONKEY_TEST(Plat003, "TLS Key get returns correct value (2)") {
  // Test with something smaller than a pointer
  TLSKey<char> key {};
  char value {'\7'};
  key.Set(&value);
  V8MONKEY_CHECK(key.Get() == &value, "Retrieved key is correct");
}


V8MONKEY_TEST(Plat004, "TLS Key set returns true if native function successful") {
  TLSKey<int> key {};
  int value {42};
  bool setResult {key.Set(&value)};
  // If fetching the value returns the correct value, then setting the key should have reported success
  V8MONKEY_CHECK(setResult == (key.Get() == &value), "Set reported success correctly");
}


V8MONKEY_TEST(Plat005, "Key creation with destructor works correctly") {
  destructorCalled = false;
  Thread t {tlsDestructorTest};

  TLSKey<int> key {tlsDestructor};
  t.Run(&key);
  t.Join();
  V8MONKEY_CHECK(destructorCalled, "Destructor for TLS key was called with correct data");
}


V8MONKEY_TEST(Plat006, "HasExecuted initially false") {
  Thread t {hasRan};
  V8MONKEY_CHECK(!t.HasExecuted(), "HasRan reported false");
}


V8MONKEY_TEST(Plat007, "HasExecuted correct after thread execution") {
  Thread t {hasRan};
  t.Run(nullptr);
  t.Join();
  V8MONKEY_CHECK(t.HasExecuted(), "HasRan reported true");
}


V8MONKEY_TEST(Plat008, "Run reports success correctly") {
  Thread t {threadRunTest};
  // Clear any extant state
  threadExecuted = false;
  bool runResult {t.Run()};
  t.Join();
  V8MONKEY_CHECK(runResult == threadExecuted, "Run reported success correctly");
}


V8MONKEY_TEST(Plat009, "Thread joining returns correct value") {
  Thread t {threadJoinTest};
  t.Run(nullptr);
  V8MONKEY_CHECK(reinterpret_cast<int*>(t.Join()) == &threadResult, "Value returned by thread join is correct");
}


V8MONKEY_TEST(Plat010, "Thread argument passing works correctly") {
  Thread t {threadArgTest};
  t.Run(reinterpret_cast<void*>(&threadArg));
  V8MONKEY_CHECK(reinterpret_cast<int*>(t.Join()) == &threadArg, "Value returned shows arg was passed correctly");
}


V8MONKEY_TEST(Plat011, "Run called with no arguments calls thread function with nullptr") {
  Thread t {threadArgTest};
  t.Run();
  V8MONKEY_CHECK(t.Join() == nullptr, "Run was called with null pointer");
}


V8MONKEY_TEST(Plat012, "OneShot called only once") {
  // Clear any extant state
  oneShotVal = 0;
  OneShot o {oneShot};
  o.Run();
  o.Run();
  V8MONKEY_CHECK(oneShotVal == 1, "One shot function only ran once");
}
