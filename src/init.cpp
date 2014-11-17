// V8
#include "../include/v8.h"
#include "init.h"

// Spidermonkey
#include "jsapi.h"


// Have we (or the user) made a call to JS_Init in SpiderMonkey?
bool gEngineHasBeenInitialized = false;

// Did our attempt to init SpiderMonkey succeed (note: this is meaningless when gEngineHasBeenInitialized is false)
bool gEngineInitSucceeded = false;

// Has the engine been destroyed?
bool gEngineHasBeenDestroyed = false;


namespace {
  // Helper class to automagically free up resources from the JS engine
  static class AutoFreeEngine {
    public:
      AutoFreeEngine() {}


      ~AutoFreeEngine() {
        if (gEngineHasBeenInitialized && gEngineInitSucceeded && !gEngineHasBeenDestroyed) {
          JS_ShutDown();
        }
      }
  } autoFreeEngine;
}

  
namespace v8 {


bool V8::Initialize()
{
  if (!gEngineHasBeenInitialized) {
    gEngineHasBeenInitialized = true;
    gEngineInitSucceeded = JS_Init();
  }

  // For V8 compat, we always return true
  return true;
}


// XXX V8::Dispose has some semantics around stopping of utility threads that we don't yet tackle
bool V8::Dispose()
{
  if (!gEngineHasBeenInitialized) {
    // Don't rely on callers to do the right thing in terms of examining the globals. If we were never initialized,
    // make it look like we once were.
    gEngineHasBeenInitialized = true;
    gEngineInitSucceeded = true;
  } else if (gEngineInitSucceeded) {
    JS_ShutDown();
  }

  gEngineHasBeenDestroyed = true;
  return true;
}


// XXX We likely need to extend this to handle OOM and other such error situations
bool V8::IsDead()
{
  return (gEngineHasBeenInitialized && gEngineInitSucceeded) || gEngineHasBeenDestroyed;
}


} // namespace v8
