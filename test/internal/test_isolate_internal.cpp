// JSTraceDataOp
#include "jsapi.h"

// Class under test
#include "runtime/isolate.h"

// Thread
#include "platform/platform.h"

// TestUtils
#include "utils/test.h"

// DeletionObject DummyV8MonkeyObject
#include "types/base_types.h"

// GetJSRuntime/GetJSContext, SetGCRegistrationHooks
#include "utils/SpiderMonkeyUtils.h"

// Isolate, V8
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;
using namespace v8::V8Platform;


// XXX V8 Dispose where necessary


namespace {
  /*
   * Returns a bool denoting whether the given isolate is seen as not locked by the executing thread.
   *
   */

  void* ThreadNotLocked(void* iso) {
    internal::Isolate* i {reinterpret_cast<internal::Isolate*>(iso)};
    return reinterpret_cast<void*>(!i->IsLockedForThisThread());
  }


  /*
   * Returns a bool denoting whether the runtime associated with a thread is initially null.
   *
   */

  void* RuntimeInitiallyNull(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    bool result {SpiderMonkey::GetJSRuntimeForThread() == nullptr};
    return reinterpret_cast<void*>(result);
  }


  /*
   * Returns a bool denoting whether the context associated with a thread is initially null.
   *
   */

  void* ContextInitiallyNull(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    bool result {SpiderMonkey::GetJSContextForThread() == nullptr};
    return reinterpret_cast<void*>(result);
  }


  /*
   * Returns a bool denoting whether the runtime associated with a thread is non-null after entry.
   *
   */

  void* RuntimeNonNullAfterEntry(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {Isolate::New()};
    i->Enter();
    bool result {SpiderMonkey::GetJSRuntimeForThread() != nullptr};
    return reinterpret_cast<void*>(result);
  }


  /*
   * Returns a bool denoting whether the context associated with a thread is non-null after entry.
   *
   */

  void* ContextNonNullAfterEntry(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {Isolate::New()};
    i->Enter();
    bool result {SpiderMonkey::GetJSContextForThread() != nullptr};
    return reinterpret_cast<void*>(result);
  }


  /*
   * Thread function which returns the current JSRuntime cast to void.
   *
   */

  void* FetchThreadRT(void* arg = nullptr) {
    Isolate* i {nullptr};
    if (!arg) {
      i = {Isolate::New()};
    } else {
      i = reinterpret_cast<Isolate*>(arg);
    }

    i->Enter();
    JSRuntime* rt {SpiderMonkey::GetJSRuntimeForThread()};

    i->Exit();
    if (!arg) {
      i->Dispose();
    }
    return rt;
  }


  /*
   * Thread function which returns the current JSContext cast to void.
   *
   */

  void* FetchThreadCX(void* arg = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {nullptr};
    if (!arg) {
      i = {Isolate::New()};
    } else {
      i = reinterpret_cast<Isolate*>(arg);
    }

    i->Enter();
    JSContext* cx {SpiderMonkey::GetJSContextForThread()};

    i->Exit();
    if (!arg) {
      i->Dispose();
    }
    return cx;
  }


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, and the child thread it spawns recieved unique
   * JSRuntimes.
   *
   */

  void* ChildrenGetDistinctRuntimes(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {Isolate::New()};
    i->Enter();
    JSRuntime* rt1 {SpiderMonkey::GetJSRuntimeForThread()};

    Thread child {FetchThreadRT};
    child.Run();
    JSRuntime* rt2 {reinterpret_cast<JSRuntime*>(child.Join())};
    bool result {rt1 != rt2};

    return reinterpret_cast<void*>(result);
  }


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, and the child thread it spawns recieved unique
   * JSContexts.
   *
   */

  void* ChildrenGetDistinctContexts(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {Isolate::New()};
    i->Enter();
    JSContext* cx1 {SpiderMonkey::GetJSContextForThread()};

    Thread child {FetchThreadCX};
    child.Run();
    JSContext* cx2 {reinterpret_cast<JSContext*>(child.Join())};
    bool result {cx1 != cx2};

    return reinterpret_cast<void*>(result);
  }


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, and the child thread it spawns recieved unique
   * JSRuntimes after entering the same isolate.
   *
   */

