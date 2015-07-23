// atomic_int
#include <atomic>

// JS_Init
#include "jsapi.h"

// unique_ptr
#include <memory>

// OneShot
#include "platform/platform.h"

// SpiderMonkeyUtils definition
#include "utils/SpiderMonkeyUtils.h"


using namespace v8::V8Platform;


/*
 *                                   Engine Initialization and destruction
 *
 * In V8, the engine is initialized if necessary by a call to V8::Initialize. As far as I can tell, this function can
 * be called from multiple threads. Further, with the current API (3.28.73.0), V8::Initialize will be implicitly called
 * for the client if they have failed to do so (this changes in future versions of the API).
 *
 * In contrast, SpiderMonkey expects that it is initialized only once, and in a single-threaded manner. To satisfy this
 * constraint, the V8::Initialize function calls SpiderMonkey::EnsureSpiderMonkey, which wraps a OneShot function (one
 * that will only be executed once, regardless of how many threads call it) to perform the actual initialization of the
 * engine.
 *
 * Of course, once SpiderMonkey has been initialized, we should be good citizens and ensure it is shutdown correctly.
 * (Although not currently mandatory, it might be in the future.) Ideally, the client will remember to call V8::Dispose,
 * and we can tear it down then.
 *
 * There is, of course, a complication. In order to tear down SpiderMonkey, all JSContexts and JSRuntimes must have
 * previously been torn down. Each thread is assigned a JSRuntime/JSContext on isolate entry, so the problem becomes
 * an ordering problem; how then can we tell that all threads (except possibly the main thread) have exited?
 *
 * Clearly, the first issue is tracking JSRuntime etc. construction. The machinery immediately following atomically
 * counts whenever a new JSRuntime is created. We assume that this will only be invoked when both the JSRuntime and
 * JSContext were successfully constructed, and that the JSRuntime was immediately disposed of if JSContext
 * construction failed.
 *
 * Next, we store the JSRuntime/JSContext in thread-local storage. By doing this, we can supply a destructor function
 * that will run on thread exit, destroying the runtime and context, and atomically decrementing the JSRuntime count.
 *
 * With this in place, we can ensure that when the client calls V8::Dispose, we don't attempt teardown if multiple
 * threads with JSRuntimes still exist. Of course, our problem now is that we still want to schedule a later teardown
 * of SpiderMonkey, were destruction at client request time not possible, or if the client never bothered calling
 * V8::Dispose.
 *
 * To that end, we heap allocate a SpiderMonkeyTearDown class, whose sole purpose is to ensure graceful shutdown of
 * SpiderMonkey. By placing the pointer in a unique_ptr with static storage duration, we can schedule an attempt to
 * destroy SpiderMonkey when static destructors for this translation unit run.
 *
 * By the point the class's destructor runs, there should be at most one surviving thread - the thread running the
 * destructors. If the client has detached threads, there's not much we can do, so we simply assert (I don't believe
 * that V8 would cope well with detached threads either). For that last thread, we cannot wait until thread
 * destruction for JSRuntime / JSContext teardown: the whole point of the exercise is that we're destroying
 * SpiderMonkey now, it'll be gone by then. so the SpiderMonkeyTearDown destructor manually forces teardown of the
 * surviving thread's JSRuntime and JSContext. We are then safe to destroy SpiderMonkey.
 *
 * Of course, before embarking on this, the SpiderMonkeyTearDown destructor first checks that SpiderMonkey has not
 * already been destroyed (or indeed never created). It returns immediately in that case.
 *
 */

namespace {
  /*
   * Track whether SpiderMonkey has been torn down
   *
   */

  bool spiderMonkeyDestroyed {false};


  /*
   * Key for retrieving a JSRuntime/JSContext pair from thread-local storage. Note that order is important here: as
   * static items in a translation unit are destroyed in reverse order, the TLS key must be defined before the static
   * destructor for SpiderMonkey, as the key must still be alive when that destructor runs.
   *
   */

  extern "C" void tearDownRuntimeAndContext(void* raw);
  TLSKey<v8::SpiderMonkey::SpiderMonkeyData> smDataKey{tearDownRuntimeAndContext};


  /*
   * Likewise, the static destructor requires the continued existence of these atomics, so they too must remain before
   * the SpiderMonkeyTearDown unique_ptr. This atomic is used to count the number of extant threads: explicit engine
   * disposal must be delayed while this value is greater than 1.
   *
   */


