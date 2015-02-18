// atomic_int
#include <atomic>

// JS_Init, JS_NewContext, JS_NewRuntime, JS::RuntimeOptionsRef, JS_ShutDown
#include "jsapi.h"

// unique_ptr
#include <memory>

// TLSKey, CreateTLSKey, GetTLSData, StoreTLSData
// XXX Remove me if we don't need this for compartments
#include "platform/platform.h"

// SpiderMonkeyUtils definition
#include "utils/SpiderMonkeyUtils.h"

// EXPORT_FOR_TESTING_ONLY
#include "test.h"

// V8_UNUSED
#include "v8config.h"

// TriggerFatalError
#include "V8MonkeyCommon.h"


namespace {
  using namespace v8::V8Platform;


  class SpiderMonkeyTearDown;
  std::unique_ptr<SpiderMonkeyTearDown> tearDown {nullptr};


  std::atomic<int> runtimeCount {0};


  void recordJSRuntimeConstruction() {
    std::atomic_fetch_add(&runtimeCount, 1);
  }


  void recordJSRuntimeDestruction() {
    std::atomic_fetch_sub(&runtimeCount, 1);
  }


  // Stores pointer to RTCX data for each thread
  TLSKey* smDataKey {nullptr};


  void tearDownRuntimeAndContext(void* raw) {
    using SpiderMonkeyData = v8::SpiderMonkey::SpiderMonkeyData;

    if (!raw) {
      return;
    }

    SpiderMonkeyData* data {reinterpret_cast<SpiderMonkeyData*>(raw)};

    V8MONKEY_ASSERT(data->rt, "JSRuntime* was null");
    V8MONKEY_ASSERT(data->cx, "JSContext* was null");

    JS_DestroyContext(data->cx);
    JS_DestroyRuntime(data->rt);

    delete data;

    // We might be called by the SpiderMonkeyTearDown class, so zero out the TLS pointer
    Platform::StoreTLSData(smDataKey, nullptr);

    recordJSRuntimeDestruction();
  }


  void ensureTLSKey() {
    static bool V8_UNUSED initialized {[]() {
      smDataKey = Platform::CreateTLSKey(tearDownRuntimeAndContext);
      return true;
    }()};
  }


  /*
   * Assign this thread a JSRuntime and JSContext. The caller should have checked that the thread doesn't already have
   * one assigned.
   *
   */

  void assignRuntimeAndContext() {
    using SpiderMonkeyData = v8::SpiderMonkey::SpiderMonkeyData;

    ensureTLSKey();
    v8::SpiderMonkey::EnsureSpiderMonkey();

    JSRuntime* rt {JS_NewRuntime(JS::DefaultHeapMaxBytes)};

    if (!rt) {
      // The game is up, abort
      v8::V8Monkey::Abort("InternalIsolate::Enter", "SpiderMonkey's JS_NewRuntime failed", false);
      return;
    }

    // At time of writing, the stackChunkSize parameter isn't actually used by SpiderMonkey. However, it might affect
    // implementation of the V8 resource constraints class.
    JSContext* cx {JS_NewContext(rt, 8192)};
    if (!cx) {
      // The game is up
      JS_DestroyRuntime(rt);
      v8::V8Monkey::Abort("InternalIsolate::Enter", "SpiderMonkey's JS_NewContext failed", false);
      return;
    }

    // Set options here. Note: the MDN page for JS_NewContext tells us to use JS_(G|S)etOptions. This changed in
    // bug 880330.
    JS::RuntimeOptionsRef(rt).setVarObjFix(true);

    SpiderMonkeyData* data {new SpiderMonkeyData {rt, cx}};
    Platform::StoreTLSData(smDataKey, data);

    recordJSRuntimeConstruction();
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

        int threadsWithRuntimes {std::atomic_load(&runtimeCount)};

        if (!isBeingDestroyed && threadsWithRuntimes > 1) {
          // Hmm, still some threads running. Let's try again when static destructors run
          return;
        } else if (isBeingDestroyed) {
          V8MONKEY_ASSERT(threadsWithRuntimes <= 1, "Attempting to destroy V8 when other threads still exist");
        }

        // The code that tears down the JSRuntime and JSContext normally won't run until thread exit, which in the
        // case of the main thread is too late. We must destroy them manually.
        if (threadsWithRuntimes == 1) {
          tearDownRuntimeAndContext(Platform::GetTLSData(smDataKey));
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
          // I doubt we can do anything useful if we failed to init SpiderMonkey
          ::v8::V8Monkey::Abort("v8::Initialize", "Failed to init Spidermonkey");
          // Unreached
          return false;
        }

        SpiderMonkeyTearDown* smTearDown {new SpiderMonkeyTearDown};
        tearDown.reset(smTearDown);
        return true;
      }()};
    }


    void TearDownSpiderMonkey() {
      if (!tearDown.get()) {
        return;
      }

      tearDown->AttemptDispose();
    }


  void EnsureRuntimeAndContext() {
    static bool V8_UNUSED initialized {[]() {
      // It is a SpiderMonkey API requirement that the first thread's runtime and context are stood up in a thread-safe
      // fashion
      assignRuntimeAndContext();
      return true;
    }()};

    // We assume that if the thread has a JSRuntime, then it must also have a JSContext
    if (GetJSRuntimeForThread()) {
      return;
    }

    assignRuntimeAndContext();
  }


  SpiderMonkeyData GetJSRuntimeAndJSContext() {
    ensureTLSKey();
    void* raw = ::v8::V8Platform::Platform::GetTLSData(smDataKey);
    if (raw) {
      SpiderMonkeyData* data {reinterpret_cast<SpiderMonkeyData*>(raw)};
      return {data->rt, data->cx};
    }

    return {nullptr, nullptr};
  }


  JSRuntime* GetJSRuntimeForThread() {
    return GetJSRuntimeAndJSContext().rt;
  }


  JSContext* GetJSContextForThread() {
    return GetJSRuntimeAndJSContext().cx;
  }


/*
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