  void* ChildrenSameIsoGetDistinctRuntimes(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {Isolate::New()};
    i->Enter();
    JSRuntime* rt1 {SpiderMonkey::GetJSRuntimeForThread()};

    Thread child {FetchThreadRT};
    child.Run(i);
    JSRuntime* rt2 {reinterpret_cast<JSRuntime*>(child.Join())};
    bool result {rt1 != rt2};

    return reinterpret_cast<void*>(result);
  }


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, and the child thread it spawns recieved unique
   * JSContexts after entering the same isolate.
   *
   */

  void* ChildrenSameIsoGetDistinctContexts(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {Isolate::New()};
    i->Enter();
    JSContext* cx1 {SpiderMonkey::GetJSContextForThread()};

    Thread child {FetchThreadCX};
    child.Run(i);
    JSContext* cx2 {reinterpret_cast<JSContext*>(child.Join())};
    bool result {cx1 != cx2};

    return reinterpret_cast<void*>(result);
  }


  bool wasRegisteredForGC {false};
  JSRuntime* runtimeRegisteredForGC {nullptr};


  void GCRegistrationHook(JSRuntime* rt, JSTraceDataOp, void*) {
    wasRegisteredForGC = true;
    runtimeRegisteredForGC = rt;
  }


  static bool wasDeregisteredFromGC {false};
  JSRuntime* runtimeDeregisteredFromGC {nullptr};


  void GCDeregistrationHook(JSRuntime* rt, JSTraceDataOp, void*) {
    wasDeregisteredFromGC = true;
    runtimeDeregisteredFromGC = rt;
  }


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, had its runtime registered for GC on entering the
   * isolate pointed to by the given argument.
   *
   */

  void* ThreadWasRooted(void* iso) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {reinterpret_cast<Isolate*>(iso)};
    wasRegisteredForGC = false;
    runtimeRegisteredForGC = nullptr;
    i->Enter();
    bool result {wasRegisteredForGC && runtimeRegisteredForGC == SpiderMonkey::GetJSRuntimeForThread()};
    i->Exit();

    return reinterpret_cast<void*>(result);
  }


  /*
   * Utility function that simply has a thread enter/exit a supplied isolate, without disposing it.
   *
   */

  void* ThreadEnterExit(void* iso) {
    // Note: can't construct an AutoIsolateCleanup - we don't want to dispose of the isolate

    Isolate* i {reinterpret_cast<Isolate*>(iso)};
    i->Enter();
    i->Exit();

    return nullptr;
  }
}


V8MONKEY_TEST(IntIsolate001, "IsEntered works correctly") {
  Isolate* i {Isolate::New()};
  i->Enter();
  V8MONKEY_CHECK(internal::Isolate::IsEntered(internal::Isolate::FromAPIIsolate(i)),
                 "IsEntered reports true for entered isolate");
  i->Exit();
  V8MONKEY_CHECK(!internal::Isolate::IsEntered(internal::Isolate::FromAPIIsolate(i)),
                 "IsEntered reports false for exited isolate");
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate002, "Isolate reports empty when not entered") {
  Isolate* i {Isolate::New()};
  V8MONKEY_CHECK(!internal::Isolate::FromAPIIsolate(i)->ContainsThreads(), "Empty isolate reports no threads active");
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate003, "Isolate reports non-empty when entered") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8MONKEY_CHECK(internal::Isolate::FromAPIIsolate(i)->ContainsThreads(), "Entered isolate reports threads active");
}


V8MONKEY_TEST(IntIsolate004, "Isolate reports empty when exited") {
  Isolate* i {Isolate::New()};
  i->Enter();
  i->Exit();
  V8MONKEY_CHECK(!internal::Isolate::FromAPIIsolate(i)->ContainsThreads(), "Entered isolate reports threads active");

  i->Dispose();
}


