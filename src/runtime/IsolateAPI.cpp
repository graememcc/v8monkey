// internal::isolate definition
#include "runtime/isolate.h"

// Class definition
#include "v8.h"


/*
 * The implementation of the API Isolate class. In common with V8, it largely delegates to the internal isolate.
 * Indeed, API isolates are not constructable: the pointers are just reinterpreted internal isolate pointers
 * (The V8 team contains some smart people, so we can take it as a given that they've checked that the alignment
 * requirement of API isolates isn't stricter than their internal brethren. Note that API isolates don't contain any
 * data, so it would be very very surprising if they were: they should be size 1).
 *
 */


/*
 #define FORWARD_TO_INTERNAL(method) \
   do { \
    V8Monkey::InternalIsolate* internal = reinterpret_cast<V8Monkey::InternalIsolate*>(this); \
    internal->method(); \
  } while (0);
*/


namespace v8 {
//   int V8::GetCurrentThreadId() {
//     if (V8::IsDead()) {
//       V8MonkeyCommon::TriggerFatalError("V8::GetCurrentThreadId", "V8 is dead");
//       // Note V8 actually soldiers on here and doesn't return early; we will too!
//     }
//
//     // The V8 API specifies that this call implicitly inits V8
//     V8::Initialize();
//
//     return fetchOrAssignThreadID();
//   }
//
//
//   void V8::SetFatalErrorHandler(FatalErrorCallback fn) {
//     // Fatal error handlers are isolate-specific
//     V8Monkey::InternalIsolate* i = V8Monkey::InternalIsolate::EnsureInIsolate();
//     i->SetFatalErrorHandler(fn);
//   }
//
//
//   Isolate* Isolate::New() {
//     V8Monkey::InternalIsolate* internal = new V8Monkey::InternalIsolate();
//     return reinterpret_cast<Isolate*>(internal);
//   }
//
//
  Isolate* Isolate::GetCurrent() {
    return reinterpret_cast<Isolate*>(internal::Isolate::GetCurrent());
  }
//
//
//   void Isolate::Dispose() {
//     V8Monkey::InternalIsolate* internal = reinterpret_cast<InternalIsolate*>(this);
//
//     // Note that we check this here: the default isolate can be disposed of while entered. In fact, it is
//     // a V8 API requirement that it is entered when V8 is disposed (which in turn disposes the default
//     // isolate)
//     // XXX Clarify this comment. In fact clarify you can really dispose of default if in it
//     // XXX Update: I'm pretty sure this is wrong
//     if (internal->ContainsThreads()) {
//       V8Monkey::V8MonkeyCommon::TriggerFatalError("v8::Isolate::Dispose",
//                                                   "Attempt to dispose isolate in which threads are active");
//       return;
//     }
//
//     internal->Dispose();
//   }
//
//
//   void Isolate::Enter() {
//     FORWARD_TO_INTERNAL(Enter);
//   }
//
//
//   void Isolate::Exit() {
//     FORWARD_TO_INTERNAL(Exit);
//   }
//
//
//   void* Isolate::GetData() {
//     return reinterpret_cast<V8Monkey::InternalIsolate*>(this)->GetEmbedderData();
//   }
//
//
//   void Isolate::SetData(void* data) {
//     reinterpret_cast<V8Monkey::InternalIsolate*>(this)->SetEmbedderData(data);
//   }
}
//#undef FORWARD_TO_INTERNAL
