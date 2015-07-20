// Thread
#include "platform/platform.h"

// Unit-testing support
#include "V8MonkeyTest.h"

// Isolate
#include "v8.h"


using namespace v8;


/*
 * The structure of these tests is a remnant of the removed multi-threading support. The test logic exists in a
 * seperate function from the test registration macro, to allow the same test to be ran on another thread. This
 * structure has been left in place in the hope that multi-threading will be re-enabled.
 *
 */

namespace {
  // Some tests trigger fatal errors: we shall check that the V8 reports death for the current isolate, and that the
  // registered FatalErrorHandler for the isolate is called. Tests should reset errorCaught before triggering error
  // generating code.
  bool errorCaught {false};
  void fatalErrorHandler(const char*, const char*) {
    errorCaught = true;
  }


  // At other times, we simply need to suppress the default error handler
  void dummyFatalErrorHandler(const char*, const char*) { return; }


  extern "C"
  void* ReturnCurrentIsolate(void* = nullptr) {
    return reinterpret_cast<void*>(Isolate::GetCurrent());
  }


  // Creates two isolates, and stores a bool in the supplied location denoting whether they were different
  extern "C"
  void* NewReturnsFreshIsolates(void* boolPtr) {
    Isolate* i {Isolate::New()};
    Isolate* j {Isolate::New()};

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = i != j;

    j->Dispose();
    i->Dispose();
    return nullptr;
  }


  // Stores a bool in the given location denoting whether GetCurrent changes upon isolate entry
  extern "C"
  void* GetCurrentCorrectAfterEntry(void* boolPtr) {
    Isolate* i {Isolate::New()};
    i->Enter();

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = Isolate::GetCurrent() == i;

    i->Exit();
    i->Dispose();
    return nullptr;
  }


  // Stores a bool in the given location denoting whether GetCurrent changes upon isolate exit
  extern "C"
  void* IsolateCorrectAfterSingleExit(void* boolPtr) {
    Isolate* i {Isolate::GetCurrent()};
    Isolate* j {Isolate::New()};
    j->Enter();
    j->Exit();
    j->Dispose();


    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = Isolate::GetCurrent() == i;
    return nullptr;
  }


  // Stores a bool in the given location denoting whether the isolate is correct after a symmetric number of entries and
  // exits to the same isolate
  extern "C"
  void* MultipleIsolateEntriesHandled(void* boolPtr) {
    Isolate* original {Isolate::GetCurrent()};
    Isolate* i {Isolate::New()};

    i->Enter();
    i->Enter();
    i->Enter();

    i->Exit();
    i->Exit();
    i->Exit();

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = Isolate::GetCurrent() == original;

    i->Dispose();
    return nullptr;
  }


  // Stores a bool in the given location denoting whether the caller remains in an isolate when it is exited fewer
  // times than it is entered
  extern "C"
  void* AsymmetricEntriesAndExitsHandled(void* boolPtr) {
    Isolate* i {Isolate::New()};

    i->Enter();
    i->Enter();
    i->Exit();

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = Isolate::GetCurrent() == i;

    i->Exit();
    i->Dispose();
    return nullptr;
  }


  // Stores a bool in the given location denoting whether isolates stack: i.e. exiting an isolate should return the
  // thread to the isolate it was previously in
  extern "C"
  void* CheckIsolateStacking(void* boolPtr) {
    Isolate* original {Isolate::GetCurrent()};
    Isolate* first {Isolate::New()};

    first->Enter();
    Isolate* second {Isolate::New()};
    second->Enter();
    Isolate* third {Isolate::New()};
    third->Enter();

    bool allDistinct {original != first && original != second && original != third && first != second &&
                      first != third && second != third};

    third->Exit();
    bool returnedToSecond {Isolate::GetCurrent() == second};

    Isolate::GetCurrent()->Exit();
    bool returnedToFirst {Isolate::GetCurrent() == first};

    Isolate::GetCurrent()->Exit();
    bool returnedToOriginal {Isolate::GetCurrent() == original};

    first->Dispose();
    second->Dispose();
    third->Dispose();

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = allDistinct && returnedToSecond && returnedToFirst && returnedToOriginal;
    return nullptr;
  }


