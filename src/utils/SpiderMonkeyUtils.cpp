// partition
#include <algorithm>

// atomic_int
#include <atomic>

// JS_(Add|Remove)ExtraGCRootsTracer, JS_Init, JS_NewContext, JS_NewRuntime, JS::RuntimeOptionsRef, JS_ShutDown
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

// internal::
// TriggerFatalError, V8MONKEY_ASSERT
#include "V8MonkeyCommon.h"

// vector, begin, end
#include <vector>


namespace v8 {
  namespace internal {
    class Isolate;
  }
}


namespace {
  using namespace v8::V8Platform;


  // XXX Actually: do they even need to have entered the isolate now?
  /*
   * As V8 clients may start creating primitive values as soon as they enter a particular isolate, we are forced to
   * allocate a JSRuntime and JSContext at that time. Further, the isolate must then participate in GC rooting to
   * guarantee that the wrapped SpiderMonkey values are not destroyed. There are, however, a number of subtleties:
   *   - We don't want to register the isolate as a rooter for a particular JSRuntime more than once
   *   - At different times, different threads - each with their own JSRuntime - can enter the isolate, so the
   *     isolate must be a rooter for each of them
   *   - We cannot control what event will happen first: the client initiates isolate teardown, the isolate goes
   *     out of scope, or the thread exits
   *
   * Clearly we must deregister the isolate on its disposal / destruction; otherwise, when SpiderMonkey next performs a
   * garbage collection and invokes the callback function, the callback function's data could contain a dangling
   * pointer to an isolate that no longer exists. Of course, the isolate doesn't have visibility in to which threads
   * are still alive, so the attempt to deregister has the potential to dereference the pointer to a JSRuntime that
   * no longer exists (because it's owning thread exited).
   *
   * We encapsulate the requisite book-keeping here. We maintain a vector of RooterRegistration structs, which contain
   * the pertinent information for a isolate/JSRuntime pair. The AddIsolateRooter function maintains the invariant that
   * there is only one such entry for the isolate/JSRuntime pair. RemoveRooter functions are supplied for both isolates
   * and runtimes. In particular, once a thread requests a JSRuntime, the thread-local storage destructor ensures that
   * it will disengage from GC rooting on death.
   *
   */


  /*
   * We create one of these structs every time an Isolate asks to be rooted for GC, in order to avoid duplicate
   * registrations, and ensure that isolates are registered for each JSRuntime that they encounter.
   *
   */

  struct RooterRegistration {
    v8::internal::Isolate* isolate;
    JSRuntime* runtime;
    JSTraceDataOp callback;
    void* callbackData;

    RooterRegistration(v8::internal::Isolate* iso, JSRuntime* rt, JSTraceDataOp cb, void* data) :
      isolate {iso}, runtime {rt}, callback {cb}, callbackData {data} {}

    RooterRegistration(const RooterRegistration& other) = default;
    RooterRegistration(RooterRegistration&& other) = default;
    RooterRegistration& operator=(const RooterRegistration& other) = default;
    RooterRegistration& operator=(RooterRegistration&& other) = default;
    ~RooterRegistration() = default;
  };


  /*
   * Every time an isolate is registered as a rooter for a particular JSRuntime, a RooterRegistration should be
   * created, and inserted here. Similarly, it should be removed when either the Isolate or the thread owning the
   * JSRuntime is torn down. We don't anticipate large numbers of isolates or threads being spawned, so I think we
   * can get away with only sorting this on the fly where required for removals.
   *
   * Note: order is important here, this must be defined before the tearDown smart_ptr, as this cannot be destroyed
   * before the SpiderMonkeyTearDown class held by the smart pointer; the destruction of that object will need to
   * inspect rooter registrations.
   *
   */

  std::vector<RooterRegistration> rooterRegistrations {};


  /*
   * Bulk deregisters all isolates rooting the given JSRuntime. Called on thread destruction to ensure that isolates do
   * not attempt to remove themselves as rooters from JSRuntimes that no longer exist.
   *
   * This is a no-op if the given JSRuntime did not have any associated rooting isolates.
   *
   * rt is assumed to be a valid dereferencable pointer.
   *
   */

