#include "v8.h"

#include "jsapi.h"

#include "types/base_types.h"
#include "test.h"
#include "v8monkey_common.h"
#include "V8MonkeyTest.h"


namespace {
  // XXX Need to fix these to take Persistent<Values>
  void* cb1Params = nullptr;
  bool cb1Called = false;
  int cb1CallCount = 0;

  void nopCallback(void* object, void* parameters) {
    cb1CallCount++;
    cb1Params = parameters;
    cb1Called = true;
  }

  void* cb2Params = nullptr;
  bool cb2Called = false;
  void MakeWeakAgainCallback(void* o, void* parameters) {
    cb2Params = parameters;
    cb2Called = true;
    v8::V8Monkey::V8MonkeyObject* object = reinterpret_cast<v8::V8Monkey::V8MonkeyObject*>(o);
    object->MakeWeak(object, parameters, MakeWeakAgainCallback);
  }

  void MakeStrongAgainCallback(void* o, void* parameters) {
    v8::V8Monkey::V8MonkeyObject* object = reinterpret_cast<v8::V8Monkey::V8MonkeyObject*>(o);
    object->ClearWeakness(object);
  }

  void disposeCallback(void* o, void* parameters) {
    v8::V8Monkey::V8MonkeyObject* object = reinterpret_cast<v8::V8Monkey::V8MonkeyObject*>(o);
    object->PersistentRelease(object);
  }
}


using namespace v8::V8Monkey;


V8MONKEY_TEST(RefCount001, "Objects initially born with refcount 0") {
   DummyV8MonkeyObject refCounted;

  V8MONKEY_CHECK(refCounted.RefCount() == 0, "Initial refcount correct");
}


V8MONKEY_TEST(RefCount002, "AddRef increases refcount") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(RefCount003, "Release decreases refcount") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  refCounted.AddRef();
  refCounted.Release();

  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Refcount correct");
}


V8MONKEY_TEST(RefCount004, "Refcount falling to zero deletes object") {
  bool deleted = false;
  DeletionObject* refCounted = new DeletionObject(&deleted);

  refCounted->AddRef();
  refCounted->Release();
  V8MONKEY_CHECK(deleted, "Refcounted object destroyed");
}


V8MONKEY_TEST(RefCount005, "Weak count initially zero") {
  DummyV8MonkeyObject refCounted;

  V8MONKEY_CHECK(refCounted.WeakCount() == 0, "Weak count initially zero");
}


V8MONKEY_TEST(RefCount006, "MakeWeak normally decreases strong count increases weak") {
  DummyV8MonkeyObject refCounted;
  // Manage the lifetime
  refCounted.AddRef();

  refCounted.MakeWeak(nullptr, nullptr, nullptr);
  V8MONKEY_CHECK(refCounted.RefCount() == 0, "Refcount changed");
  V8MONKEY_CHECK(refCounted.WeakCount() == 1, "Weak count changed");
}


V8MONKEY_TEST(RefCount007, "MakeWeak doesn't trigger deletion") {
  bool deleted = false;
  DeletionObject refCounted(&deleted);
  // Manage the lifetime
  refCounted.AddRef();

  refCounted.MakeWeak(nullptr, nullptr, nullptr);
  V8MONKEY_CHECK(!deleted, "Not deleted");
}


V8MONKEY_TEST(RefCount008, "MakeWeak doesn't change strong refcount if has already been made weak") {
  DummyV8MonkeyObject refCounted;
  // Manage the lifetime
  refCounted.AddRef();
  refCounted.AddRef();
  // ... refCount is now 2

  refCounted.MakeWeak(nullptr, nullptr, nullptr);
  refCounted.MakeWeak(nullptr, nullptr, nullptr);
  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Strong refcount doesn't decrease further after multiple MakeWeak calls");
}


V8MONKEY_TEST(RefCount009, "Object not destroyed if still weak references") {
  bool deleted = false;
  DeletionObject refCounted(&deleted);

  refCounted.AddRef();
  refCounted.AddRef();
  // ... refCount is now 2

  refCounted.MakeWeak(nullptr, nullptr, nullptr);
  // ... refCount now 1
  refCounted.Release();

  V8MONKEY_CHECK(!deleted, "Refcounted object not destroyed");
}


V8MONKEY_TEST(RefCount010, "ClearWeakness has no effect if requestor has not made object weak") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  refCounted.MakeWeak(nullptr, nullptr, nullptr);
  refCounted.ClearWeakness(&refCounted);

  V8MONKEY_CHECK(refCounted.RefCount() == 0, "Refcount unchanged");
  V8MONKEY_CHECK(refCounted.WeakCount() == 1, "Weakcount unchanged");
}


V8MONKEY_TEST(RefCount011, "ClearWeakness adds ref if value was previously weakened") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  refCounted.AddRef();
  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, nullptr, nullptr);
  refCounted.ClearWeakness(&refCounted);

  V8MONKEY_CHECK(refCounted.RefCount() == 2, "Refcount changed");
  V8MONKEY_CHECK(refCounted.WeakCount() == 0, "Weakcount changed");
}


V8MONKEY_TEST(RefCount012, "PersistentRelease decrements refcount if not weakened") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  refCounted.AddRef();
  // Slightly abuse the API by providing refCounted as our address
  refCounted.PersistentRelease(&refCounted);

  V8MONKEY_CHECK(refCounted.RefCount() == 1, "Refcount changed");
}


