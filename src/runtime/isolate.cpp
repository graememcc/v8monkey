// V8
#include "v8.h"

#include "jsapi.h"

#include "data_structures/destruct_list.h"
#include "data_structures/objectblock.h"
#include "threads/autolock.h"
#include "runtime/isolate.h"
#include "platform.h"
#include "v8monkey_common.h"


/*
 * In V8, an isolate is an object that owns a garbage-collected heap. Once threads explicitly "enter" such an isolate,
 * they can then construct values, and contexts in which scripts can be evaluated. Multiple threads can make use of
 * a single isolate, but not concurrently; only one thread can be active in the isolate at a time. V8 requires its
 * embedders to follow certain conventions to lock the isolate when a given thread is active in it.
 *
 * This contrasts with SpiderMonkey. The heap is owned by the JSRuntime, and once created, a JSRuntime is irrevocably
 * associated with the creating thread, and it is an API error to attempt to use the runtime on a different thread.
 * (Note: the MDN page on JSRuntimes claims that multiple threads can run code in the same JSRuntime. It lies: this
 * was removed in bug 650411. Don't believe the documentation for JS_NewRuntime either, which states that JSRuntimes
 * can be moved from one thread to another with an API call. This too was removed, in bug 901934).
 *
 * Clearly then, when fully emulating the V8 API, we cannot have a 1-1 correspondence from V8 Isolates to JSAPI
 * JSRuntimes. Now, although I've pulled multithreading support for the moment-I wasn't satisfied I would be able to
 * satisfactorily clean up threads leaving isolates, particularly once it became necessary to hook up isolates to
 * GC rooting-the current implementation is based on the outline of my plan for a multi-threaded world, in case it
 * proves possible to reinstate multithreading, hence the otherwise unneccessary use of Thread-Local Storage.
 *
 * Essentially, we look out for the first time a thread enters any isolate, and assign a JSRuntime and JSContext then.
 * A destructor is attached to the TLS key to teardown those objects on thread exit, and there is some separate
 * machinery to teardown the main thread's JSRuntime and JSContext: thread destruction in that case would imply that
 * OneTrueStaticInitializer's destructor has ran, and that destructor tears down SpiderMonkey, so we're too late.
 *
 * TODO: If the differences in the threading models prove insurmountable, then there are a lot of opportunities here
 *       and elsewhere for simplificication and possibly optimization.
 *
 * In terms of API implementation, we follow the lead of V8 here. The public-facing Isolate class is made of air; it
 * simply wraps pointers to the corresponding internal class.
 *
 */


#define FORWARD_TO_INTERNAL(method) \
  do { \
    V8Monkey::InternalIsolate* internal = reinterpret_cast<V8Monkey::InternalIsolate*>(this); \
    internal->method(); \
  } while (0);


namespace {
  using namespace v8::V8Platform;
  using namespace v8::V8Monkey;


  // Thread-local storage keys for isolate related thread data
  // The thread's unique ID
  TLSKey* threadIDKey = nullptr;
  // The internal isolate that the thread has entered
  TLSKey* currentIsolateKey = nullptr;
  // List of isolates that the thread has entered and not yet exited
  TLSKey* smDataKey = nullptr;


  /*
   * Returns the currently entered isolate for this thread
   *
   */

  InternalIsolate* GetCurrentIsolateFromTLS() {
    void* raw_id = Platform::GetTLSData(currentIsolateKey);
    return reinterpret_cast<InternalIsolate*>(raw_id);
  }


  /*
   * Store the currently entered isolate for this thread
   *
   */

  void SetCurrentIsolateInTLS(InternalIsolate* i) {
    Platform::StoreTLSData(currentIsolateKey, i);
  }


  /*
   * When a JSContext is destroyed, a garbage collection is performed. Therefore, when the main thread exits, prior to
   * JSContext destruction, we must ensure that the thread has disposed of each entered isolate, regardless of whether the
   * client did the right thing or not. Failure to do so could result in crashes or resource leaks. As Dispose is the
   * only isolate method that removes isolates from the teardown list, we can be sure that the isolates are still alive
   * and are safe to touch.
   *
   * TODO: This situation is pretty broken when it comes to trying to implement multi-threading. There will be a bit of
   *       work needed here
   *
   */

