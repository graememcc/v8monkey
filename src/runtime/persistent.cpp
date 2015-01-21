// ObjectBlock
#include "data_structures/objectblock.h"

// AutoGCMutex, GetCurrent, Get/SetPersistentHandleData
#include "runtime/isolate.h"

// V8MonkeyObject
#include "types/base_types.h"

// V8 interface, WeakReferenceCallback
#include "v8.h"

// CheckDeath
#include "utils/V8MonkeyCommon.h"


namespace v8 {

  /*
   * As the V8 source notes, Persistents represent storage cells that are independent of a particular HandleScope.
   * Independent Persistents are created through the API call Persistent::New, but are otherwise copied by value.
   *
   * The SpiderMonkey garbage collector doesn't have a notion of weak references, so we need to roll our own. The
   * Persistent implementation and the V8MonkeyObject base class cooperate in this effort.
   *
   * The InternalIsolate contains storage slots for persistent pointers to V8MonkeyObjects. The MakePersistent
   * method will allocate a new storage slot for a V8MonkeyObject pointer. When the SpiderMonkey garbage collector
   * requests tracing, the InternalIsolate will iterate over the slots, calling Trace on each pointed-to object.
   *
   * The base V8MonkeyObject class implements a ShouldTrace method, and every derived class must call this method during
   * tracing. The SpiderMonkey tracing method for the wrapped object should only be invoked if ShouldTrace returns true.
   * It will return true when there are strong references to the object, which is always the case for Local handles, and
   * usually the case for Persistents. If the references to an object are only held by weakened Persistents, then their
   * weak reference callbacks will be called. If those callbacks fail to strengthen the Persistent, then ShouldTrace will
   * report false, the SpiderMonkey object will not be traced, and the V8MonkeyObject deleted.
   *
   * When iterating over these slot pointers, we obviously don't want to touch dead memory. There are thus two cases
   * where an object pointed to a slot could die: via a client-initiated call to Persistent::Dispose, or from
   * SpiderMonkey root tracing, when the only references are weak Persistents. The first case is fine, and it is a
   * simple matter to zero the relevant slot in isolate storage. However, we don't control the second. Thus, when we
   * weaken a slot, we must supply the address of the slot, so that ShouldTrace may zero it if the object dies (yes,
   * ShouldTrace blithely ignores the Single Responsibility Principle). Much of the hard work of the Persistent
   * implementations is simply supplying a slot address.
   *
   * Weak references and the need to zero slots complicates the reference counting. V8MonkeyObject provides a
   * PersistentRelease method that Persistents should use to decrement the refcount. This method requires the slot
   * to be supplied, and it decrements the appropriate refcount based on whether that slot has been used to weaken
   * a reference or not.
   *
   * At isolate teardown, the isolate will PersistentRelease all remaining objects. This implies that it is OK to
   * assign a Persistent to another Persistent: although the client will lose the ability to weaken or dispose of
   * the given object, it will still be correctly deleted and the memory freed.
   *
   * Points to clarify: in V8's global-handles.cc, the comments in Node::PostGarbageCollectionProcessing seem to
   * imply that a weak callback must either dispose or strengthen the Persistent. Our implementation will prevent
   * the object being deleted if any Persistent for an object has a callback that strengthens. I wonder if this
   * could break client expectations with regards to lifetimes. Also, we always iterate over all weak callbacks,
   * even if an earlier callback has re-strengthened the reference, ensuring we're not going to die this time. I
   * believe that is what V8 also does if I've read the code correctly, but need to double-check.
   *
   */


  /*
   * Allocate a storage slot for a persistent, and add a strong reference.
   *
   */

  V8Monkey::V8MonkeyObject** V8::MakePersistent(V8Monkey::V8MonkeyObject** value) {
    using namespace V8Monkey;

    // Create a new handle in persistent storage, as the source could be a pointer into local handle storage
    V8MonkeyObject* obj = *value;

    InternalIsolate* i = InternalIsolate::GetCurrent();

    if (V8MonkeyCommon::CheckDeath("Persistent::New")) {
      return nullptr;
    }

    InternalIsolate::AutoGCMutex lock(i);

    HandleData hd = i->GetPersistentHandleData();

    // Allocate more slots if necessary
    if (hd.limit == hd.next) {
      ObjectBlock<V8MonkeyObject>::Limits limits = ObjectBlock<V8MonkeyObject>::Extend(hd.limit);
      hd.limit = limits.limit;
      hd.next = limits.top;
    }

    V8MonkeyObject** ptr = hd.next++;
    InternalIsolate::GetCurrent()->SetPersistentHandleData(hd);

    if (obj) {
      obj->AddRef();
    }

    *ptr = obj;

    return ptr;
  }


  /*
   * Deletes a persistent object, and clears its slot in the isolate's persistent storage (to prevent double-frees).
   * Uses PersistentRelease to ensure correct refcount is decremented. It also first checks that the slot is not
   * already zeroed; AFAICT, the V8 API doesn't place any restrictions on when one can call Dispose after a Persistent
   * has been weakened and the weak callback called (though logic would dictate that the only safe place is when
   * handling the weak callback, as otherwise the caller cannot reason about the liveness of the object when they didn't
   * clear the weakness).
   *
   */

  void V8::DeletePersistent(V8Monkey::V8MonkeyObject** objSlot, V8Monkey::V8MonkeyObject* obj) {
    V8Monkey::InternalIsolate::AutoGCMutex lock(V8Monkey::InternalIsolate::GetCurrent());
    obj->PersistentRelease(objSlot);
    *objSlot = nullptr;
  }


  void V8::MakePersistentWeak(V8Monkey::V8MonkeyObject** objSlot, V8Monkey::V8MonkeyObject* obj, void* parameters,
                                   WeakReferenceCallback callback) {
    V8Monkey::InternalIsolate::AutoGCMutex lock(V8Monkey::InternalIsolate::GetCurrent());
    obj->MakeWeak(objSlot, parameters, callback);
  }


  void V8::ClearPersistentWeakness(V8Monkey::V8MonkeyObject** objSlot, V8Monkey::V8MonkeyObject* obj) {
    V8Monkey::InternalIsolate::AutoGCMutex lock(V8Monkey::InternalIsolate::GetCurrent());
    obj->ClearWeakness(objSlot);
  }


  bool V8::IsPersistentWeak(V8Monkey::V8MonkeyObject** objSlot, V8Monkey::V8MonkeyObject* obj) {
    return obj->IsWeak(objSlot);
  }


  bool V8::IsPersistentNearDeath(V8Monkey::V8MonkeyObject* obj) {
    return obj->IsNearDeath();
  }
}