V8MONKEY_TEST(IntIsolate005, "IsLockedForThisThread reports false initially") {
  Isolate* i {Isolate::New()};
  V8MONKEY_CHECK(!internal::Isolate::FromAPIIsolate(i)->IsLockedForThisThread(), "IsLockedForThisThread correct");
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate006, "IsLockedForThisThread reports true after locking") {
  Isolate* i {Isolate::New()};
  internal::Isolate::FromAPIIsolate(i)->Lock();
  V8MONKEY_CHECK(internal::Isolate::FromAPIIsolate(i)->IsLockedForThisThread(), "IsLockedForThisThread correct");
  internal::Isolate::FromAPIIsolate(i)->Unlock();
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate007, "IsLockedForThisThread reports false after unlocking") {
  Isolate* i {Isolate::New()};
  internal::Isolate::FromAPIIsolate(i)->Lock();
  internal::Isolate::FromAPIIsolate(i)->Unlock();
  V8MONKEY_CHECK(!internal::Isolate::FromAPIIsolate(i)->IsLockedForThisThread(), "IsLockedForThisThread correct");
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate008, "IsLockedForThisThread reports false for another thread after locking") {
  Isolate* i {Isolate::New()};
  internal::Isolate* ii {internal::Isolate::FromAPIIsolate(i)};
  ii->Lock();
  Thread child {ThreadNotLocked};
  child.Run(ii);
  V8MONKEY_CHECK(child.Join(), "Another thread finds IsLockedForThisThread null");

  ii->Unlock();
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate009, "Associated runtime initially null (main)") {
  V8MONKEY_CHECK(RuntimeInitiallyNull(), "JSRuntime is null");
}


V8MONKEY_TEST(IntIsolate010, "Associated context initially null (main)") {
  V8MONKEY_CHECK(ContextInitiallyNull(), "JSRuntime is null");
}


V8MONKEY_TEST(IntIsolate011, "After isolate entry, JSRuntime non-null") {
  V8MONKEY_CHECK(RuntimeNonNullAfterEntry(), "JSRuntime non-null");
}


V8MONKEY_TEST(IntIsolate012, "After isolate entry, JSContext non-null") {
  V8MONKEY_CHECK(ContextNonNullAfterEntry(), "JSContext non-null");
}


V8MONKEY_TEST(IntIsolate013, "JSRuntime assignment doesn't affect other threads") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSRuntimeForThread(), "Sanity check");
  Thread child {RuntimeInitiallyNull};
  child.Run();
  V8MONKEY_CHECK(child.Join(), "Another thread's JSRuntime initially null");
}


V8MONKEY_TEST(IntIsolate014, "JSContext assignment doesn't affect other threads") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSContextForThread(), "Sanity check");
  Thread child {ContextInitiallyNull};
  child.Run();
  V8MONKEY_CHECK(child.Join(), "Another thread's JSContext initially null");
}


V8MONKEY_TEST(IntIsolate015, "JSRuntime assignment for threads works correctly") {
  // Every thread after the first exercises a slightly different code-path for RT/CX assignment
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSRuntimeForThread(), "Sanity check");
  Thread child {RuntimeNonNullAfterEntry};
  child.Run();
  V8MONKEY_CHECK(child.Join(), "Other thread's JSRuntime successfully assigned");
}


V8MONKEY_TEST(IntIsolate016, "JSContext assignment for threads works correctly") {
  // Every thread after the first exercises a slightly different code-path for RT/CX assignment
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSContextForThread(), "Sanity check");
  Thread child {ContextNonNullAfterEntry};
  child.Run();
  V8MONKEY_CHECK(child.Join(), "Other thread's JSContext successfully assigned");
}


V8MONKEY_TEST(IntIsolate017, "After isolate exit, JSRuntime unchanged") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  JSRuntime* prev {SpiderMonkey::GetJSRuntimeForThread()};
  i->Exit();
  V8MONKEY_CHECK(SpiderMonkey::GetJSRuntimeForThread() == prev, "JSRuntime unchanged");
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate018, "After isolate exit, JSContext unchanged") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  JSContext* prev {SpiderMonkey::GetJSContextForThread()};
  i->Exit();
  V8MONKEY_CHECK(SpiderMonkey::GetJSContextForThread() == prev, "JSContext unchanged");
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate019, "Threads get assigned distinct JSRuntimes (1)") {
  JSRuntime* rt1 {reinterpret_cast<JSRuntime*>(FetchThreadRT())};
  Thread child {FetchThreadRT};
  child.Run();
  JSRuntime* rt2 {reinterpret_cast<JSRuntime*>(child.Join())};
  V8MONKEY_CHECK(rt1 != rt2, "Threads were assigned distinct runtimes");
}


