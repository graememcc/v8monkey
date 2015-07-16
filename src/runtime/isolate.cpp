/*
// for_each
#include <algorithm>

// atomic_int, atomic_fetch_add
#include <atomic>

// DestructList
// XXX That needs a better name
#include "data_structures/destruct_list.h"

// ObjectBlock
#include "data_structures/objectblock.h"

// std::begin, std::end
#include <iterator>

// Class definition
#include "runtime/isolate.h"

// AddIsolateRooter, EnsureRuntimeAndContext, GetJSRuntimeForThread, RemoveRooter, RTCXData
#include "utils/SpiderMonkeyUtils.h"

// TLSKey
#include "platform/platform.h"

// TestUtils EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"

// ObjectContainer
#include "types/base_types.h"

// SMBoolean
#include "types/value_types.h"

// V8MonkeyCommon class definition, TriggerFatalError, V8MONKEY_ASSERT
#include "utils/V8MonkeyCommon.h"

// kIsolateEmbedderDataOffset
#include "v8.h"

// V8_UNUSED
#include "v8config.h"


struct JSRuntime;
*/


// XXX Review comment
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
 * XXX Fix the paragraph below. We do now handle isolate entries exits correctly, but I'm still worried about cross-
 *     thread attempts to access values (although this might not be permitted anyway - check)
 *     Also, the bit about TLS is way off. We need it regardless.
 * Clearly then, when fully emulating the V8 API, we cannot have a 1-1 correspondence from V8 Isolates to JSAPI
 * JSRuntimes. Now, although I've pulled multithreading support for the moment-I wasn't satisfied I would be able to
 * satisfactorily clean up threads leaving isolates, particularly once it became necessary to hook up isolates to
 * GC rooting-the current implementation is based on the outline of my plan for a multi-threaded world, in case it
 * proves possible to reinstate multithreading, hence the otherwise unneccessary use of Thread-Local Storage.
 *
 * Essentially, we look out for the first time a thread enters any isolate, and assign a JSRuntime and JSContext then.
 * We can then register as a GC rooter for that isolate, though we must take care to ensure we deregister (ideally when
 * the client makes the Isolate::Dispose API call, however we must ensure this happens regardless). Additional
 * machinery exists elsewhere that deregistration also happens when any thread owning a JSRuntime exits.
 *
 * XXX Is the below still really true?
 * TODO: If the differences in the threading models prove insurmountable, then there are a lot of opportunities here
 *       and elsewhere for simplificication and possibly optimization.
 *
 */


/*
namespace {
  using namespace v8::V8Platform;
*/


  /*
   * Thread-local storage keys for isolate related thread data
   *
   */

/*
  // The thread's unique ID
  TLSKey<int> threadIDKey {};
  // The internal isolate that the thread has entered
  TLSKey<v8::internal::Isolate*> currentIsolateKey {};
*/


// XXX Keep this comment, move it and repurpose it for wherever we do the rooted isolate tracking
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


  /*
   * Interface to isolate tracing for the SpiderMonkey garbage collector. When a thread enters an isolate, the isolate
   * will register itself as a GC rooter with that thread's JSRuntime. This is the function that SpiderMonkey will call
   * when tracing roots.
   *
   */

/*
  void GCTracingFunction(JSTracer* tracer, void* data) {
    v8::SpiderMonkey::TracerData* traceData {reinterpret_cast<v8::SpiderMonkey::TracerData*>(data)};
    traceData->isolate->Trace(traceData->rt, tracer);
  }
*/


  /*
   * POD type for supplying SpiderMonkey garbage collection parameters to the objects contained in handles
   *
   */

/*
  struct GCData {
    JSRuntime* rt;
    JSTracer* tracer;
  };
*/


  /*
   * Interface to isolate tracing for ObjectBlock iteration. This is the function that Isolate::Trace will supply to
   * the ObjectBlock to iterate over the handles.
   *
   */

