#include "v8.h"

#include "data_structures/objectblock.h"
#include "runtime/isolate.h"
#include "types/base_types.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;
using namespace v8::V8Monkey;

namespace {
  int errorCaught = 0;
  void fatalErrorHandler(const char* location, const char* message) {
    errorCaught = 1;
  }


  const int blockSize = ObjectBlock<DummyV8MonkeyObject>::BlockSize;


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

  V8MONKEY_CHECK(hsd.next == nullptr, "HandleScopeData next initially null");
}


V8MONKEY_TEST(IntHandleScope002, "InternalIsolate HandleScopeData limit initially null") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.limit == nullptr, "HandleScopeData limit initially null");
}


V8MONKEY_TEST(IntHandleScope003, "InternalIsolate HandleScopeData changes after handle creation (null case)") {
  TestUtils::AutoTestCleanup ac;
  HandleScope h;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  HandleScope::CreateHandle(d);
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != nullptr, "HandleScopeData next changed after first handle created");
  V8MONKEY_CHECK(hsd.limit != nullptr, "HandleScopeData limit changed after first handle created");
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

  for (int j = 0; j < blockSize + 2; j++) {
    HandleScope::CreateHandle(new DummyV8MonkeyObject);
    HandleScopeData hsd = i->GetHandleScopeData();

    V8MONKEY_CHECK(hsd.next != prevNext, "HandleScopeData next changed after another handle created");
    if (j == 0 || j == blockSize) {
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
      HandleScope i;
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
      HandleScope i;
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

  V8MONKEY_CHECK(hsd.next == nullptr, "HandleScopeData next restored");
  V8MONKEY_CHECK(hsd.limit == nullptr, "HandleScopeData limit restored");
}


V8MONKEY_TEST(IntHandleScope012, "InternalIsolate HandleScopeData restored correctly after handlescope deletion (2)") {
  TestUtils::AutoTestCleanup ac;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  {
    HandleScope h;
    for (int j = 0; j < blockSize + 2; j++) {
      HandleScope::CreateHandle(new DummyV8MonkeyObject);
    }
  }

  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == nullptr, "HandleScopeData next restored");
  V8MONKEY_CHECK(hsd.limit == nullptr, "HandleScopeData limit restored");
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

  V8MONKEY_CHECK(hsd.next == nullptr, "First HandleScopeData next restored");
  V8MONKEY_CHECK(hsd.limit == nullptr, "First HandleScopeData limit restored");
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
      for (int k = 0; k < blockSize + 2; k++) {
        HandleScope::CreateHandle(d);
      }
    }

    hsd = i->GetHandleScopeData();
    V8MONKEY_CHECK(hsd.next == next, "Second HandleScopeData next restored");
    V8MONKEY_CHECK(hsd.limit == limit, "Second HandleScopeData limit restored");
  }

  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == nullptr, "First HandleScopeData next restored");
  V8MONKEY_CHECK(hsd.limit == nullptr, "First HandleScopeData limit restored");
}


V8MONKEY_TEST(IntHandleScope015, "Handle creation returns correct pointer") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  {
    HandleScope h;
    V8MONKEY_CHECK(*HandleScope::CreateHandle(d) == d, "Pointer correct");
  }
}


// Tests the case where the value to escape is at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope016, "Closing HandleScope returns Local for same object when closing to null (1)") {
  TestUtils::AutoTestCleanup ac;

  Local<DummyV8MonkeyObject> escaped;
  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  HandleScope h;

  {
    HandleScope i;
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(d));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));

    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(*(reinterpret_cast<DummyV8MonkeyObject**>(*escaped)) == d, "Value escaped correctly");
}


// Tests the case where the value to escape is not at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope017, "Closing HandleScope returns Local for same object when closing to null (2)") {
  TestUtils::AutoTestCleanup ac;

  Local<DummyV8MonkeyObject> escaped;
  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;

  HandleScope h;
  {
    HandleScope i;
    HandleScope::CreateHandle(d);
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));
    HandleScope::CreateHandle(d);

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));

    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(*(reinterpret_cast<DummyV8MonkeyObject**>(*escaped)) == e, "Value escaped correctly");
}