  struct SpiderMonkeyData {
    // A POD struct containing the JSRuntime and JSContext pointers
    RTCXData rtcx;

    // Pointer to a list of isolates that have not been disposed or destructed, and are therefore still known to
    // SpiderMonkey for rooting
    DestructingList<InternalIsolate>* isolateList;
  };


  /*
   * Function passed to DestructingList to destroy its contents. In effect, this will iterate over a list of
   * InternalIsolates, removing them as GC rooters.
   *
   */

  void unhookFromGC(InternalIsolate* i) {
    i->RemoveGCRooter();
  }


  /*
   * Assign this thread a JSRuntime and JSContext if necessary, and create the DestructingList which will tear down
   * any isolates that the client failed to dispose.
   *
   */

  void ensureRuntimeAndContext() {
    ASSERT(smDataKey, "InternalIsolate::Enter", "SpiderMonkey TLS key not initialised");

    // Don't reassign if already assigned
    // It is acceptable for this function to be called many times, indeed InternalIsolate::Enter does so.
    void* raw_id = Platform::GetTLSData(smDataKey);
    if (raw_id) {
      return;
    }

    JSRuntime* rt = JS_NewRuntime(JS::DefaultHeapMaxBytes);

    if (!rt) {
      // The game is up
      V8MonkeyCommon::TriggerFatalError("InternalIsolate::Enter", "SpiderMonkey's JS_NewRuntime failed");
      return;
    }

    // The stackChunkSize parameter isn't actually used by SpiderMonkey. However, it might affect implementation of
    // the V8 resource constraints class.
    JSContext* cx = JS_NewContext(rt, 8192);
    if (!cx) {
      // The game is up
      JS_DestroyRuntime(rt);
      V8MonkeyCommon::TriggerFatalError("InternalIsolate::Enter", "SpiderMonkey's JS_NewContext failed");
      return;
    }

    // Set options here. Note: the MDN page for JS_NewContext tells us to use JS_(G|S)etOptions. This changed in
    // bug 880330.
    JS::RuntimeOptionsRef(rt).setVarObjFix(true);

    SpiderMonkeyData* data = new SpiderMonkeyData{{rt, cx},  new DestructingList<InternalIsolate>(unhookFromGC)};

    Platform::StoreTLSData(smDataKey, data);
  }


  /*
   * Add an entered isolate to the list of isolates that require disposal on shutdown. The list of isolates is a
   * DestructingList pointed to by a field in the smData struct in TLS.
   *
   */

  void AddIsolateToGCTLSList(v8::V8Monkey::InternalIsolate* i) {
    void* raw = Platform::GetTLSData(smDataKey);
    ASSERT(raw, "AddIsolateToGCTLSList", "Somehow entered an isolate without creating SpiderMonkey data");

    SpiderMonkeyData* data = reinterpret_cast<SpiderMonkeyData*>(raw);
    data->isolateList->Add(i);
  }


  /*
   * Remove an isolate from the GC teardown list (by calling the teardown list's Delete function, which will in
   * turn invoke RemoveGCRooter).
   *
   */

  void DeleteIsolateFromGCTLSList(v8::V8Monkey::InternalIsolate* i) {
    void* raw = Platform::GetTLSData(smDataKey);
    ASSERT(raw, "DeleteIsolateFromGCTLSList", "Somehow entered isolate without creating SpiderMonkey data");

    SpiderMonkeyData* data = reinterpret_cast<SpiderMonkeyData*>(raw);
    data->isolateList->Delete(i);
  }


  /*
   * TLS Key destructor. Disposes of any isolates that are GC roots for the thread's JSRuntime/JSContext, and then
   * tears down those SpiderMonkey objects.
   *
   */

  void tearDownCXAndRT(void* raw) {
    if (!raw) {
      return;
    }

    SpiderMonkeyData* data = reinterpret_cast<SpiderMonkeyData*>(raw);

    // Must teardown isolates hooked into GC before destroying SpiderMonkey runtime objects
    delete data->isolateList;

    ASSERT(data->rtcx.rt, "tearDownCXAndRT", "JSRuntime* was null");
    ASSERT(data->rtcx.cx, "tearDownCXAndRT", "JSContext* was null");

    JS_DestroyContext(data->rtcx.cx);
    JS_DestroyRuntime(data->rtcx.rt);

    delete data;
  }


  /*
   * Returns a unique thread ID
   *
   */

