#include "v8.h"

#include "data_structures/objectblock.h"
#include "runtime/isolate.h"
#include "types/base_types.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;
using namespace v8::V8Monkey;


namespace {
  // Doesn't really matter what type we use in the template
  const int blockSize = ObjectBlock<DummyV8MonkeyObject>::BlockSize;


  static JSTraceDataOp traceFn = nullptr;
  static void* traceData = nullptr;


  void GCRegistrationHook(JSRuntime* rt, JSTraceDataOp op, void* data) {
    traceFn = op;
    traceData = data;
  }


  void GCDeregistrationHook(JSRuntime* rt, JSTraceDataOp op, void* data) {
    // NOP
  }

  bool weakCB1Called = false;
  void* weakCB1Params = nullptr;
  void weakCB1(Persistent<Value> object, void* params) {
    printf("WeakCB1\n");
    weakCB1Called = true;
    weakCB1Params = params;
  }


  void strengthenCallback(Persistent<Value> object, void* params) {
    printf("strengthenCB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
  }


  void disposeParamCallback(Persistent<Value> object, void* params) {
    printf("disposeCB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->Dispose();
  }


  void deleteCallback(Persistent<Value> object, void* params) {
    printf("deleteCB params %p\n", params);
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    V8MonkeyObject* obj = *reinterpret_cast<V8MonkeyObject**>(**p);
    printf("p %p RC: %d WC %d\n", p, obj->RefCount(), obj->WeakCount());
    delete p;
    printf("p %p RC: %d WC %d\n", p, obj->RefCount(), obj->WeakCount());
  }


  void disposeTempCallback(Persistent<Value> object, void* params) {
    printf("disposeTempCB\n");
    object.Dispose();
  }


  bool isNearDeath = false;
  void NearDeathParamChecker(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    isNearDeath = p->IsNearDeath();
  }


  void NearDeathTempChecker(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    isNearDeath = object.IsNearDeath();
  }


  void NearDeathClearParamChecker(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    isNearDeath = p->IsNearDeath();
  }


  void NearDeathClearTempChecker(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    object.ClearWeak();
    isNearDeath = object.IsNearDeath();
  }


  void NearDeathClearTempCheckParam(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    object.ClearWeak();
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    isNearDeath = p->IsNearDeath();
  }


  void NearDeathClearParamCheckTemp(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    isNearDeath = object.IsNearDeath();
  }


  bool isWeak = false;
  void IsWeakTempChecker(Persistent<Value> object, void* params) {
    printf("IsWe CB\n");
    isWeak = object.IsWeak();
  }


  void IsWeakClearParamChecker(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    isWeak = p->IsWeak();
  }


  void IsWeakClearTempChecker(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    object.ClearWeak();
    isWeak = object.IsWeak();
  }


  void IsWeakClearTempCheckParam(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    object.ClearWeak();
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    isWeak = p->IsWeak();
  }


  void IsWeakClearParamCheckTemp(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    isWeak = object.IsWeak();
  }


  void IsWeakClearReParam(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    p->MakeWeak(p, IsWeakClearReParam);
    isWeak = p->IsWeak();
  }


  void IsNearDeathClearReParam(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    p->MakeWeak(p, IsNearDeathClearReParam);
    isNearDeath = p->IsNearDeath();
  }


  void IsWeakClearReTemp(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    object.ClearWeak();
    object.MakeWeak(params, IsWeakClearReTemp);
    isWeak = object.IsWeak();
  }


  void IsNearDeathClearReTemp(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    object.ClearWeak();
    object.MakeWeak(params, IsNearDeathClearReTemp);
    isNearDeath = object.IsNearDeath();
  }


  void IsWeakClearReParamParamTemp(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    p->MakeWeak(p, IsWeakClearReParamParamTemp);
    isWeak = object.IsWeak();
  }


  void IsNearDeathClearReParamParamTemp(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    p->MakeWeak(p, IsNearDeathClearReParamParamTemp);
    isNearDeath = object.IsNearDeath();
  }


  void IsWeakClearReParamTempParam(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    object.MakeWeak(p, IsWeakClearReParamTempParam);
    isWeak = p->IsWeak();
  }


  void IsNearDeathClearReParamTempParam(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    p->ClearWeak();
    object.MakeWeak(p, IsNearDeathClearReParamTempParam);
    isNearDeath = p->IsNearDeath();
  }


  void IsWeakClearReTempTempParam(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    object.ClearWeak();
    object.MakeWeak(p, IsWeakClearReTempTempParam);
    isWeak = p->IsWeak();
  }


  void IsNearDeathClearReTempTempParam(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    object.ClearWeak();
    object.MakeWeak(p, IsNearDeathClearReTempTempParam);
    isNearDeath = p->IsNearDeath();
  }


  void IsWeakClearReTempParamTemp(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    object.ClearWeak();
    p->MakeWeak(p, IsWeakClearReTempParamTemp);
    isWeak = object.IsWeak();
  }


  void IsNearDeathClearReTempParamTemp(Persistent<Value> object, void* params) {
    printf("IsNeDe CB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    object.ClearWeak();
    p->MakeWeak(p, IsNearDeathClearReTempParamTemp);
    isNearDeath = object.IsNearDeath();
  }


  bool wasEqual = false;
  void EqualityChecker(Persistent<Value> object, void* params) {
    printf("EQCB\n");
    Persistent<Integer>* p = reinterpret_cast<Persistent<Integer>*>(params);
    wasEqual = *p == object;
  }


  bool wasZeroed = false;
  void TempDisposeChecker(Persistent<Value> object, void* params) {
    printf("WZCB\n");
    Persistent<Integer> p = *(reinterpret_cast<Persistent<Integer>*>(params));
    v8::V8Monkey::V8MonkeyObject** slot = reinterpret_cast<V8MonkeyObject**>(*p);
    object.Dispose();
    wasZeroed = *slot == nullptr;
  }
}


V8MONKEY_TEST(IntPersistent001, "InternalIsolate Persistent HandleData next initially null") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleData hd = i->GetPersistentHandleData();

  V8MONKEY_CHECK(hd.next == nullptr, "HandleData next initially null");
}


V8MONKEY_TEST(IntPersistent002, "InternalIsolate Persistent HandleData limit initially null") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleData hd = i->GetPersistentHandleData();

  V8MONKEY_CHECK(hd.limit == nullptr, "HandleData limit initially null");
}


V8MONKEY_TEST(IntPersistent003, "InternalIsolate Persistent HandleData changes after persistent creation (from pointer, null case)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScope h;
  
  Persistent<Integer> p(*(Integer::New(123)));
  HandleData hd = i->GetPersistentHandleData();

  V8MONKEY_CHECK(hd.next != nullptr, "HandleData next changed after first persistent created");
  V8MONKEY_CHECK(hd.limit != nullptr, "HandleData limit changed after first persistent created");
}


V8MONKEY_TEST(IntPersistent004, "InternalIsolate Persistent HandleData changes after persistent creation (from pointer, usual case)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScope h;

  Persistent<Integer> p(*(Integer::New(123)));
  HandleData hd = i->GetPersistentHandleData();
  V8MonkeyObject** prevNext = hd.next;

  Persistent<Integer> p2(*(Integer::New(456)));

  hd = i->GetPersistentHandleData();
  V8MONKEY_CHECK(hd.next != prevNext, "HandleData next changed after another persistent created");
}


V8MONKEY_TEST(IntPersistent005, "InternalIsolate Persistent HandleData changes after persistent creation (from pointer, block full case)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScope h;

  HandleData hd = i->GetPersistentHandleData();
  V8MonkeyObject** prevNext = hd.next;
  V8MonkeyObject** prevLimit = hd.limit;

  for (int j = 0; j < blockSize + 2; j++) {
    Persistent<Integer> p(*(Integer::New(123)));
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


V8MONKEY_TEST(IntPersistent006, "InternalIsolate Persistent HandleData changes after persistent creation (from local, null case)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScope h;
  
  Persistent<Integer> p(Integer::New(123));
  HandleData hd = i->GetPersistentHandleData();

  V8MONKEY_CHECK(hd.next != nullptr, "HandleData next changed after first persistent created");
  V8MONKEY_CHECK(hd.limit != nullptr, "HandleData limit changed after first persistent created");
}


V8MONKEY_TEST(IntPersistent007, "InternalIsolate Persistent HandleData changes after persistent creation (from local, usual case)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScope h;

  Persistent<Integer> p(*(Integer::New(123)));
  HandleData hd = i->GetPersistentHandleData();
  V8MonkeyObject** prevNext = hd.next;

  Persistent<Integer> p2(Integer::New(456));

  hd = i->GetPersistentHandleData();
  V8MONKEY_CHECK(hd.next != prevNext, "HandleData next changed after another persistent created");
}


V8MONKEY_TEST(IntPersistent008, "InternalIsolate Persistent HandleData changes after persistent creation (from local, block full case)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScope h;

  HandleData hd = i->GetPersistentHandleData();
  V8MonkeyObject** prevNext = hd.next;
  V8MonkeyObject** prevLimit = hd.limit;

  for (int j = 0; j < blockSize + 2; j++) {
    Persistent<Integer> p(Integer::New(123));
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


V8MONKEY_TEST(IntPersistent009, "InternalIsolate Persistent HandleData changes after persistent creation (from persistent, usual case)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScope h;

  Persistent<Integer> p(Integer::New(123));
  HandleData hd = i->GetPersistentHandleData();
  V8MonkeyObject** prevNext = hd.next;

  Persistent<Integer> p2(p);
  hd = i->GetPersistentHandleData();

  V8MONKEY_CHECK(hd.next != prevNext, "HandleData next changed after another persistent created");
}


V8MONKEY_TEST(IntPersistent010, "InternalIsolate Persistent HandleData changes after persistent creation (from persistent, block full case)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScope h;

  Local<Integer> l(Integer::New(123));
  Persistent<Integer> p(l);

  HandleData hd = i->GetPersistentHandleData();
  V8MonkeyObject** prevNext = hd.next;
  V8MonkeyObject** prevLimit = hd.limit;

  for (int j = 0; j < blockSize + 2; j++) {
    Persistent<Integer> p2(p);
    hd = i->GetPersistentHandleData();

    V8MONKEY_CHECK(hd.next != prevNext, "HandleData next changed after another handle created");
    if (j == blockSize - 1) {
      V8MONKEY_CHECK(hd.limit != prevLimit, "HandleData limit changed after another handle created");
    } else {
      V8MONKEY_CHECK(hd.limit == prevLimit, "HandleData limit not changed after another handle created");
    }

    prevNext = hd.next;
    prevLimit = hd.limit;
  }
}


V8MONKEY_TEST(IntPersistent011, "Persistent refcount changes after persistent creation (from pointer)") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  int refCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

  Persistent<Integer> p(*l);
  int newRefCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

  V8MONKEY_CHECK(newRefCount == refCount + 1, "Refcount bumped");
}


V8MONKEY_TEST(IntPersistent012, "Persistent refcount changes after persistent creation (from local)") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  int refCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();
  Persistent<Integer> p(l);
  int newRefCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

  V8MONKEY_CHECK(newRefCount == refCount + 1, "Refcount bumped");
}


V8MONKEY_TEST(IntPersistent013, "Persistent refcount changes after persistent creation (from persistent)") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Persistent<Integer> p(Integer::New(123));
  int refCount = (*reinterpret_cast<V8MonkeyObject**>(*p))->RefCount();
  Persistent<Integer> p2(p);
  int newRefCount = (*reinterpret_cast<V8MonkeyObject**>(*p))->RefCount();

  V8MONKEY_CHECK(newRefCount == refCount + 1, "Refcount bumped");
}


V8MONKEY_TEST(IntPersistent014, "Persistent refcount changes after persistent destruction") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  int refCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

  {
    Persistent<Integer> p(*l);
  }

  int newRefCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

  V8MONKEY_CHECK(newRefCount == refCount, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent015, "Persistent refcount changes after persistent disposal") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  int refCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();
  Persistent<Integer> p(*l);
  p.Dispose();

  int newRefCount = (*reinterpret_cast<V8MonkeyObject**>(*l))->RefCount();

  V8MONKEY_CHECK(newRefCount == refCount, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent016, "OK to dispose empty persistent") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Persistent<Integer> p;
  p.Dispose();

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent017, "IsWeak reports correct value normally") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  Persistent<Integer> p(*l);

