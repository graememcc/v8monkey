// V8
#include "../include/v8.h"
#include "init.h"

// SpiderMonkey
#include "jsapi.h"


namespace v8 {


Isolate::Isolate()
{
  // XXX The gc bytes and nursery byte figures are taken from the JS shell in the SpiderMonkey tree. Need to figure out
  //     appropriate values
  mRuntime = JS_NewRuntime(8L * 1024L * 1024L, 2L * 1024L * 1024L);
}


Isolate::~Isolate()
{
  // Destroy our runtime, but be cogniscant that we may have received a null pointer
  if (mRuntime) {
    JS_DestroyRuntime(mRuntime);
  }
}


Isolate* Isolate::New() {
  ENSURE_ENGINE_INIT_OR_RETURN_NULL

  return new Isolate();
}


/*
 * Isolate::Dispose should be a no-op. In V8, this is an opportunity to perform housekeeping relating to the threads
 * associated with the given Isolate. Spidermonkey has a fundamentally different model, allowing many threads per
 * runtime, provided the API conventions regarding JS_Requests are followed.
 *
 */
void Isolate::Dispose() {
  // An isolate's destructor is private
  delete this;
}


} // namespace v8
