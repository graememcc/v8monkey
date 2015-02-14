#include "platform.h"

// Isolate, V8::{Initialize, SetFatalErrorHandler}
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;


/*
 * The structure of these tests is a remnant of the removed multi-threading support. The test logic exists in a
 * seperate function from the test registration macro, to allow the same test to be ran on another thread. This
 * structure has been left in place in the hope that multi-threading will be re-enabled.
 *
 */

namespace {
  // Some tests trigger fatal errors: from the API point of view we can only check that V8::IsDead reports true,
  // however that doesn't distinguish between successful V8 disposals and errors, so we install the following fake
  // error handler. Tests should reset errorCaught before triggering error generating code.
  auto errorCaught = bool {false};
  void fatalErrorHandler(const char*, const char*) {
    errorCaught = true;
  }


  // At other times, we simply need to suppress the default error handler
  void dummyFatalErrorHandler(const char*, const char*) { return; }


  void* ReturnCurrentIsolate(void* = nullptr) {
    return Isolate::GetCurrent();
  }


  // Returns a void* wrapped bool denoting whether each invocation of Isolate::New returns a fresh isolate
  void* NewReturnsFreshIsolates(void* = nullptr) {
    Isolate* i {Isolate::New()};
    Isolate* j {Isolate::New()};

    void* result {reinterpret_cast<void*>(i != j)};

    j->Dispose();
    i->Dispose();
    return result;
  }


  // Returns a void* wrapped bool denoting whether GetCurrent changes upon isolate entry
  void* GetCurrentCorrectAfterEntry(void* = nullptr) {
    Isolate* i {Isolate::New()};
    i->Enter();

    void* result {reinterpret_cast<void*>(Isolate::GetCurrent() == i)};

    i->Exit();
    i->Dispose();
    return result;
  }


  // Returns a void* wrapped bool denoting whether GetCurrent changes upon isolate exit
  void* IsolateCorrectAfterSingleExit(void* = nullptr) {
    Isolate* i {Isolate::GetCurrent()};
    Isolate* j {Isolate::New()};
    j->Enter();
    j->Exit();
    j->Dispose();

    void* result = reinterpret_cast<void*>(Isolate::GetCurrent() == i);
    return result;
  }


  // Returns a void* wrapped bool denoting whether the isolate is correct after a symmetric number of entries and
  // exits to the same isolate
  void* MultipleIsolateEntriesHandled(void* = nullptr) {
    Isolate* original {Isolate::GetCurrent()};
    Isolate* i {Isolate::New()};

    i->Enter();
    i->Enter();
    i->Enter();

    i->Exit();
    i->Exit();
    i->Exit();

    void* result {reinterpret_cast<void*>(Isolate::GetCurrent() == original)};

    i->Dispose();
    return result;
  }


  // Returns a void* wrapped bool denoting whether the caller remains in an isolate when it is exited fewer
  // times than it is entered
  void* AsymmetricEntriesAndExitsHandled(void* = nullptr) {
    Isolate* i {Isolate::New()};

    i->Enter();
    i->Enter();
    i->Exit();

    void* result {reinterpret_cast<void*>(Isolate::GetCurrent() == i)};

    i->Exit();
    i->Dispose();
    return result;
  }


  // Returns a void* wrapped bool denoting whether isolates stack: i.e. exiting an isolate should return the
  // thread to the isolate it was previously in
  void* CheckIsolateStacking(void* = nullptr) {
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
    return reinterpret_cast<void*>(allDistinct && returnedToSecond && returnedToFirst && returnedToOriginal);
  }


  // Returns a void* wrapped bool denoting whether attempts to dispose an isolate whilst inside it is fatal
  void* CheckBadDisposeIsFatal(void* = nullptr) {
    Isolate* i {Isolate::New()};
    i->Enter();
    errorCaught = false;
    V8::SetFatalErrorHandler(fatalErrorHandler);
    i->Dispose();

    void* result {reinterpret_cast<void*>(V8::IsDead() && errorCaught)};

    i->Exit();
    i->Dispose();
    return result;
  }


  void* CheckFailedDisposeDoesntDelete(void* = nullptr) {
    Isolate* i {Isolate::New()};
    i->Enter();
    V8::SetFatalErrorHandler(dummyFatalErrorHandler);
    i->Dispose();

    // If the implementation is broken, we'll dereference a dangling pointer here
    i->Exit();

    i->Dispose();
    return reinterpret_cast<void*>(true);
  }


