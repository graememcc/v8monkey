// Thread
#include "platform/platform.h"

// Isolate, V8
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;
using namespace v8::V8Platform;


namespace {
  // Some tests trigger fatal errors: we shall check that the V8 reports death for the current isolate, and that the
  // registered FatalErrorHandler for the isolate is called. Tests should reset errorCaught before triggering error
  // generating code.
  bool errorCaught {false};
  void fatalErrorHandler(const char*, const char*) {
    errorCaught = true;
  }


  // The following function is intended to be executed only from a thread. Assumes arg is an isolate that the main
  // thread has entered, and attempts to dispose of it. Returns the given pointer if it was fatal, and nullptr if
  // not
  extern "C"
  void* CrossThreadBadDispose(void* arg) {
    // Suppress errors first. Note, we must enter an isolate. (Errors are reported to the thread's current isolate
    // rather than the isolate affected. This is consistent with V8).
    Isolate* i {Isolate::New()};
    i->Enter();
    V8::SetFatalErrorHandler(fatalErrorHandler);

    Isolate* mainThreadIsolate {reinterpret_cast<Isolate*>(arg)};
    errorCaught = false;
    mainThreadIsolate->Dispose();

    // Must compute result now: V8::IsDead requires us to be in an isolate
    void* retVal = V8::IsDead() && errorCaught ? arg : nullptr;
    i->Exit();
    i->Dispose();

    return retVal;
  }
}


V8MONKEY_TEST(IntIsolate001, "Attempt to dispose in-use isolate from another thread causes fatal error") {
  // Although we're only using API functions, this is an internal test because of V8Platform visibility
  // TODO If we compile platform with the test harness, we could make this public once again

  Isolate* i {Isolate::New()};
  i->Enter();
  V8Platform::Thread child {CrossThreadBadDispose};
  child.Run(i);

  V8MONKEY_CHECK(child.Join(), "Disposing an in-use isolate is fatal");

  i->Exit();
  i->Dispose();
}


/*
 * Project reset: 16 July
 *
 * Code below this line predates the reset
 *
 */


/*
// JSTraceDataOp
#include "jsapi.h"

// Class under test
#include "runtime/isolate.h"

// TestUtils
#include "utils/test.h"

// DeletionObject DummyV8MonkeyObject V8Value
#include "types/base_types.h"

// GetJSRuntime/GetJSContext, SetGCRegistrationHooks
#include "utils/SpiderMonkeyUtils.h"
// XXX V8 Dispose where necessary


namespace {
*/
  /*
   * Returns a bool denoting whether the given isolate is seen as not locked by the executing thread.
   *
   */

/*
  void* ThreadNotLocked(void* iso) {
    internal::Isolate* i {reinterpret_cast<internal::Isolate*>(iso)};
    return reinterpret_cast<void*>(!i->IsLockedForThisThread());
  }
*/


  /*
   * Returns a bool denoting whether the runtime associated with a thread is initially null.
   *
   */

/*
  void* RuntimeInitiallyNull(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    bool result {SpiderMonkey::GetJSRuntimeForThread() == nullptr};
    return reinterpret_cast<void*>(result);
  }
*/


  /*
   * Returns a bool denoting whether the context associated with a thread is initially null.
   *
   */

/*
  void* ContextInitiallyNull(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    bool result {SpiderMonkey::GetJSContextForThread() == nullptr};
    return reinterpret_cast<void*>(result);
  }
*/


  /*
   * Returns a bool denoting whether the runtime associated with a thread is non-null after entry.
   *
   */

/*
  void* RuntimeNonNullAfterEntry(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {Isolate::New()};
    i->Enter();
    bool result {SpiderMonkey::GetJSRuntimeForThread() != nullptr};
    return reinterpret_cast<void*>(result);
  }
*/


  /*
   * Returns a bool denoting whether the context associated with a thread is non-null after entry.
   *
   */

/*
  void* ContextNonNullAfterEntry(void* = nullptr) {
    TestUtils::AutoIsolateCleanup ac {};

    Isolate* i {Isolate::New()};
    i->Enter();
    bool result {SpiderMonkey::GetJSContextForThread() != nullptr};
    return reinterpret_cast<void*>(result);
  }
*/


  /*
   * Thread function which returns the current JSRuntime cast to void.
   *
   */

/*
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
*/


  /*
   * Thread function which returns the current JSContext cast to void.
   *
   */

/*
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
*/


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, and the child thread it spawns recieved unique
   * JSRuntimes.
   *
   */

