#include "v8.h"

#include "data_structures/objectblock.h"
#include "runtime/isolate.h"


namespace v8 {
  /*
   * In V8, persistents can be assigned to from other persistents and value type pointers in a direct manner. Such an
   * approach would be incorrect in V8Monkey, due to refcounting. Imagine a scenario where we have two Persistents
   * whose value pointer points to the same slot in persistent storage. The underlying object would have a lifetime
   * equal to the first Persistent to be destroyed, leaving the other with a potentially dangling pointer. The only
   * alternative would be to never tear down any objects until the isolate itself is destroyed, which seems wasteful.
   *
   * Thus, all persistent assignments flow through this function, and will always create a distinct ref-counted slot
   * for the given object. (Note in particular that we depart from V8 and define a copy constructor. The synthesized
   * constructor doesn't pass muster.)
   *
   */

  V8Monkey::V8MonkeyObject** V8::MakePersistent(V8Monkey::V8MonkeyObject** value) {
    using namespace V8Monkey;

    // Create a new handle in persistent storage, as the existing type could be a pointer into local handle storage
    V8MonkeyObject* obj = *value;

    InternalIsolate* i = InternalIsolate::GetCurrent();

    InternalIsolate::AutoGCMutex lock(i);

    HandleData hd = i->GetPersistentHandleData();

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
   * handling the weak callback, as otherwise the caller cannot reason about the liveness of the object if they didn't
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