  /*
   * Machinery for tracking the number of extant runtimes; these must be tracked to decide whether it is safe to tear
   * down SpiderMonkey when the client requests V8 teardown.
   *
   * Ordering requirement: the atomic value must appear before the unique_ptr for the SpiderMonkeyTearDown class.
   *
   */

  std::atomic<int> runtimeCount {0};


  void recordJSRuntimeConstruction() {
    std::atomic_fetch_add(&runtimeCount, 1);
  }


  void recordJSRuntimeDestruction() {
    std::atomic_fetch_sub(&runtimeCount, 1);
  }


  class SpiderMonkeyTearDown;
  std::unique_ptr<SpiderMonkeyTearDown> tearDown {nullptr};


  /*
   * This class is used to ensure we always teardown SpiderMonkey after it has been initialized, regardless of whether
   * the client remembers to call V8::Dispose or not. When the client requests V8 init, an instance of this class will
   * be heap allocated, and it's pointer stored in the unique_ptr above. Then, when V8::Dispose invokes our external
   * TearDownSpiderMonkey function, or static destructors run, the class's AttemptDispose method will be invoked.
   *
   * It is possible that AttemptDispose may return without destroying SpiderMonkey; this might be necessary due to the
   * existence of other threads with runtimes. In that case, another attempt will be scheduled during static destructor
   * execution. During static destruction, it is expected that there are no remaining obstacles to safe destruction.
   *
   */

  class SpiderMonkeyTearDown {
    public:
      SpiderMonkeyTearDown() {}

      ~SpiderMonkeyTearDown() {
        // V8::Dispose may have already successfully destroyed SpiderMonkey
        if (!spiderMonkeyDestroyed) {
          AttemptDispose(true);
        }
      }

      void AttemptDispose(bool fromStaticDestructor = false) {
        int threadsWithRuntimes {std::atomic_load(&runtimeCount)};

        if (!fromStaticDestructor && threadsWithRuntimes > 1) {
          // As fromStaticDestructor is false, we have been called explicitly by a client call to V8::Dispose.
          // However, we can't do anything while other threads still require the engine.
          return;
        } else if (fromStaticDestructor) {
          // Detached threads?
          V8MONKEY_ASSERT(threadsWithRuntimes <= 1, "Attempting to destroy V8 when other threads still exist");
        }

        // The code that tears down the JSRuntime and JSContext for the current thread normally won't run until thread
        // exit. If we're destroying the engine now, then that's too late. We manually force their destruction.
        if (threadsWithRuntimes == 1) {
          /*
          #ifdef DEBUG
          JSRuntime* rt {v8::SpiderMonkey::GetJSRuntimeForThread()};
          auto correctRuntime = [&rt](RooterRegistration& r) { return r.runtime == rt; };
          V8MONKEY_ASSERT(std::all_of(std::begin(rooterRegistrations), std::end(rooterRegistrations), correctRuntime),
                          "Other JSRuntimes still rooted!");
          #endif
          */
        }

        // V8MONKEY_ASSERT(rooterRegistrations.empty(), "Some isolates/threads are still rooted!");

        tearDownRuntimeAndContext(smDataKey.Get());
        JS_ShutDown();
        spiderMonkeyDestroyed = true;
      }

      // Note: we're storing this class in a unique_ptr, so must be MoveConstructible
      SpiderMonkeyTearDown(const SpiderMonkeyTearDown& other) = delete;
      SpiderMonkeyTearDown(SpiderMonkeyTearDown&& other) = default;
      SpiderMonkeyTearDown& operator=(const SpiderMonkeyTearDown& other) = delete;
      SpiderMonkeyTearDown& operator=(SpiderMonkeyTearDown&& other) = default;
  };


  /*
   * Attempts to init SpiderMonkey, aborting if unsuccesful. Also heap-allocates a SpiderMonkeyTearDown instance
   * to ensure graceful shutdown of SpiderMonkey.
   *
   */

  extern "C"
  void initSpiderMonkey() {
    bool successful {JS_Init()};

    if (!successful) {
      // I doubt we can do anything useful if we failed to init SpiderMonkey
      ::v8::V8Monkey::Abort("v8::Initialize", "Failed to init Spidermonkey");
    }

    // SpiderMonkey has been successfully stood up. We must now ensure it's torn down come what may. Constructing
    // a SpiderMonkeyTearDown will ensure this.
    SpiderMonkeyTearDown* smTearDown {new SpiderMonkeyTearDown};
    // Place in a unique_ptr with static storage duration
    tearDown.reset(smTearDown);
  }


