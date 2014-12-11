#include "v8.h"

#include "data_structures/objectblock.h"
#include "data_structures/smart_pointer.h"
#include "runtime/isolate.h"
#include "runtime/handlescope.h"
#include "types/base_types.h"
#include "test.h"


/*
 * As noted in the comments in objectblock.h, we allocate slabs of memory to store the values pointed to by API created
 * handles. However, it is entirely possible for the same underlying value to be assigned to two different handles in
 * different scopes, so we must be careful about deletion. (V8 doesn't have to worry: deleting the HandleScope doesn't
 * destroy the underlying object, it simply unroots it, and the garbage collector will swoop in and delete it later,
 * whereas we need to manually unroot the JS::Heap objects we have wrapped in our V8-like objects.). Thus, we use
 * reference-counting smart pointers to really store the handles. The smart pointers will be the same size as a real
 * pointer, so we do the casting dance in CreateHandle and the destructor to keep everything in order.
 *
 */


namespace {
  // Because we secretly wrap the objects in refcount-handling smart pointers, we can't just let the ObjectBlock
  // merrily delete what it believes to be V8MonkeyObjects. We supply it this function to cast then delete.
  void deleteSmart(v8::V8Monkey::V8MonkeyObject* obj) {
    v8::V8Monkey::SmartPtr<v8::V8Monkey::V8MonkeyObject>* sp = reinterpret_cast<v8::V8Monkey::SmartPtr<v8::V8Monkey::V8MonkeyObject>*>(obj);
    delete sp;
  }
}


namespace v8 {
  HandleScope::HandleScope() {
    V8Monkey::InternalIsolate* i = V8Monkey::InternalIsolate::GetCurrent();
    V8Monkey::HandleScopeData hsd = i->GetHandleScopeData();

    isolate = i;
    prev_next = hsd.next;
    prev_limit = hsd.limit;
  }


  HandleScope::~HandleScope() {
    using namespace V8Monkey;

    HandleScopeData hsd = isolate->GetHandleScopeData();
    if (hsd.limit != prev_limit || hsd.next != prev_next) {
      V8Monkey::ObjectBlock<V8MonkeyObject>::Delete(hsd.limit, hsd.next, prev_next, deleteSmart);
    }

    hsd.limit = prev_limit;
    hsd.next = prev_next;
    InternalIsolate::GetCurrent()->SetHandleScopeData(hsd);
  }


  V8Monkey::V8MonkeyObject** HandleScope::CreateHandle(V8Monkey::V8MonkeyObject* obj) {
    using namespace V8Monkey;

    HandleScopeData hsd = InternalIsolate::GetCurrent()->GetHandleScopeData();

    if (hsd.limit == hsd.next) {
      ObjectBlock<V8MonkeyObject>::Limits limits = ObjectBlock<V8MonkeyObject>::Extend(hsd.limit);
      hsd.limit = limits.limit;
      hsd.next = limits.top;
    }

    V8MonkeyObject** ptr = hsd.next;
    SmartPtr<V8MonkeyObject>* sp = new SmartPtr<V8MonkeyObject>(obj);
    *(hsd.next++) = reinterpret_cast<V8MonkeyObject*>(sp);

    InternalIsolate::GetCurrent()->SetHandleScopeData(hsd);
    return ptr;
  }
}
