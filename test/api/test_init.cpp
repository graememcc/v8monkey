// Isolate::New, V8::Initialize
#include "v8.h"

// XXX May be removed if threading support not reinstated
// Thread
#include "platform.h"

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


V8MONKEY_TEST(Init003, "Initializing V8 returns false if entered isolate is dead") {
  Isolate* i {Isolate::New()};
  i->Enter();
  V8::SetFatalErrorHandler(dummyFatalErrorHandler);
  i->Dispose();

  V8MONKEY_CHECK(!V8::Initialize(), "V8::Initialize returned true");

  i->Exit();
  i->Dispose();
}
//
//
//// Returns false if in a dead isolate
//// Returns true if in an initted isolate
//// Current isolate always unchanged
//
//// XXX No longer relevant?
//V8MONKEY_TEST(Init001, "After V8 initialization, current isolate unchanged if originally in non-default isolate") {
//  Isolate* i = Isolate::New();
//  i->Enter();
//
//  V8::Initialize();
//  V8MONKEY_CHECK(Isolate::GetCurrent() == i, "Isolate unchanged");
//
//  i->Exit();
//  i->Dispose();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Init002, "After V8 initialization, entry count unchanged when non-default isolate already entered") {
//  Isolate* initial = Isolate::GetCurrent();
//  Isolate* i = Isolate::New();
//  i->Enter();
//  V8::Initialize();
//  i->Exit();
//
//  // If initialize re-entered the current isolate,then we won't be back at initial
//  V8MONKEY_CHECK(Isolate::GetCurrent() == initial, "Isolate entries correct");
//
//  i->Dispose();
//  V8::Dispose();
//}
//
//
//// XXX No longer relevant?
//V8MONKEY_TEST(Init003, "After V8 initialization, current isolate still default if no isolate originally entered") {
//  Isolate* i = Isolate::GetCurrent();
//  V8::Initialize();
//
//  V8MONKEY_CHECK(Isolate::GetCurrent() == i, "Isolate unchanged");
//
//  i->Exit();
//  V8::Dispose();
//}
//
//
//// XXX No longer relevant?
//V8MONKEY_TEST(Init004, "After V8 initialization, current isolate unchanged when in default") {
//  Isolate* i = Isolate::GetCurrent();
//  i->Enter();
//  V8::Initialize();
//
//  V8MONKEY_CHECK(Isolate::GetCurrent() == i, "Isolate unchanged");
//
//  i->Exit();
//  V8::Dispose();
//}
//
//
///*
// * Tests disabled after threading support disabled
// *
//namespace {
//  void* threadInitTest(void* arg) {
//    V8MONKEY_CHECK(Isolate::GetCurrent() == nullptr, "Sanity check");
//    V8::Initialize();
//
//    bool result = Isolate::GetCurrent() != nullptr;
//
//    Isolate::GetCurrent()->Exit();
//    Isolate::GetCurrent()->Dispose();
//    V8::Dispose();
//
//    return reinterpret_cast<void*>(result);
//  }
//}
//
//
//V8MONKEY_TEST(Init005, "After V8 Initialization, thread outside any isolate enters default") {
//  // The main thread will be associated with the default isolate due to static initializers. A freshly minted thread
//  // will have its current isolate equal to nullptr initially.
//  V8Platform::Thread t(threadInitTest);
//  t.Run();
//  V8MONKEY_CHECK(t.Join(), "Default isolate entered");
//}
//*/
