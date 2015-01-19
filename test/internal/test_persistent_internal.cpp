// HandleScope Integer Isolate Local Number Persistent Value V8
#include "v8.h"

// ObjectBlock
#include "data_structures/objectblock.h"

// Get/SetPersistentData
#include "runtime/isolate.h"

// DeletionObject DummyV8MonkeyObject V8MonkeyObject
#include "types/base_types.h"

// TestUtils
#include "test.h"

// Unit-testing support
#include "V8MonkeyTest.h"

// TriggerFatalError
#include "v8monkey_common.h"


using namespace v8;
using namespace v8::V8Monkey;


namespace {
  // Doesn't really matter what type we use in the template
  const int blockSize = ObjectBlock<DummyV8MonkeyObject>::BlockSize;


  bool weakCallbackCalled = false;
  void* weakCallbackParams = nullptr;

  // A weak callback that will set weakCallbackCalled when called
  void weakCallbackChecker(Persistent<Value> p, void* parameter) {
    weakCallbackCalled = true;
    weakCallbackParams = parameter;
  }


  // A weak callback that strengthens the persistent
  void strengthenCallback(Persistent<Value> p, void* parameter) {
    p.ClearWeak();
  }


  // A weak callback that disposes the persistent
  void disposeCallback(Persistent<Value> p, void* parameter) {
    p.Dispose();
  }


  // A weak callback that expects its parameter to be a Persistent<Value> pointer, which is then deleted
  void deleteCallback(Persistent<Value> p, void* parameter) {
    Persistent<Value>* p2 = reinterpret_cast<Persistent<Value>*>(parameter);
    delete p2;
  }


  bool isNearDeath = false;

  // A weak callback that notes the result of calling IsNearDeath on the supplied persistent
  void NearDeathChecker(Persistent<Value> p, void* parameter) {
    isNearDeath = p.IsNearDeath();
  }


  bool wasEqual = false;

  // A weak callback that expects its parameter to be a Persistent<Value> pointer, which is then checked for equality
  void EqualityChecker(Persistent<Value> p, void* parameter) {
    Persistent<Value>* p2 = reinterpret_cast<Persistent<Value>*>(parameter);
    wasEqual = p == *p2;
  }


  // A weak callback that clears weakness, and checks the result of IsNearDeath
  void ClearWeakNearDeathChecker(Persistent<Value> p, void* parameter) {
    p.ClearWeak();
    isNearDeath = p.IsNearDeath();
  }


  bool isWeak = false;

  // A weak callback that notes the result of calling IsWeak on the supplied persistent
  void IsWeakChecker(Persistent<Value> p, void* parameter) {
    isWeak = p.IsWeak();
  }


  // A weak callback that clears weakness, and checks the result of IsWeak
  void ClearWeakIsWeakChecker(Persistent<Value> p, void* parameter) {
    p.ClearWeak();
    isWeak = p.IsWeak();
  }


  // A weak callback that clears weakness, reweakens, and checks the result of IsNearDeath
  void IsNearDeathClearAndReweakenChecker(Persistent<Value> p, void* parameter) {
    p.ClearWeak();
    p.MakeWeak(nullptr, IsNearDeathClearAndReweakenChecker);
    isNearDeath = p.IsNearDeath();
  }


  // A weak callback that clears weakness, reweakens, and checks the result of IsWeak
  void IsWeakClearAndReweakenChecker(Persistent<Value> p, void* parameter) {
    p.ClearWeak();
    p.MakeWeak(nullptr, IsWeakClearAndReweakenChecker);
    isWeak = p.IsWeak();
  }


  // Suppress error abortions
  void dummyFatalErrorHandler(const char* location, const char* message) {return;}


  bool errorCallbackCalled = false;

  void errorCallback(const char* location, const char* message) {
    errorCallbackCalled = true;
  }
}


// Macro that performs the grunt-work of creating a Persistent from realistic data, specifically a Local. The Local
// will fall out of scope, making the Persistent the only handle holding a reference to the object.
#define PERSISTENT_TEST(num, message) \
  V8MONKEY_TEST(IntPersistent##num, message) {\
    TestUtils::AutoTestCleanup ac; \
 \
    V8::Initialize(); \
    Persistent<Integer> p; \
 \
    { \
      HandleScope h; \
 \
      Local<Value> l(Integer::New(123)); \
      p = Persistent<Integer>::New(l); \
 \
    }


V8MONKEY_TEST(IntPersistent001, "InternalIsolate Persistent HandleData next initially null") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  HandleData hd = i->GetPersistentHandleData();
  V8MONKEY_CHECK(hd.next == nullptr, "HandleData next initially null");
}