/*
  void GCIterationFunction(v8::internal::Object* obj, void* data) {
    if (!obj) {
      return;
    }

    GCData* gcData {reinterpret_cast<GCData*>(data)};
    obj->Trace(gcData->rt, gcData->tracer);
  }
*/


  /*
   * Returns the thread ID stored in TLS, creating one if it doesn't exist.
   *
   */

/*
  int fetchOrAssignThreadID() {
    static std::atomic_int idCount {1};

    int id {threadIDKey.Get()};

    if (id > 0) {
      return id;
    }

    id = std::atomic_fetch_add(&idCount, 1);
    threadIDKey.Set(id);

    return id;
  }
*/
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
/*
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
*/


  /*
   * We implement this here as death is on a per-internal isolate basis
   * NOTE: In 3.30.0, the API method moves from V8 to Isolate
   *
   */

/*
  bool V8::IsDead() {
    internal::Isolate* i {internal::Isolate::GetCurrent()};
    // Follow V8 and assert here
    V8MONKEY_ASSERT(i, "Not in an isolate");
    return i->IsDead();
  }
*/


  /*
   * We implement this here as error handlers are associated with internal isolates.
   * NOTE: In 3.30.0, the API method moves from V8 to Isolate
   *
   */

/*
  void V8::SetFatalErrorHandler(FatalErrorCallback fn) {
    // V8 assumes that the caller is in an isolate (although it asserts in debug builds)
    internal::Isolate* i {internal::Isolate::GetCurrent()};
    i->SetFatalErrorHandler(fn);
  }
*/
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
/*
  namespace internal {
*/

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

/*
    // XXX Temporary?
    void Isolate::RecordThreadEntry(Isolate* i) {
      previousIsolates.emplace_back(i);
    }


    // XXX Temporary?
    Isolate* Isolate::RecordThreadExit() {
      // XXX If we keep this, assert not empty
      auto end = std::end(previousIsolates) - 1;
      Isolate* i {*end};
      previousIsolates.erase(end);
      return i;
    }
*/

    /*
     * When an Isolate is entered, we will assign that thread a JSRuntime and a JSContext if the thread does not
     * already have them. Likewise, and in common with V8, at this point we will assign the thread a unique
     * numeric ID.
     *
     * When any thread enters an isolate, we must notify SpiderMonkey that the isolate will be participating in GC
     * rooting. Once in an isolate, a thread is free to start creating objects, and some of those objects may wrap
     * SpiderMonkey objects. Note: the reverse is not true. The objects may still be in scope when the thread exits,
     * so we must continue rooting until the thread exits or the isolate is disposed.
     * // XXX Prove and illustrate such a scenario could occur.
     *
     * Isolates stack: if a thread enters isolate A, and then creates and enters isolate B from A, then, on exiting B,
     * the thread should "return" to A. Some book-keeping here handles this.
     *
     */

/*
    void Isolate::Enter() {
      using namespace ::v8;

      // As isolate entries stack, and threads can unlock allowing other threads to enter an isolate, we must note
      // which isolate the entering thread to exit to, and which thread will be considered the most-recently entered
      // once this thread exits.

      // To aid tracking, we assign each thread a unique ID
      // XXX Either use threadID, remove it or don't assign. In any case get rid of V8_UNUSED
      int V8_UNUSED threadID {fetchOrAssignThreadID()};

      // Likewise, it should have a JSRuntime and JSContext
      SpiderMonkey::EnsureRuntimeAndContext();

      // Register this isolate with SpiderMonkey for GC
      {
        // XXX Why does AutoGCMutex need the isolate? I thought nested classes could access members of the parent?
        // XXX Do we even need to lock here? This shouldn't affect the ability of other JSRuntimes to trace us?
        AutoGCMutex {this};

        SpiderMonkey::TracerData* data {new SpiderMonkey::TracerData {SpiderMonkey::GetJSRuntimeForThread(), this}};
        SpiderMonkey::AddIsolateRooter(this, GCTracingFunction, data);
        isRegisteredForGC = true;
      }

      // What isolate was the thread in previously?
      Isolate* previousIsolate {currentIsolateKey.Get()};

      // XXX Temporary
      // Note the thread entry
      RecordThreadEntry(previousIsolate);
//
//       // Note a new entry for this thread
//       ThreadData* data = FindOrCreateThreadData(threadID, previousIsolate);
//       data->entryCount++;
//
      // Invariant: a thread's most recently entered isolate should be stored in TLS
      currentIsolateKey.Set(this);
    }
*/


    /*
     * On isolate init, we install the constants in their expected locations. We assume that we are being called from
     * V8::Initialize
     *
     */

