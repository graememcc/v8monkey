#include "v8.h"

#include "data_structures/objectblock.h"
#include "data_structures/smart_pointer.h"
#include "isolate.h"
#include "runtime/handlescope.h"
#include "types/base_types.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;
using namespace v8::V8Monkey;

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
{
  HandleScope h;
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  HandleScope::CreateHandle(d);
  HandleScopeData hsd = i->GetHandleScopeData();

  V8MONKEY_CHECK(hsd.next != NULL, "HandleScopeData next changed after first handle created");
  V8MONKEY_CHECK(hsd.limit != NULL, "HandleScopeData limit changed after first handle created");

}
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
  InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
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
    InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
    HandleScope::CreateHandle(d);

    V8MONKEY_CHECK(d->RefCount() == 1, "Refcount increased on HandleScope creation");
  }
}


V8MONKEY_TEST(IntHandleScope008, "Refcount correct when handle to object created in different scopes") {
  TestUtils::AutoTestCleanup ac;

  DummyV8MonkeyObject* d = new DummyV8MonkeyObject;

  {
    HandleScope h;
    InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
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
    InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
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
    InternalIsolate* i = InternalIsolate::FromIsolate(Isolate::GetCurrent());
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


//XXX TODO: Close tests
