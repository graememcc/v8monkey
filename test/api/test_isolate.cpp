#include "v8.h"
#include "isolate.h"
#include "platform.h"
#include "V8MonkeyTest.h"


using namespace v8;

namespace {
  V8MONKEY_TEST_HELPER(ReturnCurrentIsolate) {
    return Isolate::GetCurrent();
  }


  // Returns a bool (cast to void*) denoting whether the caller remains in an isolate if it is exited fewer
  // times than it is entered
  V8MONKEY_TEST_HELPER(EnterTwiceReturnOnce) {
    Isolate* i = Isolate::New();

    i->Enter();
    i->Enter();
    i->Exit();

    bool result = Isolate::GetCurrent() == i;
    i->Exit();
    i->Dispose();

    return reinterpret_cast<void*>(result);
  }


  // Returns a bool (cast to void*) denoting whether entering an isolate multiple times, and exiting the same number of
  // times completetly exits the isolate
  V8MONKEY_TEST_HELPER(EnterThriceReturnThrice) {
    Isolate* original = Isolate::GetCurrent();
    Isolate* i = Isolate::New();

    i->Enter();
    i->Enter();
    i->Enter();

    i->Exit();
    i->Exit();
    i->Exit();

    bool result = Isolate::GetCurrent() == original;
    i->Dispose();

    return reinterpret_cast<void*>(result);
  }


  // Returns a bool (cast to void*) denoting whether isolates stack: i.e. exiting an isolate should return the
  // thread to the isolate it was previously in
  V8MONKEY_TEST_HELPER(CheckIsolateStacking) {
    Isolate* original = Isolate::GetCurrent();
    Isolate* first = Isolate::New();
    first->Enter();
    Isolate* second = Isolate::New();
    second->Enter();
    Isolate* third = Isolate::New();
    third->Enter();
    bool allDistinct = original != first && original != second && original != third && first != second &&
                       first != third && second != third;
    third->Exit();
    bool returnedToSecond = Isolate::GetCurrent() == second;
    Isolate::GetCurrent()->Exit();
    bool returnedToFirst = Isolate::GetCurrent() == first;
    Isolate::GetCurrent()->Exit();
    bool returnedToOriginal = Isolate::GetCurrent() == original;

    first->Dispose();
    second->Dispose();
    third->Dispose();

    return reinterpret_cast<void*>(allDistinct && returnedToSecond && returnedToFirst && returnedToOriginal);
  }


  // Returns a bool (cast to void*) denoting whether attempts to dispose an in-use isolate prove fatal
  V8MONKEY_TEST_HELPER(CheckBadDisposeIsFatal) {
    Isolate* i = Isolate::New();
    i->Enter();
    i->Dispose();

    bool result = V8::IsDead();
    i->Exit();
    i->Dispose();
    return reinterpret_cast<void*>(result);
  }


  // Assumes arg is a (non-default) isolate that the main thread is currently in, and attempts to dispose of it.
  // Reports back a bool denoting whether this was fatal.
  void* CrossThreadBadDispose(void* arg) {
    Isolate* i = reinterpret_cast<Isolate*>(arg);
    i->Dispose();
    return reinterpret_cast<void*>(V8::IsDead());
  }


  // Returns a bool (cast to void*) denoting whether the current isolate equals the one prior to Scope creation when a
  // single Scope is entered and destroyed
  V8MONKEY_TEST_HELPER(CheckSingleScopeRestoresInitialState) {
    Isolate* initial = Isolate::GetCurrent();
    Isolate* i = Isolate::New();
    {
      Isolate::Scope scope(i);
    }

    return reinterpret_cast<void*>(Isolate::GetCurrent() == initial);
  }


  // Returns a bool (cast to void*) denoting whether GetCurrent is correct after constructing a Scope
  V8MONKEY_TEST_HELPER(GetCurrentCorrectAfterScopeConstruction) {
    Isolate* i = Isolate::New();
    bool result;
    {
      Isolate::Scope scope(i);
      result = Isolate::GetCurrent() == i;
    }
    i->Dispose();

    return reinterpret_cast<void*>(result);
  }


