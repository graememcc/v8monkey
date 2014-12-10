#include "v8.h"

#include "jsapi.h"

#include "types/base_types.h"
#include "data_structures/smart_pointer.h"
#include "test.h"
#include "v8monkey_common.h"
#include "V8MonkeyTest.h"


namespace {
  class DummyRefCount: public v8::V8Monkey::V8MonkeyObject {
    public:
      DummyRefCount() {}
      ~DummyRefCount() {}
      void Trace(JSRuntime* runtime, JSTracer* tracer) {}

      inline bool operator== (const DummyRefCount &other) {
        return &other == this;
      }
  };
}


using namespace v8::V8Monkey;


V8MONKEY_TEST(RefCount001, "Objects initially born with refcount 0") {
   DummyRefCount refCounted;

  V8MONKEY_CHECK(refCounted.RefCount() == 0, "Initial refcount correct");
}


V8MONKEY_TEST(RefCount002, "AddRef increases refcount") {
  DummyRefCount refCounted;

  refCounted.AddRef();
  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(RefCount003, "Release decreases refcount") {
  DummyRefCount refCounted;

  refCounted.AddRef();
  refCounted.AddRef();
  refCounted.Release();

  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(RefCount004, "Refcount falling to zero deletes object") {
  static bool deleted = true;
  class RC004 : public V8MonkeyObject {
    public:
      RC004() {}
      ~RC004() { deleted = true; }
      void Trace(JSRuntime* runtime, JSTracer* tracer) {}
  };
  RC004* refCounted = new RC004;

  refCounted->AddRef();
  refCounted->Release();
  V8MONKEY_CHECK(deleted, "Refcounted object destroyed");
}


V8MONKEY_TEST(SmartPtr001, "Construction bumps refcount") {
  DummyRefCount* refCounted = new DummyRefCount;

  SmartPtr<DummyRefCount> sp(refCounted);
  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(SmartPtr002, "Destruction decrements refcount") {
  DummyRefCount* refCounted = new DummyRefCount;

  // Don't die on us!
  refCounted->AddRef();
  {
    SmartPtr<DummyRefCount> sp(refCounted);
  }
  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount correct");
  refCounted->Release();
}


V8MONKEY_TEST(SmartPtr003, "Assigning new value decrements old value's refcount") {
  DummyRefCount* refCounted1 = new DummyRefCount;
  DummyRefCount* refCounted2 = new DummyRefCount;

  // Don't die on us!
  refCounted1->AddRef();

  SmartPtr<DummyRefCount> sp(refCounted1);
  sp = refCounted2;
  V8MONKEY_CHECK(refCounted1->RefCount() == 1, "Refcount correct");
  refCounted1->Release();
}


V8MONKEY_TEST(SmartPtr004, "Assigning new value bumps new value's refcount") {
  DummyRefCount* refCounted1 = new DummyRefCount;
  DummyRefCount* refCounted2 = new DummyRefCount;

  SmartPtr<DummyRefCount> sp(refCounted1);
  sp = refCounted2;
  V8MONKEY_CHECK(refCounted2->RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(SmartPtr005, "Assigning new value (when empty) bumps new value's refcount") {
  DummyRefCount* refCounted = new DummyRefCount;

  SmartPtr<DummyRefCount> sp;
  sp = refCounted;
  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(SmartPtr006, "Copying bumps value's refcount") {
  DummyRefCount* refCounted = new DummyRefCount;

  SmartPtr<DummyRefCount> sp1(refCounted);
  SmartPtr<DummyRefCount> sp2 = sp1;
  V8MONKEY_CHECK(refCounted->RefCount() == 2, "Refcount correct");
}


V8MONKEY_TEST(SmartPtr007, "Assigning to self doesn't delete") {
  static bool deleted = false;
  class SP007 : public V8MonkeyObject {
    public:
      SP007() {}
      ~SP007() { deleted = true; }
      void Trace(JSRuntime* runtime, JSTracer* tracer) {}
  };
  SP007* refCounted = new SP007;

  SmartPtr<SP007> sp(refCounted);
  sp = sp;
  V8MONKEY_CHECK(!deleted, "Refcounted object not deleted");
}


V8MONKEY_TEST(SmartPtr008, "Assigning to self from raw pointer doesn't delete") {
  static bool deleted = false;
  class SP008 : public V8MonkeyObject {
    public:
      SP008() {}
      ~SP008() { deleted = true; }
      void Trace(JSRuntime* runtime, JSTracer* tracer) {}
  };
  SP008* refCounted = new SP008;

  SmartPtr<SP008> sp(refCounted);
  sp = refCounted;
  V8MONKEY_CHECK(!deleted, "Refcounted object not deleted");
}


V8MONKEY_TEST(SmartPtr009, "Assigning to self doesn't change refcount") {
  DummyRefCount* refCounted = new DummyRefCount;

  SmartPtr<DummyRefCount> sp(refCounted);
  sp = sp;
  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount not changed");
}


V8MONKEY_TEST(SmartPtr010, "Assigning to self from raw pointer doesn't change refcount") {
  DummyRefCount* refCounted = new DummyRefCount;

  SmartPtr<DummyRefCount> sp(refCounted);
  sp = refCounted;
  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount not changed");
}


V8MONKEY_TEST(SmartPtr011, "Smart pointers are equal if object pointers are equal (2)") {
  DummyRefCount* refCounted = new DummyRefCount;

  SmartPtr<DummyRefCount> sp1(refCounted);
  SmartPtr<DummyRefCount> sp2(refCounted);
  V8MONKEY_CHECK(sp1 == sp2, "Smart pointers equal");
}


V8MONKEY_TEST(SmartPtr012, "Smart pointers are equal if object pointers are equal (2)") {
  DummyRefCount* refCounted1 = new DummyRefCount;
  DummyRefCount* refCounted2 = new DummyRefCount;

  SmartPtr<DummyRefCount> sp1(refCounted1);
  SmartPtr<DummyRefCount> sp2(refCounted2);
  V8MONKEY_CHECK(sp1 != sp2, "Smart pointers equal");
}


V8MONKEY_TEST(SmartPtr015, "Dereference operator works") {
  DummyRefCount* refCounted = new DummyRefCount;

  SmartPtr<DummyRefCount> sp(refCounted);
  V8MONKEY_CHECK(*sp == *refCounted, "Dereferencing works");
}


V8MONKEY_TEST(SmartPtr016, "Indirection operator works") {
  class SP016 : public V8MonkeyObject {
    public:
      SP016() {}
      ~SP016() {}
      int GetVal() { return 42; }
      void Trace(JSRuntime* runtime, JSTracer* tracer) {}
  };
  SP016* refCounted = new SP016;

  SmartPtr<SP016> sp(refCounted);
  V8MONKEY_CHECK(sp->GetVal() == 42, "Indirection works");
}


V8MONKEY_TEST(SmartPtr017, "Assigning from an empty smart pointer works") {
  DummyRefCount* refCounted = new DummyRefCount;

  SmartPtr<DummyRefCount> sp1(refCounted);
  SmartPtr<DummyRefCount> sp2;
  sp1 = sp2;

  // If we got here, we did't dereference sp2's null pointer
  V8MONKEY_CHECK(true, "Assignment from empty worked");
}


V8MONKEY_TEST(SmartPtr018, "Copying from an empty smart pointer works") {
  SmartPtr<DummyRefCount> sp1;
  SmartPtr<DummyRefCount> sp2 = sp1;

  // If we got here, we did't dereference sp2's null pointer
  V8MONKEY_CHECK(true, "Copying from empty worked");
}


V8MONKEY_TEST(SmartPtr019, "Constructing with null works") {
  SmartPtr<DummyRefCount> sp(NULL);

  // If we got here, we did't dereference sp2's null pointer
  V8MONKEY_CHECK(true, "Initializing with NULL works");
}
