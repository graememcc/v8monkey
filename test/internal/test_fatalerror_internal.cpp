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

  bool callback1Called {false};
  bool callback2Called {false};


  void callback1(const char*, const char*) {
    callback1Called = true;
  }


  void callback2(const char*, const char*) {
    callback2Called = true;
  }
}


using namespace v8;


/*
 * Note: we must manually trigger an error and check that the fatal error handler is called, as then tests checking
 * error conditions can consider a called FatalErrorHandler as witness that an error was correctly signalled.
 *
 */

V8MONKEY_TEST(FatalHandler001, "Specified fatal error handler called") {
  v8::TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  callback1Called = false;
  V8::SetFatalErrorHandler(callback1);
  V8Monkey::TriggerFatalError(nullptr, nullptr);
  V8MONKEY_CHECK(callback1Called, "Handler called");
}


V8MONKEY_TEST(FatalHandler002, "Specified fatal error handler is isolate specific (1)") {
  v8::TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8::SetFatalErrorHandler(callback1);

  Isolate* j {Isolate::New()};
  j->Enter();
  V8::SetFatalErrorHandler(callback2);

  callback1Called = false;
  callback2Called = false;
  V8Monkey::TriggerFatalError(nullptr, nullptr);
  V8MONKEY_CHECK(!callback1Called, "Old handler not called");
  V8MONKEY_CHECK(callback2Called, "Handler called");
}


V8MONKEY_TEST(FatalHandler003, "Specified fatal error handler is isolate specific (2)") {
  v8::TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8::SetFatalErrorHandler(callback1);

  Isolate* j {Isolate::New()};
  j->Enter();
  V8::SetFatalErrorHandler(callback2);
  j->Exit();
  j->Dispose();

  callback1Called = false;
  callback2Called = false;
  V8Monkey::TriggerFatalError(nullptr, nullptr);
  V8MONKEY_CHECK(!callback2Called, "Old handler not called");
  V8MONKEY_CHECK(callback1Called, "Handler called");
}