  /*
   * Assign this thread a JSRuntime and JSContext. The caller must have checked that the thread doesn't already have
   * one assigned.
   *
   */

  extern "C"
  void assignRuntimeAndContext() {
    using SpiderMonkeyData = v8::SpiderMonkey::SpiderMonkeyData;

    V8MONKEY_ASSERT(!spiderMonkeyDestroyed, "Attempting to assign JSRuntime after SpiderMonkey destroyed");

    v8::SpiderMonkey::EnsureSpiderMonkey();

    JSRuntime* rt {JS_NewRuntime(JS::DefaultHeapMaxBytes)};

    if (!rt) {
      // The game is up, abort
      v8::V8Monkey::Abort("InternalIsolate::Enter", "SpiderMonkey's JS_NewRuntime failed", false);
      return;
    }

    // XXX Verify this assertion on engine upgrade
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
    smDataKey.Set(data);

    recordJSRuntimeConstruction();
  }


  /*
   * Destroy the JSRuntime and JSContext associated with a thread. This can be called when a thread exits, or when
   * static destructors are running (to destroy the JSRuntime / JSContext for the static destructor thread)
   *
   */

  extern "C"
  void tearDownRuntimeAndContext(void* raw) {
    using SpiderMonkeyData = v8::SpiderMonkey::SpiderMonkeyData;

    if (!raw) {
      return;
    }

    SpiderMonkeyData* data {reinterpret_cast<SpiderMonkeyData*>(raw)};
    JSRuntime* rt {data->rt};

    V8MONKEY_ASSERT(rt, "JSRuntime* was null");
    V8MONKEY_ASSERT(data->cx, "JSContext* was null");

/*
    // Although (as far as I can tell) the SpiderMonkey API doesn't require us to deregister any additional rooters at
    // JSRuntime destruction, we must still do so, to ensure we don't dereference dangling JSRuntime pointers when the
    // isolate is torn down
    RemoveRooter(rt);
*/

    JS_DestroyContext(data->cx);
    JS_DestroyRuntime(rt);

    // The SpiderMonkeyData object was heap allocated
    delete data;

    // There are two different circumstances in which we might be called: thread exit, or by our static destructor for
    // the engine. In the latter case, we want to zero out the value in TLS, as this function will be called again for
    // this thread after all static destructors have executed; we want a no-op in that case.
    smDataKey.Set(nullptr);

    recordJSRuntimeDestruction();
  }
}


namespace v8{
  namespace SpiderMonkey {
    void EnsureSpiderMonkey() {
      // We ensure we only attempt to init SpiderMonkey once by wrapping the real logic in a one-shot
      static OneShot spiderMonkeyInit {initSpiderMonkey};

      spiderMonkeyInit.Run();
    }


    void TearDownSpiderMonkey() {
      // No need to teardown SpiderMonkey if we never initted it
      if (!tearDown.get()) {
        return;
      }

      tearDown->AttemptDispose();
    }


    void EnsureRuntimeAndContext() {
      // It is a SpiderMonkey API requirement that the first thread's runtime and context are stood up in a thread-safe
      // fashion. To that end, we create a OneShot function to be run by the first thread to get here.
      static OneShot firstThreadInit {assignRuntimeAndContext};

      // Note we don't need to check if the thread has a runtime and context yet: if this is the first execution, then
      // by definition it cannot have one, and later threads won't make the call anyway.
      firstThreadInit.Run();

      // If the thread already has the requisite objects, (including if the first thread just acquired them above), we
      // can quit. We assume that if the thread has a JSRuntime, then it must also have a JSContext.
      if (GetJSRuntimeForThread()) {
        return;
      }

      assignRuntimeAndContext();
    }


    SpiderMonkeyData GetJSRuntimeAndJSContext() {
      SpiderMonkeyData* data {smDataKey.Get()};
      if (data) {
        return *data;
      }

      return {nullptr, nullptr};
    }


    JSRuntime* GetJSRuntimeForThread() {
      return GetJSRuntimeAndJSContext().rt;
    }


    JSContext* GetJSContextForThread() {
      return GetJSRuntimeAndJSContext().cx;
    }
  }
}


/*
 * Project reset: 16 July. Code below this line is from before the reset
 *
 */

/*
// partition
#include <algorithm>

// EXPORT_FOR_TESTING_ONLY
#include "test.h"

// V8_UNUSED
#include "v8config.h"

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
  // XXX Actually: do they even need to have entered the isolate now?
*/
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

