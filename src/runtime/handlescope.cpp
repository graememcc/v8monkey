#include "v8.h"

#include "data_structures/objectblock.h"
#include "runtime/isolate.h"
#include "threads/autolock.h"
#include "types/base_types.h"
#include "test.h"
#include "v8monkey_common.h"


/*
 * For managing local rooted values, V8 provides the HandleScope API. The embedder stores pointers received from the V8
 * engine in to a Local<T> handle, which both roots the value and tracks its location in memory (V8, like SpiderMonkey
 * uses a moving garbage collector). To avoid having to manually manage the local handles, they have the semantics that
 * the underlying pointer is stored in the nearest HandleScope. All handles created during the lifetime of the
 * HandleScope are unrooted in one batch when the HandleScope gets destroyed.
 *
 * Our implementation largely follows the V8 approach. We allocate slabs of memory in which we stuff the pointers
 * managed by the handles, and maintain two pointers, one to the next free slot, and one to the end of the block. We
 * allocate further storage when we first try to store another pointer when we've reached the end of the block.
 * The actual HandleScope objects store the previous such pointers, to allow  rolling back to the state of the previous
 * HandleScope when the end of a scope is reached.
 *
 * There are some differences. First, note for V8, rolling back to the previous HandleScope is simply a matter of
 * twiddling the pointers; the underlying objects will be hoovered up later by the garbage collector. This is not the
 * case here, so our pointer arrays must perform some form of deletion action. Our ObjectBlock implementation provides
 * this.
 *
 * Next, observe that it is quite acceptable to assign to a Local handle from some other handle. These handles need not
 * be in the same scope. This raises the spectre of dangling pointers. Thus, we refcount the underlying objects. On
 * deletion, we adjust the refcount, and trust the underlying object to delete itself when required. At time of writing,
 * objects don't point to each other, so there is no danger of cycles. XXX Check this is still true later!
 *
 * This is currently the only place that requires refcounting, so we simply refcount manually rather than using smart
 * pointers.
 *
 * Our HandleScope and Isolate implementations cooperate to handle rooting the SpiderMonkey objects. (Our internal
 * objects are heap allocated, so must store values from SpiderMonkey in JS::Heap<T> objects, which are not rooted).
 * When a HandleScope transitions from completely empty to non-empty, the HandleScope will notify the currently entered
 * isolate that it needs to participate in rooting. Similarly, the opposite occurs on destruction if rooting is no
 * longer necessary.
 *
 */


namespace {
  // Deletion function to pass to the slab allocator, which will iterate over all the values falling out of scope and
  // call this function to adjust the refcount.
  void deleteRefCount(v8::V8Monkey::V8MonkeyObject* obj) {
    // We null out references to escaping values
    if (obj != nullptr) {
      obj->Release();
    }
  }


  // Common handle creation code, used by both CreateHandle and Close, which differ on whether they addref or not
  v8::V8Monkey::V8MonkeyObject** AddHandle(v8::V8Monkey::InternalIsolate* i, v8::V8Monkey::V8MonkeyObject* obj,
                                           bool shouldAddRef) {
    using namespace v8::V8Monkey;

    InternalIsolate::AutoHandleScopeDataMutex lock(i);

    HandleScopeData hsd = i->GetHandleScopeData();

    // We must have a handlescope to create handles in. This is an API error.
    if (hsd.level == 0) {
      V8MonkeyCommon::TriggerFatalError("HandleScope", "Cannot create handle without handlescope");
      return nullptr;
    }

    if (hsd.limit == hsd.next) {
      ObjectBlock<V8MonkeyObject>::Limits limits = ObjectBlock<V8MonkeyObject>::Extend(hsd.limit);
      hsd.limit = limits.limit;
      hsd.next = limits.top;
    }

    V8MonkeyObject** ptr = hsd.next++;
    InternalIsolate::GetCurrent()->SetHandleScopeData(hsd);

    obj->AddRef();
    *ptr = obj;

    return ptr;
  }
}


namespace v8 {
  HandleScope::HandleScope() : isClosed(false) {
    // HandleScopes are assumed to belong to the currently entered isolate, or the default if none is entered.
    // It is assumed that the currently entered isolate is the one in which the handlescope was created, as
    // data could be stomped on otherwise. (V8 asserts this in debug builds, but doesn't enforce it).
    V8Monkey::InternalIsolate* i = V8Monkey::InternalIsolate::GetCurrent();
    V8Monkey::HandleScopeData hsd = i->GetHandleScopeData();

    isolate = i;
    prevNext = hsd.next;
    prevLimit = hsd.limit;

    {
      V8Monkey::InternalIsolate::AutoHandleScopeDataMutex lock(isolate);
      hsd.level++;
      i->SetHandleScopeData(hsd);
    }

    // It is an API error to construct a HandleScope if threads are active and the isolate is not locked
    // We test down here as a concession to the testing framework, which will catch the error and still try to destroy
    // the handlescope. The handlescope will now have valid data to write back to the HandleScopeData object.
    if (Locker::IsActive() && !i->IsLockedForThisThread()) {
      V8Monkey::V8MonkeyCommon::TriggerFatalError("HandleScope::HandleScope", "A lock must be held for this isolate");
    }
  }


  HandleScope::~HandleScope() {
    // The user might have explicitly called Close, in which case this has already been torn down
    if (!isClosed) {
      Leave();
    }
  }


  // Common code for exiting a particular scope, and resetting the pointers to the previous scope's data.
  void HandleScope::Leave() {
    using namespace V8Monkey;

    ASSERT(InternalIsolate::GetCurrent() == isolate, "HandleScope::Leave", "Isolates don't match");

    InternalIsolate::AutoHandleScopeDataMutex lock(isolate);

    HandleScopeData hsd = isolate->GetHandleScopeData();
    if (hsd.limit != prevLimit || hsd.next != prevNext) {
      V8Monkey::ObjectBlock<V8MonkeyObject>::Delete(hsd.limit, hsd.next, prevNext, deleteRefCount);
    }

    hsd.limit = prevLimit;
    hsd.next = prevNext;

    InternalIsolate::GetCurrent()->SetHandleScopeData(hsd);

    isClosed = true;
  }


  // Note that V8 does not prevent adding handles after the HandleScope has been closed, even though such handles
  // will actually be added to the previous HandleScope, which is unlikely to be what the user expects.
  V8Monkey::V8MonkeyObject** HandleScope::CreateHandle(V8Monkey::V8MonkeyObject* obj) {
    return AddHandle(V8Monkey::InternalIsolate::GetCurrent(), obj, true);
  }


// XXX Test for trying to close twice. Should fail
  V8Monkey::V8MonkeyObject** HandleScope::InternalClose(V8Monkey::V8MonkeyObject** value) {
    V8Monkey::V8MonkeyObject* escaped = *value;

    // Null out the old value, to prevent iteration (and hence prevent refcount from being decremented)
    {
      // XXX Have another crack at the mutex reference
      V8Monkey::InternalIsolate::AutoHandleScopeDataMutex lock(isolate);
      *value = nullptr;
    }

    // TODO: One potential optimization is to spot cases where the handle being escaped is the first entry in this
    // scope's pointer block, when we could simply adjust the range we delete. To that end, there are tests exercising
    // that scenario in the testsuite.

    // Clear out the old data
    Leave();

    // Re-add the escaped value
    V8Monkey::V8MonkeyObject** ptr = AddHandle(V8Monkey::InternalIsolate::GetCurrent(), escaped, false);

    // Adding will have inreased the refcount. We never lowered it when we escaped the value, so we need to release to
    // correct
    escaped->Release();

    return ptr;
  }
}
