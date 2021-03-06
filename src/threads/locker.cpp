/*
// Enter Exit GetDefaultIsolate Lock Unlock
#include "runtime/isolate.h"

// Locker interface
#include "v8.h"

// V8MONKEY_ASSERT
#include "utils/V8MonkeyCommon.h"
*/


/*
 * TODO: This code was originally written for the early attempt at supporting multithreading. Much of the code could
 *       be simplified if multithreading cannot be reinstated. (Most of these operations should become NO-OPS).
 *
 */


// XXX Need to use FromIsolate here. Also looks like we want an equivalent ToIsolate
//
//
/*
namespace v8 {
  bool Locker::active_ = false;


  bool Locker::IsActive() {
    return active_;
  }


  bool Locker::IsLocked(Isolate* iso) {
    V8MONKEY_ASSERT(iso, "Attempting to lock nullptr isolate");
    internal::Isolate* i {internal::Isolate::FromAPIIsolate(iso)};
    return i->IsLockedForThisThread();
  }


  void Locker::Initialize(Isolate* iso) {
    static V8_UNUSED bool initialized {[]() {
      active_ = true;
      return true;
    }()};

    V8MONKEY_ASSERT(iso, "Attempting to lock nullptr isolate");
    isolate_ = internal::Isolate::FromAPIIsolate(iso);

    if (!isolate_->IsLockedForThisThread()) {
      isolate_->Lock();

      // XXX Needed? Does V8 init have any observable effects now?
      // Locking impicitly init V8 if required (which will in turn enter the default isolate if required)
//      if (i == V8Monkey::InternalIsolate::GetDefaultIsolate()) {
//        V8::Initialize();
//      }

      has_lock_ = true;
    } else {
      has_lock_ = false;
    }
  }


  Locker::~Locker() {
    if (has_lock_) {
      isolate_->Unlock();
    }
  }


  void Unlocker::Initialize(Isolate* iso) {
//    if (iso == nullptr) {
//      i = V8Monkey::InternalIsolate::GetDefaultIsolate();
//      isolate = reinterpret_cast<Isolate*>(i);
//    } else {
//      isolate = iso;
//      i = reinterpret_cast<V8Monkey::InternalIsolate*>(isolate);
//    }
//
//    if (i == V8Monkey::InternalIsolate::GetDefaultIsolate()) {
//      i->Exit();
//    }
//

    V8MONKEY_ASSERT(iso, "Attempting to lock nullptr isolate");
    isolate_ = internal::Isolate::FromAPIIsolate(iso);
    V8MONKEY_ASSERT(isolate_->IsLockedForThisThread(), "Attempting to unlock unlocked isolate");
    isolate_->Unlock();
  }


  Unlocker::~Unlocker() {
    isolate_->Lock();
  }
}
*/