V8MONKEY_TEST(IntPersistent002, "InternalIsolate Persistent HandleData limit initially null") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  HandleData hd = i->GetPersistentHandleData();
  V8MONKEY_CHECK(hd.limit == nullptr, "HandleData limit initially null");
}


V8MONKEY_TEST(IntPersistent003, "InternalIsolate Persistent HandleData changes after persistent creation (null case)") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;
    Local<Integer> l = Integer::New(123);
    Persistent<Integer> p = Persistent<Integer>::New(l);
    HandleData hd = i->GetPersistentHandleData();

    V8MONKEY_CHECK(hd.next != nullptr, "HandleData next changed after another persistent created");
    V8MONKEY_CHECK(hd.next != nullptr, "HandleData limit changed after another persistent created");
  }
}


V8MONKEY_TEST(IntPersistent004, "InternalIsolate Persistent HandleData changes after persistent creation (usual case)") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;
    Local<Integer> l = Integer::New(123);
    Persistent<Integer> p = Persistent<Integer>::New(l);
    HandleData hd = i->GetPersistentHandleData();
    V8MonkeyObject** prevNext = hd.next;
    Persistent<Integer> p2 = Persistent<Integer>::New(l);
    hd = i->GetPersistentHandleData();

    V8MONKEY_CHECK(hd.next != prevNext, "HandleData next changed after another persistent created");
  }
}


V8MONKEY_TEST(IntPersistent005, "InternalIsolate Persistent HandleData changes after persistent creation (full block case)") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;
    Local<Integer> l = Integer::New(123);
    HandleData hd = i->GetPersistentHandleData();
    V8MonkeyObject** prevNext = hd.next;
    V8MonkeyObject** prevLimit = hd.limit;

    for (int j = 0; j < blockSize + 2; j++) {
      Persistent<Integer> p = Persistent<Integer>::New(l);
      hd = i->GetPersistentHandleData();

      V8MONKEY_CHECK(hd.next != prevNext, "HandleData next changed after another handle created");
      if (j == 0 || j == blockSize) {
        V8MONKEY_CHECK(hd.limit != prevLimit, "HandleData limit changed after another handle created");
      } else {
        V8MONKEY_CHECK(hd.limit == prevLimit, "HandleData limit not changed after another handle created");
      }

      prevNext = hd.next;
      prevLimit = hd.limit;
    }
  }
}


V8MONKEY_TEST(IntPersistent006, "InternalIsolate Persistent HandleData doesn't change when constructing from pointer") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);
    Persistent<Integer> p(*l);
    HandleData hd = i->GetPersistentHandleData();

    V8MONKEY_CHECK(hd.next == nullptr, "HandleData unchanged after first persistent created");
    V8MONKEY_CHECK(hd.limit == nullptr, "HandleData unchanged after first persistent created");
  }
}


V8MONKEY_TEST(IntPersistent007, "InternalIsolate Persistent HandleData doesn't change when constructing from local") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);
    Persistent<Integer> p(l);
    HandleData hd = i->GetPersistentHandleData();

    V8MONKEY_CHECK(hd.next == nullptr, "HandleData unchanged after first persistent created");
    V8MONKEY_CHECK(hd.limit == nullptr, "HandleData unchanged after first persistent created");
  }
}


V8MONKEY_TEST(IntPersistent008, "InternalIsolate Persistent HandleData doesn't change when constructing from persistent") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);
    Persistent<Integer> p = Persistent<Integer>::New(l);
    HandleData hd = i->GetPersistentHandleData();
    V8MonkeyObject** prevNext = hd.next;
    Persistent<Integer> p2(p);
    hd = i->GetPersistentHandleData();

    V8MONKEY_CHECK(hd.next == prevNext, "HandleData unchanged after first persistent created");
  }
}


V8MONKEY_TEST(IntPersistent009, "Persistent refcount changes after persistent creation") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);
    int refCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

    Persistent<Integer> p = Persistent<Integer>::New(l);
    int newRefCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

    V8MONKEY_CHECK(newRefCount == refCount + 1, "Refcount bumped");
  }
}


V8MONKEY_TEST(IntPersistent010, "Persistent refcount doesn't change after persistent construction") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);

    Persistent<Integer> p = Persistent<Integer>::New(l);
    int refCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();
    Persistent<Integer> p2(p);
    int newRefCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

    V8MONKEY_CHECK(newRefCount == refCount, "Refcount bumped");
  }
}


V8MONKEY_TEST(IntPersistent011, "Persistent refcount unchanged after persistent destruction") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);
    int refCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

    {
      Persistent<Integer> p = Persistent<Integer>::New(l);
    }

    int newRefCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();
    V8MONKEY_CHECK(newRefCount == refCount + 1, "Refcount correct");
  }
}