V8MONKEY_TEST(IntIsolate020, "Threads get assigned distinct JSContexts (1)") {
  JSContext* cx1 {reinterpret_cast<JSContext*>(FetchThreadCX())};
  Thread child {FetchThreadCX};
  child.Run();
  JSContext* cx2 {reinterpret_cast<JSContext*>(child.Join())};
  V8MONKEY_CHECK(cx1 != cx2, "Threads were assigned distinct runtimes");
}


V8MONKEY_TEST(IntIsolate021, "Threads get assigned distinct JSRuntimes (2)") {
  // As the first thread that enters an isolate triggers a slightly different code path from later entries, create two
  // child threads to exercise the second codepath twice, after first acquiring a JSRuntime and JSContext on this
  // thread
  Isolate* i {Isolate::New()};
  i->Enter();
  i->Exit();
  i->Dispose();

  Thread child {ChildrenGetDistinctRuntimes};
  child.Run();
  V8MONKEY_CHECK(child.Join(), "Two child threads recieved distinct JSRuntimes");
}


V8MONKEY_TEST(IntIsolate022, "Threads get assigned distinct JSContexts (2)") {
  // As the first thread that enters an isolate triggers a slightly different code path from later entries, create two
  // child threads to exercise the second codepath twice, after first acquiring a JSRuntime and JSContext on this
  // thread
  Isolate* i {Isolate::New()};
  i->Enter();
  i->Exit();
  i->Dispose();

  Thread child {ChildrenGetDistinctContexts};
  child.Run();
  V8MONKEY_CHECK(child.Join(), "Two child threads recieved distinct JSContexts");
}


V8MONKEY_TEST(IntIsolate023, "First thread and child thread entering same isolate get distinct runtimes") {
  Isolate* i {Isolate::New()};
  i->Enter();
  JSRuntime* rt1 {SpiderMonkey::GetJSRuntimeForThread()};

  Thread child {FetchThreadRT};
  child.Run(i);
  JSRuntime* rt2 {reinterpret_cast<JSRuntime*>(child.Join())};
  V8MONKEY_CHECK(rt1 != rt2, "Threads acquired distinct runtimes");

  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate024, "First thread and child thread entering same isolate get distinct contexts") {
  Isolate* i {Isolate::New()};
  i->Enter();
  JSContext* cx1 {SpiderMonkey::GetJSContextForThread()};

  Thread child {FetchThreadCX};
  child.Run(i);
  JSContext* cx2 {reinterpret_cast<JSContext*>(child.Join())};
  V8MONKEY_CHECK(cx1 != cx2, "Threads acquired distinct contexts");

  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate025, "Child threads entering same isolate get distinct runtimes") {
  // As the first thread that enters an isolate triggers a slightly different code path from later entries, create two
  // child threads to exercise the second codepath twice, after first acquiring a JSRuntime and JSContext on this
  // thread
  Isolate* i {Isolate::New()};
  i->Enter();
  i->Exit();
  i->Dispose();

  Thread child {ChildrenSameIsoGetDistinctRuntimes};
  child.Run();
  V8MONKEY_CHECK(child.Join(), "Two child threads recieved distinct JSRuntimes");
}


V8MONKEY_TEST(IntIsolate026, "Child threads entering same isolate get distinct contexts") {
  // As the first thread that enters an isolate triggers a slightly different code path from later entries, create two
  // child threads to exercise the second codepath twice, after first acquiring a JSRuntime and JSContext on this
  // thread
  Isolate* i {Isolate::New()};
  i->Enter();
  i->Exit();
  i->Dispose();

  Thread child {ChildrenSameIsoGetDistinctContexts};
  child.Run();
  V8MONKEY_CHECK(child.Join(), "Two child threads recieved distinct JSContexts");
}