  int CreateThreadID() {
    // Thread IDs must be greater than 0; otherwise we wouldn't be able to tell if the value is the thread ID, or
    // represents the case where the given thread has no data for the TLS key
    static int nextID = 1;

    // For my own reference, preserving the comment about locking here, even though I have pulled threading support
    // for the moment:
    // We just use a plain ole mutex here. V8 drops down to ASM, using a lock-prefixed xaddl instruction to avoid
    // overhead
    int result = nextID++;
    return result;
  }


  /*
   * Returns a new thread ID for the thread, after having first stored it in TLS
   *
   */

  int createAndAssignThreadID() {
    int thread_id = CreateThreadID();
    Platform::StoreTLSData(threadIDKey, reinterpret_cast<void*>(thread_id));

    return thread_id;
  }


  /*
   * Returns the thread ID stored in TLS, creating one if there is no such ID in TLS
   *
   */

  int fetchOrAssignThreadID() {
    void* raw_id = Platform::GetTLSData(threadIDKey);
    int existing_id =  *reinterpret_cast<int*>(&raw_id);

    if (existing_id > 0) {
      return existing_id;
    }

    return createAndAssignThreadID();
  }


  // Interface to isolate tracing for the SpiderMonkey garbage collector. Each isolate will call this function on
  // entry, in order to trace the values contained in HandleScopes and Persistents
  void gcTracer(JSTracer* tracer, void* data) {
    v8::V8Monkey::InternalIsolate* i = reinterpret_cast<v8::V8Monkey::InternalIsolate*>(data);
    i->Trace(tracer);
  }


  // Originally, the ObjectBlock could potentially contain objects created by different threads, i.e. in different
  // JSRuntimes. To that end, we supply both the JSRuntime and the JSTracer to the iterated objects. They will know
  // which runtime created them, and can ignore the tracing requests for foreign runtimes.
  struct TraceData {
    JSRuntime* rt;
    JSTracer* tracer;
  };


  // Interface to isolate tracing for the ObjectBlock iteration function
  void traceV8MonkeyObject(V8MonkeyObject* object, void* data) {
    if (!object) {
      return;
    }

    TraceData* td = reinterpret_cast<TraceData*>(data);
    object->Trace(td->rt, td->tracer);
  }


  // Hook for the ObjectBlock deletion code to process remaining persistent handles
  void persistentTearDown(V8MonkeyObject* obj) {
    if (!obj) {
      return;
    }

    // We must use PersistentRelease, as we don't know if it was strong or weak-refcounted
    // We use nullptr in lieu of a slot to indicate this was triggered by isolate deletion
    obj->PersistentRelease(nullptr);
  }
}


namespace v8 {
  int V8::GetCurrentThreadId() {
    if (V8::IsDead()) {
      V8MonkeyCommon::TriggerFatalError("V8::GetCurrentThreadId", "V8 is dead");
      // Note V8 actually soldiers on here and doesn't return early; we will too!
    }

    // The V8 API specifies that this call implicitly inits V8
    V8::Initialize();

    return fetchOrAssignThreadID();
  }


  void V8::SetFatalErrorHandler(FatalErrorCallback fn) {
    // Fatal error handlers are isolate-specific
    V8Monkey::InternalIsolate* i = V8Monkey::InternalIsolate::EnsureInIsolate();
    i->SetFatalErrorHandler(fn);
  }


  Isolate* Isolate::New() {
    V8Monkey::InternalIsolate* internal = new V8Monkey::InternalIsolate();
    return reinterpret_cast<Isolate*>(internal);
  }


  Isolate* Isolate::GetCurrent() {
    return reinterpret_cast<Isolate*>(V8Monkey::InternalIsolate::GetCurrent());
  }


  void Isolate::Dispose() {
    V8Monkey::InternalIsolate* internal = reinterpret_cast<InternalIsolate*>(this);

    // Note that we check this here: the default isolate can be disposed of while entered. In fact, it is
    // a V8 API requirement that it is entered when V8 is disposed (which in turn disposes the default
    // isolate)
    // XXX Clarify this comment. In fact clarify you can really dispose of default if in it
    // XXX Update: I'm pretty sure this is wrong
    if (internal->ContainsThreads()) {
      V8Monkey::V8MonkeyCommon::TriggerFatalError("v8::Isolate::Dispose",
                                                  "Attempt to dispose isolate in which threads are active");
      return;
    }

    internal->Dispose();
  }