  // Stores a bool in the given location denoting whether attempts to dispose an isolate whilst inside it is fatal
  extern "C"
  void* CheckBadDisposeIsFatal(void* boolPtr) {
    Isolate* i {Isolate::New()};
    i->Enter();
    errorCaught = false;
    V8::SetFatalErrorHandler(fatalErrorHandler);
    i->Dispose();

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = V8::IsDead() && errorCaught;

    i->Exit();
    i->Dispose();
    return nullptr;
  }


  // Stores a bool in the given location denoting whether a failed Dispose call leaves a dangling pointer
  extern "C"
  void* CheckFailedDisposeDoesntDelete(void* boolPtr) {
    Isolate* i {Isolate::New()};
    i->Enter();
    V8::SetFatalErrorHandler(dummyFatalErrorHandler);
    i->Dispose();

    // If the implementation is broken, we'll dereference a dangling pointer here
    i->Exit();

    i->Dispose();
    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = true;
    return nullptr;
  }


  // Stores a bool in the given location denoting whether the current isolate equals the one prior to Scope creation
  // when a single Scope is entered and destroyed
  extern "C"
  void* CheckSingleScopeRestoresInitialState(void* boolPtr) {
    Isolate* initial {Isolate::GetCurrent()};
    Isolate* i {Isolate::New()};
    {
      Isolate::Scope scope {i};
    }

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = Isolate::GetCurrent() == initial;

    i->Dispose();
    return nullptr;
  }


  // Stores a bool in the given location denoting whether GetCurrent is correct after constructing a Scope
  extern "C"
  void* GetCurrentCorrectAfterScopeConstruction(void* boolPtr) {
    Isolate* i {Isolate::New()};
    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    {
      Isolate::Scope scope {i};
      *ptr = Isolate::GetCurrent() == i;
    }

    i->Dispose();
    return nullptr;
  }


  // Stores a bool in the given location denoting whether scopes stack correctly when the first isolate is explicitly
  // entered
  extern "C"
  void* CheckScopesStackAfterExplicitEntry(void* boolPtr) {
    Isolate* first {Isolate::New()};
    first->Enter();

    Isolate* second {Isolate::New()};
    {
      Isolate::Scope scope {second};
    }

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = Isolate::GetCurrent() == first;

    second->Dispose();
    first->Exit();
    first->Dispose();

    return nullptr;
  }


  // Stores a bool in the given location denoting whether scopes stack correctly
  extern "C"
  void* CheckScopesStack(void* boolPtr) {
    Isolate* first {Isolate::New()};
    bool firstOK {}, secondOK {};
    {
      Isolate::Scope firstScope {first};
      Isolate* second {Isolate::New()};
      {
        Isolate::Scope secondScope {second};
        Isolate* third {Isolate::New()};
        {
          Isolate::Scope thirdScope {third};
        }
        secondOK = Isolate::GetCurrent() == second;
        third->Dispose();
      }
      firstOK = Isolate::GetCurrent() == first;
      second->Dispose();
    }
    first->Dispose();

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = firstOK && secondOK;
    return nullptr;
  }


  // Stores a bool in the given location denoting whether scopes cope with existing entries correctly
  extern "C"
  void* CheckScopesCopeWithMultipleEntries(void* boolPtr) {
    Isolate* i {Isolate::New()};
    i->Enter();

    {
      Isolate::Scope scope(i);
    }

    bool* ptr {reinterpret_cast<bool*>(boolPtr)};
    *ptr = Isolate::GetCurrent() == i;
    i->Exit();
    i->Dispose();
  }
}


V8MONKEY_TEST(Isolate001, "Isolate::GetCurrent initially null (main thread)") {
  V8MONKEY_CHECK(!ReturnCurrentIsolate(), "Isolate::GetCurrent returned nullptr");
}


V8MONKEY_TEST(Isolate002, "Each invocation of Isolate::New returns a new isolate (main thread)") {
  bool result;
  NewReturnsFreshIsolates(&result);
  V8MONKEY_CHECK(result, "Isolate::New returned distinct isolates");
}


V8MONKEY_TEST(Isolate003, "Isolate::GetCurrent reports correct value after entry (main thread)") {
  bool result;
  GetCurrentCorrectAfterEntry(&result);
  V8MONKEY_CHECK(result, "Isolate::GetCurrent returned correct isolate");
}


V8MONKEY_TEST(Isolate004, "Isolate::GetCurrent reports correct value after exit (main thread)") {
  bool result;
  IsolateCorrectAfterSingleExit(&result);
  V8MONKEY_CHECK(result, "Isolate::GetCurrent returned correct isolate");
}