  V8MONKEY_CHECK(!p.IsWeak(), "IsWeak correct");
}


V8MONKEY_TEST(IntPersistent018, "IsWeak reports correct value after weakening") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  Persistent<Integer> p(*l);
  p.MakeWeak(nullptr, nullptr);

  V8MONKEY_CHECK(p.IsWeak(), "IsWeak correct");
}


V8MONKEY_TEST(IntPersistent019, "Possible to call IsWeak on empty") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Persistent<Integer> p;
  p.IsWeak();

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent020, "Possible to call MakeWeak on empty") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Persistent<Integer> p;
  p.MakeWeak(nullptr, nullptr);

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent021, "IsWeak reports correct value after clearing weakness") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  Persistent<Integer> p(*l);
  p.MakeWeak(nullptr, nullptr);
  p.ClearWeak();

  V8MONKEY_CHECK(!p.IsWeak(), "IsWeak correct");
}


V8MONKEY_TEST(IntPersistent022, "Items contained in persistents traced correctly") {
  TestUtils::AutoTestCleanup ac;

  bool traced = false;
  TraceFake* t = new TraceFake(&traced);
  traceFn = nullptr;
  traceData = nullptr;

  // Replace the call to SpiderMonkey with our fake function
  InternalIsolate::SetGCRegistrationHooks(GCRegistrationHook, GCDeregistrationHook);

  Isolate::GetCurrent()->Enter();
  {
    Persistent<Value> p(reinterpret_cast<Value*>(&t));
    traceFn(nullptr, traceData);
  }

  // Deregister our fake gc notifiers
  InternalIsolate::SetGCRegistrationHooks(nullptr, nullptr);
  traceFn = nullptr;

  V8MONKEY_CHECK(traced, "Value was traced");
}