V8MONKEY_TEST(IntIsolate027, "After isolate reentry, JSRuntime unchanged") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  JSRuntime* prev {SpiderMonkey::GetJSRuntimeForThread()};
  i->Exit();
  i->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSRuntimeForThread() == prev, "JSRuntime unchanged");
}


V8MONKEY_TEST(IntIsolate028, "After isolate reentry, JSContext unchanged") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  JSContext* prev {SpiderMonkey::GetJSContextForThread()};
  i->Exit();
  i->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSContextForThread() == prev, "JSContext unchanged");
}


V8MONKEY_TEST(IntIsolate029, "JSRuntime unchanged after entering another isolate (1)") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  JSRuntime* prev {SpiderMonkey::GetJSRuntimeForThread()};
  i->Exit();
  Isolate* j {Isolate::New()};
  j->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSRuntimeForThread() == prev, "JSRuntime unchanged");
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate030, "JSContext unchanged after entering another isolate (1)") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  JSContext* prev {SpiderMonkey::GetJSContextForThread()};
  i->Exit();
  Isolate* j {Isolate::New()};
  j->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSContextForThread() == prev, "JSContext unchanged");
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate031, "JSRuntime unchanged after entering another isolate (2)") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  JSRuntime* prev {SpiderMonkey::GetJSRuntimeForThread()};
  Isolate* j {Isolate::New()};
  j->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSRuntimeForThread() == prev, "JSRuntime unchanged");
}


V8MONKEY_TEST(IntIsolate032, "JSContext unchanged after entering another isolate (2)") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  JSContext* prev {SpiderMonkey::GetJSContextForThread()};
  Isolate* j {Isolate::New()};
  j->Enter();
  V8MONKEY_CHECK(SpiderMonkey::GetJSContextForThread() == prev, "JSContext unchanged");
}


V8MONKEY_TEST(IntIsolate033, "First thread to acquire runtime and context is unimportant") {
  TestUtils::AutoTestCleanup ac {};

  // Spawn a child, let it acquire the first runtime
  Thread child {FetchThreadRT};
  child.Run();
  child.Join();

  Isolate* i {Isolate::New()};
  i->Enter();

  // Nothing to check here: the real "test" is that we don't crash in static object teardown
}


V8MONKEY_TEST(IntIsolate034, "JSRuntime/JSContext always torn-down on exit") {
  // NOTE: No AutoTestCleanup here, we don't want the implicit V8::Dispose call

  Isolate* i {Isolate::New()};
  i->Enter();
  i->Exit();
  i->Dispose();

  // Nothing to check here: the real "test" is that we don't crash in static object teardown
}


V8MONKEY_TEST(IntIsolate035, "Entering an isolate registers for GC rooting") {
  TestUtils::AutoTestCleanup ac {};

  SpiderMonkey::SetGCRegistrationHooks(GCRegistrationHook, GCDeregistrationHook);
  Isolate* i {Isolate::New()};

  wasRegisteredForGC = false;
  runtimeRegisteredForGC = nullptr;
  i->Enter();

  V8MONKEY_CHECK(wasRegisteredForGC, "Notified SpiderMonkey about need to root");
  V8MONKEY_CHECK(runtimeRegisteredForGC == SpiderMonkey::GetJSRuntimeForThread(),
                 "Notified SpiderMonkey about correct runtime");

  i->Exit();
  i->Dispose();
  SpiderMonkey::SetGCRegistrationHooks(nullptr, nullptr);
}


V8MONKEY_TEST(IntIsolate036, "Exiting an isolate doesn't deregister an isolate from rooting") {
  TestUtils::AutoTestCleanup ac {};

  SpiderMonkey::SetGCRegistrationHooks(GCRegistrationHook, GCDeregistrationHook);
  Isolate* i {Isolate::New()};
  i->Enter();

  wasDeregisteredFromGC = false;
  runtimeRegisteredForGC = nullptr;
  i->Exit();

  V8MONKEY_CHECK(!wasDeregisteredFromGC, "Exiting isolate doesn't deregister");
  V8MONKEY_CHECK(!runtimeRegisteredForGC, "Exiting isolate doesn't deregister runtime");
  i->Dispose();

  SpiderMonkey::SetGCRegistrationHooks(nullptr, nullptr);
}