/*
    void Isolate::doInit() {
      V8MONKEY_ASSERT(GetCurrent() == this, "Why aren't we in the isolate we're initializing?");
      V8MONKEY_ASSERT(!isInitted, "Init called twice?");

      // We are skating on thin ice here: the JSAPI header admonishes us not to depend on the internal representation
      // of JS::Values, however we are depending on the fact that they are JSRuntime-agnostic.
      auto trueIndex = ::v8::internal::Internals::kTrueValueRootIndex;
      primitiveValues[trueIndex] = new SMBoolean {true};
      primitiveValues[trueIndex]->AddRef();

      auto falseIndex = ::v8::internal::Internals::kFalseValueRootIndex;
      primitiveValues[falseIndex] = new SMBoolean {false};
      primitiveValues[falseIndex]->AddRef();
    }
*/


    /*
     * Exiting the isolate is another simple book-keeping exercise, however note what doesn't happen: we don't
     * deregister from SpiderMonkey GC rooting. Objects created-particularly persistent objects-may have the same
     * lifetime as the isolate, so could exist until the client calls Isolate::Dispose. It would therefore be
     * incorrect to stop tracing at this point.
     * XXX What about HandleScopes/Locals. Clarify this comment.
     *
     */

/*
    void Isolate::Exit() {
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

      // Time for this thread to say goodbye. We need to pop this thread's previous isolate from the container.
      // XXX Temporary
      Isolate* i {RecordThreadExit()};
      currentIsolateKey.Set(i);

        //SetCurrentIsolateInTLS(data->previousIsolate);
//
//       // Note: after this call, data will be a dangling pointer
//       DeleteAndFreeThreadData(data);
    }
*/


    // XXX Verify the thing below about HandleScopes. Remind yourself, what checking does V8 do?
    /*
     * As hinted at in the comments above, when the isolate is disposed, we are finally able to deregister from
     * SpiderMonkey GC rooting. At this point, we also delete the contents of our ObjectBlock structure for Persistent
     * handles (the similar structure for HandleScope/Locals should have already been emptied when HandleScopes were
     * exited; if there is still an extant HandleScope, this represents an API misuse error on the part of the client,
     * so they can jolly well live with the consequences).
     *
     */

/*
    void Isolate::Dispose(bool fromDestructor) {
      if (ContainsThreads()) {
        V8Monkey::TriggerFatalError("v8::Isolate::Dispose", "Attempt to dispose isolate in which threads are active");
        return;
      }

      // Unhook this isolate from SpiderMonkey GC rooting
      if (isRegisteredForGC) {
        AutoGCMutex {this};

        ::v8::SpiderMonkey::RemoveRooter(this);
        isRegisteredForGC = false;
      }

      // TODO Should we worry about the possibility of an already-queued GC being beaten to the lock by the
      //      RemoveGCRooter call above? If that was the case, we could beat it to the lock again below, and then
      //      we delete ourselves...

      for (auto begin = std::begin(primitiveValues), it = begin, end = std::end(primitiveValues); it != end; ++it) {
        auto dist = std::distance(begin, it);
        Object* obj {primitiveValues[dist]};

        if (obj) {
          obj->Release(&primitiveValues[dist]);
        }
      }

//       // Although we have just unhooked ourselves from the garbage collector, there might already be a GC running
//       AutoGCMutex(this);
//
      // Signal to the destructor that we've unhooked from GC Rooting
      isDisposed = true;
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
     if (!fromDestructor) {
       delete this;
     }
//       } else {
//         // Clear out TLS so the destructor doesn't run again (unless on main thread)
//         int threadID = fetchOrAssignThreadID();
//         if (threadID != 1) {
//           SetCurrentIsolateInTLS(nullptr);
//         }
//       }
  }


    Isolate::~Isolate() {
      V8MONKEY_ASSERT(!ContainsThreads(), "Destructing an isolate with threads still active");

      // It's possible the client never called dispose for the isolate, although V8 API requirements mandate that they
      // should have. (One scenario: the client stashed the Isolate* in a unique_ptr without a custom deleter.) We wil
      // need to grudgingly tidy up after them; we must ensure that we have disengaged from GC Rooting, something that
      // must happen before JSRuntime and JSContext teardown, which is probably imminent.
      if (!isDisposed) {
        Dispose(true);
      }

      V8MONKEY_ASSERT(!isRegisteredForGC, "Isolate is still rooting?");
    }

*/
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
     /*
      * API for Lockers to "lock" the isolate for a given thread. As we are currently single-threaded, this is
      * essentially meaningless. Hopefully, this won't always be the case.
      *
      */

