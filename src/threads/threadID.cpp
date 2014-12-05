#include "v8.h"
#include "platform.h"
#include "v8monkey_common.h"
#include "autolock.h"


namespace {
  using namespace v8::V8Platform;
  using namespace v8::V8Monkey;


  // Returns the thread's ID from TLS, or 0 if not present
  int GetThreadIDFromTLS() {
    void* raw_id = Platform::GetTLSData(V8MonkeyCommon::TLSKeys.threadIDKey);
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
    Platform::StoreTLSData(V8MonkeyCommon::TLSKeys.threadIDKey, reinterpret_cast<void*>(thread_id));

    return thread_id;
  }


  // Ensure TLS keys are created
  void InitializeCommonTLSKeys() {
    TLSKey* threadIDKey = Platform::CreateTLSKey();
    V8MonkeyCommon::TLSKeys.threadIDKey = threadIDKey;
  }



  // Hack to ensure first thread gets ID of 1
  class ThreadIDClaimer {
    public:
      ThreadIDClaimer() {
        TLSKeyInitControl->Run();
        // XXX Should we feed this into a volatile or somesuch, to ensure this doesn't get optimized away?
        v8::V8::GetCurrentThreadId();
      }

    private:
      // Ensure TLS keys are initialized
      static OneTimeFunctionControl* TLSKeyInitControl;
  };


  OneTimeFunctionControl* ThreadIDClaimer::TLSKeyInitControl = Platform::CreateOneShotFunction(InitializeCommonTLSKeys);


  // We define this here rather than with the class declaration to ensure correct static constructor ordering
  ThreadIDClaimer thread1;
}


namespace v8 {
  namespace V8Monkey {
    V8Monkey_TLSKeys V8MonkeyCommon::TLSKeys {NULL};
  }


  int V8::GetCurrentThreadId() {
    int existing_id = GetThreadIDFromTLS();
    if (existing_id > 0) {
      return existing_id;
    }

    return CreateAndAssignThreadID();
  }
}