V8MONKEY_TEST(Isolate005, "Isolate::GetCurrent reports correct value after multiple entries and exits (main thread)") {
  bool result;
  MultipleIsolateEntriesHandled(&result);
  V8MONKEY_CHECK(result, "Isolate::GetCurrent returned correct isolate at final exit");
}


V8MONKEY_TEST(Isolate006, "Isolate::GetCurrent reports correct value after asymmetric entries and exits (main thread)") {
  bool result;
  AsymmetricEntriesAndExitsHandled(&result);
  V8MONKEY_CHECK(result, "Isolate::GetCurrent returned correct isolate at penultimate exit");
}


V8MONKEY_TEST(Isolate007, "Isolates stack correctly (main thread)") {
  bool result;
  CheckIsolateStacking(&result);
  V8MONKEY_CHECK(result, "Isolates stack correctly");
}


V8MONKEY_TEST(Isolate008, "Calling Dispose whilst within an isolate triggers an error (main thread)") {
  bool result;
  CheckBadDisposeIsFatal(&result);
  V8MONKEY_CHECK(result, "Dispose from within an isolate is fatal");
}


V8MONKEY_TEST(Isolate009, "Calling Dispose whilst within an isolate doesn't delete isolate (main thread)") {
  bool result;
  CheckFailedDisposeDoesntDelete(&result);
  V8MONKEY_CHECK(result, "Isolate not deleted if dispose failed");
}


V8MONKEY_TEST(Isolate010, "Embedder data is initially null") {
  Isolate* i {Isolate::New()};
  void* result {i->GetData(0)};

  V8MONKEY_CHECK(!result, "Data was null");
  i->Dispose();
}


V8MONKEY_TEST(Isolate011, "Setting/getting data works as expected") {
  Isolate* i {Isolate::New()};
  i->SetData(0, i);

  V8MONKEY_CHECK(i->GetData(0) == i, "Data was correct");
  i->Dispose();
}


V8MONKEY_TEST(Isolate012, "Embedder data is slot specific") {
  Isolate* i {Isolate::New()};
  i->SetData(0, i);

  V8MONKEY_CHECK(i->GetData(0) == i && !i->GetData(1), "Data was correct");

  i->Dispose();
}


V8MONKEY_TEST(Isolate013, "Embedder data is isolate specific") {
  Isolate* i {Isolate::New()};
  i->SetData(0, i);
  Isolate* j {Isolate::New()};

  V8MONKEY_CHECK(i->GetData(0) == i && j->GetData(0) == nullptr, "Data was correct");

  j->Dispose();
  i->Dispose();
}


V8MONKEY_TEST(Isolate014, "Slots don't overlap") {
  static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unexpected pointer size breaks test assumptions");

  Isolate* i {Isolate::New()};

  if (sizeof(void*) == 8) {
    void* slot1 {reinterpret_cast<void*>(0x0f0f0f0f0f0f0f0fu)};
    void* slot2 {reinterpret_cast<void*>(0xf0f0f0f0f0f0f0f0u)};
    i->SetData(0, slot1);
    i->SetData(1, slot2);

    V8MONKEY_CHECK(i->GetData(0) == slot1 && i->GetData(1) == slot2, "Data was correct");
  } else if (sizeof(void*) == 4) {
    void* slot1 {reinterpret_cast<void*>(0x0f0f0f0fu)};
    void* slot2 {reinterpret_cast<void*>(0xf0f0f0f0u)};
    i->SetData(0, slot1);
    i->SetData(1, slot2);

    V8MONKEY_CHECK(i->GetData(0) == slot1 && i->GetData(1) == slot2, "Data was correct");
  }

  i->Dispose();
}


V8MONKEY_TEST(Scope001, "Creating and destroying a single scope leaves main in its initial state") {
  bool result;
  CheckSingleScopeRestoresInitialState(&result);
  V8MONKEY_CHECK(result, "main thread returned to initial isolate after scope destruction");
}


V8MONKEY_TEST(Scope002, "GetCurrent() reports correct isolate after Scope construction (within Scope lifetime)") {
  bool result;
  GetCurrentCorrectAfterScopeConstruction(&result);
  V8MONKEY_CHECK(result, "GetCurrent() was correct");
}