V8MONKEY_TEST(IntPersistent012, "Persistent refcount changes after persistent disposal") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);
    int refCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();
    Persistent<Integer> p = Persistent<Integer>::New(l);
    p.Dispose();

    int newRefCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();
    V8MONKEY_CHECK(newRefCount == refCount, "Refcount correct");
  }
}


V8MONKEY_TEST(IntPersistent013, "Assigning to self doesn't delete") {
  TestUtils::AutoTestCleanup ac;

  V8::Initialize();
  bool deleted = false;

  Persistent<Value> p;
  DeletionObject* dummy = new DeletionObject(&deleted);
  V8MonkeyObject** slot;

  {
    HandleScope h;

    // Fake a local, and then assign p to the same object
    slot = HandleScope::CreateHandle(dummy);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);
    p = Persistent<Integer>::New(l);

    // When we exit this block, h will go out of scope, and p will hold the only ref to dummy
  }

  V8MONKEY_CHECK(dummy->RefCount() == 1, "Sanity check");

  p = p;
  V8MONKEY_CHECK(!deleted, "Refcounted object not deleted");
}


V8MONKEY_TEST(IntPersistent014, "Assigning to self from pointer doesn't delete") {
  TestUtils::AutoTestCleanup ac;

  V8::Initialize();
  bool deleted = false;

  Persistent<Value> p;
  DeletionObject* dummy = new DeletionObject(&deleted);
  V8MonkeyObject** slot;

  {
    HandleScope h;

    // Fake a local, and then assign p to the same object
    slot = HandleScope::CreateHandle(dummy);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);
    p = Persistent<Integer>::New(l);

    // When we exit this block, h will go out of scope, and p will hold the only ref to dummy
  }

  V8MONKEY_CHECK(dummy->RefCount() == 1, "Sanity check");

  p = *p;
  V8MONKEY_CHECK(!deleted, "Refcounted object not deleted");
}


PERSISTENT_TEST(015, "Equality holds for construction from other persistent") {
  Persistent<Integer> p2(p);

  V8MONKEY_CHECK(p2 == p, "Equality holds");
}}


PERSISTENT_TEST(016, "Equality holds for construction from other persistent's pointer") {
  Persistent<Integer> p2(*p);

  V8MONKEY_CHECK(p2 == p, "Equality holds");
}}


PERSISTENT_TEST(017, "Equality holds for new persistent created via Persistent::New") {
  Persistent<Integer> p2 = Persistent<Integer>::New(p);

  V8MONKEY_CHECK(p2 == p, "Equality holds");
}}


V8MONKEY_TEST(IntPersistent018, "Possible to create from empty local") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;

    Local<Integer> l;
    Persistent<Integer> p = Persistent<Integer>::New(l);
    HandleData hd = i->GetPersistentHandleData();

    V8MONKEY_CHECK(hd.next == nullptr, "Empty persistent created");
  }
}


V8MONKEY_TEST(IntPersistent019, "OK to dispose empty persistent") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  Persistent<Integer> p;
  p.Dispose();

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent020, "OK to delete disposed persistent") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);
    {
      Persistent<Integer> p = Persistent<Integer>::New(l);
      p.Dispose();
    }

    // On exiting the block, the disposed persistent will be deleted
    V8MONKEY_CHECK(true, "Didn't crash");
  }
}


PERSISTENT_TEST(021, "Containing slot zeroed after disposal") {
  V8MonkeyObject** slot = reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(*slot != 0, "Sanity check");
  p.Dispose();

  V8MONKEY_CHECK(*slot == 0, "Slot was zeroed");
}}


V8MONKEY_TEST(IntPersistent022, "Slot zeroed, but object still alive if still held alive elsewhere after disposal") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  {
    HandleScope h;

    bool deleted = false;

    // Create a DeletionObject, and manually morph it into a fake Value
    DeletionObject* dummy = new DeletionObject(&deleted);
    V8MonkeyObject** slot = HandleScope::CreateHandle(dummy);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);

    Persistent<Integer> p = Persistent<Integer>::New(l);
    V8MonkeyObject** persistentSlot = reinterpret_cast<V8MonkeyObject**>(*p);
    V8MONKEY_CHECK(*persistentSlot != 0, "Sanity check");
    p.Dispose();

    V8MONKEY_CHECK(*persistentSlot == 0, "Slot was zeroed");
    V8MONKEY_CHECK(!deleted, "Object wasn't deleted");
  }
}


V8MONKEY_TEST(IntPersistent023, "Containing slot not zeroed after destruction") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);
    V8MonkeyObject** slot;

    {
      Persistent<Integer> p = Persistent<Integer>::New(l);
      slot = reinterpret_cast<V8MonkeyObject**>(*p);
    }

    V8MONKEY_CHECK(*slot != 0, "Slot not zeroed");
  }
}