V8MONKEY_TEST(IntPersistent023, "Tracing copes with zeroed slot in handle data") {
  TestUtils::AutoTestCleanup ac;

  bool traced = false;
  TraceFake* bogus = nullptr;
  TraceFake* t = new TraceFake(&traced);
  traceFn = nullptr;
  traceData = nullptr;

  // Replace the call to SpiderMonkey with our fake function
  InternalIsolate::SetGCRegistrationHooks(GCRegistrationHook, GCDeregistrationHook);

  Isolate::GetCurrent()->Enter();
  {
    Persistent<Value> zero(reinterpret_cast<Value*>(&bogus));
    Persistent<Value> p(reinterpret_cast<Value*>(&t));
    traceFn(nullptr, traceData);
  }

  // Deregister our fake gc notifiers
  InternalIsolate::SetGCRegistrationHooks(nullptr, nullptr);
  traceFn = nullptr;

  V8MONKEY_CHECK(traced, "Legitimate value was traced");
}


V8MONKEY_TEST(IntPersistent024, "Containing slot zeroed after disposal") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  Persistent<Integer> p(*l);

  V8MonkeyObject** slot = reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(*slot != 0, "Sanity check");
  p.Dispose();

  V8MONKEY_CHECK(*slot == 0, "Slot was zeroed");
}