  // Returns a bool (cast to void*) denoting whether scopes stack correctly when the first isolate is explicitly entered
  V8MONKEY_TEST_HELPER(CheckScopesStackAfterExplicitEntry) {
    Isolate* first = Isolate::New();
    first->Enter();

    Isolate* second = Isolate::New();
    {
      Isolate::Scope scope(second);
    }

    bool result = Isolate::GetCurrent() == first;
    first->Exit();
    first->Dispose();

    return reinterpret_cast<void*>(result);
  }


  // Returns a bool (cast to void*) denoting whether scopes stack correctly
  V8MONKEY_TEST_HELPER(CheckScopesStack) {
    Isolate* first = Isolate::New();
    bool firstOK, secondOK;
    {
      Isolate::Scope firstScope(first);
      Isolate* second = Isolate::New();
      {
        Isolate::Scope secondScope(second);
        Isolate* third = Isolate::New();
        {
          Isolate::Scope thirdScope(third);
        }
        secondOK = Isolate::GetCurrent() == second;
        third->Dispose();
      }
      firstOK = Isolate::GetCurrent() == first;
      second->Dispose();
    }
    first->Dispose();

    return reinterpret_cast<void*>(firstOK && secondOK);
  }


  // Returns a bool (cast to void*) denoting whether attempts to dispose from a non-default isolate prove fatal
  V8MONKEY_TEST_HELPER(CheckV8DisposeFromNonDefaultIsFatal) {
    V8::Initialize();
    Isolate* i = Isolate::New();
    i->Enter();

    V8::Dispose();
    bool result = V8::IsDead();
    i->Exit();
    i->Dispose();

    return reinterpret_cast<void*>(result);
  }


  // Returns a bool (cast to void*) denoting whether attempts to dispose from outside any isolate proves fatal
  V8MONKEY_TEST_HELPER(CheckV8DisposeOutsideIsolateIsFatal) {
    V8::Initialize();
    // Initialization implicitly enters the default isolate
    Isolate::GetCurrent()->Exit();

    V8::Dispose();
    bool result = V8::IsDead();

    return reinterpret_cast<void*>(result);
  }
}


V8MONKEY_TEST(Isolate001, "Main thread reports Isolate::GetCurrent non-null even when API not used") {
  Isolate* mainThreadIsolate = static_cast<Isolate*>(ReturnCurrentIsolate());
  V8MONKEY_CHECK(mainThreadIsolate != NULL, "Main thread Isolate::GetCurrent was non-null");
}


V8MONKEY_TEST(Isolate002, "Non-main thread reports Isolate::GetCurrent null when API not used") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(ReturnCurrentIsolate);
  child->Run();
  Isolate* threadIsolate = static_cast<Isolate*>(child->Join());
  V8MONKEY_CHECK(threadIsolate == NULL, "Thread Isolate::GetCurrent was null");
}


V8MONKEY_TEST(Isolate003, "Main thread remains in isolate if exited fewer times than entered") {
  V8MONKEY_CHECK(EnterTwiceReturnOnce(), "Still in isolate after asymmetric exits");
}


V8MONKEY_TEST(Isolate004, "Non-main thread remains in isolate if exited fewer times than entered") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(EnterTwiceReturnOnce);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Still in isolate after asymmetric exits");
}


V8MONKEY_TEST(Isolate005, "Main thread exits to default after sufficient exits from multiply-entered isolate") {
  V8MONKEY_CHECK(EnterThriceReturnThrice(), "Returned to default after symmetric exits");
}


V8MONKEY_TEST(Isolate006, "Non-main thread exits to null after sufficient exits from multiply-entered isolate") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(EnterThriceReturnThrice);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Returned to null after symmetric exits");
}


V8MONKEY_TEST(Isolate007, "Isolate entries stack for main thread") {
  void* result = CheckIsolateStacking();
  V8MONKEY_CHECK(result, "Isolates returned to in correct sequence");
}


V8MONKEY_TEST(Isolate008, "Isolate entries stack for off-main thread") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckIsolateStacking);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Isolates returned to in correct sequence");
}


V8MONKEY_TEST(Isolate009, "Isolate::New returns a fresh isolate") {
  Isolate* defaultIsolate = Isolate::GetCurrent();
  Isolate* i = Isolate::New();
  V8MONKEY_CHECK(defaultIsolate != i, "New returned a fresh isolate");
  i->Dispose();
}