// Tests the case where the value to escape is at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope018, "Closing HandleScope doesn't lose reference count when closing to null (1)") {
  TestUtils::AutoTestCleanup ac;

  bool deleted = false;
  Local<DeletionObject> escaped;
  DeletionObject* d = new DeletionObject(&deleted);

  HandleScope h;
  {
    HandleScope i;
    DeletionObject** hsPointer = reinterpret_cast<DeletionObject**>(HandleScope::CreateHandle(d));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DeletionObject> handle(*(reinterpret_cast<DeletionObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(!deleted, "Value didn't lose reference count");
}


// Tests the case where the value to escape is not at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope019, "Closing HandleScope doesn't lose reference count when closing to null (2)") {
  TestUtils::AutoTestCleanup ac;

  bool deleted = false;
  Local<DeletionObject> escaped;
  DeletionObject* d = new DeletionObject(nullptr);
  DeletionObject* e = new DeletionObject(&deleted);

  HandleScope h;
  {
    HandleScope i;

    HandleScope::CreateHandle(d);
    DeletionObject** hsPointer = reinterpret_cast<DeletionObject**>(HandleScope::CreateHandle(e));
    HandleScope::CreateHandle(d);

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DeletionObject> handle(*(reinterpret_cast<DeletionObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(!deleted, "Value didn't lose reference count");
}


// Tests the case where the value to escape is at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope020, "Closing HandleScope adjusts previous pointers (null case) (1)") {
  TestUtils::AutoTestCleanup ac;

  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == nullptr, "Sanity check (1)");
  V8MONKEY_CHECK(hsd.limit == nullptr, "Sanity check (2)");

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  HandleScope h;
  {
    HandleScope i;

    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(d));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));

    escaped = i.Close(handle);
  }

  i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != nullptr, "HandleScopeData next changed after escaping");
  V8MONKEY_CHECK(hsd.limit != nullptr, "HandleScopeData limit changed after escaping");
}


// Tests the case where the value to escape is not at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope021, "Closing HandleScope adjusts previous pointers (null case) (2)") {
  TestUtils::AutoTestCleanup ac;

  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == nullptr, "Sanity check (1)");
  V8MONKEY_CHECK(hsd.limit == nullptr, "Sanity check (2)");

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  HandleScope h;
  {
    HandleScope i;

    HandleScope::CreateHandle(d);
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));
    HandleScope::CreateHandle(d);

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));

    escaped = i.Close(handle);
  }

  i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != nullptr, "HandleScopeData next changed after escaping");
  V8MONKEY_CHECK(hsd.limit != nullptr, "HandleScopeData limit changed after escaping");
}


// Tests the case where the value to escape is at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope022, "Closing HandleScope returns Local for same object when closing to earlier scope (1)") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  HandleScope h;
  HandleScope::CreateHandle(d);

  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  {
    HandleScope i;
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(*(reinterpret_cast<DummyV8MonkeyObject**>(*escaped)) == e, "Value escaped correctly");
}


// Tests the case where the value to escape is not at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope023, "Closing HandleScope returns Local for same object when closing to earlier scope (2)") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  HandleScope h;
  HandleScope::CreateHandle(d);

  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  {
    HandleScope i;
    HandleScope::CreateHandle(d);
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(*(reinterpret_cast<DummyV8MonkeyObject**>(*escaped)) == e, "Value escaped correctly");
}