V8MONKEY_TEST(IntPersistent025, "Slot zeroed, but object still alive if still held alive elsewhere after disposal") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  bool deleted = false;

  // Fake a local
  DeletionObject* dummy = new DeletionObject(&deleted);
  V8MonkeyObject** slot = HandleScope::CreateHandle(dummy);
  Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
  Local<Value> l(dummySlotAsValue);
  Persistent<Value> p(*l);

  V8MonkeyObject** persistentSlot = reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(*persistentSlot != 0, "Sanity check");
  p.Dispose();

  V8MONKEY_CHECK(*persistentSlot == 0, "Slot was zeroed");
  V8MONKEY_CHECK(!deleted, "Object wasn't deleted");
}


V8MONKEY_TEST(IntPersistent026, "Containing slot zeroed after destruction") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  V8MonkeyObject** slot;
  {
    Persistent<Integer> p(*l);
    slot = reinterpret_cast<V8MonkeyObject**>(*p);
    V8MONKEY_CHECK(*slot != 0, "Sanity check");
  }

  V8MONKEY_CHECK(*slot == 0, "Slot was zeroed");
}


V8MONKEY_TEST(IntPersistent027, "Slot zeroed, but object still alive if still held alive elsewhere after destruction") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  bool deleted = false;

  // Fake a local
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

  V8MONKEY_CHECK(*persistentSlot == 0, "Slot was zeroed");
  V8MONKEY_CHECK(!deleted, "Object wasn't deleted");
}


V8MONKEY_TEST(IntPersistent028, "OK to delete disposed persistent") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  V8MonkeyObject** slot;
  {
    Persistent<Integer> p(*l);
    slot = reinterpret_cast<V8MonkeyObject**>(*p);
    p.Dispose();
  }

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent029, "Objects deleted on isolate teardown") {
  TestUtils::AutoTestCleanup ac;
  bool deleted = false;

  Isolate* i = Isolate::New();
  i->Enter();
  V8MonkeyObject** slot;
  Persistent<Value>* p;

  {
    HandleScope h;
    // Fake a local
    DeletionObject* dummy = new DeletionObject(&deleted);
    V8MonkeyObject** slot = HandleScope::CreateHandle(dummy);
    Value* dummySlotAsValue = reinterpret_cast<Value*>(slot);
    Local<Value> l(dummySlotAsValue);
    p = new Persistent<Value>(*l);
  }

  // At this point, the local has gone out of scope, but not the persistent, so there is still 1 reference to the object
  V8MONKEY_CHECK(!deleted, "Sanity check");

  i->Exit();
  i->Dispose();

  V8MONKEY_CHECK(deleted, "Object was deleted on isolate teardown");
  // We deliberately leak p here. At this point it contains a dangling pointer
}