  // The following function is intended to be executed only from a thread. Assumes arg is an isolate that the main
  // thread has entered, and attempts to dispose of it. Returns a void* wrapped bool signalling whether this was fatal.
  void* CrossThreadBadDispose(void* arg) {
    Isolate* i {Isolate::New()};
    i->Enter();

    // Suppress errors first
    V8::SetFatalErrorHandler(fatalErrorHandler);

    Isolate* mainThreadIsolate {reinterpret_cast<Isolate*>(arg)};
    errorCaught = false;
    mainThreadIsolate->Dispose();
    void* result {reinterpret_cast<void*>(V8::IsDead() && errorCaught)};

    // We will have entered the default isolate courtesy of SetFatalErrorHandler
    i->Exit();
    i->Dispose();

    return result;
  }


  // Returns a void* wrapped bool denoting whether the current isolate equals the one prior to Scope creation when a
  // single Scope is entered and destroyed
  void* CheckSingleScopeRestoresInitialState(void* = nullptr) {
    Isolate* initial {Isolate::GetCurrent()};
    Isolate* i {Isolate::New()};
    {
      Isolate::Scope scope {i};
    }

    bool result {Isolate::GetCurrent() == initial};

    i->Dispose();
    return reinterpret_cast<void*>(result);
  }


  // Returns a void* wrapped bool denoting whether GetCurrent is correct after constructing a Scope
  void* GetCurrentCorrectAfterScopeConstruction(void* = nullptr) {
    Isolate* i {Isolate::New()};
    bool result {};
    {
      Isolate::Scope scope {i};
      result = Isolate::GetCurrent() == i;
    }

    i->Dispose();
    return reinterpret_cast<void*>(result);
  }


  // Returns a void* wrapped bool denoting whether scopes stack correctly when the first isolate is explicitly entered
  void* CheckScopesStackAfterExplicitEntry(void* = nullptr) {
    Isolate* first {Isolate::New()};
    first->Enter();

    Isolate* second {Isolate::New()};
    {
      Isolate::Scope scope {second};
    }

    bool result {Isolate::GetCurrent() == first};

    second->Dispose();
    first->Exit();
    first->Dispose();

    return reinterpret_cast<void*>(result);
  }


  // Returns a void* wrapped bool denoting whether scopes stack correctly
  void* CheckScopesStack(void* = nullptr) {
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

    return reinterpret_cast<void*>(firstOK && secondOK);
  }
}


V8MONKEY_TEST(Isolate001, "Isolate::GetCurrent initially null (main thread)") {
  V8MONKEY_CHECK(!ReturnCurrentIsolate(), "Isolate::GetCurrent returned nullptr");
}


V8MONKEY_TEST(Isolate002, "Each invocation of Isolate::New returns a new isolate (main thread)") {
  V8MONKEY_CHECK(NewReturnsFreshIsolates(), "Isolate::New returned distinct isolates");
}


V8MONKEY_TEST(Isolate003, "Isolate::GetCurrent reports correct value after entry (main thread)") {
  V8MONKEY_CHECK(GetCurrentCorrectAfterEntry(), "Isolate::GetCurrent returned correct isolate");
}


V8MONKEY_TEST(Isolate004, "Isolate::GetCurrent reports correct value after exit (main thread)") {
  V8MONKEY_CHECK(IsolateCorrectAfterSingleExit(), "Isolate::GetCurrent returned correct isolate");
}


V8MONKEY_TEST(Isolate005, "Isolate::GetCurrent reports correct value after multiple entries and exits (main thread)") {
  V8MONKEY_CHECK(MultipleIsolateEntriesHandled(), "Isolate::GetCurrent returned correct isolate at final exit");
}


V8MONKEY_TEST(Isolate006, "Isolate::GetCurrent reports correct value after asymmetric entries and exits (main thread)") {
  V8MONKEY_CHECK(AsymmetricEntriesAndExitsHandled(), "Isolate::GetCurrent returned correct isolate at penultimate exit");
}


V8MONKEY_TEST(Isolate007, "Isolates stack correctly(main thread)") {
  V8MONKEY_CHECK(CheckIsolateStacking(), "Isolates stack correctly");
}


V8MONKEY_TEST(Isolate008, "Calling Dispose whilst within an isolate triggers an error (main thread)") {
  V8MONKEY_CHECK(CheckBadDisposeIsFatal(), "Dispose from within an isolate is fatal");
}


V8MONKEY_TEST(Isolate009, "Calling Dispose whilst within an isolate doesn't delete isolate (main thread)") {
  V8MONKEY_CHECK(CheckFailedDisposeDoesntDelete(), "Isolate not deleted if dispose failed");
}