V8MONKEY_TEST(IntPersistent024, "Object still alive if held alive elsewhere after destruction") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  {
    HandleScope h;

    bool deleted = false;

    // Create a DeletionObject, and manually morph it into a fake Value
    DeletionObject* dummy = new DeletionObject(&deleted);
    V8MonkeyObject** slot = HandleScope::CreateHandle(dummy);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);
    V8MonkeyObject** persistentSlot;

    {
      Persistent<Value> p(*l);
      persistentSlot = reinterpret_cast<V8MonkeyObject**>(*p);
      V8MONKEY_CHECK(*persistentSlot != 0, "Sanity check");
    }

    V8MONKEY_CHECK(!deleted, "Object wasn't deleted");
  }
}


PERSISTENT_TEST(025, "IsWeak reports correct value normally") {
  V8MONKEY_CHECK(!p.IsWeak(), "IsWeak correct");
}}


PERSISTENT_TEST(026, "IsWeak reports correct value after weakening") {
  p.MakeWeak(nullptr, nullptr);

  V8MONKEY_CHECK(p.IsWeak(), "IsWeak correct");
}}


V8MONKEY_TEST(IntPersistent027, "Possible to call IsWeak on empty") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  Persistent<Integer> p;

  V8MONKEY_CHECK(!p.IsWeak(), "IsWeak correct");
}


PERSISTENT_TEST(028, "IsWeak reports correct value for persistent created from another persistent (1)") {
  Persistent<Integer> p2(p);
  V8MONKEY_CHECK(!p2.IsWeak(), "IsWeak correct");
}}


PERSISTENT_TEST(029, "IsWeak reports correct value for persistent created from another persistent (2)") {
  Persistent<Integer> p2(p);
  p.MakeWeak(nullptr, nullptr);
  V8MONKEY_CHECK(p2.IsWeak(), "IsWeak correct");
}}


PERSISTENT_TEST(030, "IsWeak reports correct value after clearing weakness") {
  p.MakeWeak(nullptr, nullptr);
  p.ClearWeak();

  V8MONKEY_CHECK(!p.IsWeak(), "IsWeak correct");
}}


PERSISTENT_TEST(031, "IsWeak reports correct value for persistent created from another (3)") {
  Persistent<Integer> p2(p);
  p.MakeWeak(nullptr, nullptr);
  p.ClearWeak();

  V8MONKEY_CHECK(!p.IsWeak(), "IsWeak correct");
}}


V8MONKEY_TEST(IntPersistent032, "Possible to call MakeWeak on empty") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  Persistent<Integer> p;
  p.MakeWeak(nullptr, nullptr);

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent033, "Weakening doesn't delete") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  bool deleted = false;

  Persistent<Value> p;
  DeletionObject* dummy = new DeletionObject(&deleted);
  V8MonkeyObject** slot;

  {
    HandleScope h;

    // Fake a local, and then assign p to the same object
    slot = HandleScope::CreateHandle(dummy);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);
    p = Persistent<Integer>::New(l);

    // When we exit this block, h will go out of scope, and p will hold the only ref to dummy
  }

  V8MONKEY_CHECK(dummy->RefCount() == 1, "Sanity check");

  p.MakeWeak(nullptr, nullptr);
  V8MONKEY_CHECK(!deleted, "Object not deleted");
}


PERSISTENT_TEST(034, "Weakening correctly adjusts refcount") {
  V8MonkeyObject* object = *reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(object->RefCount() == 1, "Sanity check");
  V8MONKEY_CHECK(object->WeakCount() == 0, "Sanity check");

  p.MakeWeak(nullptr, nullptr);

  V8MONKEY_CHECK(object->RefCount() == 0, "Refcount correct");
  V8MONKEY_CHECK(object->WeakCount() == 1, "Weakcount correct");
}}


PERSISTENT_TEST(035, "Clearing weakness has no effect on refcounts if not strengthened") {
  V8MonkeyObject* object = *reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(object->RefCount() == 1, "Sanity check");
  V8MONKEY_CHECK(object->WeakCount() == 0, "Sanity check");

  p.ClearWeak();
  V8MONKEY_CHECK(object->RefCount() == 1, "Refcount correct");
  V8MONKEY_CHECK(object->WeakCount() == 0, "Weakcount correct");
}}


PERSISTENT_TEST(036, "Clearing weakness corrects refcount") {
  V8MonkeyObject* object = *reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(object->RefCount() == 1, "Sanity check");
  V8MONKEY_CHECK(object->WeakCount() == 0, "Sanity check");

  p.MakeWeak(nullptr, nullptr);
  V8MONKEY_CHECK(object->RefCount() == 0, "Sanity check");
  V8MONKEY_CHECK(object->WeakCount() == 1, "Sanity check");

  p.ClearWeak();
  V8MONKEY_CHECK(object->RefCount() == 1, "Refcount correct");
  V8MONKEY_CHECK(object->WeakCount() == 0, "Weakcount correct");
}}


