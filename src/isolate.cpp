// V8
#include "v8.h"
#include "isolate.h"


namespace v8 {
  Isolate* Isolate::New() {
    // XXX Should we init engine here?!?
    return NULL;
  }


  Isolate* Isolate::GetCurrent() {
    return NULL;
  }


  /*
   * XXX This comment might now be a lie.
   * Isolate::Dispose should be a no-op. In V8, this is an opportunity to perform housekeeping relating to the threads
   * associated with the given Isolate. Spidermonkey has a fundamentally different model, allowing many threads per
   * runtime, provided the API conventions regarding JS_Requests are followed.
   *
   */
  void Isolate::Dispose() {
    // An isolate's destructor is private
    delete this;
  }


  void Isolate::Enter() {
  }


  void Isolate::Exit() {
  }


  namespace V8Monkey {
    bool InternalIsolate::IsDefaultIsolate(InternalIsolate* i) {
      return false;
    }


    InternalIsolate* InternalIsolate::GetDefaultIsolate() {
      return defaultIsolate;
    }


    #ifdef V8MONKEY_INTERNAL_TEST
    bool InternalIsolate::IsEntered(InternalIsolate* i) {
      return false;
    }
    #endif


    InternalIsolate* InternalIsolate::defaultIsolate = new InternalIsolate();
  }
} // namespace v8
