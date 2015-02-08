// DeletionObject DummyV8MonkeyObject V8MonkeyObject
#include "types/base_types.h"

// Persistent Value
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"
//
//
//namespace {
//  void* cb1Params = nullptr;
//  bool cb1Called = false;
//  int cb1CallCount = 0;
//
//
//  void nopCallback(v8::Persistent<v8::Value>, void* parameters) {
//    cb1CallCount++;
//    cb1Params = parameters;
//    cb1Called = true;
//  }
//
//
//  bool cb2Called = false;
//  void MakeWeakAgainCallback(v8::Persistent<v8::Value> object, void* parameters) {
//    cb2Called = true;
//    object.MakeWeak(parameters, MakeWeakAgainCallback);
//  }
//
//
//  void MakeStrongAgainCallback(v8::Persistent<v8::Value> object, void*) {
//    object.ClearWeak();
//  }
//
//
//  void disposeCallback(v8::Persistent<v8::Value> object, void*) {
//    object.Dispose();
//  }
//}
//
//
//using namespace v8::V8Monkey;
//
//
//V8MONKEY_TEST(RefCount001, "Objects initially born with refcount 0") {
//   DummyV8MonkeyObject refCounted;
//
//  V8MONKEY_CHECK(refCounted.RefCount() == 0, "Initial refcount correct");
//}
//
//
//V8MONKEY_TEST(RefCount002, "AddRef increases refcount") {
//  DummyV8MonkeyObject refCounted;
//
//  refCounted.AddRef();
//  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Refcount correct");
//}
//
//
//V8MONKEY_TEST(RefCount003, "Release decreases refcount") {
//  DummyV8MonkeyObject refCounted;
//
//  refCounted.AddRef();
//  refCounted.AddRef();
//  refCounted.Release();
//
//  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Refcount correct");
//}
//
//
//V8MONKEY_TEST(RefCount004, "Refcount falling to zero deletes object") {
//  bool deleted = false;
//  DeletionObject* refCounted = new DeletionObject(&deleted);
//
//  refCounted->AddRef();
//  refCounted->Release();
//  V8MONKEY_CHECK(deleted, "Refcounted object destroyed");
//}
//
//
//V8MONKEY_TEST(RefCount005, "Weak count initially zero") {
//  DummyV8MonkeyObject refCounted;
//
//  V8MONKEY_CHECK(refCounted.WeakCount() == 0, "Weak count initially zero");
//}
//
//
//V8MONKEY_TEST(RefCount006, "MakeWeak normally decreases strong count increases weak") {
//  DummyV8MonkeyObject refCounted;
//  // Manage the lifetime
//  refCounted.AddRef();
//
//  refCounted.MakeWeak(nullptr, nullptr, nullptr);
//  V8MONKEY_CHECK(refCounted.RefCount() == 0, "Refcount changed");
//  V8MONKEY_CHECK(refCounted.WeakCount() == 1, "Weak count changed");
//}
//
//
//V8MONKEY_TEST(RefCount007, "MakeWeak doesn't trigger deletion") {
//  bool deleted = false;
//  DeletionObject refCounted(&deleted);
//  refCounted.AddRef();
//
//  refCounted.MakeWeak(nullptr, nullptr, nullptr);
//  V8MONKEY_CHECK(!deleted, "Not deleted");
//}
//
//
//V8MONKEY_TEST(RefCount008, "MakeWeak doesn't change strong refcount if has already been made weak (1)") {
//  DummyV8MonkeyObject refCounted;
//  refCounted.AddRef();
//  refCounted.AddRef();
//  // ... refCount is now 2
//
//  refCounted.MakeWeak(nullptr, nullptr, nullptr);
//  refCounted.MakeWeak(nullptr, nullptr, nullptr);
//  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Strong refcount doesn't decrease further after multiple MakeWeak calls");
//}
//
//
//V8MONKEY_TEST(RefCount009, "MakeWeak doesn't change strong refcount if has already been made weak (2)") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  // Manage the lifetime
//  fakeSlot->AddRef();
//  fakeSlot->AddRef();
//  // ... refCount is now 2
//
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nullptr);
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nullptr);
//
//  V8MONKEY_CHECK(fakeSlot->RefCount() == 1, "Strong refcount doesn't decrease further after multiple MakeWeak calls");
//
//  fakeSlot->ClearWeakness(&fakeSlot);
//  fakeSlot->Release();
//  fakeSlot->Release();
//}
//
//
//V8MONKEY_TEST(RefCount010, "Different slot pointers make references weak correctly") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->AddRef();
//  fakeSlot->AddRef();
//  // ... refCount is now 3
//
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nullptr);
//  fakeSlot->MakeWeak(nullptr, nullptr, nullptr);
//
//  V8MONKEY_CHECK(fakeSlot->RefCount() == 1, "Refcount decreased correctly");
//
//  fakeSlot->ClearWeakness(&fakeSlot);
//  fakeSlot->ClearWeakness(nullptr);
//  fakeSlot->Release();
//  fakeSlot->Release();
//  fakeSlot->Release();
//}
//
//
//V8MONKEY_TEST(RefCount011, "Object not destroyed if weak references still exist") {
//  bool deleted = false;
//  DeletionObject refCounted(&deleted);
//
//  refCounted.AddRef();
//  refCounted.AddRef();
//  // ... refCount is now 2
//
//  refCounted.MakeWeak(nullptr, nullptr, nullptr);
//  // ... refCount now 1
//  refCounted.Release();
//
//  V8MONKEY_CHECK(!deleted, "Refcounted object not destroyed");
//}
//
//
//V8MONKEY_TEST(RefCount012, "ClearWeakness has no effect if requestor has not made object weak (1)") {
//  DummyV8MonkeyObject refCounted;
//
//  refCounted.AddRef();
//
//  refCounted.ClearWeakness(nullptr);
//
//  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Refcount unchanged");
//  V8MONKEY_CHECK(refCounted.WeakCount() == 0, "Weakcount unchanged");
//}
//
//
//V8MONKEY_TEST(RefCount013, "ClearWeakness has no effect if requestor has not made object weak (2)") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//
//  fakeSlot->MakeWeak(nullptr, nullptr, nullptr);
//  fakeSlot->ClearWeakness(&fakeSlot);
//
//  V8MONKEY_CHECK(fakeSlot->RefCount() == 0, "Refcount unchanged");
//  V8MONKEY_CHECK(fakeSlot->WeakCount() == 1, "Weakcount unchanged");
//
//  fakeSlot->ClearWeakness(nullptr);
//  fakeSlot->Release();
//}
//
//
//V8MONKEY_TEST(RefCount014, "ClearWeakness adds reference if value was previously weakened") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nullptr);
//  fakeSlot->ClearWeakness(&fakeSlot);
//
//  V8MONKEY_CHECK(fakeSlot->RefCount() == 2, "Refcount changed");
//  V8MONKEY_CHECK(fakeSlot->WeakCount() == 0, "Weakcount changed");
//
//  fakeSlot->Release();
//  fakeSlot->Release();
//}
//
//
//V8MONKEY_TEST(RefCount015, "PersistentRelease decrements refcount if requestor didn't weaken (1)") {
//  DummyV8MonkeyObject refCounted;
//
//  refCounted.AddRef();
//  refCounted.AddRef();
//  refCounted.PersistentRelease(nullptr);
//
//  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Refcount changed");
//}
//
//
//V8MONKEY_TEST(RefCount016, "PersistentRelease decrements refcount if requestor didn't weaken (2)") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->AddRef();
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(nullptr, nullptr, nullptr);
//  fakeSlot->PersistentRelease(&fakeSlot);
//
//  V8MONKEY_CHECK(fakeSlot->RefCount() == 1, "Refcount changed");
//
//  fakeSlot->PersistentRelease(nullptr);
//  fakeSlot->PersistentRelease(nullptr);
//}
//
//
//V8MONKEY_TEST(RefCount017, "PersistentRelease decrements weakcount if weakened") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nullptr);
//  fakeSlot->PersistentRelease(&fakeSlot);
//
//  V8MONKEY_CHECK(fakeSlot->WeakCount() == 0, "Weakcount changed");
//  fakeSlot->PersistentRelease(nullptr);
//}
//
//
//V8MONKEY_TEST(RefCount018, "PersistentRelease deletes object if applicable (1)") {
//  bool deleted = false;
//  V8MonkeyObject* fakeSlot = new DeletionObject(&deleted);
//
//  fakeSlot->AddRef();
//  fakeSlot->PersistentRelease(&fakeSlot);
//
//  V8MONKEY_CHECK(deleted, "Object deleted");
//}
//
//
//V8MONKEY_TEST(RefCount019, "PersistentRelease deletes object if applicable (2)") {
//  bool deleted = false;
//  V8MonkeyObject* fakeSlot = new DeletionObject(&deleted);
//
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nopCallback);
//
//  fakeSlot->PersistentRelease(&fakeSlot);
//
//  V8MONKEY_CHECK(deleted, "Object deleted");
//}
//
//
//V8MONKEY_TEST(RefCount020, "ShouldTrace returns true if there are strong references") {
//  DummyV8MonkeyObject refCounted;
//
//  refCounted.AddRef();
//
//  V8MONKEY_CHECK(refCounted.ShouldTrace(), "ShouldTrace correct");
//}
//
//
//V8MONKEY_TEST(RefCount021, "ShouldTrace calls callback for weak reference") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  cb1Called = false;
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nopCallback);
//  fakeSlot->ShouldTrace();
//
//  V8MONKEY_CHECK(cb1Called, "Weak ref callback called");
//}
//
//
//V8MONKEY_TEST(RefCount022, "Calling MakeWeak twice changes callback") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  cb1Called = false;
//  cb2Called = false;
//
//  fakeSlot->MakeWeak(&fakeSlot, &fakeSlot, MakeWeakAgainCallback);
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nopCallback);
//  fakeSlot->ShouldTrace();
//
//  V8MONKEY_CHECK(cb1Called && !cb2Called, "Correct weak ref callback called");
//}
//
//
//V8MONKEY_TEST(RefCount023, "Calling MakeWeak twice changes parameters") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  cb1Called = false;
//  cb2Called = false;
//
//  void* ptr = reinterpret_cast<void*>(0x1234);
//
//  fakeSlot->MakeWeak(&fakeSlot, ptr, nopCallback);
//  fakeSlot->MakeWeak(&fakeSlot, &fakeSlot, nopCallback);
//  cb1Params = nullptr;
//  fakeSlot->ShouldTrace();
//
//  V8MONKEY_CHECK(cb1Params == &fakeSlot, "Called with correct parameters");
//}
//
//
//V8MONKEY_TEST(RefCount024, "Calling MakeWeak twice still results in callback being called once") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nopCallback);
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nopCallback);
//  cb1CallCount = 0;
//  fakeSlot->ShouldTrace();
//
//  V8MONKEY_CHECK(cb1CallCount == 1, "Only called once");
//}
//
//
//V8MONKEY_TEST(RefCount025, "ShouldTrace calls all relevant callbacks") {
//  V8MonkeyObject* fakeSlot1 = new DummyV8MonkeyObject;
//  V8MonkeyObject* fakeSlot2 = fakeSlot1;
//
//  fakeSlot1->AddRef();
//  fakeSlot1->AddRef();
//  cb1Called = false;
//  cb2Called = false;
//
//  fakeSlot1->MakeWeak(&fakeSlot1, &fakeSlot1, MakeWeakAgainCallback);
//  fakeSlot1->MakeWeak(&fakeSlot2, nullptr, nopCallback);
//  fakeSlot1->ShouldTrace();
//
//  V8MONKEY_CHECK(cb1Called, "First callback called");
//  V8MONKEY_CHECK(cb2Called, "Second callback called");
//}
//
//
//V8MONKEY_TEST(RefCount026, "ShouldTrace calls callback with correct params") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(&fakeSlot, &fakeSlot, nopCallback);
//  fakeSlot->ShouldTrace();
//
//  V8MONKEY_CHECK(cb1Params == &fakeSlot, "Correct parameters supplied");
//}
//
//
//V8MONKEY_TEST(RefCount027, "ShouldTrace reports false if callbacks don't intervene") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nopCallback);
//
//  V8MONKEY_CHECK(!fakeSlot->ShouldTrace(), "ShouldTrace reported false");
//}
//
//
//V8MONKEY_TEST(RefCount028, "ShouldTrace reports true if callbacks intervene") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(&fakeSlot, &fakeSlot, MakeStrongAgainCallback);
//
//  V8MONKEY_CHECK(fakeSlot->ShouldTrace(), "ShouldTrace reported true");
//  fakeSlot->ClearWeakness(&fakeSlot);
//  fakeSlot->Release();
//}
//
//
//V8MONKEY_TEST(RefCount029, "PersistentRelease can be called in callback") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(nullptr, nullptr, nopCallback);
//  fakeSlot->MakeWeak(&fakeSlot, &fakeSlot, disposeCallback);
//  cb1Called = false;
//  fakeSlot->ShouldTrace();
//
//  V8MONKEY_CHECK(cb1Called, "Other callbacks unaffected by PersistentRelease");
//}
//
//
//V8MONKEY_TEST(RefCount030, "Clearing weakness ensures callback not called") {
//  DummyV8MonkeyObject refCounted;
//
//  refCounted.AddRef();
//
//  refCounted.MakeWeak(nullptr, nullptr, nopCallback);
//  refCounted.ClearWeakness(nullptr);
//  cb1Called = false;
//  refCounted.ShouldTrace();
//
//  V8MONKEY_CHECK(!cb1Called, "Not called");
//}
//
//
//V8MONKEY_TEST(RefCount031, "PersistentRelease zeroes slot if supplied when releasing strong reference") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->PersistentRelease(&fakeSlot);
//
//  V8MONKEY_CHECK(fakeSlot == nullptr, "Slot zeroed");
//}
//
//
//V8MONKEY_TEST(RefCount032, "PersistentRelease zeroes slot if supplied when releasing weak reference") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nopCallback);
//  fakeSlot->PersistentRelease(&fakeSlot);
//
//  V8MONKEY_CHECK(fakeSlot == nullptr, "Slot zeroed");
//}
//
//
//V8MONKEY_TEST(RefCount033, "ShouldTrace zeroes slot if reference was weak and no action taken") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(&fakeSlot, nullptr, nopCallback);
//  fakeSlot->ShouldTrace();
//
//  V8MONKEY_CHECK(fakeSlot == nullptr, "Slot zeroed");
//}
//
//
//V8MONKEY_TEST(RefCount034, "ShouldTrace doesn't zero slot if reference was weak and action taken") {
//  V8MonkeyObject* fakeSlot = new DummyV8MonkeyObject;
//
//  fakeSlot->AddRef();
//  fakeSlot->MakeWeak(&fakeSlot, &fakeSlot, MakeStrongAgainCallback);
//  fakeSlot->ShouldTrace();
//
//  V8MONKEY_CHECK(fakeSlot != nullptr, "Slot not zeroed");
//
//  fakeSlot->PersistentRelease(&fakeSlot);
//}
