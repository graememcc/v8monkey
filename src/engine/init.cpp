// V8
#include "v8.h"


#include "init.h"
#include "runtime/isolate.h"
#include "platform.h"
#include "test.h"
#include "v8monkey_common.h"


// Spidermonkey
#include "jsapi.h"


// exit
#include <stdlib.h>


/*
 * TODO: As with isolate.cpp, much of this code could likely be simplified if multithreaded isolates prove impossible
 *
 */


namespace {
  // Was SpiderMonkey initted succesfully? This should only be read while holding the relevant mutex
  bool engineInitAttempted = false;
  bool engineInitSucceeded = false;


  // Has V8 been initted? We sometimes need to distinguish between V8 and SM inits
  bool v8initted = false;


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


  // The single static initializer for our global state, to avoid running into static initialization ordering problems across
  // translation unit boundaries.
  class OneTrueStaticInitializer {
    public:
      OneTrueStaticInitializer() {
        // Initialize all required TLS keys
        v8::V8Monkey::V8MonkeyCommon::InitTLSKeys();
        v8::V8Monkey::V8MonkeyCommon::EnsureDefaultIsolate();

        // Just go ahead and init SpiderMonkey here too. We don't acquire the mutex: we should be single-threaded at this point
        engineInitAttempted = true;
        engineInitSucceeded = JS_Init();
      }

      // SpiderMonkey docs state that a call to JS_Shutdown-which releases any remaining resources not tied to specific
      // runtimes or contexts-is currently optional, but warns that this might not always be so. Thus we ensure we shut
      // things down in this destructor.
      ~OneTrueStaticInitializer() {
        // Ensure default isolate is disposed
        delete v8::V8Monkey::InternalIsolate::GetCurrent();

        // Force any extant threads to dispose of their JSRuntimes and JSContexts
        v8::V8Monkey::V8MonkeyCommon::ForceRTCXDisposal();
        JS_ShutDown();
      }
  } staticInitializer;
}


namespace v8 {
  bool V8::Initialize() {
    v8initted = true;

    if (!engineInitSucceeded) {
      // The V8 API appears to present engine init as infallible. In SpiderMonkey, failure—though unlikely—is possible.
      // If init failed, I doubt we'll be able to sensibly proceed.
      exit(1);
    }

    // On initialization, if the calling thread has not entered an isolate, the default isolate will be entered
    V8Monkey::InternalIsolate::EnsureInIsolate();

    // V8 compat
    V8Monkey::InternalIsolate::GetCurrent()->Init();

    return true;
  }


  bool V8::Dispose() {
    using namespace v8::V8Monkey;

    if (!engineInitSucceeded || !v8initted) {
      return true;
    }

    // XXX V8::Dispose has some semantics around stopping of utility threads that we haven't tackled
    //     For now, this is a no-op: our static object above will really shutdown SpiderMonkey

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

    // Attempt to dispose of isolate, in case we're being called from an off-main thread that entered the default
    i->Dispose();

    V8IsDisposed = true;
    return true;
  }


  // XXX We likely need to extend this to handle OOM and other such error situations
  bool V8::IsDead() {
    return V8IsDisposed || hasFatalError;
  }


  namespace V8Monkey {
    void V8MonkeyCommon::TriggerFatalError(const char* location, const char* message) {
      hasFatalError = true;
      GetFatalErrorHandlerOrDefault()(location, message);
    }


    #ifdef V8MONKEY_INTERNAL_TEST
      bool TestUtils::IsV8Initialized() {
        return v8initted;
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