V8MONKEY_TEST(IntPersistent037, "Possible to call ClearWeak on empty") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  Persistent<Integer> p;
  p.ClearWeak();

  V8MONKEY_CHECK(true, "Didn't crash");
}


PERSISTENT_TEST(038, "IsNearDeath normally false") {
  V8MONKEY_CHECK(!p.IsNearDeath(), "IsNearDeath correct");
}}


PERSISTENT_TEST(039, "IsNearDeath still false after weakening") {
  p.MakeWeak(nullptr, nullptr);

  V8MONKEY_CHECK(!p.IsNearDeath(), "IsNearDeath correct");
}}


V8MONKEY_TEST(IntPersistent040, "Possible to call IsNearDeath on empty") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();

  Persistent<Integer> p;

  V8MONKEY_CHECK(!p.IsNearDeath(), "IsNearDeath correct");
}


V8MONKEY_TEST(IntPersistent041, "Items contained in persistents traced correctly") {
  TestUtils::AutoTestCleanup ac;

  bool traced = false;
  TraceFake* t = new TraceFake(&traced);

  V8::Initialize();

  {
    HandleScope h;

    V8MonkeyObject** slot = HandleScope::CreateHandle(t);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);
    Persistent<Value> p = Persistent<Value>::New(l);
    InternalIsolate::ForceGC();
  }

  V8MONKEY_CHECK(traced, "Value was traced");
}


V8MONKEY_TEST(IntPersistent042, "Tracing copes with zeroed slot in handle data") {
  TestUtils::AutoTestCleanup ac;

  bool traced = false;
  TraceFake* t = new TraceFake(&traced);

  V8::Initialize();

  {
    HandleScope h;

    Local<Value> l = Integer::New(123);
    Persistent<Value> zero = Persistent<Value>::New(l);
    zero.Dispose();

    // Fake a local, and then assign p to the same object
    V8MonkeyObject** slot = HandleScope::CreateHandle(t);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l2(dummySlotAsValue);
    Persistent<Value> p = Persistent<Value>::New(l2);
    InternalIsolate::ForceGC();
  }

  V8MONKEY_CHECK(traced, "Legitimate value was traced");
}


PERSISTENT_TEST(043, "Weak callback called during tracing") {
  p.MakeWeak(nullptr, weakCallbackChecker);
  weakCallbackCalled = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(weakCallbackCalled, "Weak callback called");
}}


PERSISTENT_TEST(044, "Weak callback called with correct parameters") {
  // Let's use the TestCleanup object (defined in the macro) as our parameter
  p.MakeWeak(&ac, weakCallbackChecker);
  weakCallbackParams = nullptr;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(weakCallbackParams == &ac, "Weak callback called with correct parameters");
}}


PERSISTENT_TEST(045, "Weak callback called not called if strengthened") {
  p.MakeWeak(nullptr, weakCallbackChecker);
  p.ClearWeak();
  weakCallbackCalled = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!weakCallbackCalled, "Weak callback not called");
}}


PERSISTENT_TEST(046, "OK to clear weakness in callback") {
  p.MakeWeak(nullptr, strengthenCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


PERSISTENT_TEST(047, "Refcount correct after weak callback sequence") {
  Persistent<Value> q = Persistent<Integer>::New(p);
  p.MakeWeak(nullptr, strengthenCallback);
  q.MakeWeak(nullptr, weakCallbackChecker);

  // We have just created two persistents pointing at the same object, which should have 2 weakrefs. After the GC, we
  // should have 1 strong ref and 1 weak ref
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*p));
  V8MONKEY_CHECK(underlyingObject->RefCount() == 0, "RefCount sanity check");
  V8MONKEY_CHECK(underlyingObject->WeakCount() == 2, "WeakCount check");

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(underlyingObject->RefCount() == 1, "RefCount correct");
  V8MONKEY_CHECK(underlyingObject->WeakCount() == 1, "WeakCount correct");
}}