// XXX FIX ORDER
V8MONKEY_TEST(IntPersistent030, "Assigning value from another persistent decrements old refcount if strong (1)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);
  Persistent<Integer> rc2(*l2);

  printf("Should be from same\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent031, "Assigning value from another persistent decrements old refcount if strong (2)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);
  Persistent<Integer> rc2(*l2);

  printf("Should be from other\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent032, "Assigning value from another persistent decrements old refcount if strong (3)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);

  printf("Should be from handle\n");
  rc1 = l2;
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent033, "Assigning value from another persistent decrements old refcount if strong (4)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);

  printf("Should be from handle\n");
  rc1 = l2;
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent034, "Assigning value from another persistent decrements old refcount if strong (5)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);

  printf("Should be from pointer\n");
  rc1 = *l2;
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent035, "Assigning value from another persistent decrements old refcount if strong (6)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);

  printf("Should be from pointer\n");
  rc1 = *l2;
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent036, "Assigning value from another persistent decrements old refcount if weak (1)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);
  Persistent<Integer> rc2(*l2);
  rc1.MakeWeak(nullptr, nullptr);

  printf("Should be from same\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->WeakCount(), underlyingObject2->WeakCount());
  V8MONKEY_CHECK(underlyingObject->WeakCount() == 0, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent037, "Assigning value from another persistent decrements old refcount if weak (2)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);
  Persistent<Integer> rc2(*l2);
  rc1.MakeWeak(nullptr, nullptr);
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->WeakCount(), underlyingObject2->WeakCount());

  rc1 = rc2;
  underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->WeakCount(), underlyingObject2->WeakCount());
  V8MONKEY_CHECK(underlyingObject->WeakCount() == 0, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent038, "Assigning value from another persistent increments refcount (1)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);
  Persistent<Integer> rc2(*l2);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  printf("Should be from same\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent039, "Assigning value from another persistent increments refcount (2)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);
  Persistent<Integer> rc2(*l2);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  printf("Should be from other\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent040, "Assigning value from another persistent increments refcount (3)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  printf("Should be from handle\n");
  rc1 = l2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent041, "Assigning value from another persistent increments refcount (4)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  printf("Should be from handle\n");
  rc1 = l2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent042, "Assigning value from another persistent increments refcount (5)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  printf("Should be from pointer\n");
  rc1 = *l2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent043, "Assigning value from another persistent increments refcount (6)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  printf("Should be from pointer\n");
  rc1 = *l2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent044, "Assigning value from persistent increments strong refcount when old was weak (1)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);
  Persistent<Integer> rc2(*l2);
  rc1.MakeWeak(nullptr, nullptr);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  printf("Should be from same\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->WeakCount(), underlyingObject2->WeakCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent045, "Assigning value from persistent increments strong refcount when old was weak (2)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);
  Persistent<Integer> rc2(*l2);
  rc1.MakeWeak(nullptr, nullptr);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->WeakCount(), underlyingObject2->WeakCount());

  rc1 = rc2;
  printf("Refcounts are %d and %d\n", underlyingObject->WeakCount(), underlyingObject2->WeakCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent046, "Assignee weak refcount unchanged if assigned to weak persistent (1)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);
  Persistent<Integer> rc2(*l2);
  rc1.MakeWeak(nullptr, nullptr);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int weakCount = underlyingObject->WeakCount();

  printf("Should be from same\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->WeakCount(), underlyingObject2->WeakCount());
  V8MONKEY_CHECK(underlyingObject->WeakCount() == weakCount, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent047, "Assignee weak refcount unchanged if assigned to weak persistent (2)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);
  Persistent<Integer> rc2(*l2);
  rc1.MakeWeak(nullptr, nullptr);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int weakCount = underlyingObject->WeakCount();

  printf("Should be from other\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->WeakCount(), underlyingObject2->WeakCount());
  V8MONKEY_CHECK(underlyingObject->WeakCount() == weakCount, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent048, "Assignee strengthened if weak (1)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Integer> rc1(*l1);
  Persistent<Integer> rc2(*l2);
  rc2.MakeWeak(nullptr, nullptr);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  printf("Should be from same\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->WeakCount(), underlyingObject2->WeakCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent049, "Assignee strengthened if ref (2)") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);
  Local<Integer> l2 = Integer::New(456);

  Persistent<Value> rc1(*l1);
  Persistent<Integer> rc2(*l2);
  rc2.MakeWeak(nullptr, nullptr);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  int refCount = underlyingObject->RefCount();

  printf("Should be from other\n");
  rc1 = rc2;
  V8MonkeyObject* underlyingObject2 = *(reinterpret_cast<V8MonkeyObject**>(*l2));
  printf("Refcounts are %d and %d\n", underlyingObject->RefCount(), underlyingObject2->RefCount());
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount + 1, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent050, "Assigning to self doesn't change strong refcount") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);

  Persistent<Value> rc1(*l1);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  int refCount = underlyingObject->RefCount();

  printf("Should be from same\n");
  rc1 = rc1;
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent051, "Assigning to self doesn't change weak refcount") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);

  Persistent<Value> rc1(*l1);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  int weakCount = underlyingObject->WeakCount();

  printf("Should be from same\n");
  rc1 = rc1;
  V8MONKEY_CHECK(underlyingObject->WeakCount() == weakCount, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent052, "Assigning to self from slot pointer doesn't change strong refcount") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);

  Persistent<Value> rc1(*l1);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  int refCount = underlyingObject->RefCount();

  printf("Should be from pointer\n");
  rc1 = *rc1;
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent053, "Assigning to self from slot pointer doesn't change weak refcount") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);

  Persistent<Value> rc1(*l1);

  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  int weakCount = underlyingObject->WeakCount();

  printf("Should be from pointer\n");
  rc1 = *rc1;
  V8MONKEY_CHECK(underlyingObject->WeakCount() == weakCount, "Refcount correct");
}


V8MONKEY_TEST(IntPersistent054, "Assigning to self doesn't strengthen weak persistent") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);

  Persistent<Value> rc1(*l1);
  rc1.MakeWeak(nullptr, nullptr);
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  int refCount = underlyingObject->RefCount();
  int weakCount = underlyingObject->WeakCount();

  printf("Should be from same\n");
  rc1 = rc1;
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount, "Refcount correct");
  V8MONKEY_CHECK(underlyingObject->WeakCount() == weakCount, "Weakcount correct");
}


V8MONKEY_TEST(IntPersistent055, "Assigning to self from pointer doesn't strengthen weak persistent") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l1 = Integer::New(123);

  Persistent<Value> rc1(*l1);
  rc1.MakeWeak(nullptr, nullptr);
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*l1));
  int refCount = underlyingObject->RefCount();
  int weakCount = underlyingObject->WeakCount();

  printf("Should be from pointer\n");
  rc1 = *rc1;
  V8MONKEY_CHECK(underlyingObject->RefCount() == refCount, "Refcount correct");
  V8MONKEY_CHECK(underlyingObject->WeakCount() == weakCount, "Weakcount correct");
}


