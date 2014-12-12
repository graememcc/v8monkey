#include "v8.h"

#include "data_structures/objectblock.h"
#include "isolate.h"
#include "runtime/handlescope.h"
#include "types/base_types.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;
using namespace v8::V8Monkey;

namespace {
  static bool wasGCOnNotified = false;


  static JSTraceDataOp traceFn = nullptr;
  static void* traceData = nullptr;


  void gcOnNotifier(JSRuntime* rt, JSTraceDataOp op, void* data) {
    wasGCOnNotified = true;
    traceFn = op;
    traceData = data;
  }


  static bool wasGCOffNotified = false;


  void gcOffNotifier(JSRuntime* rt, JSTraceDataOp op, void* data) {
    wasGCOffNotified = true;
  }
}


V8MONKEY_TEST(IntHandleScope001, "InternalIsolate HandleScopeData next initially null") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == NULL, "HandleScopeData next initially null");
}


V8MONKEY_TEST(IntHandleScope002, "InternalIsolate HandleScopeData limit initially null") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.limit == NULL, "HandleScopeData limit initially null");
}


V8MONKEY_TEST(IntHandleScope003, "InternalIsolate HandleScopeData changes after handle creation (null case)") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  HandleScope::CreateHandle(d);
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != NULL, "HandleScopeData next changed after first handle created");
  V8MONKEY_CHECK(hsd.limit != NULL, "HandleScopeData limit changed after first handle created");
}


V8MONKEY_TEST(IntHandleScope004, "InternalIsolate HandleScopeData changes after handle creation (usual case)") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  HandleScope::CreateHandle(d);
  HandleScopeData hsd = i->GetHandleScopeData();
  V8MonkeyObject** prevNext = hsd.next;

  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  HandleScope::CreateHandle(e);

  hsd = i->GetHandleScopeData();
  V8MONKEY_CHECK(hsd.next != prevNext, "HandleScopeData next changed after another handle created");
}


V8MONKEY_TEST(IntHandleScope005, "InternalIsolate HandleScopeData changes after handle creation (block full case)") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  HandleScopeData hsd = i->GetHandleScopeData();
  V8MonkeyObject** prevNext = hsd.next;
  V8MonkeyObject** prevLimit = hsd.limit;

  for (int j = 0; j < ObjectBlock<DummyV8MonkeyObject>::EffectiveBlockSize + 2; j++) {
    HandleScope::CreateHandle(new DummyV8MonkeyObject);
    HandleScopeData hsd = i->GetHandleScopeData();

    V8MONKEY_CHECK(hsd.next != prevNext, "HandleScopeData next changed after another handle created");
    if (j == 0 || j == ObjectBlock<DummyV8MonkeyObject>::EffectiveBlockSize) {
      V8MONKEY_CHECK(hsd.limit != prevLimit, "HandleScopeData limit changed after another handle created");
    } else {
      V8MONKEY_CHECK(hsd.limit == prevLimit, "HandleScopeData limit not changed after another handle created");
    }

    prevNext = hsd.next;
    prevLimit = hsd.limit;
  }
}


V8MONKEY_TEST(IntHandleScope006, "Object refcount doesn't drop after Handle deletion") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  // Add an addref so it doesn't die on us
  d->AddRef();

  HandleScope h;
  {
    HandleScope::CreateHandle(reinterpret_cast<V8MonkeyObject*>(d));
  }

  V8MONKEY_CHECK(d->RefCount() == 2, "Refcount doesn't drop on Handle destruction");
  d->Release();
}


V8MONKEY_TEST(IntHandleScope007, "Object refcount increases on handle creation") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  {
    HandleScope h;
    HandleScope::CreateHandle(d);

    V8MONKEY_CHECK(d->RefCount() == 1, "Refcount increased on HandleScope creation");
  }
}


V8MONKEY_TEST(IntHandleScope008, "Refcount correct when handle to object created in different scopes") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  {
    HandleScope h;
    HandleScope::CreateHandle(d);
    V8MONKEY_CHECK(d->RefCount() == 1, "Refcount increased on first handlescope addition");
    {
      HandleScope h;
      HandleScope::CreateHandle(d);
      V8MONKEY_CHECK(d->RefCount() == 2, "Refcount increased on second handlescope addition");
    }
  }
}


V8MONKEY_TEST(IntHandleScope009, "Object refcount drops on handlescope deletion") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  // Add an addref so it doesn't die on us
  d->AddRef();

  {
    HandleScope h;
    {
      HandleScope::CreateHandle(d);
    }
  }

  V8MONKEY_CHECK(d->RefCount() == 1, "Refcount drops on HandleScope destruction");
  d->Release();
}