PERSISTENT_TEST(048, "OK to dispose in callback") {
  p.MakeWeak(nullptr, disposeCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


PERSISTENT_TEST(049, "Disposing in callback does not affect other weak references (1)") {
  // This test and the next exercise both possible orderings
  Persistent<Value> q = Persistent<Integer>::New(p);
  q.MakeWeak(nullptr, weakCallbackChecker);
  p.MakeWeak(nullptr, disposeCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


PERSISTENT_TEST(050, "Disposing in callback does not affect other weak references (2)") {
  Persistent<Value> q = Persistent<Integer>::New(p);
  p.MakeWeak(nullptr, disposeCallback);
  q.MakeWeak(nullptr, weakCallbackChecker);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


V8MONKEY_TEST(IntPersistent051, "OK to delete real persistent in callback") {
  // We don't use the macro here due to the need to create via operator new
  TestUtils::AutoTestCleanup ac;

  V8::Initialize();
  Persistent<Value>* p = new Persistent<Value>;

  {
    HandleScope h;

    // Fake a local, and then assign p to the same object
    Local<Value> l(Integer::New(123));
    Persistent<Value> p2 = Persistent<Value>::New(l);
    *p = p2;

    // When we exit this block, h will go out of scope, and p will hold the only ref to the integer
  }
  p->MakeWeak(p, deleteCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent052, "Deleting in callback via real persistent does not affect other weak references (1)") {
  // We don't use the macro here due to the need to create via operator new
  TestUtils::AutoTestCleanup ac;

  V8::Initialize();
  Persistent<Value>* p = new Persistent<Value>;

  {
    HandleScope h;

    Local<Value> l(Integer::New(123));
    Persistent<Value> p2 = Persistent<Integer>::New(l);
    *p = p2;

    // When we exit this block, h will go out of scope, and p will hold the only ref to the integer
  }

  // This test and the next show that other weakrefs are unaffected, using both possible orders
  Persistent<Value> q = Persistent<Integer>::New(*p);
  q.MakeWeak(nullptr, weakCallbackChecker);
  p->MakeWeak(p, deleteCallback);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent053, "Deleting in callback via real persistent does not affect other weak references (2)") {
  // We don't use the macro here due to the need to create via operator new
  TestUtils::AutoTestCleanup ac;

  V8::Initialize();
  Persistent<Value>* p = new Persistent<Value>;

  {
    HandleScope h;

    Local<Value> l(Integer::New(123));
    Persistent<Value> p2 = Persistent<Integer>::New(l);
    *p = p2;

    // When we exit this block, h will go out of scope, and p will hold the only ref to the integer
  }

  Persistent<Value> q = Persistent<Integer>::New(*p);
  p->MakeWeak(p, deleteCallback);
  q.MakeWeak(nullptr, weakCallbackChecker);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}


PERSISTENT_TEST(054, "IsNearDeath reports correct value in callback") {
  p.MakeWeak(nullptr, NearDeathChecker);
  isNearDeath = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct");
}}


PERSISTENT_TEST(055, "Equality holds for temporary persistent passed to callback") {
  // The persistent created when invoking the callback should be "equal" to the original
  p.MakeWeak(&p, EqualityChecker);
  wasEqual = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(wasEqual, "Values were equal");
}}


PERSISTENT_TEST(056, "IsNearDeath reports false after ClearWeakness call in callback") {
  p.MakeWeak(nullptr, ClearWeakNearDeathChecker);
  isNearDeath = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isNearDeath, "IsNearDeath correct");
}}


PERSISTENT_TEST(057, "Reporting of IsNearDeath unaffected by other callbacks clearing weakness (1)") {
  Persistent<Value> q = Persistent<Integer>::New(p);

  // This test and the next show that other weakrefs are unaffected, using both possible orders
  p.MakeWeak(nullptr, strengthenCallback);
  q.MakeWeak(nullptr, NearDeathChecker);

  isNearDeath = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct for other callback");
}}


PERSISTENT_TEST(058, "Reporting of IsNearDeath unaffected by other callbacks clearing weakness (2)") {
  Persistent<Value> q = Persistent<Integer>::New(p);

  // This test and the next show that other weakrefs are unaffected, using both possible orders
  q.MakeWeak(nullptr, NearDeathChecker);
  p.MakeWeak(nullptr, strengthenCallback);

  isNearDeath = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct for other callback");
}}


PERSISTENT_TEST(059, "IsWeak reports false after ClearWeakness call in callback") {
  p.MakeWeak(nullptr, ClearWeakIsWeakChecker);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isWeak, "IsWeak correct");
}}


PERSISTENT_TEST(060, "Reporting of IsWeak unaffected by other callbacks clearing weakness (1)") {
  Persistent<Value> q = Persistent<Integer>::New(p);

  // This test and the next show that other weakrefs are unaffected, using both possible orders
  p.MakeWeak(nullptr, strengthenCallback);
  q.MakeWeak(nullptr, IsWeakChecker);

  isWeak = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct for other callback");
}}


PERSISTENT_TEST(061, "Reporting of IsWeak unaffected by other callbacks clearing weakness (2)") {
  Persistent<Value> q = Persistent<Integer>::New(p);

  // This test and the next show that other weakrefs are unaffected, using both possible orders
  q.MakeWeak(nullptr, IsWeakChecker);
  p.MakeWeak(nullptr, strengthenCallback);

  isWeak = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct for other callback");
}}