V8MONKEY_TEST(IntPersistent056, "Assigning to self doesn't delete") {
  TestUtils::AutoTestCleanup ac;

  Isolate::GetCurrent()->Enter();
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
    p = l;

    // When we exit this block, h will go out of scope, and p will hold the only ref to dummy
  }

  V8MONKEY_CHECK(dummy->RefCount() == 1, "Sanity check");

  p = p;
  V8MONKEY_CHECK(!deleted, "Refcounted object not deleted");
}


V8MONKEY_TEST(IntPersistent057, "Assigning to self from pointer doesn't delete") {
  TestUtils::AutoTestCleanup ac;

  Isolate::GetCurrent()->Enter();
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
    p = l;

    // When we exit this block, h will go out of scope, and p will hold the only ref to dummy
  }

  V8MONKEY_CHECK(dummy->RefCount() == 1, "Sanity check");

  p = *p;
  V8MONKEY_CHECK(!deleted, "Refcounted object not deleted");
}


// XXX Explanatory comment
// XXX Rename
#define CALLBACKTEST(num, message) \
  V8MONKEY_TEST(IntPersistent##num, message) {\
    TestUtils::AutoTestCleanup ac; \
 \
    Isolate::GetCurrent()->Enter(); \
    Persistent<Value> p; \
 \
    { \
      HandleScope h; \
 \
      Local<Value> l(Integer::New(123)); \
      p = l; \
 \
    }


CALLBACKTEST(058, "Weak callback called during tracing") {
  p.MakeWeak(nullptr, weakCB1);
  weakCB1Called = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(weakCB1Called, "Weak callback called");
}}


CALLBACKTEST(059, "Weak callback called not called if strengthened") {
  p.MakeWeak(nullptr, weakCB1);
  p.ClearWeak();
  weakCB1Called = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!weakCB1Called, "Weak callback not called");
}}