V8MONKEY_TEST(RefCount013, "PersistentRelease decrements weakcount if weakened") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  refCounted.AddRef();
  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, nullptr, nullptr);
  refCounted.PersistentRelease(&refCounted);

  V8MONKEY_CHECK(refCounted.WeakCount() == 0, "Weakcount changed");
  refCounted.PersistentRelease(&refCounted);
}


V8MONKEY_TEST(RefCount014, "PersistentRelease deletes object if applicable (1)") {
  bool deleted = false;
  DeletionObject* refCounted = new DeletionObject(&deleted);

  refCounted->AddRef();
  // Slightly abuse the API by providing refCounted as our address
  refCounted->PersistentRelease(refCounted);

  V8MONKEY_CHECK(deleted, "Object deleted");
}


V8MONKEY_TEST(RefCount015, "PersistentRelease deletes object if applicable (2)") {
  bool deleted = false;
  DeletionObject* refCounted = new DeletionObject(&deleted);

  refCounted->AddRef();
  refCounted->MakeWeak(refCounted, nullptr, nopCallback);

  // Slightly abuse the API by providing refCounted as our address
  refCounted->PersistentRelease(refCounted);

  V8MONKEY_CHECK(deleted, "Object deleted");
}


V8MONKEY_TEST(RefCount016, "ShouldTrace returns true if there are strong references") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();

  V8MONKEY_CHECK(refCounted.ShouldTrace(), "ShouldTrace correct");
}


V8MONKEY_TEST(RefCount017, "ShouldTrace calls callback for weak reference") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  cb1Called = false;
  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, nullptr, nopCallback);
  refCounted.ShouldTrace();

  V8MONKEY_CHECK(cb1Called, "Weak ref callback called");
}


V8MONKEY_TEST(RefCount018, "Calling MakeWeak twice changes callback") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  cb1Called = false;
  cb2Called = false;

  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, nullptr, MakeWeakAgainCallback);
  refCounted.MakeWeak(&refCounted, nullptr, nopCallback);
  refCounted.ShouldTrace();

  V8MONKEY_CHECK(cb1Called && !cb2Called, "Correct weak ref callback called");
}


V8MONKEY_TEST(RefCount019, "Calling MakeWeak twice changes parameters") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  cb1Called = false;
  cb2Called = false;

  void* ptr = reinterpret_cast<void*>(0x1234);

  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, &refCounted, MakeWeakAgainCallback);
  refCounted.MakeWeak(&refCounted, ptr, nopCallback);
  cb1Params = nullptr;
  refCounted.ShouldTrace();

  V8MONKEY_CHECK(cb1Params == ptr, "Called with correct parameters");
}


V8MONKEY_TEST(RefCount020, "Calling MakeWeak twice still results in callback being called once") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();

  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, &refCounted, nopCallback);
  refCounted.MakeWeak(&refCounted, &refCounted, nopCallback);
  cb1CallCount = 0;
  refCounted.ShouldTrace();

  V8MONKEY_CHECK(cb1CallCount == 1, "Only called once");
}


V8MONKEY_TEST(RefCount021, "ShouldTrace calls all relevant callbacks") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  refCounted.AddRef();
  cb1Called = false;
  cb2Called = false;

  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, nullptr, MakeWeakAgainCallback);
  refCounted.MakeWeak(nullptr, nullptr, nopCallback);
  refCounted.ShouldTrace();

  V8MONKEY_CHECK(cb1Called, "Correct callback called");
  V8MONKEY_CHECK(cb2Called, "Correct callback called");
}


V8MONKEY_TEST(RefCount022, "ShouldTrace calls callback with correct params") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  cb1Called = false;
  cb1Params = nullptr;
  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, &cb1Called, nopCallback);
  refCounted.ShouldTrace();

  V8MONKEY_CHECK(cb1Params == &cb1Called, "Correct parameters supplied");
}


V8MONKEY_TEST(RefCount023, "ShouldTrace reports false if callbacks accede") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, &cb1Called, nopCallback);

  V8MONKEY_CHECK(!refCounted.ShouldTrace(), "ShouldTrace reported false");
}


V8MONKEY_TEST(RefCount024, "ShouldTrace reports true if callbacks intervene") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, &cb1Called, MakeStrongAgainCallback);

  V8MONKEY_CHECK(refCounted.ShouldTrace(), "ShouldTrace reported false");
}


V8MONKEY_TEST(RefCount025, "PersistentRelease can be called in callback") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();
  refCounted.AddRef();
  refCounted.MakeWeak(nullptr, nullptr, nopCallback);
  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, nullptr, disposeCallback);
  cb1Called = false;
  refCounted.ShouldTrace();

  V8MONKEY_CHECK(cb1Called, "Other callbacks unaffected by PersistentRelease");
}


V8MONKEY_TEST(RefCount026, "Clearing weakness ensures callback not called") {
  DummyV8MonkeyObject refCounted;

  refCounted.AddRef();

  // Slightly abuse the API by providing refCounted as our address
  refCounted.MakeWeak(&refCounted, &refCounted, nopCallback);
  refCounted.ClearWeakness(&refCounted);
  cb1Called = false;
  refCounted.ShouldTrace();

  V8MONKEY_CHECK(!cb1Called, "Not called");
}