PERSISTENT_TEST(062, "In callback, IsWeak reports true for after clearing weakness and then reweakening") {
  p.MakeWeak(nullptr, IsWeakClearAndReweakenChecker);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct");
}}


PERSISTENT_TEST(063, "In callback, IsNearDeath reports false for after clearing weakness and then reweakening") {
  p.MakeWeak(nullptr, IsNearDeathClearAndReweakenChecker);
  isNearDeath = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isNearDeath, "IsNearDeath correct");
}}


PERSISTENT_TEST(064, "IsNearDeath reports false after callback sequence") {
  p.MakeWeak(nullptr, weakCallbackChecker);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!p.IsNearDeath(), "No longer reports near death");
}}


PERSISTENT_TEST(065, "IsNearDeath reports false after callback sequence if strengthened") {
  p.MakeWeak(nullptr, strengthenCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!p.IsNearDeath(), "No longer reports near death");
}}


PERSISTENT_TEST(066, "Disposing in callback zeroes object slot") {
  V8MonkeyObject** slot = reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(*slot != 0, "Sanity check");

  p.MakeWeak(nullptr, disposeCallback);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(*slot == 0, "Slot was zeroed");
}}


PERSISTENT_TEST(067, "Slot zeroed after tracing if callbacks decline to strengthen") {
  V8MonkeyObject** slot = reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(*slot != 0, "Sanity check");

  p.MakeWeak(nullptr, weakCallbackChecker);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(*slot == 0, "Slot was zeroed");
}}


PERSISTENT_TEST(068, "Slot not zeroed after tracing if callbacks strengthened") {
  V8MonkeyObject** slot = reinterpret_cast<V8MonkeyObject**>(*p);

  p.MakeWeak(nullptr, strengthenCallback);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(*slot != 0, "Slot was not zeroed");
}}


V8MONKEY_TEST(IntPersistent069, "Weak objects deleted after tracing if callbacks didn't strengthen") {
  TestUtils::AutoTestCleanup ac;

  bool deleted = false;
  Isolate* i = Isolate::New();
  i->Enter();
  V8::Initialize();
  Persistent<Value> p;

  {
    HandleScope h;

    // Create a DeletionObject, and manually morph it into a fake Value
    DeletionObject* dummy = new DeletionObject(&deleted);
    V8MonkeyObject** slot = HandleScope::CreateHandle(dummy);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);
    p = Persistent<Value>::New(l);
  }

  // At this point, the local has gone out of scope, but not the persistent, so there is still 1 reference to the object
  V8MONKEY_CHECK(!deleted, "Sanity check");

  p.MakeWeak(nullptr, weakCallbackChecker);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(deleted, "Object was deleted after tracing");
}


V8MONKEY_TEST(IntPersistent070, "Weak objects not deleted after tracing if callbacks strengthen") {
  TestUtils::AutoTestCleanup ac;

  bool deleted = false;
  Isolate* i = Isolate::New();
  i->Enter();
  V8::Initialize();
  Persistent<Value> p;

  {
    HandleScope h;

    // Create a DeletionObject, and manually morph it into a fake Value
    DeletionObject* dummy = new DeletionObject(&deleted);
    V8MonkeyObject** slot = HandleScope::CreateHandle(dummy);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);
    p = Persistent<Value>::New(l);
  }

  // At this point, the local has gone out of scope, but not the persistent, so there is still 1 reference to the object
  V8MONKEY_CHECK(!deleted, "Sanity check");

  p.MakeWeak(nullptr, strengthenCallback);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!deleted, "Object was not deleted after tracing");
}


PERSISTENT_TEST(071, "All slots zeroed after tracing if callbacks decline to strengthen") {
  Persistent<Integer> q = Persistent<Integer>::New(p);
  Persistent<Integer> r = Persistent<Integer>::New(p);

  V8MonkeyObject** pSlot = reinterpret_cast<V8MonkeyObject**>(*p);
  V8MonkeyObject** qSlot = reinterpret_cast<V8MonkeyObject**>(*q);
  V8MonkeyObject** rSlot = reinterpret_cast<V8MonkeyObject**>(*r);
  V8MONKEY_CHECK(*pSlot != 0, "p slot sanity check");
  V8MONKEY_CHECK(*qSlot != 0, "q slot sanity check");
  V8MONKEY_CHECK(*rSlot != 0, "r slot sanity check");

  p.MakeWeak(nullptr, weakCallbackChecker);
  q.MakeWeak(nullptr, nullptr);
  r.MakeWeak(nullptr, weakCallbackChecker);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(*pSlot == 0, "Slot was zeroed (1)");
  V8MONKEY_CHECK(*qSlot == 0, "Slot was zeroed (2)");
  V8MONKEY_CHECK(*rSlot == 0, "Slot was zeroed (3)");
}}


