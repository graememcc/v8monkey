// The class under test
#include "data_structures/smart_pointer.h"

// DummyV8MonkeyObject, DeletionObject
#include "types/base_types.h"

// Unit-testing support
#include "V8MonkeyTest.h"
//
//
//using namespace v8::V8Monkey;
//
//
//class JSRuntime;
//class JSTracer;
//
//
//V8MONKEY_TEST(SmartPtr001, "Construction bumps refcount") {
//  DummyV8MonkeyObject* refCounted = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp(refCounted);
//  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount correct");
//}
//
//
//V8MONKEY_TEST(SmartPtr002, "Destruction decrements refcount") {
//  DummyV8MonkeyObject* refCounted = new DummyV8MonkeyObject;
//
//  // Don't die on us!
//  refCounted->AddRef();
//  {
//    SmartPtr<DummyV8MonkeyObject> sp(refCounted);
//  }
//  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount correct");
//  refCounted->Release();
//}
//
//
//V8MONKEY_TEST(SmartPtr003, "Assigning new value decrements old value's refcount") {
//  DummyV8MonkeyObject* refCounted1 = new DummyV8MonkeyObject;
//  DummyV8MonkeyObject* refCounted2 = new DummyV8MonkeyObject;
//
//  // Don't die on us!
//  refCounted1->AddRef();
//
//  SmartPtr<DummyV8MonkeyObject> sp(refCounted1);
//  sp = refCounted2;
//  V8MONKEY_CHECK(refCounted1->RefCount() == 1, "Refcount correct");
//  refCounted1->Release();
//}
//
//
//V8MONKEY_TEST(SmartPtr004, "Assigning new value bumps new value's refcount") {
//  DummyV8MonkeyObject* refCounted1 = new DummyV8MonkeyObject;
//  DummyV8MonkeyObject* refCounted2 = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp(refCounted1);
//  sp = refCounted2;
//  V8MONKEY_CHECK(refCounted2->RefCount() == 1, "Refcount correct");
//}
//
//
//V8MONKEY_TEST(SmartPtr005, "Assigning new value (when empty) bumps new value's refcount") {
//  DummyV8MonkeyObject* refCounted = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp;
//  sp = refCounted;
//  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount correct");
//}
//
//
//V8MONKEY_TEST(SmartPtr006, "Copying bumps value's refcount") {
//  DummyV8MonkeyObject* refCounted = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp1(refCounted);
//  SmartPtr<DummyV8MonkeyObject> sp2 = sp1;
//  V8MONKEY_CHECK(refCounted->RefCount() == 2, "Refcount correct");
//}
//
//
//V8MONKEY_TEST(SmartPtr007, "Assigning to self doesn't delete") {
//  bool deleted = false;
//  DeletionObject* refCounted = new DeletionObject(&deleted);
//
//  SmartPtr<DeletionObject> sp(refCounted);
//  sp = sp;
//  V8MONKEY_CHECK(!deleted, "Refcounted object not deleted");
//}
//
//
//V8MONKEY_TEST(SmartPtr008, "Assigning to self from raw pointer doesn't delete") {
//  bool deleted = false;
//  DeletionObject* refCounted = new DeletionObject(&deleted);
//
//  SmartPtr<DeletionObject> sp(refCounted);
//  sp = refCounted;
//  V8MONKEY_CHECK(!deleted, "Refcounted object not deleted");
//}
//
//
//V8MONKEY_TEST(SmartPtr009, "Assigning to self doesn't change refcount") {
//  DummyV8MonkeyObject* refCounted = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp(refCounted);
//  sp = sp;
//  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount not changed");
//}
//
//
//V8MONKEY_TEST(SmartPtr010, "Assigning to self from raw pointer doesn't change refcount") {
//  DummyV8MonkeyObject* refCounted = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp(refCounted);
//  sp = refCounted;
//  V8MONKEY_CHECK(refCounted->RefCount() == 1, "Refcount not changed");
//}
//
//
//V8MONKEY_TEST(SmartPtr011, "Smart pointers are equal if object pointers are equal (2)") {
//  DummyV8MonkeyObject* refCounted = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp1(refCounted);
//  SmartPtr<DummyV8MonkeyObject> sp2(refCounted);
//  V8MONKEY_CHECK(sp1 == sp2, "Smart pointers equal");
//}
//
//
//V8MONKEY_TEST(SmartPtr012, "Smart pointers are equal if object pointers are equal (2)") {
//  DummyV8MonkeyObject* refCounted1 = new DummyV8MonkeyObject;
//  DummyV8MonkeyObject* refCounted2 = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp1(refCounted1);
//  SmartPtr<DummyV8MonkeyObject> sp2(refCounted2);
//  V8MONKEY_CHECK(sp1 != sp2, "Smart pointers equal");
//}
//
//
//V8MONKEY_TEST(SmartPtr015, "Dereference operator works") {
//  DummyV8MonkeyObject* refCounted = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp(refCounted);
//  V8MONKEY_CHECK(*sp == *refCounted, "Dereferencing works");
//}
//
//
//V8MONKEY_TEST(SmartPtr016, "Indirection operator works") {
//  class SP016 : public V8MonkeyObject {
//    public:
//      SP016() {}
//      ~SP016() {}
//      int GetVal() { return 42; }
//      void Trace(JSRuntime*, JSTracer*) {}
//  };
//  SP016* refCounted = new SP016;
//
//  SmartPtr<SP016> sp(refCounted);
//  V8MONKEY_CHECK(sp->GetVal() == 42, "Indirection works");
//}
//
//
//V8MONKEY_TEST(SmartPtr017, "Assigning from an empty smart pointer works") {
//  DummyV8MonkeyObject* refCounted = new DummyV8MonkeyObject;
//
//  SmartPtr<DummyV8MonkeyObject> sp1(refCounted);
//  SmartPtr<DummyV8MonkeyObject> sp2;
//  sp1 = sp2;
//
//  // If we got here, we did't dereference sp2's null pointer
//  V8MONKEY_CHECK(true, "Assignment from empty worked");
//}
//
//
//V8MONKEY_TEST(SmartPtr018, "Copying from an empty smart pointer works") {
//  SmartPtr<DummyV8MonkeyObject> sp1;
//  SmartPtr<DummyV8MonkeyObject> sp2 = sp1;
//
//  // If we got here, we did't dereference sp2's null pointer
//  V8MONKEY_CHECK(true, "Copying from empty worked");
//}
//
//
//V8MONKEY_TEST(SmartPtr019, "Constructing with null works") {
//  SmartPtr<DummyV8MonkeyObject> sp(nullptr);
//
//  // If we got here, we did't dereference sp2's null pointer
//  V8MONKEY_CHECK(true, "Initializing with nullptr works");
//}
//
//
//V8MONKEY_TEST(SmartPtr020, "Smart pointer is correct size") {
//  // TODO: This should be a static assert. Finding out now is way too late.
//  V8MONKEY_CHECK(sizeof(SmartPtr<DummyV8MonkeyObject>) == sizeof(DummyV8MonkeyObject*), "Pointer size OK");
//}