V8MONKEY_TEST(IntIsolate037, "Disposing an isolate deregisters it from rooting") {
  TestUtils::AutoTestCleanup ac {};

  SpiderMonkey::SetGCRegistrationHooks(GCRegistrationHook, GCDeregistrationHook);
  Isolate* i {Isolate::New()};
  i->Enter();
  i->Exit();
  wasDeregisteredFromGC = false;
  runtimeDeregisteredFromGC = nullptr;
  i->Dispose();

  V8MONKEY_CHECK(wasDeregisteredFromGC, "Disposing an isolate deregisters");
  V8MONKEY_CHECK(runtimeDeregisteredFromGC == SpiderMonkey::GetJSRuntimeForThread(),
                 "Exiting isolate deregistered correct runtime");
  SpiderMonkey::SetGCRegistrationHooks(nullptr, nullptr);
}


V8MONKEY_TEST(IntIsolate038, "Entering an isolate on different threads triggers GC registration for each runtime") {
  TestUtils::AutoTestCleanup ac {};

  SpiderMonkey::SetGCRegistrationHooks(GCRegistrationHook, GCDeregistrationHook);
  Isolate* i {Isolate::New()};

  wasRegisteredForGC = false;
  i->Enter();
  V8MONKEY_CHECK(wasRegisteredForGC, "Sanity check");

  i->Exit();
  Thread child {ThreadWasRooted};
  child.Run(i);

  V8MONKEY_CHECK(child.Join(), "Other thread registered seperately");

  i->Dispose();
  SpiderMonkey::SetGCRegistrationHooks(nullptr, nullptr);
}


V8MONKEY_TEST(IntIsolate039, "Multiple entries does not result in multiple calls to SpiderMonkey requesting rooting") {
  TestUtils::AutoTestCleanup ac {};

  SpiderMonkey::SetGCRegistrationHooks(GCRegistrationHook, GCDeregistrationHook);
  Isolate* i {Isolate::New()};

  wasRegisteredForGC = false;
  i->Enter();
  V8MONKEY_CHECK(wasRegisteredForGC, "Sanity check");

  i->Exit();
  wasRegisteredForGC = false;
  i->Enter();

  V8MONKEY_CHECK(!wasRegisteredForGC, "No duplicate registration");

  i->Exit();
  i->Dispose();
  SpiderMonkey::SetGCRegistrationHooks(nullptr, nullptr);
}


V8MONKEY_TEST(IntIsolate040, "Entering different isolates results in separate rooting requests") {
  TestUtils::AutoTestCleanup ac {};

  SpiderMonkey::SetGCRegistrationHooks(GCRegistrationHook, GCDeregistrationHook);
  Isolate* i {Isolate::New()};

  wasRegisteredForGC = false;
  i->Enter();
  V8MONKEY_CHECK(wasRegisteredForGC, "Sanity check");
  V8MONKEY_CHECK(runtimeRegisteredForGC == SpiderMonkey::GetJSRuntimeForThread(), "Sanity check");
  i->Exit();

  Isolate* j {Isolate::New()};
  wasRegisteredForGC = false;
  runtimeRegisteredForGC = nullptr;
  j->Enter();

  V8MONKEY_CHECK(wasRegisteredForGC, "Entering a different isolate triggered a separate registration");
  V8MONKEY_CHECK(runtimeRegisteredForGC == SpiderMonkey::GetJSRuntimeForThread(), "Runtime correct");

  j->Exit();
  j->Dispose();

  i->Dispose();
  SpiderMonkey::SetGCRegistrationHooks(nullptr, nullptr);
}


V8MONKEY_TEST(IntIsolate041, "Able to dispose an isolate after it was entered/exited on another thread") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  Thread child {ThreadEnterExit};
  child.Run(i);
  child.Join();

  i->Dispose();
  // If we get here disposing worked as expected
  V8MONKEY_CHECK(true, "Didn't crash");
}

// XXX Write the following test when we have the mechanisms to support it
// (Required mechanisms: Can inject TraceFakes etc into isolates, TraceFakes can count number of traces)
// Follow-on to test 039 (duplicate rooting registrations)
// Inject a TraceFake into an isolate we created
// Exit and re-enter the isolate
// zero the trace count, and force a GC
// trace count == 1. Proves that we don't add the same rooting function multiple times