  void Isolate::Enter() {
    FORWARD_TO_INTERNAL(Enter);
  }


  void Isolate::Exit() {
    FORWARD_TO_INTERNAL(Exit);
  }


  void* Isolate::GetData() {
    return reinterpret_cast<V8Monkey::InternalIsolate*>(this)->GetEmbedderData();
  }


  void Isolate::SetData(void* data) {
    reinterpret_cast<V8Monkey::InternalIsolate*>(this)->SetEmbedderData(data);
  }


  namespace V8Monkey {
    InternalIsolate::~InternalIsolate() {
      ASSERT(!ContainsThreads(), "InternalIsolate::~InternalIsolate", "Threads still active");
      ASSERT(isDisposed || IsDefaultIsolate(this), "InternalIsolate::~InternalIsolate", "Isolate not disposed");

      // Ensure default isolate deregisters for SpiderMonkey GC rooting
      if (!isDisposed) {
        Dispose();
      }
    }


      // We need to ensure this thread has an ID. GetCurrentThreadId creates one if necessary
      int threadID = fetchOrAssignThreadID();

      while (data && data->threadID != threadID) {
        data = data->next;
      }

      return data;
    }


    /*
     * Exiting the isolate is another simple book-keeping exercise, however note what doesn't happen: we don't
     * deregister from SpiderMonkey GC rooting. Objects created-particularly persistent objects-may have the same
     * lifetime as the isolate, so could exist until the client calls Isolate::Dispose. It would therefore be
     * incorrect to stop tracing at this point.
     * XXX What about HandleScopes/Locals. Clarify this comment.
     *
     */

    void InternalIsolate::Exit() {
      int threadID = fetchOrAssignThreadID();

      ThreadData* data = FindThreadData(threadID);
      if (data) {
        return data;
      }

      // Need to create new ThreadData. We will insert it at the head of the list
      ThreadData* td = new ThreadData(threadID, previousIsolate, nullptr, threadData);
      if (threadData) {
        threadData->prev = td;
      }
      threadData = td;
      return td;
    }


    // Delete (and free) the given ThreadData
    // XXX Check invariants here and document
    // XXX Key question: in the face of locking can threads exit iso in different order from entry
    void InternalIsolate::DeleteThreadData(ThreadData* td) {
      // Assumption: td is non-null
      if (td->prev) {
        td->prev->next = td->next;
      }

      if (td->next) {
        td->next->prev = td->prev;
      }

      if (threadData == td) {
        threadData = td->next;
      }

      delete td;
    }


    void InternalIsolate::AddGCRooter() {
      AutoGCMutex(this);

      if (isRegisteredForGC) {
        return;
      }

      #ifdef V8MONKEY_INTERNAL_TEST
        if (GCRegistrationHookFn) {
          GCRegistrationHookFn(GetJSRuntimeForThread(), gcTracer, this);
        } else {
          JS_AddExtraGCRootsTracer(GetJSRuntimeForThread(), gcTracer, this);
        }
      #else
        JS_AddExtraGCRootsTracer(GetJSRuntimeForThread(), gcTracer, this);
      #endif

      AddIsolateToGCTLSList(this);
      isRegisteredForGC = true;
    }


    void InternalIsolate::RemoveGCRooter() {
      AutoGCMutex(this);

      if (!isRegisteredForGC) {
        return;
      }

      // Deregister this isolate from SpiderMonkey
      #ifdef V8MONKEY_INTERNAL_TEST
        if (GCDeregistrationHookFn) {
          GCDeregistrationHookFn(GetJSRuntimeForThread(), gcTracer, this);
        } else {
          JS_RemoveExtraGCRootsTracer(GetJSRuntimeForThread(), gcTracer, this);
        }
      #else
        JS_RemoveExtraGCRootsTracer(GetJSRuntimeForThread(), gcTracer, this);
      #endif

      DeleteIsolateFromGCTLSList(this);
      isRegisteredForGC = false;
    }


    /*
     * Searches the linked list of thread data, and finds the ThreadData object for the given thread ID.
     *
     * Retuns nullptr if there is no entry for that threadID
     *
     */