CALLBACKTEST(060, "OK to clear weakness in callback") {
  p.MakeWeak(&p, strengthenCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


CALLBACKTEST(061, "Refcount correct after weak callback sequence") {
  Persistent<Value> q = p;
  p.MakeWeak(&p, strengthenCallback);
  q.MakeWeak(&q, weakCB1);

  // We have just created two persistents pointing at the same object, which should have 2 weakrefs. After the GC, we
  // should have 1 strong ref and 1 weak ref (the temporary created for passing to the callbacks might temporarily add
  // a reference, but this should be removed once the GC ends)
  V8MonkeyObject* underlyingObject = *(reinterpret_cast<V8MonkeyObject**>(*p));
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(underlyingObject->RefCount() == 1, "RefCount correct");
  V8MONKEY_CHECK(underlyingObject->WeakCount() == 1, "WeakCount correct");
}}


CALLBACKTEST(062, "OK to dispose via real persistent in callback") {
  // Our callback will receive a pointer to the persistent p. It will dispose through that pointer.
  p.MakeWeak(&p, disposeParamCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


CALLBACKTEST(063, "Disposing in callback via real persistent does not affect other weak references (1)") {
  // This test and the next test both possible orderings
  Persistent<Value> q = p;
  q.MakeWeak(&q, weakCB1);
  p.MakeWeak(&p, disposeParamCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


CALLBACKTEST(064, "Disposing in callback via real persistent does not affect other weak references (2)") {
  Persistent<Value> q = p;
  p.MakeWeak(&p, disposeParamCallback);
  q.MakeWeak(&q, weakCB1);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


V8MONKEY_TEST(IntPersistent065, "OK to delete real persistent in callback") {
  // We don't use the macro here due to the need to create via operator new
  TestUtils::AutoTestCleanup ac;

  Isolate::GetCurrent()->Enter();
  Persistent<Value>* p = new Persistent<Value>;

  {
    HandleScope h;

    // Fake a local, and then assign p to the same object
    Local<Value> l(Integer::New(123));
    *p = l;

    // When we exit this block, h will go out of scope, and p will hold the only ref to the integer
  }
  p->MakeWeak(p, deleteCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent066, "Deleting in callback via real persistent does not affect other weak references (1)") {
  // We don't use the macro here due to the need to create via operator new
  TestUtils::AutoTestCleanup ac;

  Isolate::GetCurrent()->Enter();
  Persistent<Value>* p = new Persistent<Value>;

  {
    HandleScope h;

    // Fake a local, and then assign p to the same object
    Local<Value> l(Integer::New(123));
    *p = l;

    // When we exit this block, h will go out of scope, and p will hold the only ref to the integer
  }
  // This test and the next show that other weakrefs are unaffected, using both possible orders
  Persistent<Value> q = *p;
  q.MakeWeak(&q, weakCB1);
  p->MakeWeak(p, deleteCallback);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(IntPersistent067, "Deleting in callback via real persistent does not affect other weak references (2)") {
  // We don't use the macro here due to the need to create via operator new
  TestUtils::AutoTestCleanup ac;

  Isolate::GetCurrent()->Enter();
  Persistent<Value>* p = new Persistent<Value>;

  {
    HandleScope h;

    // Fake a local, and then assign p to the same object
    Local<Value> l(Integer::New(123));
    *p = l;

    // When we exit this block, h will go out of scope, and p will hold the only ref to the integer
  }
  // This test and the next show that other weakrefs are unaffected, using both possible orders
  Persistent<Value> q = *p;
  p->MakeWeak(p, deleteCallback);
  q.MakeWeak(&q, weakCB1);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}


CALLBACKTEST(068, "OK to dispose via temporary persistent in callback") {
  // Our callback receives a fresh persistent as its first argument. We dispose that object.
  p.MakeWeak(&p, disposeTempCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


CALLBACKTEST(069, "Disposing in callback via temporary persistent does not affect other weak references (1)") {
  // This test and the next test both possible orderings
  Persistent<Value> q = p;
  q.MakeWeak(&q, weakCB1);
  p.MakeWeak(&p, disposeTempCallback);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


CALLBACKTEST(070, "Disposing in callback via temporary persistent does not affect other weak references (2)") {
  Persistent<Value> q = p;
  p.MakeWeak(&p, disposeTempCallback);
  q.MakeWeak(&q, weakCB1);

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}}


CALLBACKTEST(071, "Weak callback called with correct parameters") {
  // We use the TestCleanup object (defined in the macro) as our parameter
  p.MakeWeak(&ac, weakCB1);
  weakCB1Params = nullptr;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(weakCB1Params == &ac, "Weak callback called with correct parameters");
}}


V8MONKEY_TEST(IntPersistent072, "Weakening doesn't delete") {
  TestUtils::AutoTestCleanup ac;

  Isolate::GetCurrent()->Enter();
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
    p = l;

    // When we exit this block, h will go out of scope, and p will hold the only ref to dummy
  }

  V8MONKEY_CHECK(dummy->RefCount() == 1, "Sanity check");

  p.MakeWeak(nullptr, nullptr);
  V8MONKEY_CHECK(!deleted, "Object not deleted");
}


CALLBACKTEST(073, "Weakening correctly adjusts refcount") {
  V8MonkeyObject* object = *reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(object->RefCount() == 1, "Sanity check");
  V8MONKEY_CHECK(object->WeakCount() == 0, "Sanity check");

  p.MakeWeak(nullptr, nullptr);

  V8MONKEY_CHECK(object->RefCount() == 0, "Refcount correct");
  V8MONKEY_CHECK(object->WeakCount() == 1, "Weakcount correct");
}}


CALLBACKTEST(074, "Clearing weakness has no effect on refcounts if not strengthened") {
  V8MonkeyObject* object = *reinterpret_cast<V8MonkeyObject**>(*p);
  V8MONKEY_CHECK(object->RefCount() == 1, "Sanity check");
  V8MONKEY_CHECK(object->WeakCount() == 0, "Sanity check");

  p.ClearWeak();
  V8MONKEY_CHECK(object->RefCount() == 1, "Refcount correct");
  V8MONKEY_CHECK(object->WeakCount() == 0, "Weakcount correct");
}}


CALLBACKTEST(075, "Clearing weakness corrects refcount") {
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


CALLBACKTEST(076, "IsNearDeath normally false") {
  V8MONKEY_CHECK(!p.IsNearDeath(), "IsNearDeath correct");
}}


CALLBACKTEST(077, "IsNearDeath reports correct value in callback") {
  p.MakeWeak(&p, NearDeathParamChecker);
  isNearDeath = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(078, "Equality holds for temporary persistent passed to callback") {
  // The persistent created when invoking the callback should be "equal" to the original
  p.MakeWeak(&p, EqualityChecker);
  wasEqual = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(wasEqual, "Values were equal");
}}


CALLBACKTEST(079, "IsNearDeath reports correct value in callback when called on temporary") {
  // If our persistent is near death (which, in the callback, it will be) then so should the temporary persistent
  // supplied to the callback
  p.MakeWeak(&p, NearDeathTempChecker);
  isNearDeath = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(080, "IsWeak reports correct value in callback when called on temporary") {
  // The temporary persistent supplied to the callback is a proxy for our real value. As our value is weak, the proxy
  // persistent should report it is too
  p.MakeWeak(&p, IsWeakTempChecker);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct");
}}


CALLBACKTEST(081, "IsNearDeath reports false after ClearWeakness call in callback on real persistent") {
  p.MakeWeak(&p, NearDeathClearParamChecker);
  isNearDeath = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(082, "IsNearDeath reports false after ClearWeakness call in callback on temporary persistent") {
  p.MakeWeak(&p, NearDeathClearTempChecker);
  isNearDeath = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(083, "IsNearDeath called on real persistent reports false after ClearWeakness call on temporary") {
  p.MakeWeak(&p, NearDeathClearTempCheckParam);
  isNearDeath = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(084, "IsNearDeath called on temporary reports false after ClearWeakness call on real persistent") {
  p.MakeWeak(&p, NearDeathClearParamCheckTemp);
  isNearDeath = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(085, "IsWeak reports false after ClearWeakness call in callback on real persistent") {
  p.MakeWeak(&p, IsWeakClearParamChecker);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isWeak, "IsWeak correct");
}}


CALLBACKTEST(086, "IsWeak reports false after ClearWeakness call in callback on temporary persistent") {
  p.MakeWeak(&p, IsWeakClearTempChecker);
  isWeak = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isWeak, "IsWeak correct");
}}


CALLBACKTEST(087, "IsWeak called on real persistent reports false after ClearWeakness call on temporary") {
  p.MakeWeak(&p, IsWeakClearTempCheckParam);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isWeak, "IsWeak correct");
}}


CALLBACKTEST(088, "IsWeak called on temporary persistent reports false after ClearWeakness call on real") {
  p.MakeWeak(&p, IsWeakClearParamCheckTemp);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(!isWeak, "IsWeak correct");
}}


CALLBACKTEST(089, "IsWeak reports true for real after clearing and weakening real persistent in callback") {
  p.MakeWeak(&p, IsWeakClearReParam);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct");
}}


CALLBACKTEST(090, "IsWeak reports true for temporary after clearing and weakening temporary persistent in callback") {
  p.MakeWeak(&p, IsWeakClearReTemp);
  isWeak = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct");
}}


CALLBACKTEST(091, "IsNearDeath reports true for real after clearing and weakening real persistent in callback") {
  p.MakeWeak(&p, IsNearDeathClearReParam);
  isNearDeath = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(092, "IsNearDeath reports true for temporary after clearing and weakening it in callback") {
  p.MakeWeak(&p, IsNearDeathClearReTemp);
  isNearDeath = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(093, "IsWeak reports true for temporary after clearing and weakening real persistent in callback") {
  p.MakeWeak(&p, IsWeakClearReParamParamTemp);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct");
}}


CALLBACKTEST(094, "IsNearDeath reports true for temporary after clearing and weakening real persistent in callback") {
  p.MakeWeak(&p, IsNearDeathClearReParamParamTemp);
  isNearDeath = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(095, "IsWeak reports true for real after clearing real and weakening temporary persistent in callback") {
  p.MakeWeak(&p, IsWeakClearReParamTempParam);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct");
}}


CALLBACKTEST(096, "IsNearDeath reports true for real after clearing real and weakening temporary persistent in callback") {
  p.MakeWeak(&p, IsNearDeathClearReParamTempParam);
  isNearDeath = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(097, "IsWeak reports true for real after clearing and weakening temporary persistent in callback") {
  p.MakeWeak(&p, IsWeakClearReTempTempParam);
  isWeak = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct");
}}


CALLBACKTEST(098, "IsNearDeath reports true for real after clearing and weakening temporary persistent in callback") {
  p.MakeWeak(&p, IsNearDeathClearReTempTempParam);
  isNearDeath = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(099, "IsWeak reports true for temporary after clearing temporary and weakening real in callback") {
  p.MakeWeak(&p, IsWeakClearReTempParamTemp);
  isWeak = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isWeak, "IsWeak correct");
}}


CALLBACKTEST(100, "IsNearDeath reports true for temporary after clearing temporary and weakening real in callback") {
  p.MakeWeak(&p, IsNearDeathClearReTempParamTemp);
  isNearDeath = false;
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(isNearDeath, "IsNearDeath correct");
}}


