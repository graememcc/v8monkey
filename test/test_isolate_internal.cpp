#include "v8.h"
#include "platform.h"
#include "isolate.h"
#include "V8MonkeyTest.h"


using namespace v8;
using V8Monkey::InternalIsolate;


// XXX V8 Dispose where necessary
namespace {
  InternalIsolate* AsInternal(Isolate* i) {
    return reinterpret_cast<InternalIsolate*>(i);
  }


  InternalIsolate* CurrentAsInternal() {
    return AsInternal(Isolate::GetCurrent());
  }


  // Function designed to be run either by main or a thread, which will initialize V8, and return a bool (cast to
  // void* for the threading API) denoting whether it entered the default isolate
  void* InitializingEnteredDefault(void* arg) {
    V8::Initialize();
    return reinterpret_cast<void*>(InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()));
  }


  // Function designed to be run either by main or a thread, which will initialize V8, and return a bool (cast to
  // void* for the threading API) denoting whether it successfully exited the default isolate after V8 Init
  void* CanExitDefaultAfterInit(void* arg) {
    V8::Initialize();
    Isolate::GetCurrent()->Exit();
    return reinterpret_cast<void*>(InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()) == false);
  }


  // Thread function which explicitly enters/exits default isolate, and afterwards returns internal version of current
  // isolate (cast to void* for threading API)
  void* ExplicitEnterAndExitDefault(void* dIso) {
    Isolate* defaultIsolate = reinterpret_cast<Isolate*>(dIso);
    defaultIsolate->Enter();
    defaultIsolate->Exit();
    return CurrentAsInternal();
  }


  // Thread function which implicitly enters the default isolate via an API call, but explicitly exits, and afterwards
  // returns internal version of current isolate (cast to void* for threading API)
  void* ImplicitEnterExplicitExit(void* arg) {
    V8::Initialize();
    Isolate::GetCurrent()->Exit();
    return CurrentAsInternal();
  }


  // Return a bool (cast to void* for threading API) denoting whether V8 Initialization left the entered isolate
  // unchanged when the entered isolate is not the default
  void* InitAfterEnterStaysInIsolate(void* arg) {
    Isolate* i = Isolate::New();
    i->Enter();
    V8::Initialize();
    bool result = Isolate::GetCurrent() == i && InternalIsolate::IsEntered(CurrentAsInternal());
    i->Exit();
    return reinterpret_cast<void*>(result);
  }


  // Returns a bool (cast to void*) denoting whether scopes enter the given isolate
  void* CheckScopesEnter(void* arg) {
    Isolate* i = Isolate::New();
    bool result;
    {
      Isolate::Scope scope(i);
      result = InternalIsolate::IsEntered(AsInternal(i));
    }
    return reinterpret_cast<void*>(result);
  }


  // Returns a bool (cast to void*) denoting whether scopes can enter the default isolate
  void* CheckScopesEnterDefault(void* i) {
    Isolate* defaultIsolate = reinterpret_cast<Isolate*>(i);
    bool result;
    {
      Isolate::Scope scope(defaultIsolate);
      result = InternalIsolate::IsEntered(AsInternal(defaultIsolate));
    }
    return reinterpret_cast<void*>(result);
  }
}


V8MONKEY_TEST(IntIsolate001, "Default InternalIsolate is non-null") {
  V8MONKEY_CHECK(!InternalIsolate::IsDefaultIsolate(NULL), "IsDefaultIsolate returns false for NULL");
}


V8MONKEY_TEST(IntIsolate002, "GetDefaultIsolate/IsDefaultIsolate works correctly") {
  V8MONKEY_CHECK(InternalIsolate::IsDefaultIsolate(InternalIsolate::GetDefaultIsolate()),
                 "IsDefaultIsolate and GetDefaultIsolate agree");
}


V8MONKEY_TEST(IntIsolate003, "Isolate::GetCurrent is default for main thread with no API activity") {
  // Note: we test in test_isolate that GetCurrent() is initially null for non-main thread
  V8MONKEY_CHECK(InternalIsolate::IsDefaultIsolate(CurrentAsInternal()),
                 "IsDefaultIsolate returns true for main thread's first isolate");
}


V8MONKEY_TEST(IntIsolate004, "IsEntered works correctly") {
  Isolate* mainThreadIsolate = Isolate::GetCurrent();
  // XXX Confirm Isolates can be entered without V8 Init
  mainThreadIsolate->Enter();
  V8MONKEY_CHECK(InternalIsolate::IsEntered(AsInternal(mainThreadIsolate)),
                 "IsEntered reports true for entered isolate");
  mainThreadIsolate->Exit();
  V8MONKEY_CHECK(!InternalIsolate::IsEntered(AsInternal(mainThreadIsolate)),
                 "IsEntered reports false for exited isolate");
}


