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
 * GC rooting-but the current implementation is based on the outline of my plan for a multi-threaded world, hence the
 * otherwise unneccessary use of Thread-Local Storage.
 *
 * Essentially, the plan was to look out for the first time a thread enters any isolate, and assign a JSRuntime and
 * JSContext then, with a destructor attached to the TLS key to teardown those objects on thread exit, with some separate
 * machinery to teardown the main thread's objects (as thread destruction in that case would imply the static object whose
 * destructor shuts down SpiderMonkey has already ran).
 *
 * TODO: If the differences in the threading models prove insurmountable, then there are a lot of opportunities here
 *       and elsewhere for simplficication and possibly optimization.
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
  TLSKey* threadIDKey = NULL;
  TLSKey* isolateKey = NULL;
  TLSKey* rtcxKey = NULL;


  v8::V8Monkey::InternalIsolate* GetIsolateFromTLS() {
    void* raw_id = v8::V8Platform::Platform::GetTLSData(isolateKey);
    return reinterpret_cast<v8::V8Monkey::InternalIsolate*>(raw_id);
  }


  void SetIsolateInTLS(v8::V8Monkey::InternalIsolate* i) {
    v8::V8Platform::Platform::StoreTLSData(isolateKey, i);
  }


  // Per-thread SpiderMonkey data
  struct RTCXData {
    // Pointer to a list of isolates that have not been exited or destructed, and are therefore still attached to
    // SpiderMonkey for GC
    v8::V8Monkey::DestructingList<v8::V8Monkey::InternalIsolate>* isolateList;

    JSRuntime* rt;
    JSContext* cx;
  };


  // Tear down a context and runtime for a thread.
  void tearDownCXAndRT(void* raw) {
    if (!raw) {
      return;
    }

    RTCXData* data = reinterpret_cast<RTCXData*>(raw);

    // Must teardown GC hook before destroying runtime objects
    delete data->isolateList;

    JS_DestroyContext(data->cx);
    JS_DestroyRuntime(data->rt);

    delete data;
  }


  // Remove an isolate from the GC teardown list (by calling the teardown list's Delete function, which will in
  // turn invoke RemoveGCRooter
  void AddIsolateToGCTLSList(v8::V8Monkey::InternalIsolate* i) {
    void* raw = Platform::GetTLSData(rtcxKey);
    ASSERT(raw != nullptr, "AddIsolateToGCTLSList", "Somehow entered isolate without JSRuntime/JSContext data");

    RTCXData* data = reinterpret_cast<RTCXData*>(raw);
    data->isolateList->Add(i);
  }


  // Remove an isolate from the GC teardown list (by calling the teardown list's Delete function, which will in
  // turn invoke RemoveGCRooter
  void DeleteIsolateFromGCTLSList(v8::V8Monkey::InternalIsolate* i) {
    void* raw = Platform::GetTLSData(rtcxKey);
    ASSERT(raw != nullptr, "DeleteIsolateFromGCTLSList", "Somehow entered isolate without JSRuntime/JSContext data");

    RTCXData* data = reinterpret_cast<RTCXData*>(raw);
    data->isolateList->Delete(i);
  }


  // Function passed to DestructingList to destroy its contents
  void unhookFromGC(InternalIsolate* i) {
    i->RemoveGCRooter();
  }


  // Assigns a JSRuntime and JSContext to this thread if necessary. If a JSRuntime* is present in TLS, then we assume
  // that the context is present also
  void EnsureRuntimeAndContext() {
    void* raw_id = Platform::GetTLSData(rtcxKey);
    if (raw_id) {
      return;
    }

    RTCXData* data = new RTCXData;

    JSRuntime* rt = JS_NewRuntime(JS::DefaultHeapMaxBytes);

    if (!rt) {
      // The game is up
      delete data;
      V8MonkeyCommon::TriggerFatalError("EnsureRuntimeAndContext", "SpiderMonkey's JS_NewRuntime failed");
      return;
    }

    data->rt = rt;

    // The stackChunkSize parameter isn't actually used by SpiderMonkey. This might affect implementation of
    // the V8 resource constraints class.
    JSContext* cx = JS_NewContext(rt, 8192);
    if (!cx) {
      // The game is up
      JS_DestroyRuntime(rt);
      delete data;
      V8MonkeyCommon::TriggerFatalError("EnsureRuntimeAndContext", "SpiderMonkey's JS_NewContext failed");
      return;
    }

    data->cx = cx;
    // Set options here. Note: the MDN page for JS_NewContext uses JS_(G|S)etOptions. This changed in bug 880330.
    JS::RuntimeOptionsRef(rt).setVarObjFix(true);

    data->isolateList = new DestructingList<InternalIsolate>(unhookFromGC);

    Platform::StoreTLSData(rtcxKey, data);
  }


  // Returns the thread's ID from TLS, or 0 if not present
  int GetThreadIDFromTLS() {
    void* raw_id = Platform::GetTLSData(threadIDKey);
    return *reinterpret_cast<int*>(&raw_id);
  }


  // Returns a unique thread ID
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


  // Returns a new thread id for the thread, after having first stored it in TLS
  int CreateAndAssignThreadID() {
    int thread_id = CreateThreadID();
    Platform::StoreTLSData(threadIDKey, reinterpret_cast<void*>(thread_id));

    return thread_id;
  }


  int FetchOrAssignThreadId() {
    int existing_id = GetThreadIDFromTLS();
    if (existing_id > 0) {
      return existing_id;
    }

    return CreateAndAssignThreadID();
  }


  // Interface to isolate tracing for the SpiderMonkey garbage collector. Each isolate that has tracable roots stored
  // in handlescopes will call JS_AddExtraGCRoots with this function, to participate in rooting.
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
    TraceData* td = reinterpret_cast<TraceData*>(data);
    object->Trace(td->rt, td->tracer);
  }
}