V8MONKEY_TEST(IntHandleScope010, "Refcount correct when handle to object created in different scopes and scopes die") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  // Add an addref so it doesn't die on us
  d->AddRef();

  {
    HandleScope h;
    HandleScope::CreateHandle(d);
    {
      HandleScope j;
      HandleScope::CreateHandle(d);
    }
    V8MONKEY_CHECK(d->RefCount() == 2, "Refcount drops on first HandleScope destruction");
  }

  V8MONKEY_CHECK(d->RefCount() == 1, "Refcount drops on second HandleScope destruction");
  d->Release();
}


V8MONKEY_TEST(IntHandleScope011, "InternalIsolate HandleScopeData restored correctly after handlescope deletion") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;
    DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
    HandleScope::CreateHandle(d);
  }

  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == NULL, "HandleScopeData next restored");
  V8MONKEY_CHECK(hsd.limit == NULL, "HandleScopeData limit restored");
}


V8MONKEY_TEST(IntHandleScope012, "InternalIsolate HandleScopeData restored correctly after handlescope deletion (2)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;
    for (int j = 0; j < ObjectBlock<DummyV8MonkeyObject>::EffectiveBlockSize + 2; j++) {
      HandleScope::CreateHandle(new DummyV8MonkeyObject);
    }
  }

  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == NULL, "HandleScopeData next restored");
  V8MONKEY_CHECK(hsd.limit == NULL, "HandleScopeData limit restored");
}


V8MONKEY_TEST(IntHandleScope013, "HandleScopeData restored correctly from multiple handlescope deletions (1)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;
    DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
    HandleScope::CreateHandle(d);

    HandleScopeData hsd = i->GetHandleScopeData();
    V8MonkeyObject** next = hsd.next;
    V8MonkeyObject** limit = hsd.limit;
    {
      HandleScope j;
      HandleScope::CreateHandle(d);
    }

    hsd = i->GetHandleScopeData();
    V8MONKEY_CHECK(hsd.next == next, "Second HandleScopeData next restored");
    V8MONKEY_CHECK(hsd.limit == limit, "Second HandleScopeData limit restored");
  }

  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == NULL, "First HandleScopeData next restored");
  V8MONKEY_CHECK(hsd.limit == NULL, "First HandleScopeData limit restored");
}


V8MONKEY_TEST(IntHandleScope014, "HandleScopeData restored correctly from multiple handlescope deletions (2)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;
    DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
    HandleScope::CreateHandle(d);

    HandleScopeData hsd = i->GetHandleScopeData();
    V8MonkeyObject** next = hsd.next;
    V8MonkeyObject** limit = hsd.limit;
    {
      HandleScope j;
      for (int k = 0; k < ObjectBlock<DummyV8MonkeyObject>::EffectiveBlockSize + 2; k++) {
        HandleScope::CreateHandle(d);
      }
    }

    hsd = i->GetHandleScopeData();
    V8MONKEY_CHECK(hsd.next == next, "Second HandleScopeData next restored");
    V8MONKEY_CHECK(hsd.limit == limit, "Second HandleScopeData limit restored");
  }

  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == NULL, "First HandleScopeData next restored");
  V8MONKEY_CHECK(hsd.limit == NULL, "First HandleScopeData limit restored");
}


V8MONKEY_TEST(IntHandleScope015, "Handle creation returns correct pointer") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  {
    HandleScope h;
    V8MONKEY_CHECK(*HandleScope::CreateHandle(d) == d, "Pointer correct");
  }
}