V8MONKEY_TEST(IntIsolate005, "Default isolate initially not entered by main thread") {
  V8MONKEY_CHECK(!InternalIsolate::IsEntered(CurrentAsInternal()), "IsEntered reports false for default isolate");
}


V8MONKEY_TEST(IntIsolate006, "IsDefaultIsolate works correctly for non-default isolate") {
  V8MONKEY_CHECK(InternalIsolate::IsDefaultIsolate(AsInternal(Isolate::New())),
                 "IsDefaultIsolate reports false for non-default");
}


V8MONKEY_TEST(IntIsolate007, "Initializing V8 implicitly enters default isolate (main thread)") {
  V8MONKEY_CHECK(InitializingEnteredDefault(NULL), "Default isolate was entered on main thread as consequence of V8 init");
}


V8MONKEY_TEST(IntIsolate008, "Initializing V8 implicitly enters default isolate (non-main thread)") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(InitializingEnteredDefault);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Default isolate was entered on thread as consequence of V8 init");
}


V8MONKEY_TEST(IntIsolate009, "Main thread can exit default isolate after initialization implicitly enters") {
  V8MONKEY_CHECK(CanExitDefaultAfterInit(NULL), "Default isolate was exited successfully from main thread after consequence of V8 init");
}


V8MONKEY_TEST(IntIsolate010, "Initializing V8 implicitly enters default isolate (non-main thread)") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CanExitDefaultAfterInit);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Default isolate was exited successfully from thread after consequence of V8 init");
}


V8MONKEY_TEST(IntIsolate011, "Isolate::GetCurrent() still reports default for main thread after explicit entry/exit") {
  Isolate* defaultIsolate = Isolate::GetCurrent();
  defaultIsolate->Enter();
  defaultIsolate->Exit();
  V8MONKEY_CHECK(InternalIsolate::IsDefaultIsolate(CurrentAsInternal()), "GetCurrent() still reports default");
}


V8MONKEY_TEST(IntIsolate012, "Isolate::GetCurrent() still reports default for main thread after implicit entry / explicit exit") {
  V8::Initialize();
  Isolate* defaultIsolate = Isolate::GetCurrent();
  defaultIsolate->Exit();
  V8MONKEY_CHECK(InternalIsolate::IsDefaultIsolate(CurrentAsInternal()), "GetCurrent() still reports default");
}


V8MONKEY_TEST(IntIsolate013, "Isolate::GetCurrent() doesn't report default for thread after explicit default entry/exit") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(ExplicitEnterAndExitDefault);
  child->Run(Isolate::GetCurrent());
  InternalIsolate* threadIsolate = reinterpret_cast<InternalIsolate*>(child->Join());
  V8MONKEY_CHECK(!InternalIsolate::IsDefaultIsolate(threadIsolate), "GetCurrent() no longer reports default");
}


V8MONKEY_TEST(IntIsolate014, "Isolate::GetCurrent() doesn't report default for thread after implicit default entry / explicit exit") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(ImplicitEnterExplicitExit);
  child->Run();
  InternalIsolate* threadIsolate = reinterpret_cast<InternalIsolate*>(child->Join());
  V8MONKEY_CHECK(!InternalIsolate::IsDefaultIsolate(threadIsolate), "GetCurrent() no longer reports default");
}


V8MONKEY_TEST(IntIsolate015, "V8 Initialization doesn't change entered isolate for main thread if entered isolate isn't default") {
  V8MONKEY_CHECK(InitAfterEnterStaysInIsolate(NULL), "Entered isolate didn't change across V8 initialization");
}


V8MONKEY_TEST(IntIsolate016, "V8 Initialization doesn't change entered isolate for main thread if entered isolate isn't default") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(InitAfterEnterStaysInIsolate);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Entered isolate didn't change across V8 initialization");
}


V8MONKEY_TEST(IntScope001, "Scopes enter isolates on main thread") {
  V8MONKEY_CHECK(CheckScopesEnter(NULL), "Isolate was entered");
}


V8MONKEY_TEST(IntScope002, "Scopes enter isolates on thread") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckScopesEnter);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Isolate was entered");
}


V8MONKEY_TEST(IntScope003, "Scopes can enter default isolate on main thread") {
  Isolate* defaultIsolate = Isolate::GetCurrent();
  V8MONKEY_CHECK(CheckScopesEnterDefault(defaultIsolate), "Default isolate was entered");
}


V8MONKEY_TEST(IntScope004, "Scopes can enter default isolate on thread") {
  Isolate* defaultIsolate = Isolate::GetCurrent();
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckScopesEnterDefault);
  child->Run(defaultIsolate);
  V8MONKEY_CHECK(child->Join(), "Default isolate was entered");
}
