// V8MonkeyObject interface
#include "base_types.h"

// Handle Persistent Value
#include "v8.h"
//
//
//
//namespace v8 {
//  namespace V8Monkey {
//
//    V8MonkeyObject::~V8MonkeyObject() {
//      // Tear down the callback list if required. In some rare circumstances, it might be non-empty (for example if
//      // an isolate is disposed before a weak Persistent that wraps an object created in that Isolate). Hence, it is
//      // unsafe to assume the slot is still alive, so we don't zero it here
//      if (callbackList) {
//        WeakRefListElem* current = callbackList;
//
//        while (current) {
//          WeakRefListElem* next = current->next;
//          delete current;
//          current = next;
//        }
//      }
//    }
//
//
//    // Provides object level support for Persistent weak-references. The goal here is to adjust the refcounts, and
//    // note the callback (if any) that should be invoked during garbage collection when there are only weak
//    // references remaining.
//    void V8MonkeyObject::MakeWeak(V8MonkeyObject** slotPtr, void* parameters, WeakReferenceCallback callback) {
//      WeakRefListElem* cbListElem = FindForSlot(slotPtr);
//
//      // If the caller has already weakened this object, simply update callback information
//      if (cbListElem) {
//        cbListElem->params = parameters;
//        cbListElem->callback = callback;
//        return;
//      }
//
//      weakCount++;
//      refCount--;
//
//      // Create a new entry in our linked list of weak callbacks. As the contents of this list are used to identify
//      // slots that have weakened objects, we do this even when the given callback pointer is null
//      cbListElem = new WeakRefListElem;
//      cbListElem->slotPtr = slotPtr;
//      cbListElem->params = parameters;
//      cbListElem->callback = callback;
//      cbListElem->prev = nullptr;
//      cbListElem->next = callbackList;
//
//      if (callbackList) {
//        callbackList->prev = cbListElem;
//      }
//
//      callbackList = cbListElem;
//    }
//
//
//    // Undo the effects of a MakeWeak call, after first checking that the given slot has been weakened. (This is
//    // a no-op when called with a slot that hasn't weakened us).
//    void V8MonkeyObject::ClearWeakness(V8MonkeyObject** slotPtr) {
//      WeakRefListElem* cbListElem = FindForSlot(slotPtr);
//
//      if (!cbListElem) {
//        // If there was no entry for the given slot, then it was never weakened. Continuing would mess up the
//        // refcounting, so bail now.
//        return;
//      }
//
//      RemoveFromWeakList(cbListElem);
//      ++refCount;
//      --weakCount;
//
//      if (isNearDeath) {
//        isNearDeath = false;
//      }
//    }
//
//
//    // Answers the question of whether a given handle is weak. For V8 compatability, persistents only contain a
//    // pointer to their API object, so we must answer the question at this level. The persistent should calculate
//    // its object slot, and pass it here. The slot pointer will be used to search the weak callback linked list
//    // (which we assume has an entry for every weak persistent even if there was no callback function supplied).
//    bool V8MonkeyObject::IsWeak(V8MonkeyObject** slotPtr) {
//      return FindForSlot(slotPtr) != nullptr;
//    }
//
//
//    // As with IsWeak, this function is used to support the interface for Persistent handles without storing
//    // information on the persistent itself. As I understand it, this can only be true during garbage collection,
//    // when calling WeakReference callbacks.
//    bool V8MonkeyObject::IsNearDeath() {
//      return isNearDeath;
//    }
//
//
//    // Customised version of Release required due to the weak pointer semantics afforded by Persistents.
//    // Essentially, we have to decide whether the given Persistent is a weak ref or not, and decrement the correct
//    // reference counter. If a Persistent calls MakeWeak, then when deleting a reference, it must call this function
//    // instead of Release. (It is safe to call this function even when MakeWeak has not been called.)
//    //
//    // The Persistent should supply its slot pointer, as this will be cleared to avoid dereferencing null later when
//    // the owning Isolate is traced or deleted. (Note: isolates should also use this function in their own teardown,
//    // passing in a null slot pointer).
//    void V8MonkeyObject::PersistentRelease(V8MonkeyObject** slotPtr) {
//      if (slotPtr == nullptr) {
//        IsolateRelease();
//        return;
//      }
//
//      WeakRefListElem* cbListElem = FindForSlot(slotPtr);
//
//      // We assume that we're going to decrement the strong refcount. However, if we find a weak callback entry, we
//      // decrement the weakref count instead.
//      int* counter = &refCount;
//
//      if (cbListElem) {
//        // Need to delete the callback entry if the Persistent is going away
//        RemoveFromWeakList(cbListElem);
//        counter = &weakCount;
//      }
//
//      RemoveRef(slotPtr, counter);
//    }
//
//
//    // SpiderMonkey's handles don't provide a weak reference mechanism, so to support Persistent's weak references, we
//    // roll our own here. All derived classes that participate in rooting must call this function in order to decide
//    // whether to root their SpiderMonkey GCThing.
//    //
//    // This function will always report true if there are any strong references to this object. If the only references
//    // weak, then all installed weak callbacks will be called. If one or more of the callbacks restore a strong
//    // reference, then this function will return true. Otherwise, it will return false, in which case it is no longer
//    // safe for the caller to touch the object, as it may be deleted.
//    //
//    // TODO: One possible optimization is short-circuiting, bailing as soon as a weak reference restores a strong
//    //       reference. In some cases this could avoid a weak reference callback being invoked if it is already
//    //       clear that we are not going to be destroyed. Would this break any assumptions for API users?
//
//    bool V8MonkeyObject::ShouldTrace() {
//      // If there are strong references, then the answer is an unequivocal yes
//      if (refCount > 0) {
//        return true;
//      }
//
//      // Weak callbacks are free to invoke Dispose. If all the callbacks did so, then the last one would trigger
//      // destruction, and then we're in no-mans land. Let's temporarily bump the refcount to ensure that doesn't
//      // happen.
//      refCount++;
//
//      WeakRefListElem* current = callbackList;
//      if (current) {
//        while (current) {
//          WeakRefListElem* next = current->next;
//          if (current->callback) {
//            // In the V8 API, the callback takes a Persistent<Value>. Of course, a Value* is just a dressed-up
//            // V8MonkeyObject**, so if we could get our hands on the address of something that points to us, we're
//            // golden. Fortunately, each WeakRefListElem contains the slot pointer relevant to that callback. We
//            // manufacture a handle, which in turn let's us abuse the API and create a persistent. Handle equality
//            // checks will work as expected, as those are based on equality of the underlying V8MonkeyObject*.
//            Handle<Value> h(reinterpret_cast<Value*>(current->slotPtr));
//            Persistent<Value> p(h);
//
//            // The only time a global node has the "near death" state in V8 is when the weak callbacks are called in
//            // PostGarbageCollectionProcessing. We emulate that behaviour here. Note that we must reset IsNearDeath
//            // each time round the loop, as a particular callback may clear weakness, which in turn should cause
//            // IsNearDeath() to report false
//            isNearDeath = true;
//
//            current->callback(p, current->params);
//          }
//          current = next;
//        }
//
//        isNearDeath = false;
//      }
//
//      // We added a refCount above, so undo that now. The callbacks may have disposed/deleted us, so we should use
//      // RemoveRef as opposed to simply decrementing, to trigger destruction if appropriate.
//      //
//      // Given we might be about to die, we should note the correct refcounts now, to avoid touching a dead object
//      // afterwards.
//      int rc = refCount - 1;
//      int wc = weakCount;
//
//      RemoveRef(nullptr, &refCount);
//
//      if (rc == 0 && wc > 0) {
//        // We're not dead yet, due to the weak references. However, this means that the callbacks didn't strengthen us,
//        // so we should go ahead and die
//        WeakRefListElem* callbackEntry = callbackList;
//        while (callbackEntry) {
//          // Stash the information we need from the WeakRefListElem. It won't exist after the PersistentRelease call
//          WeakRefListElem* next = callbackEntry->next;
//          V8MonkeyObject** slotPtr = callbackEntry->slotPtr;
//
//          PersistentRelease(slotPtr);
//
//          if (next && slotPtr) {
//            // If next is non-null then there are more weakrefs to delete. RemoveRef only zeroes the last, so we need
//            // to manually zero the rest
//            *slotPtr = nullptr;
//          }
//
//          callbackEntry = next;
//        }
//      }
//
//      return rc > 0;
//    }
//
//
//    // Find the weak reference callback information if it exists
//    V8MonkeyObject::WeakRefListElem* V8MonkeyObject::FindForSlot(V8MonkeyObject** slotPtr) {
//      WeakRefListElem* cbListElem = nullptr;
//
//      if (callbackList) {
//        WeakRefListElem* current = callbackList;
//        while (current && !cbListElem) {
//          if (current->slotPtr == slotPtr) {
//            cbListElem = current;
//          } else {
//            current = current->next;
//          }
//        }
//      }
//
//      return cbListElem;
//    }
//
//
//    // Clear out the given weak reference data
//    void V8MonkeyObject::RemoveFromWeakList(WeakRefListElem* cbListElem) {
//      if (!cbListElem) {
//        return;
//      }
//
//      if (cbListElem->prev) {
//        cbListElem->prev->next = cbListElem->next;
//      }
//
//      if (cbListElem->next) {
//        cbListElem->next->prev = cbListElem->prev;
//      }
//
//      if (callbackList == cbListElem) {
//        callbackList = cbListElem->next;
//      }
//
//      delete cbListElem;
//    }
//
//
//    // Decrement the given refcount, and, if appropriate, zero the contents of the given slot (which should contain
//    // a pointer to this object) and delete this object
//    void V8MonkeyObject::RemoveRef(V8MonkeyObject** slotPtr, int* counter) {
//      (*counter)--;
//
//      // Should we delete this object?
//      if (refCount == 0 && weakCount == 0) {
//        delete this;
//
//        // We work on the assumption that slotPtr is the slot in which the pointer to this is stored.
//        // If slotPtr is null, then we assume that this call came from isolate teardown of persistent storage,
//        // or that the handle releasing is a handlescope (in both cases fixing up the pointer is irrelevant).
//        if (slotPtr) {
//          *slotPtr = nullptr;
//        }
//      }
//    }
//
//
//    // When an isolate releases an object, we don't need to worry about clearing the slot, which will shortly be a
//    // dangling pointer. However, we may be called multiple times, as there may be multiple slots referencing this
//    // object. Thus, each call we delete one refCount, though it doesn't matter which. All being well, once the
//    // isolate has iterated over all the handles it knows about, our refCount should have dropped to zero, and we
//    // will be dead.
//    void V8MonkeyObject::IsolateRelease() {
//      int* counter = &refCount;
//
//      // Delete weakCounts first (as they could only have came from persistents), then others
//      if (weakCount > 0) {
//        counter = &weakCount;
//      }
//
//      RemoveRef(nullptr, counter);
//    }
//  }
//}
