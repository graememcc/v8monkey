// DestructList
// XXX That needs a better name
#include "data_structures/destruct_list.h"

// Objectblock
#include "data_structures/objectblock.h"

// JSContext JS_GC JS_NewContext JS_NewRuntime JSRuntime JS::RuntimeOptionsRef JSTracer JSTraceDataOp
#include "jsapi.h"

// Class definition
#include "runtime/isolate.h"

// AssignJSRuntimeAndJSContext, GetJS[Runtime|Context]ForThread, InitialiseSpiderMonkeyDataTLSKeys, RTCXData
#include "utils/SpiderMonkeyUtils.h"

// TLSKey, CreateTLSKey
#include "platform.h"

// TestUtils EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"

// V8MonkeyCommon class definition, TriggerFatalError, ASSERT
#include "utils/V8MonkeyCommon.h"


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
 */


namespace {
  using namespace v8::V8Platform;
//   using namespace v8::V8Monkey;
//
//
//   // Thread-local storage keys for isolate related thread data
//   // The thread's unique ID
//   TLSKey* threadIDKey = nullptr;
//   // The internal isolate that the thread has entered
  TLSKey* currentIsolateKey = nullptr;
//   // List of isolates that the thread has entered and not yet exited
//   TLSKey* smDataKey = nullptr;
//
//


  void ensureTLSKeys() {
    static bool initialized = []() noexcept {
      //threadIDKey = Platform::CreateTLSKey();
      currentIsolateKey = Platform::CreateTLSKey();
      //smDataKey = Platform::CreateTLSKey(tearDownCXAndRT);

      return true;
    }();
  }


  /*
   * Returns the currently entered isolate for this thread
   *
   */

