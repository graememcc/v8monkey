// exit
#include <cstdlib>

// JS_Init, JS_Shutdown
#include "jsapi.h"

// InternalIsolate::{EnsureInIsolate, GetCurrent, GetDefaultIsolate, GetFatalErrorHandler}
#include "runtime/isolate.h"

// Print
#include "platform/platform.h"

// string
#include <string>

// TestUtils interface
#include "utils/test.h"

// V8MonkeyCommon interface
#include "utils/V8MonkeyCommon.h"

// V8 interface
#include "v8.h"


/*
 * TODO: As with isolate.cpp, much of this code could likely be simplified if multithreaded isolates prove impossible
 *
 */


// XXX This file really needs tidied up
//
//
//namespace {
//  // Assorted static asserts that don't really have a good home
//  static_assert(sizeof(v8::Local<v8::Value>) == sizeof(v8::Value*), "Handles are pointer-sized");
//  static_assert(sizeof(v8::Local<v8::Value>) == sizeof(v8::Value*), "Locals are pointer-sized");
//  static_assert(sizeof(v8::Persistent<v8::Value>) == sizeof(v8::Value*), "Persistents are pointer-sized");
//
//  // Was SpiderMonkey initted succesfully?
//  bool engineInitAttempted = false;
//  bool engineInitSucceeded = false;
//
//
//  // Has V8 been initted? We sometimes need to distinguish between V8 and SM inits
//  bool v8initted = false;
//
//
//  // Has V8 been 'disposed'?
//  bool V8IsDisposed = false;
//
//
//  /*
//   * The single static initializer for our global state, to avoid running into static initialization ordering problems across
//   * translation unit boundaries.
//   *
//   */
//
//  class OneTrueStaticInitializer {
//    public:
//      OneTrueStaticInitializer() {
//        v8::V8Monkey::V8MonkeyCommon::InitTLSKeys();
//
//        // Just go ahead and init SpiderMonkey here too. We don't use a mutex here: we should be single-threaded at this point
//        engineInitAttempted = true;
//        engineInitSucceeded = JS_Init();
//
//        if (engineInitSucceeded) {
//          v8::V8Monkey::InternalIsolate::EnsureDefaultIsolateForStaticInitializerThread();
//        }
//
//        v8::V8Monkey::V8MonkeyCommon::InitPrimitiveSingletons();
//      }
//
//      /*
//       * SpiderMonkey docs state that a call to JS_Shutdown-which releases any remaining resources not tied to specific
//       * runtimes or contexts-is currently optional, but warns that this might not always be so. Thus we ensure we shut
//       * things down in this destructor.
//       *
//       */
//
//      ~OneTrueStaticInitializer() {
//        v8::V8Monkey::V8MonkeyCommon::TearDownPrimitiveSingletons();
//
//        // Ensure default isolate is disposed
//        delete v8::V8Monkey::InternalIsolate::GetCurrent();
//
//        if (engineInitSucceeded) {
//          // Force any extant threads to dispose of their JSRuntimes and JSContexts
//          v8::V8Monkey::V8MonkeyCommon::ForceMainThreadRTCXDisposal();
//          JS_ShutDown();
//        }
//      }
//  } staticInitializer;
//}
//
//
namespace v8 {
//  // XXX V8 Version bump: now that we have a) C++11/VS 2013 and no default isolate, I suspect we can init SpiderMonkey using a
//  // function local static, initialized by a function call. We will want to make the called function noexcept so that we terminate
//  // hard if SpiderMonkey init fails.
  bool V8::Initialize() {
    // XXX Shouldn't need two calls here
    if (Isolate::GetCurrent() && V8::IsDead()) {
      return false;
    }
//    v8initted = true;
//
//    if (!engineInitSucceeded) {
//      // The V8 API appears to present engine init as infallible. In SpiderMonkey, failure—though unlikely—is possible.
//      // If init failed, I doubt we'll be able to sensibly proceed.
//      exit(1);
//    }
//
//    // On initialization, if the calling thread has not entered an isolate, the default isolate will be entered
//    V8Monkey::InternalIsolate::EnsureInIsolate();
//
//    // V8 compat
//    V8Monkey::InternalIsolate::GetCurrent()->Init();
//
    return true;
  }
//
//
  bool V8::Dispose() {
//    using namespace v8::V8Monkey;
//
//    if (!engineInitSucceeded || !v8initted) {
//      return true;
//    }
//
//    // XXX V8::Dispose has some semantics around stopping of utility threads that we haven't tackled
//    //     For now, this is a no-op: our static object above will really shutdown SpiderMonkey
//
//    InternalIsolate* i = InternalIsolate::GetCurrent();
//    if (i == nullptr || i != InternalIsolate::GetDefaultIsolate()) {
//      V8MonkeyCommon::TriggerFatalError("v8::V8::Dispose", "Must dispose V8 from main thread outside any isolate");
//      return false;
//    }
//// XXX REMOVE ME
//if (InternalIsolate::IsEntered(i)) {
//      V8MonkeyCommon::TriggerFatalError("v8::V8::Dispose", "You're still in the isolate!");
//      return false;
//}
//
//    // Attempt to dispose of isolate, in case we're being called from an off-main thread that entered the default
//    i->Dispose();
//
//    V8IsDisposed = true;
    return true;
  }


  namespace V8Monkey {
    void TriggerFatalError(const char* location, const char* message) {
      v8::FatalErrorCallback fn {nullptr};
      v8::internal::Isolate* i {v8::internal::Isolate::GetCurrent()};
      if (i) {
        fn = i->GetFatalErrorHandler();
      }

      if (i && fn) {
        fn(location, message);
        i->SignalFatalError();
        return;
      }

      std::string s {"Error at "};
      s += location;
      s += ": ";
      s += message;
      s += "\n";
      V8Platform::Platform::PrintError(s.c_str());
      exit(1);
    }


//    bool V8MonkeyCommon::CheckDeath(const char* method) {
//      if (V8::IsDead()) {
//        TriggerFatalError(method, "V8 is dead");
//        return true;
//      }
//
//      return false;
//    }
//
//
//    #ifdef V8MONKEY_INTERNAL_TEST
//      bool TestUtils::IsV8Initialized() {
//        return v8initted;
//      }
//
//
//      /*
//       * Many API functions implicitly init V8 but first confirm it's not dead. They test this by setting up a fatal
//       * error handler and triggering the implicitly initting function. There are a couple of key problems: calling
//       * TriggerFatalError, though killing V8, invokes the fatal error handler itself, defeating the point of the
//       * test, and calling SetFatalErrorHandler implicitly inits V8, which is again something the test needs to
//       * control.
//       *
//       * The following function thus circumvents API conventions, and installs an error handler without init, and kills
//       * V8 without triggering the handler.
//       *
//       */
//
//      void TestUtils::SetHandlerAndKill(FatalErrorCallback f) {
//        InternalIsolate* i = InternalIsolate::GetCurrent();
//        i->SetFatalErrorHandler(f);
//        hasFatalError = true;
//      }
//    #endif
  }
}
