#include "v8.h"

#include "runtime/isolate.h"


namespace v8 {
  bool Locker::active = false;


  bool Locker::IsActive() {
    return active;
  }


  bool Locker::IsLocked(Isolate* isolate) {
    V8Monkey::InternalIsolate* i;

    if (isolate != NULL) {
      i = reinterpret_cast<V8Monkey::InternalIsolate*>(isolate);
    } else {
      i = V8Monkey::InternalIsolate::GetDefaultIsolate();
    }
    return i->IsLocked();
  }


  Locker::Locker(Isolate* iso) {
    active = true;

    V8Monkey::InternalIsolate* i;

    if (iso == NULL) {
      i = V8Monkey::InternalIsolate::GetDefaultIsolate();
      isolate = reinterpret_cast<Isolate*>(i);
    } else {
      isolate = iso;
      i = reinterpret_cast<V8Monkey::InternalIsolate*>(isolate);
    }


    if (!i->IsLockedForThisThread()) {
      i->Lock();

      // Locking impicitly init V8 if required (which will in turn enter the default isolate if required)
      if (i == V8Monkey::InternalIsolate::GetDefaultIsolate()) {
        V8::Initialize();
      }

      hasLock = true;
    } else {
      hasLock = false;
    }
  }


  Locker::~Locker() {
    if (hasLock) {
      V8Monkey::InternalIsolate* i = reinterpret_cast<V8Monkey::InternalIsolate*>(isolate);

      if (i == V8Monkey::InternalIsolate::GetDefaultIsolate()) {
        i->Exit();
      }

      i->Unlock();
    }
  }


  Unlocker::Unlocker(Isolate* iso) {
    V8Monkey::InternalIsolate* i;

    if (iso == NULL) {
      i = V8Monkey::InternalIsolate::GetDefaultIsolate();
      isolate = reinterpret_cast<Isolate*>(i);
    } else {
      isolate = iso;
      i = reinterpret_cast<V8Monkey::InternalIsolate*>(isolate);
    }

    if (i == V8Monkey::InternalIsolate::GetDefaultIsolate()) {
      i->Exit();
    }

    i->Unlock();
  }


  Unlocker::~Unlocker() {
    V8Monkey::InternalIsolate* i = reinterpret_cast<V8Monkey::InternalIsolate*>(isolate);

    if (i == V8Monkey::InternalIsolate::GetDefaultIsolate()) {
      i->Enter();
    }

    i->Lock();
  }
}