  void RemoveRooter(JSRuntime* rt) {
    V8MONKEY_ASSERT(rt, "RemoveRooter called with nullptr");

    if (rooterRegistrations.empty()) {
      return;
    }

    // We want to identify and group together the RooterRegistrations for the given JSRuntime, as we can then iterate
    // over them to remove them as rooters and delete them from our container
    auto begin = std::begin(rooterRegistrations);
    auto end = std::end(rooterRegistrations);
    auto sameRuntime = [&rt](const RooterRegistration& r) {
      return r.runtime == rt;
    };
    end = std::partition(begin, end, sameRuntime);

    // partition does not invalidate first
    std::for_each(begin, end, [&rt](const RooterRegistration& r) {
      JS_RemoveExtraGCRootsTracer(rt, r.callback, r.callbackData);
    });

    rooterRegistrations.erase(begin, end);
  }



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
    JSRuntime* rt {data->rt};

    V8MONKEY_ASSERT(rt, "JSRuntime* was null");
    V8MONKEY_ASSERT(data->cx, "JSContext* was null");

    // Although (as far as I can tell) the SpiderMonkey API doesn't require us to deregister any additional rooters at
    // JSRuntime destruction, we must still do so, to ensure we don't dereference dangling JSRuntime pointers when the
    // isolate is torn down
    RemoveRooter(rt);

    JS_DestroyContext(data->cx);
    JS_DestroyRuntime(rt);

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
   * The game here is that we have a unique_ptr - tearDown - with static storage duration. When SpiderMonkey is
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
          #ifdef DEBUG
          JSRuntime* rt {v8::SpiderMonkey::GetJSRuntimeForThread()};
          auto correctRuntime = [&rt](RooterRegistration& r) { return r.runtime == rt; };
          V8MONKEY_ASSERT(std::all_of(std::begin(rooterRegistrations), std::end(rooterRegistrations), correctRuntime),
                          "Other JSRuntimes still rooted!");
          #endif

          tearDownRuntimeAndContext(Platform::GetTLSData(smDataKey));
        }

        V8MONKEY_ASSERT(rooterRegistrations.empty(), "Some isolates/threads are still rooted!");

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


    void AddIsolateRooter(::v8::internal::Isolate* isolate, JSTraceDataOp callback, void* data) {
      // This function maintains the variant that, while there can be multiple registrations for a particular isolate,
      // there can be at most one for a particular isolate for a specific JSRuntime.
      JSRuntime* rt {GetJSRuntimeForThread()};
      V8MONKEY_ASSERT(rt, "Cannot add rooter: No JSRuntime!");

      auto end = std::end(rooterRegistrations);
      auto sameIsoAndRT = [&isolate, &rt](const RooterRegistration& r) {
        return r.isolate == isolate && r.runtime == rt;
      };
      auto pos = std::find_if(std::begin(rooterRegistrations), std::end(rooterRegistrations), sameIsoAndRT);

      if (pos != end) {
        V8MONKEY_ASSERT(pos->callback == callback, "Attempt to add root for same isolate/JSRuntime combination with "
                                                   "different callback");
        V8MONKEY_ASSERT(pos->callbackData == data, "Attempt to add root for same isolate/JSRuntime combination with different "
                                                   "client data");
        // Isolate is already rooting this runtime: nothing to do
        return;
      }

      rooterRegistrations.emplace_back(isolate, rt, callback, data);
      JS_AddExtraGCRootsTracer(rt, callback, data);
    }


    void RemoveRooter(::v8::internal::Isolate* isolate) {
      // Note: in contrast with AddIsolateRooter above, we do not require the thread destroying the isolate to have
      // an associated JSRuntime. However, we expect that the JSAPI call to remove rooters for a runtime does not
      // require the calling thread to be the owner of said runtime; were this ever to change, we'd be completely
      // screwed.

      if (rooterRegistrations.empty()) {
        return;
      }

      // We want to identify and group together the RooterRegistrations for the given isolate, as we can then iterate
      // over them to remove them as rooters and delete them from our container
      auto begin = std::begin(rooterRegistrations);
      auto end = std::end(rooterRegistrations);
      auto sameIsolate = [&isolate](const RooterRegistration& r) {
        return r.isolate == isolate;
      };
      end = std::partition(begin, end, sameIsolate);

      // partition does not invalidate first iterator
      std::for_each(begin, end, [](const RooterRegistration& r) {
        JS_RemoveExtraGCRootsTracer(r.runtime, r.callback, r.callbackData);
      });

      rooterRegistrations.erase(begin, end);
    }


    #ifdef V8MONKEY_INTERNAL_TEST
    void ForceGC() {
      JSRuntime* rt {GetJSRuntimeForThread()};
      V8MONKEY_ASSERT(rt, "Cannot force GC: No JSRuntime!");
      JS_GC(rt);
    }
    #endif


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