V8MONKEY_TEST(Isolate010, "Attempt to dispose in-use isolate from another thread causes fatal error") {
  Isolate* i {Isolate::New()};
  i->Enter();
  V8Platform::Thread child {CrossThreadBadDispose};
  child.Run(i);

  V8MONKEY_CHECK(child.Join(), "Disposing an in-use isolate is fatal");

  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(Isolate011, "Embedder data is initially null") {
  Isolate* i {Isolate::New()};
  void* result = i->GetData(0);

  V8MONKEY_CHECK(!result, "Data was null");
  i->Dispose();
}


V8MONKEY_TEST(Isolate012, "Setting/getting data works as expected") {
  Isolate* i {Isolate::New()};
  i->SetData(0, i);
  bool result = i->GetData(0) == i;

  V8MONKEY_CHECK(result, "Data was correct");
  i->Dispose();
}


V8MONKEY_TEST(Isolate013, "Embedder data is slot specific") {
  Isolate* i {Isolate::New()};
  i->SetData(0, i);

  V8MONKEY_CHECK(i->GetData(0) == i && !i->GetData(1), "Data was correct");

  i->Dispose();
}


V8MONKEY_TEST(Isolate014, "Embedder data is isolate specific") {
  Isolate* i {Isolate::New()};
  i->SetData(0, i);
  Isolate* j {Isolate::New()};

  V8MONKEY_CHECK(i->GetData(0) == i && j->GetData(0) == nullptr, "Data was correct");

  j->Dispose();
  i->Dispose();
}


V8MONKEY_TEST(Isolate015, "Slots don't overlap") {
  static_assert(sizeof(void*) == 4 || sizeof(void*) == 8, "Unexpected pointer size breaks test");

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
  V8MONKEY_CHECK(CheckSingleScopeRestoresInitialState(), "main thread returned to initial isolate after scope destruction");
}


V8MONKEY_TEST(Scope002, "GetCurrent() reports correct isolate after Scope construction (within Scope lifetime)") {
  V8MONKEY_CHECK(GetCurrentCorrectAfterScopeConstruction(), "GetCurrent() was correct");
}


V8MONKEY_TEST(Scope003, "Scopes stack correctly for main with explicitly entered Isolates") {
  V8MONKEY_CHECK(CheckScopesStackAfterExplicitEntry(), "Scope stacked correctly");
}


V8MONKEY_TEST(Scope004, "Multiple Scopes stack correctly for main thread") {
  V8MONKEY_CHECK(CheckScopesStack(), "Scopes stacked correctly");
}


//
//
// using namespace v8;
//
//
// /*
//  * TODO: As a relic of the attempt at multithreading, most tests delegate to a helper function for actual computation.
//  *       If multithreading doesn't return, those intermediate functions can be dropped, and the code moved in to the
//  *       main test.
//  *
//  */
//
//
// namespace {
//
//
//   XXX No longer relevant?
//   // Returns a void* wrapped bool denoting whether attempts to dispose from a non-default isolate prove fatal
//   V8MONKEY_TEST_HELPER(CheckV8DisposeFromNonDefaultIsFatal) {
//     V8::Initialize();
//     Isolate* i {Isolate::New()};
//     i->Enter();
//
//     errorCaught = 0;
//     V8::SetFatalErrorHandler(fatalErrorHandler);
//     V8::Dispose();
//     bool result = V8::IsDead() && errorCaught != 0;
//
//     i->Exit();
//     i->Dispose();
//     Isolate::GetCurrent()->Exit();
//
//     return reinterpret_cast<void*>(result);
//   }
//
//
//   // Returns a void* wrapped bool denoting whether attempts to dispose without init (when the executing thread is
//   // associated with the default isolate) work
//   V8MONKEY_TEST_HELPER(CheckV8DisposeWithoutInit) {
//     // Reminder: in V8, any thread that calls a function that implicitly enters the default isolate will then be
//     // permanently associated with the default isolate (by which I mean that Isolate::GetCurrent will report the
//     // default isolate even when every possible isolate is exited). SetFatalErrorHandler is such a function.
//     V8::SetFatalErrorHandler(dummyFatalErrorHandler);
//     bool result = V8::Dispose();
//     Isolate::GetCurrent()->Exit();
//
//     return reinterpret_cast<void*>(result);
//   }
// }
//
//
// V8MONKEY_TEST(Isolate009, "Embedder data is initially null") {
//   Isolate* i {Isolate::GetCurrent()};
//   void* result = i->GetData();
//   V8MONKEY_CHECK(!result, "Data was null");
// }
//
//
// V8MONKEY_TEST(Isolate010, "Setting/getting data works as expected") {
//   Isolate* i {Isolate::GetCurrent()};
//   i->SetData(i);
//   bool result = i->GetData() == i;
//   V8MONKEY_CHECK(result, "Data was correct");
// }
//
//
// V8MONKEY_TEST(Isolate011, "Embedder data is isolate specific") {
//   Isolate* i {Isolate::GetCurrent()};
//   i->SetData(i);
//   Isolate* j {Isolate::New()};
//   bool result = i->GetData() == i && j->GetData() == nullptr;
//   V8MONKEY_CHECK(result, "Data was correct");
//   j->Dispose();
// }
// V8MONKEY_TEST(Dispose001, "Attempt to dispose V8 when in non-default isolate (main thread) triggers fatal error") {
//   void* result = CheckV8DisposeFromNonDefaultIsFatal();
//   V8MONKEY_CHECK(result, "Disposing V8 from a non-default isolate is fatal");
//
//   Isolate::GetCurrent()->Dispose();
//   V8::Dispose();
// }
//
//
// V8MONKEY_TEST(Dispose002, "V8::Dispose returns true when successful") {
//   // Init enters the default isolate
//   V8::Initialize();
//   // Exit default isolate in preparation for disposal
//   Isolate::GetCurrent()->Exit();
//
//   bool result = V8::Dispose();
//   V8MONKEY_CHECK(result, "Disposing V8 returned true");
// }
//
//
// V8MONKEY_TEST(Dispose003, "V8::Dispose returns false when unsuccessful") {
//   V8::Initialize();
//   Isolate* i {Isolate::New()};
//   i->Enter();
//
//   V8::SetFatalErrorHandler(dummyFatalErrorHandler);
//   bool result = V8::Dispose();
//
//   i->Exit();
//   i->Dispose();
//   Isolate::GetCurrent()->Exit();
//   Isolate::GetCurrent()->Dispose();
//
//   V8MONKEY_CHECK(!result, "Failed dispose returned false");
// }
//
//
// V8MONKEY_TEST(Dispose004, "Dispose without init works correctly from main") {
//   void* result = CheckV8DisposeWithoutInit();
//   V8MONKEY_CHECK(result, "Disposing V8 without init from main works");
// }
//
//
// /*
//  * Tests disabled after threading support disabled
//  *
//
//
// V8MONKEY_TEST(Isolate002, "Non-main thread reports Isolate::GetCurrent null when API not used") {
//   V8Platform::Thread child(ReturnCurrentIsolate);
//   child.Run();
//   Isolate* threadIsolate = static_cast<Isolate*>(child.Join());
//   V8MONKEY_CHECK(threadIsolate == nullptr, "Thread Isolate::GetCurrent was null");
// }
//
//
// V8MONKEY_TEST(Isolate004, "Non-main thread remains in isolate if exited fewer times than entered") {
//   V8Platform::Thread child(EnterTwiceReturnOnce);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "Still in isolate after asymmetric exits");
// }
//
//
// V8MONKEY_TEST(Isolate006, "Non-main thread exits to null after sufficient exits from multiply-entered isolate") {
//   V8Platform::Thread child(EnterThriceReturnThrice);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "Returned to null after symmetric exits");
// }
//
//
// V8MONKEY_TEST(Isolate008, "Isolate entries stack for off-main thread") {
//   V8Platform::Thread child(CheckIsolateStacking);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "Isolates returned to in correct sequence");
// }
//
//
// V8MONKEY_TEST(Isolate013, "Attempt to dispose in-use isolate causes fatal error for off-main thread") {
//   V8Platform::Thread child(CheckBadDisposeIsFatal);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "Disposing an in-use isolate is fatal");
// }
//
//
// V8MONKEY_TEST(Scope002, "Creating and destroying a single scope leaves thread in its initial state") {
//   V8Platform::Thread child(CheckSingleScopeRestoresInitialState);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "thread returned to initial isolate after scope destruction");
// }
//
//
// V8MONKEY_TEST(Scope004, "GetCurrent() reports correct isolate after Scope construction (within Scope lifetime)") {
//   V8Platform::Thread child(GetCurrentCorrectAfterScopeConstruction);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "GetCurrent() was correct");
// }
//
//
// V8MONKEY_TEST(Scope006, "Scopes stack correctly for thread with explicitly entered Isolates") {
//   V8Platform::Thread child(CheckScopesStackAfterExplicitEntry);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "Scope stacked correctly");
// }
//
//
// V8MONKEY_TEST(Scope008, "Multiple Scopes stack correctly for thread") {
//   V8Platform::Thread child(CheckScopesStack);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "Scopes stacked correctly");
// }
//
//
// V8MONKEY_TEST(Dispose002, "Attempt to dispose V8 when in non-default isolate (thread) triggers fatal error") {
//   V8Platform::Thread child(CheckV8DisposeFromNonDefaultIsFatal);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "Disposing V8 from a non-default isolate is fatal");
// }
//
//
// V8MONKEY_TEST(Dispose006, "Dispose without init works from thread if associated with default isolate") {
//   V8Platform::Thread child(CheckV8DisposeWithoutInit);
//   child.Run();
//   V8MONKEY_CHECK(child.Join(), "Disposing V8 without init from thread works in right circumstances");
// }
// */