// Tests the case where the value to escape is at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope024, "Closing HandleScope doesn't lose reference count when closing to earlier (1)") {
  TestUtils::AutoTestCleanup ac;

  bool deleted = false;

  HandleScope h;
  DeletionObject* d = new DeletionObject(nullptr);
  HandleScope::CreateHandle(d);

  DeletionObject* e = new DeletionObject(&deleted);
  Local<DeletionObject> escaped;

  {
    HandleScope i;
    DeletionObject** hsPointer = reinterpret_cast<DeletionObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DeletionObject> handle(*(reinterpret_cast<DeletionObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(!deleted, "Value didn't lose reference count");
}


// Tests the case where the value to escape is not at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope025, "Closing HandleScope doesn't lose reference count when closing to earlier (2)") {
  TestUtils::AutoTestCleanup ac;

  bool deleted = false;

  HandleScope h;
  DeletionObject* d = new DeletionObject(nullptr);
  HandleScope::CreateHandle(d);

  DeletionObject* e = new DeletionObject(&deleted);
  Local<DeletionObject> escaped;

  {
    HandleScope i;
    HandleScope::CreateHandle(d);
    DeletionObject** hsPointer = reinterpret_cast<DeletionObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DeletionObject> handle(*(reinterpret_cast<DeletionObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(!deleted, "Value didn't lose reference count");
}


// Tests the case where the value to escape is at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope026, "Closing HandleScope adjusts previous pointers when closing to previous scope (1)") {
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
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != prevNext, "HandleScopeData next changed after escaping");
  V8MONKEY_CHECK(hsd.limit == limit, "HandleScopeData limit unchanged after escaping");
}


// Tests the case where the value to escape is not at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope027, "Closing HandleScope adjusts previous pointers when closing to previous scope (2)") {
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
    HandleScope::CreateHandle(d);
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != prevNext, "HandleScopeData next changed after escaping");
  V8MONKEY_CHECK(hsd.limit == limit, "HandleScopeData limit unchanged after escaping");
}


// Tests the case where the value to escape is at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope028, "Closing HandleScope returns Local for same object when closing to full scope (1)") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  HandleScope h;

  // Fill up a block
  for (int i = 0; i < blockSize; i++) {
    HandleScope::CreateHandle(d);
  }

  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  {
    HandleScope i;
    HandleScope::CreateHandle(d);
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(*(reinterpret_cast<DummyV8MonkeyObject**>(*escaped)) == e, "Value escaped correctly");
}


// Tests the case where the value to escape is not at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope029, "Closing HandleScope returns Local for same object when closing to full scope (2)") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;
  HandleScope h;

  // Fill up a block
  for (int i = 0; i < blockSize; i++) {
    HandleScope::CreateHandle(d);
  }

  DummyV8MonkeyObject* e = new DummyV8MonkeyObject;
  Local<DummyV8MonkeyObject> escaped;

  {
    HandleScope i;
    HandleScope::CreateHandle(d);
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(*(reinterpret_cast<DummyV8MonkeyObject**>(*escaped)) == e, "Value escaped correctly");
}


