#ifndef V8MONKEY_ISOLATE_H
#define V8MONKEY_ISOLATE_H

// JSRuntime, JSTracer JSTraceDataOp
#include "jsapi.h"

// Mutex
#include "platform.h"

// V8MonkeyObject
#include "types/base_types.h"

// EXPORT_FOR_TESTING_ONLY
#include "test.h"

// FatalErrorCallback
#include "v8.h"


namespace v8 {
  namespace V8Monkey {


    /*
     * Container class for Handle information. Although only manipulated by HandleScopes and Persistents, it is
     * convenient to have all the traced values in one place, and only one object that needs to participate in
     * GC rooting. Hence the HandleData, and the blocks of object pointers they refer to are stored in
     * InternalIsolates.
     */

    struct HandleData {
      V8MonkeyObject** next;

      V8MonkeyObject** limit;

      int level;

      inline void Initialize() {
        next = limit = nullptr;
        level = 0;
      }
    };


    /*
     * An internal variant of the public facing Isolate class. A central repository for a large amount of book-keeping
     * data. See the comments in isolate.cpp for further information on how this relates to SpiderMonkey structures.
     *
     */

    class EXPORT_FOR_TESTING_ONLY InternalIsolate {
      public:
        InternalIsolate() : isDisposed(false), isRegisteredForGC(false), fatalErrorHandler(nullptr), threadData(nullptr),
                            embedderData(nullptr), lockingThread(0), isInitted(false) {
          handleScopeData.Initialize();
          persistentData.Initialize();
        }

        ~InternalIsolate();

        // Enter the given isolate
        void Enter();

        // Exit the given isolate
        void Exit();

        // Dispose of the given isolate
        void Dispose();

        // Is the given isolate the special default isolate?
        static bool IsDefaultIsolate(InternalIsolate* i) { return i == defaultIsolate; }

        // Return the default isolate
        static InternalIsolate* GetDefaultIsolate() { return defaultIsolate; }

        // Find the current InternalIsolate for the calling thread
        static InternalIsolate* GetCurrent();

        // Reports whether any threads are active in this isolate
        bool ContainsThreads() const { return threadData != nullptr; }

        // Many API functions will implicitly enter the default isolate if required. To that end, this function returns
        // the current internal isolate if non-null, otherwise enters the default isolate and returns that
        // XXX Look for opportunities to use this
        static InternalIsolate* EnsureInIsolate();

        // Has the current thread entered the given isolate?
        static bool IsEntered(InternalIsolate* i);

        // Set the fatal error handler for this isolate
        void SetFatalErrorHandler(FatalErrorCallback fn) { fatalErrorHandler = fn; }

        // Get the fatal error handler for this isolate
        FatalErrorCallback GetFatalErrorHandler() const { return fatalErrorHandler; }

        // Set the embedder data for this isolate
        void SetEmbedderData(void* data) { embedderData = data; }

        // Get the embedder data for this isolate
        void* GetEmbedderData() const { return embedderData; }

        // Lock this isolate for the current thread
        void Lock();

        // Unlock this isolate
        void Unlock();

        // Returns true if a Locker has locked this isolate on behalf of some thread. Largely for future use.
        bool IsLocked() const {
          // This assumes that a thread's ID will never be zero. (Storing thread IDs in TLS would also be broken were
          // that not the case).
          return lockingThread != 0;
        }

        // Is the current thread the one that locked me?
        bool IsLockedForThisThread() const;

        // Return a copy of the handle scope data for this isolate. If manipulating this data, the caller must hold the
        // GC Mutex
        // XXX Check need to hold it in dispose/destructor
        HandleData GetLocalHandleData();

        // Copy the given HandleData into our own. The caller must hold the GC Mutex.
        // XXX Check need to hold it in dispose/destructor
        void SetLocalHandleData(HandleData& hd);

        // Return a copy of the persistent data for this isolate. If manipulating this data, the caller must hold the
        // GC Mutex.
        // XXX Check need to hold it in dispose/destructor
        HandleData GetPersistentHandleData();

        // Copy the given HandleData into our persistent data. The caller must hold the GC Mutex.
        // XXX Check need to hold it in dispose/destructor
        void SetPersistentHandleData(HandleData& hd);

        // Lock the GC Mutex to allow the caller to safely mutate HandleData
        void LockGCMutex() {
          GCMutex.Lock();
        }

        // Signal that the caller has ceased manipulating the HandleData and that it is safe to trace objects
        void UnlockGCMutex() {
          GCMutex.Unlock();
        }

        // GC Rooting
        void Trace(JSTracer* tracer);

        // Cease object rooting. Public as a TLS destructor function must be able to call it.
        void RemoveGCRooter();

        // RAII helper
        class AutoGCMutex {
          public:
            AutoGCMutex(InternalIsolate* i) : isolate(i) {
              isolate->LockGCMutex();
            }

            ~AutoGCMutex() {
              isolate->UnlockGCMutex();
            }

          private:
            InternalIsolate* isolate;
        };


        // Provided for V8 compat
        bool IsInitted() const { return isInitted; }
        void Init() { isInitted = true; }

        #ifdef V8MONKEY_INTERNAL_TEST
        static InternalIsolate* FromIsolate(Isolate* i) {
          return reinterpret_cast<InternalIsolate*>(i);
        }

        static void SetGCRegistrationHooks(void (*onNotifier)(JSRuntime*, JSTraceDataOp, void*), void (*offNotifier)(JSRuntime*, JSTraceDataOp, void*)) {
          GCRegistrationHookFn = onNotifier;
          GCDeregistrationHookFn = offNotifier;
        }

        static void ForceGC();

        #endif

        // Initialize the default isolate and claim thread id 1
        static void EnsureDefaultIsolateForStaticInitializerThread();

      private:
        struct ThreadData;

        // Tell SpiderMonkey about this isolate
        void AddGCRooter();

        // Has this isolate been disposed?
        bool isDisposed;

        // Have we told SpiderMonkey we're a rooter?
        bool isRegisteredForGC;

        // Fatal error handler for this isolate
        FatalErrorCallback fatalErrorHandler;

        // Our linked list of data about active threads in this isolate
        ThreadData* threadData;

        // Embedder data
        void* embedderData;

        // ID of the thread that has "locked" this isolate. For future use.
        int lockingThread;

        // Thread entry mutex. For future use.
        V8Platform::Mutex lockingMutex;

        // GC Mutex
        V8Platform::Mutex GCMutex;

        // In a single threaded application, there is no need to explicitly construct an isolate. V8 constructs a
        // "default" isolate automatically, and use it where necessary. Note that this changes in V8 3.29.79
        static InternalIsolate* defaultIsolate;

        // HandleScope data for Locals
        HandleData handleScopeData;

        // HandleScope data for Persistents
        HandleData persistentData;

        // ThreadData linked list manipulations
        ThreadData* FindOrCreateThreadData(int threadID, InternalIsolate* previousIsolate);
        ThreadData* FindThreadData(int threadID);
        void DeleteAndFreeThreadData(ThreadData* data);

        // V8 compat
        bool isInitted;

        #ifdef V8MONKEY_INTERNAL_TEST
        static void (*GCRegistrationHookFn)(JSRuntime*, JSTraceDataOp, void*);
        static void (*GCDeregistrationHookFn)(JSRuntime*, JSTraceDataOp, void*);
        #endif
    };
  }
}


#endif
