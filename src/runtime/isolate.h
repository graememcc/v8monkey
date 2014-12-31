#ifndef V8MONKEY_ISOLATE_H
#define V8MONKEY_ISOLATE_H

#include "jsapi.h"

#include "types/base_types.h"
#include "platform.h"
#include "test.h"
#include "v8monkey_common.h"


namespace v8 {
  namespace V8Monkey {
    class V8MonkeyObject;


    // Container class for Handle information. Stored in the isolate, but manipulated by HandleScopes and Persistents.
    class HandleData {
     public:
      V8MonkeyObject** next;

      V8MonkeyObject** limit;

      int level;

      inline void Initialize() {
        next = limit = NULL;
        level = 0;
      }
    };


    // An internal variant of the public facing Isolate class
    class EXPORT_FOR_TESTING_ONLY InternalIsolate {
      public:
        InternalIsolate() : isDisposed(false), isRegisteredForGC(false), fatalErrorHandler(NULL), threadData(NULL),
                            embedderData(NULL), lockingThread(0) {
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
        static bool IsDefaultIsolate(InternalIsolate* i);

        // Return the default isolate
        static InternalIsolate* GetDefaultIsolate();

        // Find the current InternalIsolate for the given thread
        static InternalIsolate* GetCurrent();

        // Reports whether any threads are active in this isolate
        bool ContainsThreads() { return threadData != NULL; }

        // Many API functions will implicitly enter the default isolate if required. To that end, this function returns
        // the current internal isolate if non-null, otherwise enters the default isolate and returns that
        // XXX Look for opportunities to use this
        static InternalIsolate* EnsureInIsolate();

        // Has the current thread entered the given isolate?
        static bool IsEntered(InternalIsolate* i);

        // Set the fatal error handler for this isolate
        void SetFatalErrorHandler(FatalErrorCallback fn) { fatalErrorHandler = fn; }

        // Get the fatal error handler for this isolate
        FatalErrorCallback GetFatalErrorHandler() { return fatalErrorHandler; }

        // Set the embedder data for this isolate
        void SetEmbedderData(void* data) { embedderData = data; }

        // Get the embedder data for this isolate
        void* GetEmbedderData() { return embedderData; }

        // Lock this isolate for the current thread
        void Lock();

        // Unlock this isolate
        void Unlock();

        // This assumes that a thread's ID will never be zero
        bool IsLocked() {
          return lockingThread != 0;
        }

        // Is the current thread the one that locked me?
        bool IsLockedForThisThread();

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

        // Get the GC Mutex for mutating HandleData
        void LockGCMutex() {
          GCMutex.Lock();
        }

        // Unlock the GC Mutex for mutating HandleData
        void UnlockGCMutex() {
          GCMutex.Unlock();
        }

        // GC Rooting
        void Trace(JSTracer* tracer);

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

        void RemoveGCRooter();

        // Isolates stack, can be entered multiple times, and can be used by multiple threads. As V8 allows threads to
        // "unlock" themselves to yield the isolate, we can't even be sure that threads will enter and exit in a LIFO
        // order-the ordering will be at the mercy of the locking mechanism. Thus we need some way of answering the
        // following questions:
        //   Which threads have entered us?
        //   How many times has a given thread entered us?
        //   When a thread leaves, which isolate is it returning to?
        // We use this class to answer such questions
        struct ThreadData {
          int entryCount;
          int threadID;
          InternalIsolate* previousIsolate;
          ThreadData* prev;
          ThreadData* next;

          ThreadData(int id, InternalIsolate* previous, ThreadData* previousElement, ThreadData* nextElement) :
            entryCount(0), threadID(id), previousIsolate(previous), prev(previousElement), next(nextElement) {}
        };

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

        // Return the JSRuntime associated with this thread. May be null
        static JSRuntime* GetJSRuntimeForThread();

        // Return the JSContext associated with this thread. May be null
        static JSContext* GetJSContextForThread();

      private:
        // Tell SpiderMonkey about this isolate
        void AddGCRooter();

        // Unfortunately needs to be public so the DestructList function can access it
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

        // ID of the thread that has locked this isolate
        int lockingThread;

        // Thread entry mutex
        V8Platform::Mutex lockingMutex;

        // GC Mutex
        V8Platform::Mutex GCMutex;

        // In a single threaded application, there is no need to explicitly construct an isolate. V8 constructs a
        // "default" isolate automatically, and use it where necessary.
        static InternalIsolate* defaultIsolate;

        // HandleScope data for Locals
        HandleData handleScopeData;

        // HandleScope data for Persistents
        HandleData persistentData;

        // Linked list manipulations
        ThreadData* FindOrCreateThreadData(int threadID, InternalIsolate* previousIsolate);
        ThreadData* FindThreadData(int threadID);
        void DeleteThreadData(ThreadData* data);

        #ifdef V8MONKEY_INTERNAL_TEST
        static void (*GCRegistrationHookFn)(JSRuntime*, JSTraceDataOp, void*);
        static void (*GCDeregistrationHookFn)(JSRuntime*, JSTraceDataOp, void*);
        #endif

        friend class V8MonkeyCommon;
    };
  }
}


#endif
