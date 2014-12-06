// V8
#include "v8.h"

#include "autolock.h"
#include "isolate.h"
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
 * Clearly then, we cannot have a 1-1 correspondence from V8 Isolates to JSAPI JSRuntimes.
 *
 * TODO: What approach are we taking
 *
 * We follow the lead of V8 here. The public-facing Isolate class is made of air; it simply wraps pointers to the
 * internal class.
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


  // Returns the thread's ID from TLS, or 0 if not present
  int GetThreadIDFromTLS() {
    void* raw_id = Platform::GetTLSData(threadIDKey);
    return *reinterpret_cast<int*>(&raw_id);
  }


  // Returns a unique thread ID
  int CreateThreadID() {
    static Mutex* threadIDMutex = Platform::CreateMutex();

    // Thread IDs must be greater than 0; otherwise we wouldn't be able to tell if the value is the thread ID, or
    // represents the case where the given thread has no data for the TLS key
    static int nextID = 1;

    // We just use a plain ole mutex here. V8 drops down to ASM, using a lock-prefixed xaddl instruction to avoid
    // overhead
    AutoLock lock(threadIDMutex);
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


  // Ensure TLS keys are created
  void InitializeCommonTLSKeys() {
    threadIDKey = Platform::CreateTLSKey();
    isolateKey = Platform::CreateTLSKey();
  }



  // We again follow V8's lead, and ensure that a static initializer bootstraps the default isolate
  class DefaultIsolateCreator {
    public:
      DefaultIsolateCreator() {
        InternalIsolate::CreateDefaultIsolate();
      }
  } defaultCreator;


  // We define this here rather than with the class declaration to ensure correct static constructor ordering
  //DefaultIsolateCreator thread1;
}


namespace v8 {
  int V8::GetCurrentThreadId() {
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


    InternalIsolate* InternalIsolate::GetIsolateFromTLS() {
      void* raw_id = Platform::GetTLSData(isolateKey);
      return reinterpret_cast<InternalIsolate*>(raw_id);
    }


    void InternalIsolate::SetIsolateInTLS(InternalIsolate* i) {
      Platform::StoreTLSData(isolateKey, i);
    }


    void InternalIsolate::Enter() {
      // We need to ensure this thread has an ID. GetCurrentThreadId creates one if necessary
      int threadID = FetchOrAssignThreadId();

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
      if (this != defaultIsolate) {
        delete this;
      }
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


    #ifdef V8MONKEY_INTERNAL_TEST
    bool InternalIsolate::IsEntered(InternalIsolate* i) {
      // Note: we can't simply grab the isolate* from TLS, as main will have a non-null value, even when it hasn't
      // entered the isolate
      int threadID = FetchOrAssignThreadId();
      return i->FindThreadData(threadID) != NULL;
    }
    #endif


    void InternalIsolate::CreateDefaultIsolate() {
      static V8Platform::Mutex* mutex = V8Platform::Platform::CreateMutex();

      AutoLock lock(mutex);
      if (defaultIsolate == NULL) {
        InitializeCommonTLSKeys();
        defaultIsolate = new InternalIsolate();
        SetIsolateInTLS(defaultIsolate);
        FetchOrAssignThreadId();
      }
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


    InternalIsolate* InternalIsolate::defaultIsolate = NULL;
  }
}
