// Isolate, V8
#include "v8.h"

// XXX Can be removed if threading is not restored
// Thread
#include "platform.h"

// InternalIsolate::IsEntered
#include "runtime/isolate.h"

// TestUtils
#include "test.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;
using namespace v8::V8Monkey;


namespace {
  // XXX I think this can be replaced by FromIsolate
  InternalIsolate* AsInternal(Isolate* i) {
    return reinterpret_cast<InternalIsolate*>(i);
  }


  V8MONKEY_TEST_HELPER(IsolateStaysEntered) {
    TestUtils::AutoIsolateCleanup ac;

    Isolate* i = Isolate::New();
    i->Enter();
    V8::Initialize();

    bool result = InternalIsolate::IsEntered(AsInternal(i));
    return reinterpret_cast<void*>(result);
  }


  void* DefaultIsolateGetsEntered(void* iso) {
    TestUtils::AutoIsolateCleanup ac;

    Isolate* defaultIsolate = reinterpret_cast<Isolate*>(iso);
    Isolate* i = Isolate::GetCurrent();
    // Remember: i will be null for the thread case!
    bool sanityCheck = i == NULL || i == defaultIsolate;

    V8::Initialize();
    bool result = InternalIsolate::IsEntered(AsInternal(defaultIsolate));

    return reinterpret_cast<void*>(sanityCheck && result);
  }


  void* DefaultIsolateRemainsEntered(void* iso) {
    TestUtils::AutoIsolateCleanup ac;

    Isolate* i = reinterpret_cast<Isolate*>(iso);
    i->Enter();

    V8::Initialize();
    bool result = InternalIsolate::IsEntered(AsInternal(i));

    return reinterpret_cast<void*>(result);
  }


  void* DefaultIsolateNotReentered(void* iso) {
    TestUtils::AutoIsolateCleanup ac;

    Isolate* i = reinterpret_cast<Isolate*>(iso);
    i->Enter();

    V8::Initialize();
    i->Exit();
    // The point here is that if Initialize incorrectly re-entered the default isolate, we'll still be inside it
    bool result = !InternalIsolate::IsEntered(AsInternal(i));

    return reinterpret_cast<void*>(result);
  }
}


V8MONKEY_TEST(IntInit001, "After V8 initialization, current isolate still entered if originally in non-default isolate") {
  TestUtils::AutoTestCleanup ac;
  V8MONKEY_CHECK(IsolateStaysEntered(), "Entered isolate unchanged");
}


V8MONKEY_TEST(IntInit002, "After V8 initialization, default isolate entered if not already entered (main)") {
  TestUtils::AutoTestCleanup ac;
  Isolate* defaultIsolate = Isolate::GetCurrent();
  V8MONKEY_CHECK(DefaultIsolateGetsEntered(defaultIsolate), "Default was entered");
}


V8MONKEY_TEST(IntInit003, "After V8 initialization, default isolate remains entered if already entered (main)") {
  TestUtils::AutoTestCleanup ac;
  Isolate* defaultIsolate = Isolate::GetCurrent();
  V8MONKEY_CHECK(DefaultIsolateRemainsEntered(defaultIsolate), "Default still entered");
}


V8MONKEY_TEST(IntInit004, "After V8 initialization, default isolate not re-entered if already entered (main)") {
  TestUtils::AutoTestCleanup ac;
  Isolate* defaultIsolate = Isolate::GetCurrent();
  V8MONKEY_CHECK(DefaultIsolateNotReentered(defaultIsolate), "Default still entered");
}


/*
 * Tests deleted due to abandoning multithreaded support
 *


V8MONKEY_TEST(IntInit002, "After V8 initialization, current isolate still entered if originally in non-default isolate (thread)") {
  TestUtils::AutoTestCleanup ac;
  V8Platform::Thread child(IsolateStaysEntered);
  child.Run();
  V8MONKEY_CHECK(child.Join(), "Entered isolate unchanged");
}


V8MONKEY_TEST(IntInit004, "After V8 initialization, default isolate entered if not already entered (thread)") {
  TestUtils::AutoTestCleanup ac;
  Isolate* defaultIsolate = Isolate::GetCurrent();
  V8Platform::Thread child(DefaultIsolateGetsEntered);
  child.Run(defaultIsolate);
  V8MONKEY_CHECK(child.Join(), "Default was entered");
}


V8MONKEY_TEST(IntInit005, "After V8 initialization, default isolate not re-entered if already entered (thread)") {
  TestUtils::AutoTestCleanup ac;
  Isolate* defaultIsolate = Isolate::GetCurrent();
  V8Platform::Thread child(DefaultIsolateNotReentered);
  child.Run(defaultIsolate);
  V8MONKEY_CHECK(child.Join(), "Default still entered");
}


V8MONKEY_TEST(IntInit006, "After V8 initialization, default isolate remains entered if already entered (thread)") {
  TestUtils::AutoTestCleanup ac;
  Isolate* defaultIsolate = Isolate::GetCurrent();
  V8Platform::Thread child(DefaultIsolateRemainsEntered);
  child.Run(defaultIsolate);
  V8MONKEY_CHECK(child.Join(), "Default still entered");
}

*/
