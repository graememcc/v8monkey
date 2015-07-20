// Isolate::New, V8::Initialize
#include "v8.h"

// XXX May be removed if threading support not reinstated
// Thread
#include "platform/platform.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;


namespace {
  void dummyFatalErrorHandler(const char*, const char*) {}
}


V8MONKEY_TEST(Init001, "Returns true if not in an isolate") {
  V8MONKEY_CHECK(V8::Initialize(), "V8::Initialize returned true");
}


V8MONKEY_TEST(Init002, "Initializing V8 does not enter an isolate") {
  V8::Initialize();

  V8MONKEY_CHECK(!Isolate::GetCurrent(), "Isolate::GetCurrent still nullptr");
  V8::Dispose();
}


V8MONKEY_TEST(Init005, "Initializing V8 doesn't change isolate") {
  Isolate* i {Isolate::New()};
  i->Enter();
  V8::Initialize();

  V8MONKEY_CHECK(Isolate::GetCurrent() == i, "V8::Initialize leaves isolate unchanged");

  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(Init003, "Initializing V8 returns false if entered isolate is dead") {
  Isolate* i {Isolate::New()};
  i->Enter();
  V8::SetFatalErrorHandler(dummyFatalErrorHandler);
  i->Dispose();

  V8MONKEY_CHECK(!V8::Initialize(), "V8::Initialize returned true");

  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(Init004, "Initializing V8 returns true if in live isolate") {
  Isolate* i {Isolate::New()};
  i->Enter();

  V8MONKEY_CHECK(V8::Initialize(), "V8::Initialize returned true");

  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(Dispose001, "V8::Dispose returns true when successful") {
  Isolate* i {Isolate::New()};
  i->Enter();
  V8::Initialize();
  i->Exit();
  i->Dispose();
  V8MONKEY_CHECK(V8::Dispose(), "Disposing V8 returned true");
}


V8MONKEY_TEST(Dispose002, "OK to call V8::Dispose without entering isolate") {
  V8::Initialize();
  V8MONKEY_CHECK(V8::Dispose(), "Disposing V8 returned true");
}