    InternalIsolate::ThreadData* InternalIsolate::FindThreadData(int threadID) {
      ThreadData* data = threadData;

      while (data && data->threadID != threadID) {
        data = data->next;
      }

      return data;
    }



    /*
     * Searches the linked list of thread data, and finds the ThreadData object for the given thread ID.
     * If no entry exists for the thread, then one will be created, with the previous isolate field set to the given
     * isolate.
     *
     */

    InternalIsolate::ThreadData* InternalIsolate::FindOrCreateThreadData(int threadID, InternalIsolate* previousIsolate) {
      ThreadData* data = FindThreadData(threadID);
      if (data) {
        return data;
      }

      // Need to create new ThreadData. We will insert it at the head of the list
      ThreadData* td = new ThreadData(threadID, previousIsolate, nullptr, threadData);
      if (threadData) {
        threadData->prev = td;
      }
      threadData = td;

      return td;
    }


    /*
     * Delete the given entry from the ThreadData linked list and delete it.
     *
     */

    void InternalIsolate::DeleteAndFreeThreadData(ThreadData* td) {
      ASSERT(td, "InternalIsolate::DeleteAndFreeThreadData", "Attempting to delete null entry");
      if (td->prev) {
        td->prev->next = td->next;
      }

      if (td->next) {
        td->next->prev = td->prev;
      }

      if (threadData == td) {
        threadData = td->next;
      }

      delete td;
    }


    /*
     * API for Lockers to "lock" the isolate for a given thread. As we are currently single-threaded, this is
     * essentially meaningless. Hopefully, this won't always be the case.
     *
     */

    void InternalIsolate::Lock() {
      lockingMutex.Lock();
      lockingThread = fetchOrAssignThreadID();
    }


    void InternalIsolate::Unlock() {
      lockingThread = 0;
      lockingMutex.Unlock();
    }


    bool InternalIsolate::IsLockedForThisThread() const {
      return lockingThread == fetchOrAssignThreadID();
    }


    InternalIsolate* InternalIsolate::GetCurrent() {
      return GetIsolateFromTLS();
    }


    bool InternalIsolate::IsDefaultIsolate(InternalIsolate* i) {
      return i == defaultIsolate;
    }


    InternalIsolate* InternalIsolate::GetDefaultIsolate() {
      return defaultIsolate;
    }


    bool InternalIsolate::IsEntered(InternalIsolate* i) {
      // The naive approach here would be to simply grab the isolate pointer from TLS and compare. However, this would
      // be incorrect for the main thread, as, for V8 compatability, it has a pointer to the default isolate regardless
      // of entry status.
      int threadID = fetchOrAssignThreadID();
      return i->FindThreadData(threadID) != nullptr;
    }


    // Must only be called from static initializer
    void V8MonkeyCommon::InitTLSKeys() {
      threadIDKey = Platform::CreateTLSKey();
      isolateKey = Platform::CreateTLSKey();
      rtcxKey = Platform::CreateTLSKey(tearDownCXAndRT);
    }


    // Must only be called from a static initializer, and after InitTLSKeys
    void V8MonkeyCommon::EnsureDefaultIsolate() {
      // The internal isolate will be disposed elsewhere
      // XXX Where?
      InternalIsolate::defaultIsolate = new InternalIsolate;

      // Associate the default isolate with this thread permanently
      SetIsolateInTLS(InternalIsolate::defaultIsolate);

      // We should be thread number 1!
      int threadID = fetchOrAssignThreadID();

      ASSERT(threadID == 1, "InternalIsolate::EnsureDefaultIsolateForStaticInitializerThread", "Wrong thread ID!");
      initialized = true;
    }


    InternalIsolate* InternalIsolate::EnsureInIsolate() {
      InternalIsolate* current = GetCurrent();
      int threadID = fetchOrAssignThreadID();
      if (current && current->FindThreadData(threadID) != nullptr) {
        return current;
      }

      // V8 permanently associates threads that implicitly enter the default isolate with the default isolate
      SetIsolateInTLS(defaultIsolate);
      defaultIsolate->Enter();

      return defaultIsolate;
    }


    HandleData InternalIsolate::GetLocalHandleData() {
      return handleScopeData;
    }


    void InternalIsolate::SetLocalHandleData(HandleData& hd) {
      handleScopeData = hd;
    }


