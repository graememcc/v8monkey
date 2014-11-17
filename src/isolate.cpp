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


} // namespace v8