// XXX Write the following test when we have the mechanisms to support it
// (Required mechanisms: Can inject TraceFakes etc into isolates)
// We supply the location for a TraceFake to update
// Call a thread fn that enters/exits the supplied isolate, and somehow creates a TraceFake with the given address
// On thread join, set the location to false, and force a GC
// value still false is correct. It shows that we don't trace objects for a runtime after the runtime is destroyed

// XXX Write the following test when we have the mechanisms to support it
// (Required mechanisms: Can inject shared_ptr wrapped objects into isolates)
// Enter an isolate, inject a TraceFake THAT WE HOLD IN A SHARED_PTR, exit and dispose
// Set the TraceFake location to false, and force a GC
// value still false is correct. It shows that we don't trace objects for an isolate after the isolate is destroyed

// XXX Write the following test once we have the mechanisms to support it:
// (Required mechanisms: Can inject TraceFakes etc into isolates)
// Inject a TraceFake into an isolate
// Call a thread that simply exits. This will still trigger the thread destruction code in SpiderMonkeyUtils
// After joining, set TraceFake boolean to false, and force a GC
// If TraceFake boolean is true, we've proved that destruction of threads not associated with isolates doesn't affect
// rooting of other isolates

// XXX Write the following test once we have the mechanisms to support it:
// (Required mechanisms: Can inject TraceFakes etc into isolates)
// Inject a TraceFake into an isolate
// Call a thread that creates an isolate and enters and exits it.
// After joining, set TraceFake boolean to false, and force a GC
// If TraceFake boolean is true, we've proved that destruction of threads associated with another isolate doesn't affect
// rooting of other isolates

// XXX Write the following test once we have the mechanisms to support it:
// (Required mechanisms: Can inject TraceFakes etc into isolates)
// Inject a TraceFake into an isolate
// Call a thread with our isolate, have the thread enter and exit it
// After joining, set TraceFake boolean to false, and force a GC
// If TraceFake boolean is true, we've proved that destruction of threads associated with this isolate doesn't affect
// this thread's rooting of the isolate

V8MONKEY_TEST(IntScope001, "Scopes exit copes with multiple entries on main thread") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();

  {
    Isolate::Scope scope(i);
  }

  V8MONKEY_CHECK(internal::Isolate::IsEntered(internal::Isolate::FromAPIIsolate(i)),
                 "Isolate::Scopes behave correctly in the face of multiple entries");
}


// V8MONKEY_TEST(IntIsolate035, "IsInitted initially reports false (1)") {
//   TestUtils::AutoTestCleanup ac {};
//   Isolate* i {Isolate::New()};
//   internal::Isolate* ii = CurrentAsInternal();
//
//   V8MONKEY_CHECK(!ii->IsInitted(), "Init false");
//   i->Dispose();
// }
//
//
// V8MONKEY_TEST(IntIsolate036, "IsInitted initially reports false (2)") {
//   TestUtils::AutoTestCleanup ac {};
//   Isolate* i {Isolate::New()};
//   i->Enter();
//   internal::Isolate* ii = CurrentAsInternal();
//
//   V8MONKEY_CHECK(!ii->IsInitted(), "Init false");
// }
//
//
// V8MONKEY_TEST(IntIsolate037, "IsInitted reports true after init") {
//   TestUtils::AutoTestCleanup ac {};
//   Isolate* i {Isolate::New()};
//   i->Enter();
//
//   internal::Isolate* ii = CurrentAsInternal();
//   ii->Init();
//
//   V8MONKEY_CHECK(ii->IsInitted(), "Init true");
// }
//
//
// V8MONKEY_TEST(IntIsolate038, "V8 Init inits currently entered isolate") {
//   TestUtils::AutoTestCleanup ac {};
//   Isolate* i {Isolate::New()};
//   i->Enter();
//   V8::Initialize();
//   internal::Isolate* ii = CurrentAsInternal();
//
//   V8MONKEY_CHECK(ii->IsInitted(), "Init true");
// }
