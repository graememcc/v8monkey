// atomic_int
#include <atomic>

// JS_Init, JS_ShutDown
#include "jsapi.h"

// unique_ptr
#include <memory>

// TLSKey, CreateTLSKey, GetTLSData, StoreTLSData
// XXX Remove me if we don't need this for compartments
#include "platform/platform.h"

// SpiderMonkeyUtils definition
#include "utils/SpiderMonkeyUtils.h"

// V8_UNUSED
#include "v8config.h"

// TriggerFatalError
#include "V8MonkeyCommon.h"


// XXX Remove me if we don't need this for compartments
using namespace v8::V8Platform;


namespace {
  class SpiderMonkeyTearDown;


  std::unique_ptr<SpiderMonkeyTearDown> tearDown {nullptr};

  std::atomic<int> runtimeCount {0};


  void RecordJSRuntimeConstruction() {
    std::atomic_fetch_add(&runtimeCount, 1);
  }


  void RecordJSRuntimeDestruction() {
    std::atomic_fetch_sub(&runtimeCount, 1);
  }


  /*
   * This class is used to ensure we always teardown SpiderMonkey after it has been initialized, regardless of whether
   * the client remembers to call V8::Dispose or not.
   *
   * The game here is that we have a unique_ptr tearDown with static storage duration. When SpiderMonkey is
   * initialized, we heap allocate a new instance of SpiderMonkeyTearDown, and give it to the unique_ptr to manage the
   * lifetime. Then, all we need is for the destructor to check to see if SM has been correctly shutdown, and if not
   * go ahead and invoke the shutdown.
   *
   */

  class SpiderMonkeyTearDown {
    public:
      SpiderMonkeyTearDown() : isDisposed {false} {}

      ~SpiderMonkeyTearDown() {
        if (!isDisposed) {
          AttemptDispose(true);
        }
      }

      void AttemptDispose(bool isBeingDestroyed = false) {
        V8MONKEY_ASSERT(!isDisposed || isBeingDestroyed, "V8::Dispose called more than once?");

        if (isDisposed) {
          return;
        }

        if (isBeingDestroyed) {
          int threadsWithRuntimes {std::atomic_load(&runtimeCount)};
          V8MONKEY_ASSERT(threadsWithRuntimes <= 1, "Attempting to destroy V8 when other threads still exist");

          // XXX Add logic to force disposal of last thread's runtime
          if (threadsWithRuntimes == 1) {
          }
        }

        JS_ShutDown();
        isDisposed = true;
      }

      SpiderMonkeyTearDown(const SpiderMonkeyTearDown& other) = delete;
      SpiderMonkeyTearDown(SpiderMonkeyTearDown&& other) = delete;
      SpiderMonkeyTearDown& operator=(const SpiderMonkeyTearDown& other) = delete;
      SpiderMonkeyTearDown& operator=(SpiderMonkeyTearDown&& other) = delete;

    private:
      bool isDisposed {false};
  };
}


namespace v8{
  namespace SpiderMonkey {
    void EnsureSpiderMonkey() {
      static bool V8_UNUSED initialized {[]() {
        bool successful {JS_Init()};

        if (!successful) {
          // I doubt we can do anything useful if we failed to init SpiderMonkey (assumes TriggerFatalError aborts)
          ::v8::V8Monkey::TriggerFatalError("v8::Initialize", "Failed to init Spidermonkey", false);
          // Unreached
          return false;
        }

        SpiderMonkeyTearDown* smTearDown {new SpiderMonkeyTearDown};
        tearDown.reset(smTearDown);
        return true;
      }()};
    }


    void TearDownSpiderMonkey() {
      // Allow harmless V8::Dispose calls without init
      if (!tearDown.get()) {
        return;
      }

      tearDown->AttemptDispose();
    }

/*
    JSRuntime* SpiderMonkeyUtils::GetJSRuntimeForThread() {
      RTCXData rtcx = GetJSRuntimeAndJSContext();
      return rtcx.rt;
    }


    JSContext* SpiderMonkeyUtils::GetJSContextForThread() {
      RTCXData rtcx = GetJSRuntimeAndJSContext();
      return rtcx.cx;
    }


    JSCompartment* SpiderMonkeyUtils::GetCurrentCompartment() {
      // XXX TODO
      return nullptr;
    }


    JSCompartment* SpiderMonkeyUtils::GetPreviousCompartment() {
      // XXX TODO
      return nullptr;
    }
*/
  }
}