/*
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
*/


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, and the child thread it spawns recieved unique
   * JSContexts.
   *
   */

/*
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
*/


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, and the child thread it spawns recieved unique
   * JSRuntimes after entering the same isolate.
   *
   */

/*
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
*/


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, and the child thread it spawns recieved unique
   * JSContexts after entering the same isolate.
   *
   */

/*
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
*/


  /*
   * Returns a void*-cast bool denoting whether the spawned thread, had its runtime registered for GC on entering the
   * isolate pointed to by the given argument.
   *
   */

/*
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
*/


  /*
   * Utility function that simply has a thread enter/exit a supplied isolate, without disposing it.
   *
   */

/*
  void* ThreadEnterExit(void* iso) {
    // Note: can't construct an AutoIsolateCleanup - we don't want to dispose of the isolate

    Isolate* i {reinterpret_cast<Isolate*>(iso)};
    i->Enter();
    i->Exit();

    return nullptr;
  }


  struct IsolateAndAddr {
    Isolate* isolate;
    bool* updateAddress;
  };
*/

  /*
   * Function that creates a TraceFake in the supplied isolate that updates the given address.
   * The point of the function is to prove that the TraceFake is no longer traced after this
   * thread with it's own JSRuntime exits.
   *
   */

/*
   void* MakeTraceFake(void* info) {
     // Note: can't construct an AutoIsolateCleanup - we don't want to dispose of the isolate

     IsolateAndAddr* isolateAndAddr {reinterpret_cast<IsolateAndAddr*>(info)};
     Isolate* apiIsolate {isolateAndAddr->isolate};
     apiIsolate->Enter();
     internal::Isolate* i {internal::Isolate::FromAPIIsolate(apiIsolate)};

     internal::TraceFake* dummy {new internal::TraceFake {isolateAndAddr->updateAddress}};
     i->AddLocalHandle(dummy);
     apiIsolate->Exit();
     return nullptr;
  }
*/

  /*
   * A thread function that should simply exit.
   *
   * This function will be called through a function pointer, so the call shouldn't be optimized out
   *
   */

/*
  void* SimplyExit(void*) {
    return nullptr;
  }
*/


  /*
   * Utility function that simply has a thread create an isolate, and enter/exit it.
   *
   */

/*
  void* CreateEnterExit(void*) {
    Isolate* i {Isolate::New()};
    i->Enter();
    i->Exit();
    i->Dispose();

    return nullptr;
  }
*/


  /*
   * Utility function that simply has a thread enter the supplied isolate, create an object, and exit it.
   *
   */

/*
  void* ThreadEnterCreateExit(void* iso) {
    // Note: cannot create an AutoIsolateCleanup. We don't want to destroy the isolate yet.
    Isolate* i {reinterpret_cast<Isolate*>(iso)};
    i->Enter();
    internal::DummyV8MonkeyObject* dummy {new internal::DummyV8MonkeyObject {}};
    internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(dummy);
    i->Exit();

    return nullptr;
  }
}
*/