// XXX FIXME
/*
V8MONKEY_TEST(IntHandleScope016, "Closing HandleScope returns Local for same object when closing to null") {
  TestUtils::AutoTestCleanup ac;

  Local<DummyV8MonkeyObject> escaped;
  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  {
    HandleScope h;
    DummyV8MonkeyObject* ptr = static_cast<DummyV8MonkeyObject*>(*HandleScope::CreateHandle(d));
    Local<DummyV8MonkeyObject> l(ptr);
    escaped = h.Close(l);
  }

  V8MONKEY_CHECK(*escaped == d, "Value escaped correctly");
}


V8MONKEY_TEST(IntHandleScope017, "Closing HandleScope doesn't lose reference count when closing to null") {
  TestUtils::AutoTestCleanup ac;

  static bool deleted = true;
  class DontDeleteMe : public V8MonkeyObject {
    public:
      DontDeleteMe() {}
      ~DontDeleteMe() { deleted = true; }
      void Trace(JSRuntime* runtime, JSTracer* tracer) {}
  };

  Local<DontDeleteMe> escaped;
  DontDeleteMe* d = new DontDeleteMe;

  {
    HandleScope h;
    DontDeleteMe* ptr = static_cast<DontDeleteMe*>(*HandleScope::CreateHandle(d));
    Local<DontDeleteMe> l(ptr);
    escaped = h.Close(l);
  }

  V8MONKEY_CHECK(!deleted, "Value didn't lose reference count");
}


V8MONKEY_TEST(IntHandleScope018, "Closing HandleScope adjusts previous pointers (null case)") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  {
    HandleScope h;
    DummyV8MonkeyObject* ptr = static_cast<DummyV8MonkeyObject*>(*HandleScope::CreateHandle(d));
    Local<DummyV8MonkeyObject> l(ptr);
    escaped = h.Close(l);
  }

  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != NULL, "HandleScopeData next changed after escaping");
  V8MONKEY_CHECK(hsd.limit != NULL, "HandleScopeData limit changed after escaping");
}


V8MONKEY_TEST(IntHandleScope019, "Closing HandleScope returns Local for same object when closing to earlier scope") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  HandleScope h;
  HandleScope::CreateHandle(d);

  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  {
    HandleScope i;
    DummyV8MonkeyObject* ptr = static_cast<DummyV8MonkeyObject*>(*HandleScope::CreateHandle(e));
    Local<DummyV8MonkeyObject> l(ptr);
    escaped = i.Close(l);
  }

  V8MONKEY_CHECK(*escaped == e, "Value escaped correctly");
}


V8MONKEY_TEST(IntHandleScope020, "Closing HandleScope doesn't lose reference count when closing to earlier") {
  TestUtils::AutoTestCleanup ac;

  HandleScope h;
  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  HandleScope::CreateHandle(d);

  static bool deleted = true;
  class DontDeleteMe : public V8MonkeyObject {
    public:
      DontDeleteMe() {}
      ~DontDeleteMe() { deleted = true; }
      void Trace(JSRuntime* runtime, JSTracer* tracer) {}
  };

  DontDeleteMe* e = new DontDeleteMe;
  Local<DontDeleteMe> escaped;

  {
    HandleScope i;
    DontDeleteMe* ptr = static_cast<DontDeleteMe*>(*HandleScope::CreateHandle(e));
    Local<DontDeleteMe> l(ptr);
    escaped = i.Close(l);
  }

  V8MONKEY_CHECK(!deleted, "Value didn't lose reference count");
}


V8MONKEY_TEST(IntHandleScope021, "Closing HandleScope adjusts previous pointers (typical case)") {
  TestUtils::AutoTestCleanup ac;

  HandleScope h;
  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  HandleScope::CreateHandle(d);

  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MonkeyObject** prevNext = hsd.next;
  V8MonkeyObject** limit = hsd.limit;

  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  {
    HandleScope i;
    DummyV8MonkeyObject* ptr = static_cast<DummyV8MonkeyObject*>(*HandleScope::CreateHandle(e));
    Local<DummyV8MonkeyObject> l(ptr);
    escaped = i.Close(l);
  }

  hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != prevNext, "HandleScopeData next changed after escaping");
  V8MONKEY_CHECK(hsd.limit == limit, "HandleScopeData limit unchanged after escaping");
}


V8MONKEY_TEST(IntHandleScope022, "Closing HandleScope returns Local for same object when closing to full scope") {
  TestUtils::AutoTestCleanup ac;
  const int blockSize = ObjectBlock<V8MonkeyObject>::EffectiveBlockSize;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  HandleScope h;

  for (int i = 0; i < blockSize; i++) {
    HandleScope::CreateHandle(d);
  }

  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  {
    HandleScope i;
    DummyV8MonkeyObject* ptr = static_cast<DummyV8MonkeyObject*>(*HandleScope::CreateHandle(e));
    Local<DummyV8MonkeyObject> l(ptr);
    escaped = i.Close(l);
  }

  V8MONKEY_CHECK(*escaped == e, "Value escaped correctly");
}


V8MONKEY_TEST(IntHandleScope023, "Closing HandleScope doesn't lose reference count when closing to full scope") {
  TestUtils::AutoTestCleanup ac;
  const int blockSize = ObjectBlock<V8MonkeyObject>::EffectiveBlockSize;

  HandleScope h;
  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  for (int i = 0; i < blockSize; i++) {
    HandleScope::CreateHandle(d);
  }

  static bool deleted = true;
  class DontDeleteMe : public V8MonkeyObject {
    public:
      DontDeleteMe() {}
      ~DontDeleteMe() { deleted = true; }
      void Trace(JSRuntime* runtime, JSTracer* tracer) {}
  };

  DontDeleteMe* e = new DontDeleteMe;
  Local<DontDeleteMe> escaped;

  {
    HandleScope i;
    DontDeleteMe* ptr = static_cast<DontDeleteMe*>(*HandleScope::CreateHandle(e));
    Local<DontDeleteMe> l(ptr);
    escaped = i.Close(l);
  }

  V8MONKEY_CHECK(!deleted, "Value didn't lose reference count");
}


V8MONKEY_TEST(IntHandleScope024, "Closing HandleScope adjusts previous pointers (full case)") {
  TestUtils::AutoTestCleanup ac;
  const int blockSize = ObjectBlock<V8MonkeyObject>::EffectiveBlockSize;

  HandleScope h;
  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  for (int i = 0; i < blockSize; i++) {
    HandleScope::CreateHandle(d);
  }

  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MonkeyObject** prevNext = hsd.next;
  V8MonkeyObject** limit = hsd.limit;

  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  {
    HandleScope i;
    DummyV8MonkeyObject* ptr = static_cast<DummyV8MonkeyObject*>(*HandleScope::CreateHandle(e));
    Local<DummyV8MonkeyObject> l(ptr);
    escaped = i.Close(l);
  }

  hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != prevNext, "HandleScopeData next changed after escaping");
  V8MONKEY_CHECK(hsd.limit != limit, "HandleScopeData limit unchanged after escaping");
}
*/