// Tests the case where the value to escape is at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope030, "Closing HandleScope doesn't lose reference count when closing to full scope (1)") {
  TestUtils::AutoTestCleanup ac;

  bool deleted = false;

  HandleScope h;
  DeletionObject* d = new DeletionObject(nullptr);

  // Fill a block
  for (int i = 0; i < blockSize; i++) {
    HandleScope::CreateHandle(d);
  }

  DeletionObject* e = new DeletionObject(&deleted);
  Local<DeletionObject> escaped;

  {
    HandleScope i;
    DeletionObject** hsPointer = reinterpret_cast<DeletionObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DeletionObject> handle(*(reinterpret_cast<DeletionObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(!deleted, "Value didn't lose reference count");
}


// Tests the case where the value to escape is not at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope031, "Closing HandleScope doesn't lose reference count when closing to full scope (2)") {
  TestUtils::AutoTestCleanup ac;

  bool deleted = false;

  HandleScope h;
  DeletionObject* d = new DeletionObject(nullptr);

  // Fill a block
  for (int i = 0; i < blockSize; i++) {
    HandleScope::CreateHandle(d);
  }

  DeletionObject* e = new DeletionObject(&deleted);
  Local<DeletionObject> escaped;

  {
    HandleScope i;
    HandleScope::CreateHandle(d);
    DeletionObject** hsPointer = reinterpret_cast<DeletionObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DeletionObject> handle(*(reinterpret_cast<DeletionObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(!deleted, "Value didn't lose reference count");
}


// Tests the case where the value to escape is at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope032, "Closing HandleScope adjusts previous pointers when previous block full (1)") {
  TestUtils::AutoTestCleanup ac;

  HandleScope h;
  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  // Fill up the block in this handlescope
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
    HandleScope::CreateHandle(e);
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != prevNext, "HandleScopeData next changed after escaping");
  V8MONKEY_CHECK(hsd.limit != limit, "HandleScopeData limit unchanged after escaping");
}


// Tests the case where the value to escape is not at the beginning of the closing scope
V8MONKEY_TEST(IntHandleScope033, "Closing HandleScope adjusts previous pointers when previous block full (2)") {
  TestUtils::AutoTestCleanup ac;

  HandleScope h;
  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  // Fill up the block in this handlescope
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
    HandleScope::CreateHandle(d);
    HandleScope::CreateHandle(e);
    DummyV8MonkeyObject** hsPointer = reinterpret_cast<DummyV8MonkeyObject**>(HandleScope::CreateHandle(e));

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DummyV8MonkeyObject> handle(*(reinterpret_cast<DummyV8MonkeyObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != prevNext, "HandleScopeData next changed after escaping");
  V8MONKEY_CHECK(hsd.limit != limit, "HandleScopeData limit unchanged after escaping");
}


V8MONKEY_TEST(IntHandleScope034, "Transitioning from one empty handlescope to another works") {
  TestUtils::AutoTestCleanup ac;

  HandleScope h;
  {
    HandleScope i;
  }

  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next == nullptr, "Moving from empty handlescope to empty handlescope works");
  V8MONKEY_CHECK(hsd.limit == nullptr, "Moving from empty handlescope to empty handlescope works");
}


V8MONKEY_TEST(IntHandleScope035, "Adding first handle notifies SpiderMonkey about GC") {
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


V8MONKEY_TEST(IntHandleScope036, "Adding later handles dont notify") {
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


V8MONKEY_TEST(IntHandleScope037, "Removing last handle notifies SpiderMonkey about GC") {
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


V8MONKEY_TEST(IntHandleScope038, "Removing handle other than last doesn't notify SpiderMonkey about GC") {
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


V8MONKEY_TEST(IntHandleScope039, "Items contained in handlescopes traced correctly") {
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
    // Replace the call to SpiderMonkey with our fake function
    InternalIsolate::SetGCNotifier(gcOnNotifier, gcOffNotifier);

    HandleScope::CreateHandle(t);
    traceFn(nullptr, traceData);

    InternalIsolate::SetGCNotifier(nullptr, nullptr);
    traceFn = nullptr;
  }

  V8MONKEY_CHECK(traced, "Value was traced");
}


V8MONKEY_TEST(IntHandleScope040, "Closing HandleScope doesn't increase refcount of escaped value") {
  TestUtils::AutoTestCleanup ac;

  Local<DeletionObject> escaped;
  DeletionObject* d = new DeletionObject(nullptr);
  int refCount;

  HandleScope h;
  {
    HandleScope i;
    DeletionObject** hsPointer = reinterpret_cast<DeletionObject**>(HandleScope::CreateHandle(d));
    refCount = d->RefCount();

    // In real V8Monkey usage, all API type pointers are really double pointers to the internal representation.
    // It makes no difference to this test, but we emulate that style of usage here.
    Handle<DeletionObject> handle(*(reinterpret_cast<DeletionObject**>(&hsPointer)));
    escaped = i.Close(handle);
  }

  V8MONKEY_CHECK(d->RefCount() <= refCount, "RefCount didn't increase");
}


V8MONKEY_TEST(IntHandleScope041, "CreateHandle triggers fatal error if no HandleScope constructed") {
  Isolate* i = Isolate::New();
  i->Enter();

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  errorCaught = 0;
  V8::SetFatalErrorHandler(fatalErrorHandler);
  HandleScope::CreateHandle(d);

  V8MONKEY_CHECK(V8::IsDead() && errorCaught != 0, "Fatal error if CreateHandle called without HandleScope");
  delete d;
  i->Exit();
  i->Dispose();
}