PERSISTENT_TEST(072, "No slots zeroed after tracing if at least one callback strengthens") {
  Persistent<Integer> q = Persistent<Integer>::New(p);
  Persistent<Integer> r = Persistent<Integer>::New(p);

  V8MonkeyObject** pSlot = reinterpret_cast<V8MonkeyObject**>(*p);
  V8MonkeyObject** qSlot = reinterpret_cast<V8MonkeyObject**>(*q);
  V8MonkeyObject** rSlot = reinterpret_cast<V8MonkeyObject**>(*r);

  p.MakeWeak(nullptr, weakCallbackChecker);
  q.MakeWeak(nullptr, strengthenCallback);
  r.MakeWeak(nullptr, nullptr);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(*pSlot != 0, "Slot was not zeroed (1)");
  V8MONKEY_CHECK(*qSlot != 0, "Slot was not zeroed (2)");
  V8MONKEY_CHECK(*rSlot != 0, "Slot was not zeroed (3)");
}}


PERSISTENT_TEST(073, "OK to dispose persistent zeroed by tracing") {
  p.MakeWeak(nullptr, weakCallbackChecker);
  InternalIsolate::ForceGC();
  p.Dispose();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


V8MONKEY_TEST(IntPersistent074, "All objects torn down at isolate teardown") {
  TestUtils::AutoTestCleanup ac;

  bool deleted = false;
  Isolate* i = Isolate::New();
  i->Enter();
  V8::Initialize();
  Persistent<Value>* p = new Persistent<Value>;

  {
    HandleScope h;

    // Create a DeletionObject, and manually morph it into a fake Value
    DeletionObject* dummy = new DeletionObject(&deleted);
    V8MonkeyObject** slot = HandleScope::CreateHandle(dummy);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);
    Persistent<Value> p2 = Persistent<Value>::New(l);
    *p = p2;
  }

  // At this point, the local has gone out of scope, but not the persistent, so there is still 1 reference to the object
  V8MONKEY_CHECK(!deleted, "Sanity check");

  i->Exit();
  i->Dispose();

  V8MONKEY_CHECK(deleted, "Object was deleted after tracing");
  delete p;
}


V8MONKEY_TEST(IntPersistent075, "Alignment of pointers is suitable for tagging pointers") {
  // TODO This should be some sort of static assert
  // TODO Also alignof support is patchy. That computation should be in a platform-specific macro
  V8MONKEY_CHECK(alignof(Persistent<Value>*) > 1, "Alignment suitable");
}


V8MONKEY_TEST(IntPersistent076, "Persistent is same size as pointer") {
  // TODO This should be some sort of static assert
  V8MONKEY_CHECK(sizeof(Persistent<Integer>) <= sizeof(void*), "Persistent fits in a pointer");
}


V8MONKEY_TEST(IntPersistent077, "Persistent::New returns empty if V8 dead") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);

    V8::SetFatalErrorHandler(dummyFatalErrorHandler);
    V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);

    Persistent<Integer> p = Persistent<Integer>::New(l);

    V8MONKEY_CHECK(*p == nullptr, "Persistent is empty");
  }
}


V8MONKEY_TEST(IntPersistent078, "Persistent::New triggers error if V8 dead") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);

    TestUtils::SetHandlerAndKill(errorCallback);
    errorCallbackCalled = false;
    Persistent<Integer> p = Persistent<Integer>::New(l);

    V8MONKEY_CHECK(errorCallbackCalled, "Error triggered");
  }
}


V8MONKEY_TEST(IntPersistent079, "Persistent::New works when isolate not initted") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;

    Local<Integer> l = Integer::New(123);

    V8::SetFatalErrorHandler(dummyFatalErrorHandler);
    Persistent<Integer> p = Persistent<Integer>::New(l);

    V8MONKEY_CHECK(*p != nullptr, "Persistent not empty");
  }
}


V8MONKEY_TEST(IntPersistent080, "InternalIsolate Persistent HandleData is per isolate") {
  TestUtils::AutoTestCleanup ac;
  V8::Initialize();
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;
    Local<Integer> l = Integer::New(123);
    Persistent<Integer> p = Persistent<Integer>::New(l);

    Isolate* j = Isolate::New();
    InternalIsolate* i = InternalIsolate::FromIsolate(j);

    HandleData hd = i->GetPersistentHandleData();

    V8MONKEY_CHECK(hd.next == nullptr, "HandleData next changed after another persistent created");
    V8MONKEY_CHECK(hd.next == nullptr, "HandleData limit changed after another persistent created");
    j->Dispose();
  }
}


#undef PERSISTENT_TEST