CALLBACKTEST(101, "Disposing temporary in callback zeroes real object slot") {
  p.MakeWeak(&p, TempDisposeChecker);
  wasZeroed = false;

  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(wasZeroed, "Slot was zeroed");
}}


/*
// What do we want to do? We want to show that after a ClearWeakness on the temporary, IsWeak on the original is false
V8MONKEY_TEST(IntPersistent060, "OK to clear weakness in callback") {
  TestUtils::AutoTestCleanup ac;

  Isolate::GetCurrent()->Enter();
  Persistent<Value> p;

  {
    HandleScope h;

    // Fake a local, and then assign p to the same object
    Local<Value> l(Integer::New(123));
    p = l;

    // When we exit this block, h will go out of scope, and p will hold the only ref to the integer
  }
  p.MakeWeak(&p, strengthenCallback);
  InternalIsolate::ForceGC();

  V8MONKEY_CHECK(true, "Didn't crash");
}
*/


/* FIXME
V8MONKEY_TEST(IntPersistent029, "Ob") {
  TestUtils::AutoTestCleanup ac;
  Isolate::GetCurrent()->Enter();
  HandleScope h;

  Local<Integer> l = Integer::New(123);
  V8MonkeyObject** slot;
  {
    Persistent<Integer> p(*l);
    slot = reinterpret_cast<V8MonkeyObject**>(*p);
    p.Dispose();
  }

  V8MONKEY_CHECK(true, "Didn't crash");
}
*/


V8MONKEY_TEST(IntPersistent998, "Alignment of pointers is suitable for tagging pointers") {
  // TODO This should be some sort of static assert
  // TODO Also alignof support is patchy. That computation should be in a platform-specific macro
  V8MONKEY_CHECK(alignof(Persistent<Value>*) > 1, "Alignment suitable");
}


V8MONKEY_TEST(IntPersistent999, "Persistent is same size as pointer") {
  // TODO This should be some sort of static assert
  V8MONKEY_CHECK(sizeof(Persistent<Integer>) <= sizeof(void*), "Persistent fits in a pointer");
}


// XXX Slot zeroed after destroying trace
// XXX Tracing
// XXX Safe to call dispose/destroy after tracing destruction
// XXX Think Dispose and ClearWeakness on temporary should affect original