  v8::internal::Isolate* GetCurrentIsolateFromTLS() {
    ensureTLSKeys();
    void* raw_id = Platform::GetTLSData(currentIsolateKey);
    return reinterpret_cast<v8::internal::Isolate*>(raw_id);
  }
//
//
//   /*
//    * Store the currently entered isolate for this thread
//    *
//    */
//
//   void SetCurrentIsolateInTLS(InternalIsolate* i) {
//     Platform::StoreTLSData(currentIsolateKey, i);
//   }
//
//
//   /*
//    * When a JSContext is destroyed, a garbage collection is performed. Therefore, when the main thread exits, prior to
//    * JSContext destruction, we must ensure that the thread has disposed of each entered isolate, regardless of whether the
//    * client did the right thing or not. Failure to do so could result in crashes or resource leaks. As Dispose is the
//    * only isolate method that removes isolates from the teardown list, we can be sure that the isolates are still alive
//    * and are safe to touch.
//    *
//    * TODO: This situation is pretty broken when it comes to trying to implement multi-threading. There will be a bit of
//    *       work needed here
//    *
//    */
//
//   struct SpiderMonkeyData {
//     // A POD struct containing the JSRuntime and JSContext pointers
//     RTCXData rtcx;
//
//     // Pointer to a list of isolates that have not been disposed or destructed, and are therefore still known to
//     // SpiderMonkey for rooting
//     DestructingList<InternalIsolate>* isolateList;
//   };
//
//
//   /*
//    * Function passed to DestructingList to destroy its contents. In effect, this will iterate over a list of
//    * InternalIsolates, removing them as GC rooters.
//    *
//    */
//
//   void unhookFromGC(InternalIsolate* i) {
//     i->RemoveGCRooter();
//   }
//
//
//   /*
//    * Assign this thread a JSRuntime and JSContext if necessary, and create the DestructingList which will tear down
//    * any isolates that the client failed to dispose.
//    *
//    */
//
//   void ensureRuntimeAndContext() {
//     ASSERT(smDataKey, "InternalIsolate::Enter", "SpiderMonkey TLS key not initialised");
//
//     // Don't reassign if already assigned
//     // It is acceptable for this function to be called many times, indeed InternalIsolate::Enter does so.
//     void* raw_id = Platform::GetTLSData(smDataKey);
//     if (raw_id) {
//       return;
//     }
//
//     JSRuntime* rt = JS_NewRuntime(JS::DefaultHeapMaxBytes);
//
//     if (!rt) {
//       // The game is up
//       V8MonkeyCommon::TriggerFatalError("InternalIsolate::Enter", "SpiderMonkey's JS_NewRuntime failed");
//       return;
//     }
//
//     // The stackChunkSize parameter isn't actually used by SpiderMonkey. However, it might affect implementation of
//     // the V8 resource constraints class.
//     JSContext* cx = JS_NewContext(rt, 8192);
//     if (!cx) {
//       // The game is up
//       JS_DestroyRuntime(rt);
//       V8MonkeyCommon::TriggerFatalError("InternalIsolate::Enter", "SpiderMonkey's JS_NewContext failed");
//       return;
//     }
//
//     // Set options here. Note: the MDN page for JS_NewContext tells us to use JS_(G|S)etOptions. This changed in
//     // bug 880330.
//     JS::RuntimeOptionsRef(rt).setVarObjFix(true);
//
//     SpiderMonkeyData* data = new SpiderMonkeyData{{rt, cx},  new DestructingList<InternalIsolate>(unhookFromGC)};
//
//     Platform::StoreTLSData(smDataKey, data);
//   }
//
//
//   /*
//    * Add an entered isolate to the list of isolates that require disposal on shutdown. The list of isolates is a
//    * DestructingList pointed to by a field in the smData struct in TLS.
//    *
//    */
//
//   void AddIsolateToGCTLSList(v8::V8Monkey::InternalIsolate* i) {
//     void* raw = Platform::GetTLSData(smDataKey);
//     ASSERT(raw, "AddIsolateToGCTLSList", "Somehow entered an isolate without creating SpiderMonkey data");
//
//     SpiderMonkeyData* data = reinterpret_cast<SpiderMonkeyData*>(raw);
//     data->isolateList->Add(i);
//   }
//
//
//   /*
//    * Remove an isolate from the GC teardown list (by calling the teardown list's Delete function, which will in
//    * turn invoke RemoveGCRooter).
//    *
//    */
//
//   void DeleteIsolateFromGCTLSList(v8::V8Monkey::InternalIsolate* i) {
//     void* raw = Platform::GetTLSData(smDataKey);
//     ASSERT(raw, "DeleteIsolateFromGCTLSList", "Somehow entered isolate without creating SpiderMonkey data");
//
//     SpiderMonkeyData* data = reinterpret_cast<SpiderMonkeyData*>(raw);
//     data->isolateList->Delete(i);
//   }
//
//
//   /*
//    * TLS Key destructor. Disposes of any isolates that are GC roots for the thread's JSRuntime/JSContext, and then
//    * tears down those SpiderMonkey objects.
//    *
//    */
//
//   void tearDownCXAndRT(void* raw) {
//     if (!raw) {
//       return;
//     }
//
//     SpiderMonkeyData* data = reinterpret_cast<SpiderMonkeyData*>(raw);
//
//     // Must teardown isolates hooked into GC before destroying SpiderMonkey runtime objects
//     delete data->isolateList;
//
//     ASSERT(data->rtcx.rt, "tearDownCXAndRT", "JSRuntime* was null");
//     ASSERT(data->rtcx.cx, "tearDownCXAndRT", "JSContext* was null");
//
//     JS_DestroyContext(data->rtcx.cx);
//     JS_DestroyRuntime(data->rtcx.rt);
//
//     delete data;
//   }
//
//
//   /*
//    * Returns a unique thread ID
//    *
//    */
//
//   int CreateThreadID() {
//     // Thread IDs must be greater than 0; otherwise we wouldn't be able to tell if the value is the thread ID, or
//     // represents the case where the given thread has no data for the TLS key
//     static int nextID = 1;
//
//     // For my own reference, preserving the comment about locking here, even though I have pulled threading support
//     // for the moment:
//     // We just use a plain ole mutex here. V8 drops down to ASM, using a lock-prefixed xaddl instruction to avoid
//     // overhead
//     int result = nextID++;
//     return result;
//   }
//
//
//   /*
//    * Returns a new thread ID for the thread, after having first stored it in TLS
//    *
//    */
//
//   int createAndAssignThreadID() {
//     int thread_id = CreateThreadID();
//     Platform::StoreTLSData(threadIDKey, reinterpret_cast<void*>(thread_id));
//
//     return thread_id;
//   }
//
//
//   /*
//    * Returns the thread ID stored in TLS, creating one if there is no such ID in TLS
//    *
//    */
//
//   int fetchOrAssignThreadID() {
//     void* raw_id = Platform::GetTLSData(threadIDKey);
//     int existing_id =  *reinterpret_cast<int*>(&raw_id);
//
//     if (existing_id > 0) {
//       return existing_id;
//     }
//
//     return createAndAssignThreadID();
//   }
//
//
//   /*
//    * Interface to isolate tracing for the SpiderMonkey garbage collector. When a thread enters an isolate, the isolate
//    * will register itself as a GC rooter with that thread's JSRuntime. This is the function that SpiderMonkey will call
//    * when tracing roots.
//    *
//    */
//
//   void GCTracingFunction(JSTracer* tracer, void* data) {
//     // All we need to do is cast to the isolate, and invoke Trace
//     v8::V8Monkey::InternalIsolate* i = reinterpret_cast<v8::V8Monkey::InternalIsolate*>(data);
//     i->Trace(tracer);
//   }
//
//
//   /*
//    * This design harks back to the original plan to support V8's multi-threading model. In such a world, the
//    * ObjectBlocks could potentially contain objects that were created by different threads, and so potentially wrap
//    * SpiderMonkey objects from different JSRuntimes. As a consequence, each isolate would be registered as a GC
//    * rooter for each of those runtimes. Of course, we would want to ensure that only the objects relevant to a
//    * particular runtime are traced. To that end, we supply both the JSRuntime and the JSTracer to the iterated objects.
//    * They will know which runtime created them, and can ignore the tracing requests for foreign runtimes.
//    *
//    * Of course, things are currently much simpler; there can only be one thread, one runtime. However, we will leave
//    * the design untouched for the moment until we can confirm that multi-threading won't be possible.
//    *
//    */
//
//   struct TraceData {
//     JSRuntime* rt;
//     JSTracer* tracer;
//   };
//
//
//   /*
//    * The objects to be traced are stored in ObjectBlocks. This iteration function will be supplied to them to trace all
//    * the objects contained within them in sequence.
//    *
//    */
//
//   void tracingIterationFunction(V8MonkeyObject* object, void* data) {
//     if (!object) {
//       return;
//     }
//
//     TraceData* td = reinterpret_cast<TraceData*>(data);
//     object->Trace(td->rt, td->tracer);
//   }
//
//
//   // Hook for the ObjectBlock deletion code to process remaining persistent handles
//   void persistentTearDown(V8MonkeyObject* obj) {
//     if (!obj) {
//       return;
//     }
//
//     // We must use PersistentRelease, as we don't know if it was strong or weak-refcounted
//     // We use nullptr in lieu of a slot to indicate this was triggered by isolate deletion
//     obj->PersistentRelease(nullptr);
//   }
}


 namespace v8 {
//   int V8::GetCurrentThreadId() {
//     if (V8::IsDead()) {
//       V8MonkeyCommon::TriggerFatalError("V8::GetCurrentThreadId", "V8 is dead");
//       // Note V8 actually soldiers on here and doesn't return early; we will too!
//     }
//
//     // The V8 API specifies that this call implicitly inits V8
//     V8::Initialize();
//
//     return fetchOrAssignThreadID();
//   }


  /*
   * We implement this here as error handlers are associated with internal isolates.
   * NOTE: In 3.30.0, the method moves from V8 to Isolate
   *
   */

  void V8::SetFatalErrorHandler(FatalErrorCallback fn) {
    // V8 assumes that the caller is in an isolate (although it asserts in debug builds)
    internal::Isolate* i = internal::Isolate::GetCurrent();
    i->SetFatalErrorHandler(fn);
  }
//
//
//   void Isolate::Dispose() {
//     V8Monkey::InternalIsolate* internal = reinterpret_cast<InternalIsolate*>(this);
//
//     // Note that we check this here: the default isolate can be disposed of while entered. In fact, it is
//     // a V8 API requirement that it is entered when V8 is disposed (which in turn disposes the default
//     // isolate)
//     // XXX Clarify this comment. In fact clarify you can really dispose of default if in it
//     // XXX Update: I'm pretty sure this is wrong
//     if (internal->ContainsThreads()) {
//       V8Monkey::V8MonkeyCommon::TriggerFatalError("v8::Isolate::Dispose",
//                                                   "Attempt to dispose isolate in which threads are active");
//       return;
//     }
//
//     internal->Dispose();
//   }
//
//
//   void Isolate::Enter() {
//     FORWARD_TO_INTERNAL(Enter);
//   }
//
//
//   void Isolate::Exit() {
//     FORWARD_TO_INTERNAL(Exit);
//   }
//
//
//   void* Isolate::GetData() {
//     return reinterpret_cast<V8Monkey::InternalIsolate*>(this)->GetEmbedderData();
//   }
//
//
//   void Isolate::SetData(void* data) {
//     reinterpret_cast<V8Monkey::InternalIsolate*>(this)->SetEmbedderData(data);
//   }
//
//
   namespace internal {
//
//     /*
//      * Isolates stack, can be entered multiple times, and can be used by multiple threads. As V8 allows threads to
//      * "unlock" themselves to yield the isolate, we can't even be sure that threads will enter and exit in a LIFO
//      * order-the ordering will be at the mercy of the locking mechanism. Thus we need some way of answering the
//      * following questions:
//      *
//      * - Which threads have entered a given isolate?
//      * - How many times has a given thread entered an isolate?
//      * - When a thread leaves, which isolate (if any) is it returning to?
//      *
//      * We use the following class to answer such questions. The isolate maintains a linked list of these objects. We
//      * expect the following invariants to hold:
//      *
//      * - There is only one ThreadData object for a thread, regardless of how many times a thread has entered the
//      *   isolate. The entryCount member will correctly reflect the number of times a thread has entered the isolate
//      *
//      * - If a thread has not entered an isolate, there will be no ThreadData entry for that thread
//      *
//      * - When no threads have entered the isolate, or all threads have exited the isolate, the isolate's threadData
//      *   member will be null
//      *
//      * Note that we do not assume an ordering on the linked list. In particular, we don't assume the head of the linked
//      * list is the currently entered thread's data.
//      *
//      */
//
//     struct InternalIsolate::ThreadData {
//       int entryCount;
//       int threadID;
//       InternalIsolate* previousIsolate;
//       ThreadData* prev;
//       ThreadData* next;
//
//       ThreadData(int id, InternalIsolate* previous, ThreadData* previousElement, ThreadData* nextElement) :
//         entryCount(0), threadID(id), previousIsolate(previous), prev(previousElement), next(nextElement) {}
//     };
//
//     /*
//      * When an InternalIsolate is entered, we will assign that thread a JSRuntime and a JSContext if the thread has
//      * not yet had one assigned. Likewise, and in common with V8, at this point we will assign the thread a unique
//      * numeric ID.
//      *
//      * When any thread enters an isolate, we must notify SpiderMonkey that the isolate will be participating in GC
//      * rooting. Once in an isolate, a thread is free to start creating objects, and some of those objects may wrap
//      * SpiderMonkey objects.
//      *
//      * Isolates stack: if a thread enters isolate A, and then creates and enters isolate B from A, then on exiting B,
//      * the thread should be considered in A once more. Some book-keeping here handles this.
//      *
//      */
//
//     void InternalIsolate::Enter() {
//       // XXX What should we do if we re-enter a disposed isolate? In V8, the Enter function succeeds, although you're
//       //     going to be in a world of pain pretty soon: if the isolate wasn't the default then the dispose call would
//       //     delete it. Should we error out or something, just for appearances sakes?
//       //     In any case, I think we still need the isDisposed marker to note whether there is any outstanding book-keeping
//       isDisposed = false;
//
//       // We need to ensure this thread has an ID. GetCurrentThreadId creates one if necessary
//       int threadID = fetchOrAssignThreadID();
//
//       // Likewise, it should have a JSRuntime and JSContext
//       SpiderMonkeyUtils::AssignJSRuntimeAndJSContext();
//
//       // Register this isolate with SpiderMonkey for GC
//       AddGCRooter();
//
//       // What isolate was the thread in previously?
//       InternalIsolate* previousIsolate = GetCurrentIsolateFromTLS();
//
//       // Note a new entry for this thread
//       ThreadData* data = FindOrCreateThreadData(threadID, previousIsolate);
//       data->entryCount++;
//
//       SetCurrentIsolateInTLS(this);
//     }
//
//
//     /*
//      * Exiting the isolate is another simple book-keeping exercise, however note what doesn't happen: we don't
//      * deregister from SpiderMonkey GC rooting. Objects created-particularly persistent objects-may have the same
//      * lifetime as the isolate, so could exist until the client calls Isolate::Dispose. It would therefore be
//      * incorrect to stop tracing at this point.
//      * XXX What about HandleScopes/Locals. Clarify this comment.
//      *
//      */
//
//     void InternalIsolate::Exit() {
//       int threadID = fetchOrAssignThreadID();
//
//       ThreadData* data = FindThreadData(threadID);
//       // Attempting to exit a thread we haven't entered is an API misuse error
//       // XXX What does V8 do here? Document. No doubt I'll ask just this question again if you don't.
//       ASSERT(data, "InternalIsolate::Exit", "Exiting a thread we didn't enter?");
//
//       // Note that we have exited the isolate
//       data->entryCount--;
//
//       // Nothing more to do if the thread has other outstanding exits
//       if (data->entryCount > 0) {
//         return;
//       }
//
//       // Time for this thread to say goodbye. We need to remove the ThreadData
//       // from the linked list, and pop that thread's isolate stack
//       SetCurrentIsolateInTLS(data->previousIsolate);
//
//       // Note: after this call, data will be a dangling pointer
//       DeleteAndFreeThreadData(data);
//     }
//
//
//     // XXX Verify the thing below about HandleScopes. Remind yourself, what checking does V8 do?
//     /*
//      * As hinted at in the comments above, when the isolate is disposed, we are finally able to deregister from
//      * SpiderMonkey GC rooting. At this point, we also delete the contents of our ObjectBlock structure for Persistent
//      * handles (the similar structure for HandleScope/Locals should have already been emptied when HandleScopes were
//      * exited; if there is still an extant HandleScope, this represents an API misuse error on the part of the client,
//      * so he/she can jolly well live with the consequences).
//      *
//      */
//
//     void InternalIsolate::Dispose() {
//       // Don't dispose of the default isolate (or indeed any isolate) multiple times
//       if (isDisposed) {
//         return;
//       }
//
//       // Unhook this isolate from SpiderMonkey GC rooting
//       if (isRegisteredForGC) {
//         DeleteIsolateFromGCTLSList(this);
//       }
//
//       // Although we have just unhooked ourselves from the garbage collector, there might already be a GC running
//       AutoGCMutex(this);
//
//       // Ensure we don't attempt to delete the default isolate multiple times
//       isDisposed = true;
//
//       // As we no longer participate in rooting, we must release any remaining objects if their persistents failed to
//       // do so. Once a particular isolate is destroyed, it is an API misuse error to dereference Persistents containing
//       // references to objects created in that isolate.
//       // XXX This seems to be the case: need to break on Utils::OpenHandle and look further
//       if (persistentData.limit != nullptr) {
//         // We assume if one field in persistentData is non-null then they both are, i.e. handles exist
//         ObjectBlock<V8MonkeyObject>::Delete(persistentData.limit, persistentData.next, nullptr, persistentTearDown);
//       }
//
//       // There should be no extant HandleScopes at this point-if there is, the client has misused the API. HandleScopes
//       // contain pointers to our Local handle object block. Those pointers will be dangling shortly.
//       ASSERT(handleScopeData.limit == nullptr && handleScopeData.next == nullptr, "Isolate::Dispose",
//              "HandleScopes not destroyed!");
//
//       // The default isolate gets deleted by OneTrueStaticInitializer. This is consistent with V8 behaviour.
//       if (this != defaultIsolate) {
//         delete this;
//       } else {
//         // Clear out TLS so the destructor doesn't run again (unless on main thread)
//         int threadID = fetchOrAssignThreadID();
//         if (threadID != 1) {
//           SetCurrentIsolateInTLS(nullptr);
//         }
//       }
//     }
//
//
//     /*
//      * The destructor must check to see if the isolate is the default isolate, and if so dispose it to force
//      * GC unrooting in case the client never called Dispose. For all other isolates, this should be a no-op.
//      *
//      */
//
//     InternalIsolate::~InternalIsolate() {
//       // XXX Should we assert about containing threads on Dispose too/instead of?
//       ASSERT(!ContainsThreads(), "InternalIsolate::~InternalIsolate", "Threads still active");
//       // XXX Why are we asserting isDisposed here? Is that a V8 compat thing?
//       ASSERT(isDisposed || IsDefaultIsolate(this), "InternalIsolate::~InternalIsolate", "Isolate not disposed");
//
//       // XXX Does V8 assert if the default isolate has not been disposed? Reword comment below based on the results of
//       //     your investigation.
//       // It's possible the client never called dispose for the default isolate, although they probably should have.
//       // In those circumstances, we must ensure that Dispose is called in order to cease GC Rooting. This must happen
//       // before JSRuntime and JSContext teardown, which, if the default isolate is being deleted, is about to happen.
//       if (!isDisposed) {
//         Dispose();
//       }
//     }
//
//
//     /*
//      * An InternalIsolate contains two structures containing pointers to V8MonkeyObjects. Objects created whilst the
//      * thread has entered this isolate will be referenced there. Some of those objects might wrap SpiderMonkey objects,
//      * and need to participate in GC rooting. This function adds a tracing function which will traverse and trace those
//      * structures.
//      *
//      */
//
//     void InternalIsolate::AddGCRooter() {
//       AutoGCMutex(this);
//
//       // Protect against isolate re-entry
//       if (isRegisteredForGC) {
//         return;
//       }
//
//       #ifdef V8MONKEY_INTERNAL_TEST
//         if (GCRegistrationHookFn) {
//           GCRegistrationHookFn(SpiderMonkeyUtils::GetJSRuntimeForThread(), GCTracingFunction, this);
//         } else {
//           JS_AddExtraGCRootsTracer(SpiderMonkeyUtils::GetJSRuntimeForThread(), GCTracingFunction, this);
//         }
//       #else
//         JS_AddExtraGCRootsTracer(SpiderMonkeyUtils::GetJSRuntimeForThread(), GCTracingFunction, this);
//       #endif
//
//       AddIsolateToGCTLSList(this);
//       isRegisteredForGC = true;
//     }
//
//
//     /*
//      * Called on isolate disposal to unhook the isolate from GC Rooting. Objects created whilst in this isolate are
//      * now effectively dead, so the SpiderMonkey objects they wrapped no longer need to be rooted, and can be reaped
//      * by the SpiderMonkey garbage collector.
//      *
//      */
//
//     void InternalIsolate::RemoveGCRooter() {
//       AutoGCMutex(this);
//
//       if (!isRegisteredForGC) {
//         return;
//       }
//
//       // Deregister this isolate from SpiderMonkey
//       #ifdef V8MONKEY_INTERNAL_TEST
//         if (GCDeregistrationHookFn) {
//           GCDeregistrationHookFn(SpiderMonkeyUtils::GetJSRuntimeForThread(), GCTracingFunction, this);
//         } else {
//           JS_RemoveExtraGCRootsTracer(SpiderMonkeyUtils::GetJSRuntimeForThread(), GCTracingFunction, this);
//         }
//       #else
//         JS_RemoveExtraGCRootsTracer(SpiderMonkeyUtils::GetJSRuntimeForThread(), GCTracingFunction, this);
//       #endif
//
//       DeleteIsolateFromGCTLSList(this);
//       isRegisteredForGC = false;
//     }
//
//
//     /*
//      * Searches the linked list of thread data, and finds the ThreadData object for the given thread ID.
//      *
//      * Retuns nullptr if there is no entry for that threadID
//      *
//      */
//
//     InternalIsolate::ThreadData* InternalIsolate::FindThreadData(int threadID) {
//       ThreadData* data = threadData;
//
//       while (data && data->threadID != threadID) {
//         data = data->next;
//       }
//
//       return data;
//     }
//
//
//
//     /*
//      * Searches the linked list of thread data, and finds the ThreadData object for the given thread ID.
//      * If no entry exists for the thread, then one will be created, with the previous isolate field set to the given
//      * isolate.
//      *
//      */
//
//     InternalIsolate::ThreadData* InternalIsolate::FindOrCreateThreadData(int threadID, InternalIsolate* previousIsolate) {
//       ThreadData* data = FindThreadData(threadID);
//       if (data) {
//         return data;
//       }
//
//       // Need to create new ThreadData. We will insert it at the head of the list
//       ThreadData* td = new ThreadData(threadID, previousIsolate, nullptr, threadData);
//       if (threadData) {
//         threadData->prev = td;
//       }
//       threadData = td;
//
//       return td;
//     }
//
//
//     /*
//      * Delete the given entry from the ThreadData linked list and delete it.
//      *
//      */
//
//     void InternalIsolate::DeleteAndFreeThreadData(ThreadData* td) {
//       ASSERT(td, "InternalIsolate::DeleteAndFreeThreadData", "Attempting to delete null entry");
//       if (td->prev) {
//         td->prev->next = td->next;
//       }
//
//       if (td->next) {
//         td->next->prev = td->prev;
//       }
//
//       if (threadData == td) {
//         threadData = td->next;
//       }
//
//       delete td;
//     }
//
//
//     /*
//      * API for Lockers to "lock" the isolate for a given thread. As we are currently single-threaded, this is
//      * essentially meaningless. Hopefully, this won't always be the case.
//      *
//      */
//
//     void InternalIsolate::Lock() {
//       lockingMutex.Lock();
//       lockingThread = fetchOrAssignThreadID();
//     }
//
//
//     void InternalIsolate::Unlock() {
//       lockingThread = 0;
//       lockingMutex.Unlock();
//     }
//
//
//     bool InternalIsolate::IsLockedForThisThread() const {
//       return lockingThread == fetchOrAssignThreadID();
//     }
//
//
    /*
     * Returns the currently entered isolate. May return nullptr.
     *
     */

     Isolate* Isolate::GetCurrent() {
       return GetCurrentIsolateFromTLS();
     }
//
//
//     /*
//      * Answers the question as to whether a thread has entered a given isolate.
//      *
//      */
//
//     bool InternalIsolate::IsEntered(InternalIsolate* i) {
//       // The naive approach here would be to simply grab the isolate pointer from TLS and compare. However, this would
//       // be incorrect for the main thread, as, for V8 compatability, it has a pointer to the default isolate regardless
//       // of entry status.
//       int threadID = fetchOrAssignThreadID();
//       return i->FindThreadData(threadID) != nullptr;
//     }
//
//
//     /*
//      * In V8, the thread that runs the static initializers is permanently associated with the default isolate,
//      * and has thread ID 1. This function ensures that this is also the case in V8Monkey. Additionally, it
//      * ensures the default isolate is constructed.
//      *
//      * This function is intended to only be called once, by the OneTrueStaticIntializer, and must be called after
//      * V8MonkeyCommon::InitTLSKeys.
//      *
//      */
//
//     void InternalIsolate::EnsureDefaultIsolateForStaticInitializerThread() {
//       static bool initialized = false;
//
//       ASSERT(!initialized, "V8MonkeyCommon::EnsureDefaultIsolateForInitializerThread",
//                            "Default isolate already initialised");
//
//       // We create the default isolate. OneTrueStaticInitializer's destructor will destroy and release it
//       InternalIsolate::defaultIsolate = new InternalIsolate;
//
//       // Permanently associate the default isolate with this thread
//       SetCurrentIsolateInTLS(InternalIsolate::defaultIsolate);
//
//       // We should be thread number 1!
//       int threadID = fetchOrAssignThreadID();
//
//       ASSERT(threadID == 1, "InternalIsolate::EnsureDefaultIsolateForStaticInitializerThread", "Wrong thread ID!");
//       initialized = true;
//     }
//
//
//     InternalIsolate* InternalIsolate::EnsureInIsolate() {
//       InternalIsolate* current = GetCurrent();
//       int threadID = fetchOrAssignThreadID();
//       if (current && current->FindThreadData(threadID) != nullptr) {
//         return current;
//       }
//
//       // V8 permanently associates threads that implicitly enter the default isolate with the default isolate
//       SetCurrentIsolateInTLS(defaultIsolate);
//       defaultIsolate->Enter();
//
//       return defaultIsolate;
//     }
//
//
//     /*
//      * API for HandleScopes and Persistents.. The structure containing pointers to the wrapped objects lives here in the
//      * isolate, but is managed by the HandleScopes and Persistents. Having the structures and the associated data live
//      * in the isolate means we can keep the logic for rooting/tracing in one place.
//      *
//      */
//
//     // XXX Should the getters return references? Document the decision so you don't ask the question again
//     HandleData InternalIsolate::GetLocalHandleData() {
//       return handleScopeData;
//     }
//
//
//     void InternalIsolate::SetLocalHandleData(HandleData& hd) {
//       handleScopeData = hd;
//     }
//
//
//     HandleData InternalIsolate::GetPersistentHandleData() {
//       return persistentData;
//     }
//
//
//     void InternalIsolate::SetPersistentHandleData(HandleData& hd) {
//       persistentData = hd;
//     }
//
//
//
//     /*
//      * Allocates JSRuntimes and JSContexts to this thread
//      *
//      */
//     void SpiderMonkeyUtils::AssignJSRuntimeAndJSContext() {
//       ensureRuntimeAndContext();
//     }
//
//
//     /*
//      * Although part of SpiderMonkeyUtils, we must implement this here, as the lifetime of the objects in TLS storage
//      * is related to isolate teardown.
//      *
//      */
//
//     RTCXData SpiderMonkeyUtils::GetJSRuntimeAndJSContext() {
//       void* raw = Platform::GetTLSData(smDataKey);
//       if (raw) {
//         SpiderMonkeyData* data = reinterpret_cast<SpiderMonkeyData*>(raw);
//         return {data->rtcx.rt, data->rtcx.cx};
//       }
//
//       return {nullptr, nullptr};
//     }
//
//
//     /*
//      * The thread's assigned JSRuntime and JSContext are normally torn down when the thread exits. However, in the
//      * case of the last surviving thread, this will be too late, as OneTrueStaticInitializer's destructor may have
//      * ran by that point, killing SpiderMonkey. This function is intended to be called by OneTrueStaticInitializer's
//      * destructor to force the teardown of the SpiderMonkey objects (preceded by the disposal of any isolates that
//      * are still participating in rooting).
//      *
//      */
//
//     void V8MonkeyCommon::ForceMainThreadRTCXDisposal() {
//       void* raw = Platform::GetTLSData(smDataKey);
//       if (!raw) {
//         return;
//       }
//
//       tearDownCXAndRT(raw);
//       // Ensure the TLSKey destructor is not invoked when we do exit
//       Platform::StoreTLSData(smDataKey, nullptr);
//     }
//
//
//     /*
//      * Invoked by the SpiderMonkey garbage collector. Compiles the information required by individual object's tracing
//      * functions, then iterates over the object blocks for Locals and Persistents respectively.
//      *
//      */
//
//     void InternalIsolate::Trace(JSTracer* tracer) {
//       // Don't allow API mutation of the handle structures during tracing
//       AutoGCMutex(this);
//
//       TraceData td = {SpiderMonkeyUtils::GetJSRuntimeForThread(), tracer};
//
//       // Trace Local handles from HandleScopes
//       ObjectBlock<V8MonkeyObject>::Iterate(handleScopeData.limit, handleScopeData.next, tracingIterationFunction, &td);
//
//       // Trace Persistent handles
//       ObjectBlock<V8MonkeyObject>::Iterate(persistentData.limit, persistentData.next, tracingIterationFunction, &td);
//     }
//
//
//     InternalIsolate* InternalIsolate::defaultIsolate = nullptr;
//
//
//     #ifdef V8MONKEY_INTERNAL_TEST
//
//     void (*InternalIsolate::GCRegistrationHookFn)(JSRuntime*, JSTraceDataOp, void*) = nullptr;
//     void (*InternalIsolate::GCDeregistrationHookFn)(JSRuntime*, JSTraceDataOp, void*) = nullptr;
//
//
//     void InternalIsolate::ForceGC() {
//       JS_GC(SpiderMonkeyUtils::GetJSRuntimeForThread());
//     }
//
//
//     // XXX Can we fix these up to watch out for isolate construction?
//     TestUtils::AutoIsolateCleanup::~AutoIsolateCleanup() {
//       while (Isolate::GetCurrent() && InternalIsolate::IsEntered(InternalIsolate::GetCurrent())) {
//         Isolate* i = Isolate::GetCurrent();
//         InternalIsolate* ii = InternalIsolate::FromIsolate(i);
//
//         // Isolates can be entered multiple times
//         while (InternalIsolate::IsEntered(ii)) {
//           i->Exit();
//         }
//
//         i->Dispose();
//       }
//     }
//
//
//     TestUtils::AutoTestCleanup::AutoTestCleanup() {
//       // Use fetchOrThreadID to avoid implicitly initting V8
//       if (fetchOrAssignThreadID() != 1) {
//         V8Platform::Platform::ExitWithError("Why are you using AutoTestCleanup on a thread, idiot?");
//       }
//     }
//
//
//     TestUtils::AutoTestCleanup::~AutoTestCleanup() {
//       while (Isolate::GetCurrent() && InternalIsolate::IsEntered(InternalIsolate::GetCurrent())) {
//         Isolate* i = Isolate::GetCurrent();
//         InternalIsolate* ii = InternalIsolate::FromIsolate(i);
//
//         // Isolates can be entered multiple times
//         while (InternalIsolate::IsEntered(ii)) {
//           i->Exit();
//         }
//
//         i->Dispose();
//       }
//       V8::Dispose();
//     }
//     #endif
  }
}