V8MONKEY_TEST(IntHandleScope025, "Transitioning from one empty handlescope to another works") {
  TestUtils::AutoTestCleanup ac;

  HandleScope h;
  {
    HandleScope i;
  }

  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == NULL, "Moving from empty handlescope to empty handlescope works");
  V8MONKEY_CHECK(hsd.limit == NULL, "Moving from empty handlescope to empty handlescope works");
}


V8MONKEY_TEST(IntHandleScope026, "Adding first handle notifies SpiderMonkey about GC") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  {
    InternalIsolate::SetGCNotifier(gcOnNotifier, gcOffNotifier);
    wasGCOnNotified = false;
    HandleScope h;
    HandleScope::CreateHandle(d);
    V8MONKEY_CHECK(wasGCOnNotified, "Adding first handle notifies");
    InternalIsolate::SetGCNotifier(nullptr, nullptr);
  }
  traceFn = nullptr;
}


V8MONKEY_TEST(IntHandleScope027, "Adding later handles dont notify") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  {
    HandleScope h;
    HandleScope::CreateHandle(d);

    wasGCOnNotified = false;
    InternalIsolate::SetGCNotifier(gcOnNotifier, gcOffNotifier);
    HandleScope::CreateHandle(d);

    V8MONKEY_CHECK(!wasGCOnNotified, "Adding later handles don't notify");
    InternalIsolate::SetGCNotifier(nullptr, nullptr);
  }
  traceFn = nullptr;
}


V8MONKEY_TEST(IntHandleScope028, "Removing last handle notifies SpiderMonkey about GC") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  {
    InternalIsolate::SetGCNotifier(gcOnNotifier, gcOffNotifier);
    HandleScope h;
    HandleScope::CreateHandle(d);
    wasGCOffNotified = false;
  }
  V8MONKEY_CHECK(wasGCOffNotified, "Removing last handle notifies");
  InternalIsolate::SetGCNotifier(nullptr, nullptr);
  traceFn = nullptr;
}


V8MONKEY_TEST(IntHandleScope029, "Removing handle other than last doesn't notify SpiderMonkey about GC") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  {
    HandleScope h;
    HandleScope::CreateHandle(d);
    {
      HandleScope i;
      HandleScope::CreateHandle(d);
      wasGCOffNotified = false;
      InternalIsolate::SetGCNotifier(gcOnNotifier, gcOffNotifier);
    }
    V8MONKEY_CHECK(!wasGCOffNotified, "Removing handle doesn't notify");
    InternalIsolate::SetGCNotifier(nullptr, nullptr);
    traceFn = nullptr;
  }
}


V8MONKEY_TEST(IntHandleScope030, "Items traced correctly") {
  TestUtils::AutoTestCleanup ac;

  static bool traced = false;
  class TraceFake : public V8MonkeyObject {
    public:
      TraceFake() {}
      ~TraceFake() {}
      void Trace(JSRuntime* runtime, JSTracer* tracer) {traced = true; }
  };

  TraceFake* t = new TraceFake;
  traceFn = nullptr;
  traceData = nullptr;

  {
    HandleScope h;
    InternalIsolate::SetGCNotifier(gcOnNotifier, gcOffNotifier);
    HandleScope::CreateHandle(t);
    traceFn(nullptr, traceData);
    InternalIsolate::SetGCNotifier(nullptr, nullptr);
    traceFn = nullptr;
  }

  V8MONKEY_CHECK(traced, "Value was traced");
}