V8MONKEY_TEST(Scope003, "Scopes stack correctly for main with explicitly entered Isolates") {
  bool result;
  CheckScopesStackAfterExplicitEntry(&result);
  V8MONKEY_CHECK(result, "Scope stacked correctly");
}


V8MONKEY_TEST(Scope004, "Multiple Scopes stack correctly for main thread") {
  bool result;
  CheckScopesStack(&result);
  V8MONKEY_CHECK(result, "Scopes stacked correctly");
}


V8MONKEY_TEST(Scope005, "Scopes exit copes with multiple entries (main thread)") {
  bool result;
  CheckScopesCopeWithMultipleEntries(&result);
  V8MONKEY_CHECK(result, "Scopes cope with multiple entries correctly");
}


/*
 * Tests disabled until threading support reinstated
 *
 *

using namespace v8::V8Platform;


V8MONKEY_TEST(IsolateXXX, "Isolate::GetCurrent initially null (child thread)") {
  Thread t {ReturnCurrentIsolate};
  t.Run();
  void* reportedCurrentIsolate {t.Join()};
  V8MONKEY_CHECK(!reportedCurrentIsolate, "Isolate::GetCurrent returned nullptr");
}


V8MONKEY_TEST(IsolateXXX, "Each invocation of Isolate::New returns a new isolate (child thread)") {
  bool result;
  Thread t {NewReturnsFreshIsolates};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Isolate::New returned distinct isolates");
}


V8MONKEY_TEST(IsolateXXX, "Isolate::GetCurrent reports correct value after entry (child thread)") {
  bool result;
  Thread t {GetCurrentCorrectAfterEntry};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Isolate::GetCurrent returned correct isolate");
}


V8MONKEY_TEST(IsolateXXX, "Isolate::GetCurrent reports correct value after entry (child thread)") {
  bool result;
  Thread t {IsolateCorrectAfterSingleExit};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Isolate::GetCurrent returned correct isolate");
}


V8MONKEY_TEST(IsolateXXX, "Isolate::GetCurrent reports correct value after multiple entries and exits (child thread)") {
  bool result;
  Thread t {MultipleIsolateEntriesHandled};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Isolate::GetCurrent returned correct isolate at final exit");
}


V8MONKEY_TEST(IsolateXXX, "Isolate::GetCurrent reports correct value after asymmetric entries and exits (child thread)") {
  bool result;
  thread t {AsymmetricEntriesAndExitsHandled};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Isolate::GetCurrent returned correct isolate at penultimate exit");
}


V8MONKEY_TEST(IsolateXXX, "Isolates stack correctly (child thread)") {
  bool result;
  thread t {CheckIsolateStacking};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Isolates stack correctly");
}


V8MONKEY_TEST(IsolateXXX, "Calling Dispose whilst within an isolate triggers an error (main thread)") {
  bool result;
  thread t {CheckBadDisposeIsFatal};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Dispose from within an isolate is fatal");
}


V8MONKEY_TEST(IsolateXXX, "Calling Dispose whilst within an isolate doesn't delete isolate (main thread)") {
  bool result;
  thread t {CheckFailedDisposeDoesntDelete};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Isolate not deleted if dispose failed");
}


V8MONKEY_TEST(ScopeXXX, "Creating and destroying a single scope leaves main in its initial state (child thread)") {
  bool result;
  thread t {CheckSingleScopeRestoresInitialState};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "main thread returned to initial isolate after scope destruction");
}


V8MONKEY_TEST(ScopeXXX, "GetCurrent() reports correct isolate after Scope construction (within Scope lifetime: child thread") {
  bool result;
  thread t {GetCurrentCorrectAfterScopeConstruction};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "GetCurrent() was correct (child thread");
}


V8MONKEY_TEST(ScopeXXX, "Scopes stack correctly for main with explicitly entered Isolates (child thread") {
  bool result;
  thread t {CheckScopesStackAfterExplicitEntry};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Scope stacked correctly (child thread");
}


V8MONKEY_TEST(ScopeXXX, "Multiple Scopes stack correctly for main thread (child thread") {
  bool result;
  thread t {CheckScopesStack};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Scopes stacked correctly");
}


V8MONKEY_TEST(ScopeXXX, "Scopes exit copes with multiple entries (child thread)") {
  bool result;
  thread t {CheckScopesCopeWithMultipleEntries};
  t.Run(&result);
  t.Join();
  V8MONKEY_CHECK(result, "Scopes cope with multiple entries correctly");
}
*/
