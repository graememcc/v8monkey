// TestUtils
#include "utils/test.h"

// Initialize, SetFatalErrorHandler
#include "v8.h"

// TriggerFatalError
#include "utils/V8MonkeyCommon.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;


namespace {
  // Suppress error arbortions
  void dummyFatalErrorHandler(const char*, const char*) {}
}


V8MONKEY_TEST(DeathInternal001, "V8::IsDead returns true after internal error (no init)") {
  v8::TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();

  v8::V8::SetFatalErrorHandler(dummyFatalErrorHandler);
  v8::V8Monkey::TriggerFatalError(nullptr, nullptr);
  V8MONKEY_CHECK(v8::V8::IsDead(), "Dead after internal error");
}


V8MONKEY_TEST(DeathInternal002, "V8::IsDead is isolate specific (1)") {
  v8::TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  Isolate* j {Isolate::New()};

  j->Enter();
  v8::V8::SetFatalErrorHandler(dummyFatalErrorHandler);
  v8::V8Monkey::TriggerFatalError(nullptr, nullptr);
  V8MONKEY_CHECK(v8::V8::IsDead(), "Dead after internal error");
  j->Exit();
  j->Dispose();

  i->Enter();
  V8MONKEY_CHECK(!v8::V8::IsDead(), "Not dead once entered another isolate");
}


V8MONKEY_TEST(DeathInternal003, "V8::IsDead is isolate specific (2)") {
  v8::TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  Isolate* j {Isolate::New()};

  i->Enter();
  j->Enter();
  v8::V8::SetFatalErrorHandler(dummyFatalErrorHandler);
  v8::V8Monkey::TriggerFatalError(nullptr, nullptr);

  V8MONKEY_CHECK(v8::V8::IsDead(), "Dead after internal error");

  j->Exit();
  j->Dispose();

  V8MONKEY_CHECK(!v8::V8::IsDead(), "Not dead once exited to another isolate");
}
