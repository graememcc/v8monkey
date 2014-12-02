// V8
#include "../include/v8.h"
#include "init.h"
#include "platform.h"
#include "autolock.h"

// Spidermonkey
#include "jsapi.h"


// pthreads, for pthread_once and co.
#include <pthread.h>


// exit
#include <stdlib.h>
// XXX Temp
#include <stdio.h>


namespace {
  // SpiderMonkey docs state that a call to JS_Shutdown—which releases any remaining resources not tied to specific
  // runtimes or contexts—is currently optional, but warns that this might not always be so. Thus we create an object
  // in static storage, whose destructor will ensure that the appropriate shutdown call is made.
  // XXX Need to be sure that there won't be any extant threads holding runtimes in existence, in the face of no
  //     guarantees with regards to static destructor ordering
  class AutoSpiderMonkeyShutdown {
    public:
      AutoSpiderMonkeyShutdown() {}


      ~AutoSpiderMonkeyShutdown() {
        JS_ShutDown();
      }
  } autoFreeEngine;


  // Was SpiderMonkey initted succesfully? This will be set by InitializeSpiderMonkey. This must not be read prior to a
  // pthread_once call for initialization
  bool gEngineInitSucceeded = false;


  // Initialize SpiderMonkey at most once
  void InitializeSpiderMonkey()
  {
    gEngineInitSucceeded = JS_Init();
  }


  // Mutex for checking/modifying engine disposal state
  v8::V8Monkey::Mutex* gEngineDisposalMutex = v8::V8Monkey::Platform::CreateMutex();


  // Ensure SpiderMonkey is initialized at most once
  v8::V8Monkey::OneTimeFunctionControl* gSpiderMonkeyInitControl = v8::V8Monkey::Platform::CreateOneShotFunction(InitializeSpiderMonkey);


  // Has V8 been 'disposed'?
  bool gV8IsDisposed = false;
}

  
namespace v8 {


bool V8::Initialize()
{
  gSpiderMonkeyInitControl->Run();

  // gEngineInitSucceeded will now be in a stable state
  if (!gEngineInitSucceeded) {
    // The V8 API appears to present engine init as infallible. In SpiderMonkey, failure—though unlikely—is possible.
    // If init failed, I doubt we'll be able to sensibly proceed.
    exit(1);
  }

  return true;
}


bool V8::Dispose()
{
  // XXX V8::Dispose has some semantics around stopping of utility threads that we haven't tackled
  //     For now, this is a no-op: our static object above will really shutdown SpiderMonkey
  V8Monkey::AutoLock mutex(gEngineDisposalMutex);

  gV8IsDisposed = true;
  return true;
}


// XXX We likely need to extend this to handle OOM and other such error situations
bool V8::IsDead()
{
  V8Monkey::AutoLock mutex(gEngineDisposalMutex);

  bool result = gV8IsDisposed;
  return result;
}


} // namespace v8