namespace v8 {
  int V8::GetCurrentThreadId() {
    if (V8::IsDead()) {
      V8MonkeyCommon::TriggerFatalError("V8::GetCurrentThreadId", "V8 is dead");
      // Note V8 actually soldiers on here and doesn't return early, so we will too!
    }

    // The V8 API specifies that this call implicitly inits V8
    V8::Initialize();

    return FetchOrAssignThreadId();
  }


  void V8::SetFatalErrorHandler(FatalErrorCallback fn) {
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


    // Searches the linked list, and finds the ThreadData object for the given thread ID, or returns NULL
    InternalIsolate::ThreadData* InternalIsolate::FindThreadData(int threadID) {
      ThreadData* data = threadData;

      while (data && data->threadID != threadID) {
        data = data->next;
      }

      return data;
    }


    // Searches the linked list, and finds the ThreadData object for the given thread ID, creating one if it didn't
    // already exist
    InternalIsolate::ThreadData* InternalIsolate::FindOrCreateThreadData(int threadID, InternalIsolate* previousIsolate) {
      ThreadData* data = FindThreadData(threadID);
      if (data) {
        return data;
      }

      // Need to create new ThreadData. We will insert it at the head of the list
      ThreadData* td = new ThreadData(threadID, previousIsolate, NULL, threadData);
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
      if (isRegisteredForGC) {
        return;
      }

JS_GC(GetJSRuntimeForThread());
      #ifdef V8MONKEY_INTERNAL_TEST
        if (gcOnNotifierFn) {
          gcOnNotifierFn(GetJSRuntimeForThread(), gcTracer, this);
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
      if (!isRegisteredForGC) {
        return;
      }

      // Deregister this isolate from SpiderMonkey
      #ifdef V8MONKEY_INTERNAL_TEST
        if (gcOffNotifierFn) {
          gcOffNotifierFn(GetJSRuntimeForThread(), gcTracer, this);
        } else {
          JS_RemoveExtraGCRootsTracer(GetJSRuntimeForThread(), gcTracer, this);
        }
      #else
        JS_RemoveExtraGCRootsTracer(GetJSRuntimeForThread(), gcTracer, this);
      #endif

      DeleteIsolateFromGCTLSList(this);
      isRegisteredForGC = false;
    }


    void InternalIsolate::Enter() {
      isDisposed = false;

      // We need to ensure this thread has an ID. GetCurrentThreadId creates one if necessary
      int threadID = FetchOrAssignThreadId();

      // Likewise, it should have a JSRuntime and JSContext
      EnsureRuntimeAndContext();

      // Register this isolate with SpiderMonkey for GC
      AddGCRooter();

      // What isolate was the thread in previously?
      InternalIsolate* previousIsolate = GetIsolateFromTLS();

      ThreadData* data = FindOrCreateThreadData(threadID, previousIsolate);

      // Note a new entry for this thread
      data->entryCount++;

      if (data->entryCount > 1) {
        // The thread was already in this isolate
        return;
      }

      SetIsolateInTLS(this);
    }


    void InternalIsolate::Exit() {
      int threadID = FetchOrAssignThreadId();
      ThreadData* data = FindThreadData(threadID);

      // Note that we have exited the isolate
      data->entryCount--;

      // Nothing more to do if the thread has other outstanding exits
      if (data->entryCount > 0) {
        return;
      }

      // Time for this thread to say goodbye. We need to remove the ThreadData
      // from the linked list, and pop that thread's isolate stack
      SetIsolateInTLS(data->previousIsolate);

      // data will be dangling after this call, so make this the last thing we do
      DeleteThreadData(data);
    }


    void InternalIsolate::Dispose() {
      // Don't dispose of the default isolate (or indeed any isolate) multiple times
      if (isDisposed) {
        return;
      }

      if (isRegisteredForGC) {
        DeleteIsolateFromGCTLSList(this);
      }

      // Ensure we don't attempt to delete the default isolate multiple times
      isDisposed = true;

      if (this != defaultIsolate) {
        delete this;
      } else {
        // Clear out TLS so the destructor doesn't run again (unless on main thread)
        int threadID = FetchOrAssignThreadId();
        if (threadID != 1) {
          SetIsolateInTLS(nullptr);
        }
      }
    }


    void InternalIsolate::Lock() {
      lockingMutex.Lock();
      lockingThread = FetchOrAssignThreadId();
    }


    void InternalIsolate::Unlock() {
      lockingThread = 0;
      lockingMutex.Unlock();
    }


    bool InternalIsolate::IsLockedForThisThread() {
      return lockingThread == FetchOrAssignThreadId();
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
      // Note: we can't simply grab the isolate* from TLS, as main will have a non-null value, even when it hasn't
      // entered the isolate
      int threadID = FetchOrAssignThreadId();
      return i->FindThreadData(threadID) != NULL;
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

      // We should be thread 1!
      FetchOrAssignThreadId();
    }


    InternalIsolate* InternalIsolate::EnsureInIsolate() {
      InternalIsolate* current = GetCurrent();
      int threadID = FetchOrAssignThreadId();
      if (current && current->FindThreadData(threadID) != NULL) {
        return current;
      }

      // V8 permanently associates threads that implicitly enter the default isolate with the default isolate
      SetIsolateInTLS(defaultIsolate);
      defaultIsolate->Enter();

      return defaultIsolate;
    }


    HandleScopeData InternalIsolate::GetHandleScopeData() {
      return handleScopeData;
    }


    void InternalIsolate::SetHandleScopeData(HandleScopeData& hsd) {
      handleScopeData = hsd;
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


    // Force JSRuntime and JSContext disposal for the thread that runs static initializers
    void V8MonkeyCommon::ForceRTCXDisposal() {
      void* raw = Platform::GetTLSData(rtcxKey);
      if (raw == nullptr) {
        return;
      }

      tearDownCXAndRT(raw);
      Platform::StoreTLSData(rtcxKey, 0);
    }


    void InternalIsolate::Trace(JSTracer* tracer) {
      printf("ISOLATE TRACE\n");
      TraceData* td = new TraceData;
      td->rt = GetJSRuntimeForThread();
      td->tracer = tracer;

      // Don't allow mutation of handle scope data whilst iterating over it
      handleScopeDataMutex.Lock();
      ObjectBlock<V8MonkeyObject>::Iterate(handleScopeData.limit, handleScopeData.next, traceV8MonkeyObject, td);
      handleScopeDataMutex.Unlock();
      delete td;
    }


    InternalIsolate* InternalIsolate::defaultIsolate = NULL;


    #ifdef V8MONKEY_INTERNAL_TEST

    void (*InternalIsolate::gcOnNotifierFn)(JSRuntime*, JSTraceDataOp, void*) = nullptr;
    void (*InternalIsolate::gcOffNotifierFn)(JSRuntime*, JSTraceDataOp, void*) = nullptr;


    // XXX Can we fix these up to look out for construction?
    TestUtils::AutoIsolateCleanup::~AutoIsolateCleanup() {
      while (Isolate::GetCurrent() && InternalIsolate::IsEntered(InternalIsolate::GetCurrent())) {
        Isolate* i = Isolate::GetCurrent();
        i->Exit();
        i->Dispose();

      }
    }


    TestUtils::AutoTestCleanup::AutoTestCleanup() {
      if (GetThreadIDFromTLS() != 1) {
        V8Platform::Platform::ExitWithError("Why are you using AutoTestCleanup on a thread, idiot?");
      }
    }


    TestUtils::AutoTestCleanup::~AutoTestCleanup() {
      while (Isolate::GetCurrent() && InternalIsolate::IsEntered(InternalIsolate::GetCurrent())) {
        Isolate* i = Isolate::GetCurrent();
        i->Exit();
        i->Dispose();
      }
      V8::Dispose();
    }
    #endif
  }
}