V8MONKEY_TEST(Isolate010, "Dispose refuses to delete default isolate") {
  Isolate* defaultIsolate = Isolate::GetCurrent();
  defaultIsolate->Dispose();

  // If the implementation is broken, we'll dereference a dangling pointer here
  defaultIsolate->Enter();

  // If we're still alive, then things are working correctly
  V8MONKEY_CHECK(true, "Default isolate not deleted");
  defaultIsolate->Exit();
}


V8MONKEY_TEST(Isolate011, "Isolate not deleted on Dispose if still in use") {
  Isolate* i = Isolate::New();
  i->Enter();
  i->Dispose();

  // If the implementation is broken, we'll dereference a dangling pointer here
  i->Exit();

  // If we're still alive, then things are working correctly
  V8MONKEY_CHECK(i, "In-use isolate not deleted");
}


V8MONKEY_TEST(Isolate012, "Attempt to dispose in-use isolate causes fatal error (main)") {
  void* result = CheckBadDisposeIsFatal();
  V8MONKEY_CHECK(result, "Disposing an in-use isolate is fatal");
}


V8MONKEY_TEST(Isolate013, "Attempt to dispose in-use isolate causes fatal error for off-main thread") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckBadDisposeIsFatal);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Disposing an in-use isolate is fatal");
}


V8MONKEY_TEST(Isolate014, "Attempt to dispose in-use isolate from another thread causes fatal error") {
  Isolate* i = Isolate::New();
  i->Enter();
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CrossThreadBadDispose);
  child->Run(i);
  V8MONKEY_CHECK(child->Join(), "Disposing an in-use isolate is fatal");
  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(Scope001, "Creating and destroying a single scope leaves main in its initial state") {
  V8MONKEY_CHECK(CheckSingleScopeRestoresInitialState(), "main thread returned to initial isolate after scope destruction");
}


V8MONKEY_TEST(Scope002, "Creating and destroying a single scope leaves thread in its initial state") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckSingleScopeRestoresInitialState);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "thread returned to initial isolate after scope destruction");
}


V8MONKEY_TEST(Scope003, "GetCurrent() reports correct isolate after Scope construction (within Scope lifetime)") {
  V8MONKEY_CHECK(GetCurrentCorrectAfterScopeConstruction(), "GetCurrent() was correct");
}


V8MONKEY_TEST(Scope004, "GetCurrent() reports correct isolate after Scope construction (within Scope lifetime)") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(GetCurrentCorrectAfterScopeConstruction);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "GetCurrent() was correct");
}


V8MONKEY_TEST(Scope005, "Scopes stack correctly for main with explicitly entered Isolates") {
  V8MONKEY_CHECK(CheckScopesStackAfterExplicitEntry(), "Scope stacked correctly");
}


V8MONKEY_TEST(Scope006, "Scopes stack correctly for thread with explicitly entered Isolates") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckScopesStackAfterExplicitEntry);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Scope stacked correctly");
}


V8MONKEY_TEST(Scope007, "Multiple Scopes stack correctly for main thread") {
  V8MONKEY_CHECK(CheckScopesStack(), "Scopes stacked correctly");
}


V8MONKEY_TEST(Scope008, "Multiple Scopes stack correctly for thread") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckScopesStack);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Scopes stacked correctly");
}


V8MONKEY_TEST(Dispose001, "Attempt to dispose V8 when in non-default isolate (main thread) triggers fatal error") {
  void* result = CheckV8DisposeFromNonDefaultIsFatal();
  V8MONKEY_CHECK(result, "Disposing V8 from a non-default isolate is fatal");
}


V8MONKEY_TEST(Dispose002, "Attempt to dispose V8 when in non-default isolate (thread) triggers fatal error") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckV8DisposeFromNonDefaultIsFatal);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Disposing V8 from a non-default isolate is fatal");
}


V8MONKEY_TEST(Dispose003, "Attempt to dispose V8 outside any isolate (main thread) triggers fatal error") {
  void* result = CheckV8DisposeOutsideIsolateIsFatal();
  V8MONKEY_CHECK(result, "Disposing V8 from outside an isolate is fatal");
}


V8MONKEY_TEST(Dispose004, "Attempt to dispose V8 outside any isolate (thread) triggers fatal error") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckV8DisposeOutsideIsolateIsFatal);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Disposing V8 from outside an isolate is fatal");
}
