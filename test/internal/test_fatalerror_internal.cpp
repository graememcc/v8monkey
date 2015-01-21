// IsEntered
#include "runtime/isolate.h"

// TestUtils
#include "utils/test.h"

// TriggerFatalError
#include "utils/V8MonkeyCommon.h"

// Isolate SetFatalErrorHandler
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"


namespace {
  // To verify that the fatal error handlers we supply are called, we will have them set these variables, and then
  // test for equality

  bool callback1Called = false;
  bool callback2Called = false;


  void callback1(const char* location, const char* message) {
    callback1Called = true;
  }


  void callback2(const char* location, const char* message) {
    callback2Called = true;
  }


// XXX Switch to FromIsolate
  v8::V8Monkey::InternalIsolate* AsInternal(v8::Isolate* i) {
    return reinterpret_cast<v8::V8Monkey::InternalIsolate*>(i);
  }
}


using namespace v8;


V8MONKEY_TEST(FatalHandler001, "Calling SetFatalErrorHandler implicitly enters default isolate") {
  V8Monkey::TestUtils::AutoTestCleanup ac;

  V8::SetFatalErrorHandler(callback1);
  V8MONKEY_CHECK(V8Monkey::InternalIsolate::IsEntered(AsInternal(Isolate::GetCurrent())), "Correctly entered default isolate");
}


V8MONKEY_TEST(FatalHandler002, "Calling SetFatalErrorHandler doesn't change isolate if already entered") {
  V8Monkey::TestUtils::AutoTestCleanup ac;

  Isolate* i = Isolate::New();
  i->Enter();

  V8::SetFatalErrorHandler(callback1);
  V8MONKEY_CHECK(V8Monkey::InternalIsolate::IsEntered(AsInternal(i)), "Correctly remained in isolate");
}


V8MONKEY_TEST(FatalHandler003, "Specified fatal error handler called") {
  V8Monkey::TestUtils::AutoTestCleanup ac;

  callback1Called = false;
  V8::SetFatalErrorHandler(callback1);
  V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);
  V8MONKEY_CHECK(callback1Called, "Handler called");
}


V8MONKEY_TEST(FatalHandler004, "Specified fatal error handler is isolate specific (1)") {
  V8Monkey::TestUtils::AutoTestCleanup ac;

  Isolate* i = Isolate::New();
  i->Enter();
  V8::SetFatalErrorHandler(callback1);

  Isolate* j = Isolate::New();
  j->Enter();
  V8::SetFatalErrorHandler(callback2);

  callback1Called = false;
  callback2Called = false;
  V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);
  V8MONKEY_CHECK(!callback1Called, "Old handler not called");
  V8MONKEY_CHECK(callback2Called, "Handler called");
}


V8MONKEY_TEST(FatalHandler005, "Specified fatal error handler is isolate specific (2)") {
  V8Monkey::TestUtils::AutoTestCleanup ac;

  Isolate* i = Isolate::New();
  i->Enter();
  V8::SetFatalErrorHandler(callback1);

  Isolate* j = Isolate::New();
  j->Enter();
  V8::SetFatalErrorHandler(callback2);
  j->Exit();

  callback1Called = false;
  callback2Called = false;
  V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);
  V8MONKEY_CHECK(!callback2Called, "Old handler not called");
  V8MONKEY_CHECK(callback1Called, "Handler called");

  j->Dispose();
}
