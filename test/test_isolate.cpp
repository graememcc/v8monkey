#include "v8.h"
#include "isolate.h"
#include "platform.h"
#include "V8MonkeyTest.h"


using namespace v8;

namespace {
  void* ReturnCurrentIsolate(void* arg) {
    return Isolate::GetCurrent();
  }


  void* EnterTwiceReturnOnce(void* arg) {
    Isolate* i = static_cast<Isolate*>(arg);
    i->Enter();
    i->Enter();
    i->Exit();
    return Isolate::GetCurrent();
  }


  void* EnterThriceReturnThrice(void* arg) {
    Isolate* i = static_cast<Isolate*>(arg);

    i->Enter();
    i->Enter();
    i->Enter();

    i->Exit();
    i->Exit();
    i->Exit();

    return Isolate::GetCurrent();
  }


  void* CheckIsolateStacking(void* arg) {
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

    return reinterpret_cast<void*>(allDistinct && returnedToSecond && returnedToFirst && returnedToOriginal);
  }


  // Returns a bool (cast to void*) denoting whether the current isolate equals the one prior to Scope creation when a
  // single Scope is entered and destroyed
  void* CheckSingleScopeRestoresInitialState(void* arg) {
    Isolate* initial = Isolate::GetCurrent();
    {
      Isolate::Scope scope(initial);
    }

    return reinterpret_cast<void*>(Isolate::GetCurrent() == initial);
  }


  // Returns a bool (cast to void*) denoting whether GetCurrent is correct after constructing a Scope
  void* GetCurrentCorrectAfterScopeConstruction(void* arg) {
    Isolate* i = Isolate::New();
    bool result;
    {
      Isolate::Scope scope(i);
      result = Isolate::GetCurrent() == i;
    }

    return reinterpret_cast<void*>(result);
  }


  // Returns a bool (cast to void*) denoting whether scopes stack correctly when the first isolate is explicitly entered
  void* CheckScopesStackAfterExplicitEntry(void* arg) {
    Isolate* first = Isolate::New();
    first->Enter();
    Isolate* second = Isolate::New();
    {
      Isolate::Scope scope(second);
    }
    bool result = Isolate::GetCurrent() == first;
    first->Exit();
    return reinterpret_cast<void*>(result);
  }


  // Returns a bool (cast to void*) denoting whether scopes stack correctly
  void* CheckScopesStack(void* arg) {
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
      }
      firstOK = Isolate::GetCurrent() == first;
    }

    return reinterpret_cast<void*>(firstOK && secondOK);
  }
}


V8MONKEY_TEST(Isolate001, "Main thread reports Isolate::GetCurrent non-null even when API not used") {
  Isolate* mainThreadIsolate = static_cast<Isolate*>(ReturnCurrentIsolate(NULL));
  V8MONKEY_CHECK(mainThreadIsolate != NULL, "Main thread Isolate::GetCurrent was non-null");
}


V8MONKEY_TEST(Isolate002, "Non-main thread reports Isolate::GetCurrent null when API not used") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(ReturnCurrentIsolate);
  child->Run();
  Isolate* threadIsolate = static_cast<Isolate*>(child->Join());
  V8MONKEY_CHECK(threadIsolate == NULL, "Thread Isolate::GetCurrent was null");
}


V8MONKEY_TEST(Isolate003, "Main thread remains in isolate if exited fewer times than entered") {
  Isolate* i = Isolate::New();

  // Sanity check: this should be a fresh isolate
  V8MONKEY_CHECK(i != Isolate::GetCurrent(), "Sanity check: we got a new isolate");
  
  Isolate* extant = static_cast<Isolate*>(EnterTwiceReturnOnce(i));
  V8MONKEY_CHECK(i == extant, "Still in isolate after asymmetric exits");
  i->Exit();
}


V8MONKEY_TEST(Isolate004, "Non-main thread remains in isolate if exited fewer times than entered") {
  Isolate* i = Isolate::New();

  V8Platform::Thread* child = V8Platform::Platform::CreateThread(EnterTwiceReturnOnce);
  child->Run(i);
  Isolate* extant = static_cast<Isolate*>(EnterTwiceReturnOnce(i));
  V8MONKEY_CHECK(i == extant, "Still in isolate after asymmetric exits");
}


V8MONKEY_TEST(Isolate005, "Main thread exits to default after sufficient exits from multiply-entered isolate") {
  Isolate* defaultIsolate = Isolate::GetCurrent();
  Isolate* i = Isolate::New();

  // Sanity check: this should be a fresh isolate
  V8MONKEY_CHECK(i != defaultIsolate, "Sanity check: we got a new isolate");
  
  Isolate* extant = static_cast<Isolate*>(EnterThriceReturnThrice(i));
  V8MONKEY_CHECK(defaultIsolate == extant, "Returned to default after symmetric exits");
}


V8MONKEY_TEST(Isolate006, "Non-main thread exits to null after sufficient exits from multiply-entered isolate") {
  Isolate* current = Isolate::GetCurrent();
  Isolate* i = Isolate::New();

  V8Platform::Thread* child = V8Platform::Platform::CreateThread(EnterThriceReturnThrice);
  child->Run(i);
  Isolate* extant = static_cast<Isolate*>(child->Join());
  V8MONKEY_CHECK(current == extant, "Returned to null after symmetric exits");
}


V8MONKEY_TEST(Isolate007, "Isolate entries stack for main thread") {
  void* result = CheckIsolateStacking(NULL);
  V8MONKEY_CHECK(result, "Isolates returned to in correct sequence");
}


V8MONKEY_TEST(Isolate008, "Isolate entries stack for off-main thread") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckIsolateStacking);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Isolates returned to in correct sequence");
}


V8MONKEY_TEST(Scope001, "Creating and destroying a single scope leaves main in its initial state") {
  V8MONKEY_CHECK(CheckSingleScopeRestoresInitialState(NULL), "main thread returned to initial isolate after scope destruction");
}


V8MONKEY_TEST(Scope002, "Creating and destroying a single scope leaves thread in its initial state") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckSingleScopeRestoresInitialState);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "thread returned to initial isolate after scope destruction");
}


V8MONKEY_TEST(Scope003, "GetCurrent() reports correct isolate after Scope construction (within Scope lifetime)") {
  V8MONKEY_CHECK(GetCurrentCorrectAfterScopeConstruction(NULL), "GetCurrent() was correct");
}


V8MONKEY_TEST(Scope004, "GetCurrent() reports correct isolate after Scope construction (within Scope lifetime)") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(GetCurrentCorrectAfterScopeConstruction);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "GetCurrent() was correct");
}


V8MONKEY_TEST(Scope005, "Scopes stack correctly for main with explicitly entered Isolates") {
  V8MONKEY_CHECK(CheckScopesStackAfterExplicitEntry(NULL), "Scope stacked correctly");
}


V8MONKEY_TEST(Scope006, "Scopes stack correctly for thread with explicitly entered Isolates") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckScopesStackAfterExplicitEntry);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Scope stacked correctly");
}


V8MONKEY_TEST(Scope007, "Multiple Scopes stack correctly for main thread") {
  V8MONKEY_CHECK(CheckScopesStack(NULL), "Scopes stacked correctly");
}


V8MONKEY_TEST(Scope008, "Multiple Scopes stack correctly for thread") {
  V8Platform::Thread* child = V8Platform::Platform::CreateThread(CheckScopesStack);
  child->Run();
  V8MONKEY_CHECK(child->Join(), "Scopes stacked correctly");
}