/*
  struct RooterRegistration {
    v8::internal::Isolate* isolate;
    JSRuntime* runtime;
    v8::SpiderMonkey::RooterCallback callback;
    void* callbackData;

    RooterRegistration(v8::internal::Isolate* iso, JSRuntime* rt, v8::SpiderMonkey::RooterCallback cb, void* data) :
      isolate {iso}, runtime {rt}, callback {cb}, callbackData {data} {}

    RooterRegistration(const RooterRegistration& other) = default;
    RooterRegistration(RooterRegistration&& other) = default;
    RooterRegistration& operator=(const RooterRegistration& other) = default;
    RooterRegistration& operator=(RooterRegistration&& other) = default;
    ~RooterRegistration() = default;
  };
*/


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

/*
  std::vector<RooterRegistration> rooterRegistrations {};
*/


  /*
   * Bulk deregisters all isolates rooting the given JSRuntime. Called on thread destruction to ensure that isolates do
   * not attempt to remove themselves as rooters from JSRuntimes that no longer exist.
   *
   * This is a no-op if the given JSRuntime did not have any associated rooting isolates.
   *
   * rt is assumed to be a valid dereferencable pointer.
   *
   */

/*
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
    std::for_each(begin, end, [&rt](RooterRegistration& r) {
      JS_RemoveExtraGCRootsTracer(rt, r.callback, r.callbackData);

      // We are responsible for the lifetime of the data
      v8::SpiderMonkey::TracerData* td {reinterpret_cast<v8::SpiderMonkey::TracerData*>(r.callbackData)};
      delete td;
      r.callbackData = nullptr;
    });

    rooterRegistrations.erase(begin, end);
  }
*/


/*
  #ifdef V8MONKEY_INTERNAL_TEST
    v8::SpiderMonkey::GCRegistrationHook GCRegistrationHookFn {nullptr};
    v8::SpiderMonkey::GCDeregistrationHook GCDeregistrationHookFn {nullptr};
  #endif
}


namespace v8{
  namespace SpiderMonkey {


    void AddIsolateRooter(::v8::internal::Isolate* isolate, RooterCallback callback, TracerData* data) {
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
        // Isolate is already rooting this runtime: nothing to do, except free the TracerData (which we took ownership
        // of). Note though, we must guard against cases where the exact same TraceData was supplied - we cannot free
        // it in that case.
        if (pos->callbackData != data) {
          delete data;
        }
        return;
      }

      rooterRegistrations.emplace_back(isolate, rt, callback, data);

      #ifdef V8MONKEY_INTERNAL_TEST
        if (GCRegistrationHookFn) {
          GCRegistrationHookFn(rt, callback, data);
        } else {
          JS_AddExtraGCRootsTracer(rt, callback, data);
        }
      #else
        JS_AddExtraGCRootsTracer(rt, callback, data);
      #endif
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
      std::for_each(begin, end, [](RooterRegistration& r) {
        #ifdef V8MONKEY_INTERNAL_TEST
          if (GCDeregistrationHookFn) {
            GCDeregistrationHookFn(r.runtime, r.callback, r.callbackData);
          } else {
            JS_RemoveExtraGCRootsTracer(r.runtime, r.callback, r.callbackData);
          }
        #else
          JS_RemoveExtraGCRootsTracer(r.runtime, r.callback, r.callbackData);
        #endif
          // We are responsible for the lifetime of the data
          TracerData* td {reinterpret_cast<TracerData*>(r.callbackData)};
          delete td;
          r.callbackData = nullptr;
      });

      rooterRegistrations.erase(begin, end);
    }


    #ifdef V8MONKEY_INTERNAL_TEST
      void ForceGC() {
        JSRuntime* rt {GetJSRuntimeForThread()};
        V8MONKEY_ASSERT(rt, "Cannot force GC: No JSRuntime!");
        JS_GC(rt);
      }


      void SetGCRegistrationHooks(GCRegistrationHook on, GCDeregistrationHook off) {
        GCRegistrationHookFn = on;
        GCDeregistrationHookFn = off;
      }
    #endif


    JSCompartment* SpiderMonkeyUtils::GetCurrentCompartment() {
      // XXX TODO
      return nullptr;
    }


    JSCompartment* SpiderMonkeyUtils::GetPreviousCompartment() {
      // XXX TODO
      return nullptr;
    }
  }
}
*/