/*
     void Isolate::Lock() {
       lockingMutex.Lock();
       lockingThread = fetchOrAssignThreadID();
     }


     void Isolate::Unlock() {
       lockingThread = 0;
       lockingMutex.Unlock();
     }


     bool Isolate::IsLockedForThisThread() const {
       return lockingThread == fetchOrAssignThreadID();
     }
*/


    /*
     * Returns the currently entered isolate. May return nullptr.
     *
     */

/*
    Isolate* Isolate::GetCurrent() {
      return currentIsolateKey.Get();
    }
*/


     /*
      * Answers the question as to whether a thread has entered a given isolate.
      *
      */

/*
    bool Isolate::IsEntered(Isolate* i) {
      return currentIsolateKey.Get() == i;
    }
*/


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
    /*
     * Invoked by the SpiderMonkey garbage collector. Compiles the information required by individual object's tracing
     * functions, then iterates over the object blocks for Locals and Persistents respectively.
     *
     */

/*
    void Isolate::Trace(JSRuntime* rt, JSTracer* tracer) {
      // Don't allow API mutation of the handle structures during tracing
      AutoGCMutex {this};

      GCData gcData {rt, tracer};

      // Trace constants
      auto primEnd = std::end(primitiveValues);
      for (auto it = std::begin(primitiveValues); it != primEnd; ++it) {
        GCIterationFunction(*it, &gcData);
      }

      localHandleData.Iterate(GCIterationFunction, &gcData);

      // Trace Persistent handles
      //ObjectBlock<V8MonkeyObject>::Iterate(persistentData.limit, persistentData.next, tracingIterationFunction, &td);
    }


#ifdef V8MONKEY_INTERNAL_TEST
  }


  namespace TestUtils {
    // XXX Can we fix these up to watch out for isolate construction?

    AutoIsolateCleanup::~AutoIsolateCleanup() {
      while (Isolate::GetCurrent()) {
        // Isolates can be entered multiple times
        Isolate* i {Isolate::GetCurrent()};

        internal::Isolate* ii {internal::Isolate::FromAPIIsolate(i)};
        V8MONKEY_ASSERT(!ii->IsLockedForThisThread(), "An isolate was still locked");

        while (Isolate::GetCurrent() == i) {
          i->Exit();
        }

        i->Dispose();
      }
    }


    AutoTestCleanup::~AutoTestCleanup() {
      while (Isolate::GetCurrent()) {
        // Isolates can be entered multiple times
        Isolate* i {Isolate::GetCurrent()};

        internal::Isolate* ii {internal::Isolate::FromAPIIsolate(i)};
        V8MONKEY_ASSERT(!ii->IsLockedForThisThread(), "An isolate was still locked");

        while (Isolate::GetCurrent() == i) {
          i->Exit();
        }

        i->Dispose();
      }

      V8::Dispose();
    }

#endif

  }
}
*/