/*
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

  Isolate* i {Isolate::New()};
  i->Enter();
  bool wasTraced {false};
  internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(new internal::TraceFake {&wasTraced});
  i->Exit();

  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(wasTraced, "Object traced after exiting isolate");

  i->Dispose();
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


V8MONKEY_TEST(IntIsolate039, "Multiple entries does not result in multiple calls to SpiderMonkey requesting rooting (1)") {
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


V8MONKEY_TEST(IntIsolate042, "GetLocalHandleLimits initially returns a struct with nullptrs") {
  Isolate* i {Isolate::New()};
  internal::LocalHandleLimits limits = internal::Isolate::FromAPIIsolate(i)->GetLocalHandleLimits();

  V8MONKEY_CHECK(!limits.next, "next field was a nullptr");
  V8MONKEY_CHECK(!limits.limit, "next field was a nullptr");

  i->Dispose();
}


V8MONKEY_TEST(IntIsolate043, "LocalHandleCount initially zero") {
  Isolate* i {Isolate::New()};

  V8MONKEY_CHECK(internal::Isolate::FromAPIIsolate(i)->LocalHandleCount() == 0u, "No handles");

  i->Dispose();
}


V8MONKEY_TEST(IntIsolate044, "GetLocalHandleLimits no longer null after adding an object") {
  Isolate* apiIsolate {Isolate::New()};
  internal::Isolate* i {internal::Isolate::FromAPIIsolate(apiIsolate)};

  i->AddLocalHandle(new internal::DummyV8MonkeyObject {});

  internal::LocalHandleLimits limits = i->GetLocalHandleLimits();
  V8MONKEY_CHECK(limits.next, "next field was not nullptr");
  V8MONKEY_CHECK(limits.limit, "next field was not nullptr");

  apiIsolate->Dispose();
}


V8MONKEY_TEST(IntIsolate045, "LocalHandleCount non-zero after handle added") {
  Isolate* apiIsolate {Isolate::New()};
  internal::Isolate* i {internal::Isolate::FromAPIIsolate(apiIsolate)};
  i->AddLocalHandle(new internal::DummyV8MonkeyObject {});

  V8MONKEY_CHECK(i->LocalHandleCount() != 0u, "No handles");

  apiIsolate->Dispose();
}


V8MONKEY_TEST(IntIsolate046, "AddLocalHandle does not return nullptr") {
  Isolate* i {Isolate::New()};

  V8MONKEY_CHECK(internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(new internal::DummyV8MonkeyObject {}),
                 "Address non-null");

  i->Dispose();
}


V8MONKEY_TEST(IntIsolate047, "Objects are traced after added as a local handle") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  bool wasTraced {false};
  internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(new internal::TraceFake {&wasTraced});

  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(wasTraced, "Value was traced");
}


V8MONKEY_TEST(IntIsolate048, "Multiple entries does not result in multiple calls to SpiderMonkey requesting rooting (2)") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  bool wasTraced {false};
  int traceCount {0};
  internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(new internal::TraceFake {&wasTraced, &traceCount});
  i->Exit();
  i->Enter();

  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(traceCount == 1, "Value was only traced once");
}


V8MONKEY_TEST(IntIsolate049, "Values not traced after runtime destroyed") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  bool wasTraced {false};
  IsolateAndAddr* threadInfo {new IsolateAndAddr {i, &wasTraced}};

  Thread child {MakeTraceFake};
  child.Run(threadInfo);
  child.Join();

  delete threadInfo;

  i->Enter();
  // Child thread destruction may have initiated a trace
  wasTraced = false;
  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(!wasTraced, "Value not traced after JSRuntime destroyed");
}


V8MONKEY_TEST(IntIsolate050, "Destruction of threads not associated with isolates doesn't affect an isolate tracing") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  bool wasTraced {false};
  internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(new internal::TraceFake {&wasTraced});

  Thread child {SimplyExit};
  child.Run();
  child.Join();

  // Child thread destruction may have initiated a trace
  wasTraced = false;
  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(wasTraced, "Tracing unaffected by thread destruction");
}


V8MONKEY_TEST(IntIsolate051,
              "Destruction of threads that entered other isolates doesn't affect different isolate's tracing") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  bool wasTraced {false};
  internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(new internal::TraceFake {&wasTraced});

  Thread child {CreateEnterExit};
  child.Run();
  child.Join();

  // Child thread destruction may have initiated a trace
  wasTraced = false;
  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(wasTraced, "Tracing unaffected by thread destruction");
}


V8MONKEY_TEST(IntIsolate052,
              "Destruction of thread that entered our isolates doesn't affect our isolate's tracing (1)") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  bool wasTraced {false};
  internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(new internal::TraceFake {&wasTraced});

  Thread child {ThreadEnterExit};
  child.Run(i);
  child.Join();

  // Child thread destruction may have initiated a trace
  wasTraced = false;
  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(wasTraced, "Tracing unaffected by thread destruction");
}


V8MONKEY_TEST(IntIsolate053,
              "Destruction of thread that entered our isolates doesn't affect our isolate's tracing (2)") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  bool wasTraced {false};
  internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(new internal::TraceFake {&wasTraced});

  Thread child {ThreadEnterCreateExit};
  child.Run(i);
  child.Join();

  // Child thread destruction may have initiated a trace
  wasTraced = false;
  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(wasTraced, "Tracing unaffected by thread destruction");
}


V8MONKEY_TEST(IntIsolate054,
              "Destruction of previously entered isolates doesn't affect tracing of other isolates for same runtime") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  bool wasTraced {false};
  internal::Isolate::FromAPIIsolate(i)->AddLocalHandle(new internal::TraceFake {&wasTraced});

  Isolate* j {Isolate::New()};
  j->Enter();
  j->Exit();
  j->Dispose();

  // Isolate destruction may have initiated a trace
  wasTraced = false;
  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(wasTraced, "Tracing unaffected by thread destruction");
}


V8MONKEY_TEST(IntIsolate055, "IsInitted initially reports false (1)") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  internal::Isolate* ii {internal::Isolate::FromAPIIsolate(i)};

  V8MONKEY_CHECK(!ii->IsInitted(), "Init false");
  i->Dispose();
}


V8MONKEY_TEST(IntIsolate056, "IsInitted initially reports false (2)") {
  TestUtils::AutoTestCleanup ac {};

  // Confirming that entering alone does not trigger init
  Isolate* i {Isolate::New()};
  i->Enter();
  internal::Isolate* ii {internal::Isolate::FromAPIIsolate(i)};

  V8MONKEY_CHECK(!ii->IsInitted(), "Init false");
}


V8MONKEY_TEST(IntIsolate057, "V8 Init inits currently entered isolate") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8::Initialize();

  internal::Isolate* ii {internal::Isolate::FromAPIIsolate(i)};
  V8MONKEY_CHECK(ii->IsInitted(), "Init true");
}


V8MONKEY_TEST(IntIsolate058, "Isolate tracing copes with nullptrs in local handle data") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  internal::Isolate* ii {internal::Isolate::GetCurrent()};

  bool wasTraced {false};
  internal::TraceFake* dummy {new internal::TraceFake {&wasTraced}};
  internal::Object** slot1 {ii->AddLocalHandle(dummy)};
  ii->AddLocalHandle(dummy);
  // Manually delete the first dummy slot. Note: need to handle the refcount
  (*slot1)->Release(slot1);
  *slot1 = nullptr;

  wasTraced = false;
  SpiderMonkey::ForceGC();
  V8MONKEY_CHECK(wasTraced, "Tracing unaffected by nullptr existence");
}


V8MONKEY_TEST(IntIsolate059, "After isolate init, true and false refcounted") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8::Initialize();

  internal::Object** trueSlot {internal::Internals::GetRoot(i, internal::Internals::kTrueValueRootIndex)};
  V8MONKEY_CHECK((*trueSlot)->RefCount() == 1, "True refcounted");

  internal::Object** falseSlot {internal::Internals::GetRoot(i, internal::Internals::kFalseValueRootIndex)};
  V8MONKEY_CHECK((*falseSlot)->RefCount() == 1, "False refcounted");
}


V8MONKEY_TEST(IntIsolate060, "After isolate init, true and false are of correct type") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8::Initialize();

  internal::Object** trueSlot {internal::Internals::GetRoot(i, internal::Internals::kTrueValueRootIndex)};
  internal::V8Value* asValue {dynamic_cast<internal::V8Value*>(*trueSlot)};
  V8MONKEY_CHECK(asValue->IsBoolean(), "True is a boolean");

  internal::Object** falseSlot {internal::Internals::GetRoot(i, internal::Internals::kFalseValueRootIndex)};
  asValue  = dynamic_cast<internal::V8Value*>(*falseSlot);
  V8MONKEY_CHECK(asValue->IsBoolean(), "False is a boolean");
}


V8MONKEY_TEST(IntIsolate061, "After isolate init, true and false have correct values") {
  TestUtils::AutoTestCleanup ac {};

  Isolate* i {Isolate::New()};
  i->Enter();
  V8::Initialize();

  internal::Object** trueSlot {internal::Internals::GetRoot(i, internal::Internals::kTrueValueRootIndex)};
  internal::V8Value* asValue {dynamic_cast<internal::V8Value*>(*trueSlot)};
  V8MONKEY_CHECK(asValue->BooleanValue(), "True is true");

  internal::Object** falseSlot {internal::Internals::GetRoot(i, internal::Internals::kFalseValueRootIndex)};
  asValue  = dynamic_cast<internal::V8Value*>(*falseSlot);
  V8MONKEY_CHECK(!asValue->BooleanValue(), "False is false");
}


V8MONKEY_TEST(IntIsolate062, "LocalHandleCount zeroed after deleting all handles") {
  Isolate* apiIsolate {Isolate::New()};
  internal::Isolate* i {internal::Isolate::FromAPIIsolate(apiIsolate)};

  i->AddLocalHandle(new internal::DummyV8MonkeyObject {});
  V8MONKEY_CHECK(i->LocalHandleCount() > 0u, "Sanity check");

  i->DeleteLocalHandleSlots(nullptr);
  V8MONKEY_CHECK(i->LocalHandleCount() == 0u, "No handles");

  apiIsolate->Dispose();
}


V8MONKEY_TEST(IntIsolate063, "LocalHandleCount correct after deleting some handles") {
  Isolate* apiIsolate {Isolate::New()};
  internal::Isolate* i {internal::Isolate::FromAPIIsolate(apiIsolate)};

  i->AddLocalHandle(new internal::DummyV8MonkeyObject {});
  auto original = i->LocalHandleCount();
  internal::LocalHandleLimits limits = i->GetLocalHandleLimits();

  i->AddLocalHandle(new internal::DummyV8MonkeyObject {});
  V8MONKEY_CHECK(i->LocalHandleCount() > original, "Sanity check");

  i->DeleteLocalHandleSlots(limits.next);
  V8MONKEY_CHECK(i->LocalHandleCount() == original, "Handle count correct");

  apiIsolate->Dispose();
}


V8MONKEY_TEST(IntIsolate064, "GetLocalHandleLimits null after deleting all slots") {
  Isolate* apiIsolate {Isolate::New()};
  internal::Isolate* i {internal::Isolate::FromAPIIsolate(apiIsolate)};

  i->AddLocalHandle(new internal::DummyV8MonkeyObject {});
  internal::LocalHandleLimits limits = i->GetLocalHandleLimits();
  V8MONKEY_CHECK(limits.next, "Sanity check");
  V8MONKEY_CHECK(limits.limit, "Sanity check");

  i->DeleteLocalHandleSlots(nullptr);
  internal::LocalHandleLimits newLimits = i->GetLocalHandleLimits();
  V8MONKEY_CHECK(!newLimits.next, "next field now nullptr");
  V8MONKEY_CHECK(!newLimits.limit, "next field now nullptr");

  apiIsolate->Dispose();
}


V8MONKEY_TEST(IntIsolate065, "GetLocalHandleLimits correctly reverted after deleting some slots") {
  Isolate* apiIsolate {Isolate::New()};
  internal::Isolate* i {internal::Isolate::FromAPIIsolate(apiIsolate)};

  i->AddLocalHandle(new internal::DummyV8MonkeyObject {});
  internal::LocalHandleLimits limits = i->GetLocalHandleLimits();
  V8MONKEY_CHECK(limits.next, "Sanity check");
  V8MONKEY_CHECK(limits.limit, "Sanity check");

  i->AddLocalHandle(new internal::DummyV8MonkeyObject {});
  internal::LocalHandleLimits nextLimits = i->GetLocalHandleLimits();
  V8MONKEY_CHECK(nextLimits.next && nextLimits.next != limits.next, "Sanity check");
  V8MONKEY_CHECK(nextLimits.limit, "Sanity check");

  i->DeleteLocalHandleSlots(limits.next);
  internal::LocalHandleLimits finalLimits = i->GetLocalHandleLimits();
  V8MONKEY_CHECK(finalLimits.next == limits.next, "next field reverted");
  V8MONKEY_CHECK(finalLimits.limit == limits.limit, "limit field reverted");

  apiIsolate->Dispose();
}


V8MONKEY_TEST(IntIsolate066, "Refcount of affected objects dropped after slots deleted (1)") {
  Isolate* apiIsolate {Isolate::New()};
  internal::Isolate* i {internal::Isolate::FromAPIIsolate(apiIsolate)};

  internal::Object* dummy {new internal::DummyV8MonkeyObject {}};
  dummy->AddRef();
  auto refCount = dummy->RefCount();
  i->AddLocalHandle(dummy);
  V8MONKEY_CHECK(dummy->RefCount() == refCount + 1, "Sanity check");

  i->DeleteLocalHandleSlots(nullptr);
  V8MONKEY_CHECK(dummy->RefCount() == refCount, "Refcount dropped");
  dummy->Release(&dummy);

  apiIsolate->Dispose();
}


V8MONKEY_TEST(IntIsolate067, "Refcount of affected objects dropped after slots deleted (2)") {
  Isolate* apiIsolate {Isolate::New()};
  internal::Isolate* i {internal::Isolate::FromAPIIsolate(apiIsolate)};

  bool wasDeleted {false};
  i->AddLocalHandle(new internal::DeletionObject {&wasDeleted});

  i->DeleteLocalHandleSlots(nullptr);
  V8MONKEY_CHECK(wasDeleted, "Refcount dropped");

  apiIsolate->Dispose();
}
*/