    HandleData InternalIsolate::GetPersistentHandleData() {
      return persistentData;
    }


    void InternalIsolate::SetPersistentHandleData(HandleData& hd) {
      persistentData = hd;
    }


    // Return the JSRuntime associated with this thread. May be null
    JSRuntime* InternalIsolate::GetJSRuntimeForThread() {
      void* raw = Platform::GetTLSData(rtcxKey);
      if (raw == nullptr) {
        return nullptr;
      }
      RTCXData* data = reinterpret_cast<RTCXData*>(raw);
      return data->rt;
    }


    // Return the JSContext associated with this thread. May be null
    JSContext* InternalIsolate::GetJSContextForThread() {
      void* raw = Platform::GetTLSData(rtcxKey);
      if (raw == nullptr) {
        return nullptr;
      }
      RTCXData* data = reinterpret_cast<RTCXData*>(raw);
      return data->cx;
    }


    /*
     * The thread's assigned JSRuntime and JSContext are normally torn down when the thread exits. However, in the
     * case of the last surviving thread, this will be too late, as OneTrueStaticInitializer's destructor may have
     * ran by that point, killing SpiderMonkey. This function is intended to be called by OneTrueStaticInitializer's
     * destructor to force the teardown of the SpiderMonkey objects (preceded by the disposal of any isolates that
     * are still participating in rooting).
     *
     */

    void V8MonkeyCommon::ForceMainThreadRTCXDisposal() {
      void* raw = Platform::GetTLSData(smDataKey);
      if (!raw) {
        return;
      }

      tearDownCXAndRT(raw);
      // Ensure the TLSKey destructor is not invoked when we do exit
      Platform::StoreTLSData(smDataKey, nullptr);
    }


    void InternalIsolate::Trace(JSTracer* tracer) {
      // Don't allow API mutation of the handle structures when we're tracing them
      AutoGCMutex(this);

      TraceData* td = new TraceData;
      td->rt = GetJSRuntimeForThread();
      td->tracer = tracer;

      // Trace Local handles from HandleScopes
      ObjectBlock<V8MonkeyObject>::Iterate(handleScopeData.limit, handleScopeData.next, traceV8MonkeyObject, td);

      // Trace Persistent handles
      ObjectBlock<V8MonkeyObject>::Iterate(persistentData.limit, persistentData.next, traceV8MonkeyObject, td);

      delete td;
    }


    InternalIsolate* InternalIsolate::defaultIsolate = nullptr;


    #ifdef V8MONKEY_INTERNAL_TEST

    void (*InternalIsolate::GCRegistrationHookFn)(JSRuntime*, JSTraceDataOp, void*) = nullptr;
    void (*InternalIsolate::GCDeregistrationHookFn)(JSRuntime*, JSTraceDataOp, void*) = nullptr;


    void InternalIsolate::ForceGC() {
      JS_GC(GetJSRuntimeForThread());
    }


    // XXX Can we fix these up to watch out for isolate construction?
    TestUtils::AutoIsolateCleanup::~AutoIsolateCleanup() {
      while (Isolate::GetCurrent() && InternalIsolate::IsEntered(InternalIsolate::GetCurrent())) {
        Isolate* i = Isolate::GetCurrent();
        InternalIsolate* ii = InternalIsolate::FromIsolate(i);

        // Isolates can be entered multiple times
        while (InternalIsolate::IsEntered(ii)) {
          i->Exit();
        }

        i->Dispose();
      }
    }


    TestUtils::AutoTestCleanup::AutoTestCleanup() {
      // Use fetchOrThreadID to avoid implicitly initting V8
      if (fetchOrAssignThreadID() != 1) {
        V8Platform::Platform::ExitWithError("Why are you using AutoTestCleanup on a thread, idiot?");
      }
    }


    TestUtils::AutoTestCleanup::~AutoTestCleanup() {
      while (Isolate::GetCurrent() && InternalIsolate::IsEntered(InternalIsolate::GetCurrent())) {
        Isolate* i = Isolate::GetCurrent();
        InternalIsolate* ii = InternalIsolate::FromIsolate(i);

        // Isolates can be entered multiple times
        while (InternalIsolate::IsEntered(ii)) {
          i->Exit();
        }

        i->Dispose();
      }
      V8::Dispose();
    }
    #endif
  }
}
#undef FORWARD_TO_INTERNAL
