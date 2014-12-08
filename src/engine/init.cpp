// V8
#include "v8.h"


#include "autolock.h"
#include "init.h"
#include "isolate.h"
#include "platform.h"
#include "test.h"
#include "v8monkey_common.h"


// Spidermonkey
#include "jsapi.h"


// exit
#include <stdlib.h>


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


  // Was SpiderMonkey initted succesfully? This should only be read while holding the mutex
  bool engineInitAttempted = false;
  bool engineInitSucceeded = false;
  v8::V8Platform::Mutex engineInitMutex;


  // Initialize SpiderMonkey
  void InitializeSpiderMonkey() {
    v8::V8Monkey::AutoLock lock(engineInitMutex);
    if (engineInitAttempted)
      return;

    engineInitAttempted = true;
    engineInitSucceeded = JS_Init();
  }


  // Mutex for checking/modifying engine disposal state
  v8::V8Platform::Mutex engineDisposalMutex;


  // Has V8 been 'disposed'?
  bool V8IsDisposed = false;


  // Does the engine have a fatal error?
  bool hasFatalError = false;


  // The default fatal error handler
  void DefaultFatalErrorHandler(const char* location, const char* message) {
    using namespace v8::V8Platform;

    // XXX FIX ME
    Platform::PrintError("Error at ");
    Platform::PrintError(location);
    Platform::PrintError(": ");
    Platform::PrintError(message);
    Platform::PrintError("\n");
    exit(1);
  }


  v8::FatalErrorCallback GetFatalErrorHandlerOrDefault() {
    using namespace v8::V8Monkey;

    InternalIsolate* i = InternalIsolate::GetCurrent();
    if (i == NULL) {
      i = InternalIsolate::GetDefaultIsolate();
    }

    v8::FatalErrorCallback fn = i->GetFatalErrorHandler();
    return fn ? fn : DefaultFatalErrorHandler;
  }
}


namespace v8 {
  bool V8::Initialize() {
    InitializeSpiderMonkey();

    // engineInitSucceeded will now be in a stable state
    if (!engineInitSucceeded) {
      // The V8 API appears to present engine init as infallible. In SpiderMonkey, failure—though unlikely—is possible.
      // If init failed, I doubt we'll be able to sensibly proceed.
      exit(1);
    }

    // On initialization, if the calling thread has not entered an isolate, the default isolate will be entered
    V8Monkey::InternalIsolate::EnsureInIsolate();

    return true;
  }


  bool V8::Dispose() {
    using namespace v8::V8Monkey;

    {
      AutoLock lock(engineInitMutex);
      if (!engineInitSucceeded) {
        return true;
      }
    }

    // XXX V8::Dispose has some semantics around stopping of utility threads that we haven't tackled
    //     For now, this is a no-op: our static object above will really shutdown SpiderMonkey
    AutoLock mutex(engineDisposalMutex);

    InternalIsolate* i = InternalIsolate::GetCurrent();
    if (i == NULL || i != InternalIsolate::GetDefaultIsolate()) {
      V8MonkeyCommon::TriggerFatalError("v8::V8::Dispose", "Must dispose V8 from main thread outside any isolate");
      return false;
    }
// XXX REMOVE ME
if (InternalIsolate::IsEntered(i)) {
      V8MonkeyCommon::TriggerFatalError("v8::V8::Dispose", "You're still in the isolate!");
      return false;
}
    // The dispose method won't delete default isolates, so just delete here
    delete i;

    V8IsDisposed = true;
    return true;
  }


  // XXX We likely need to extend this to handle OOM and other such error situations
  bool V8::IsDead() {
    V8Monkey::AutoLock mutex(engineDisposalMutex);

    return V8IsDisposed || hasFatalError;
  }


  namespace V8Monkey {
    void V8MonkeyCommon::TriggerFatalError(const char* location, const char* message) {
      hasFatalError = true;
      GetFatalErrorHandlerOrDefault()(location, message);
    }


    #ifdef V8MONKEY_INTERNAL_TEST
      bool TestUtils::IsV8Initialized() {
        return engineInitAttempted;
      }


      // Many API functions implicitly init V8 but first confirm it's not dead. They test this by setting up a fatal
      // error handler and triggering the implicitly initting function. There are a couple of key problems: calling
      // TriggerFatalError, though killing V8, invokes the fatal error handler itself, defeating the point of the
      // test, and calling SetFatalErrorHandler implicitly inits V8, which is again something the test needs to
      // control.
      //
      // The following function thus cicumvents API conventions, and installs an error handler without init, and kills
      // V8 without triggering the handler.
      void TestUtils::SetHandlerAndKill(FatalErrorCallback f) {
        InternalIsolate* i = InternalIsolate::GetCurrent();
        i->SetFatalErrorHandler(f);
        hasFatalError = true;
      }
    #endif
  }
}
