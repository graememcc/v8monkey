#ifndef V8MONKEY_V8_H
#define V8MONKEY_V8_H

#include "jsapi.h"


#define APIEXPORT __attribute__ ((visibility("default")))


// V8 API Compatibility
#define V8COMPAT "3.14.5.9"


namespace v8 {
  class APIEXPORT Isolate {
    public:
      // XXX Isolate::Scope
      static Isolate* New();
      void Dispose();
      /* Other public methods that need implemented
      static Isolate* GetCurrent();
      void Enter();
      void Exit();
      inline void SetData(void* data);
      inline void* GetData();
     private:
      Isolate(const Isolate&);
      Isolate& operator=(const Isolate&);
      void* operator new(size_t size);
      void operator delete(void*, size_t);
      */
    private:
      Isolate();
      ~Isolate();
      static int runtime_count;

      // The Isolate owns a JSRuntime
      JSRuntime* mRuntime;
  };


  class APIEXPORT V8 {
    public:
      static bool Initialize();
      static bool Dispose();
      static bool IsDead();
      static const char* GetVersion() { return  version_string; };
      static int GetCurrentThreadId();
      /* TO IMPLEMENT:
    static void SetFatalErrorHandler(FatalErrorCallback that);
    static void SetAllowCodeGenerationFromStringsCallback(
    static void IgnoreOutOfMemoryException();
    static StartupData::CompressionAlgorithm GetCompressedStartupDataAlgorithm();
    static int GetCompressedStartupDataCount();
    static void GetCompressedStartupData(StartupData* compressed_data);
    static void SetDecompressedStartupData(StartupData* decompressed_data);
    static bool AddMessageListener(MessageCallback that,
                                   Handle<Value> data = Handle<Value>());
    static void RemoveMessageListeners(MessageCallback that);
    static void SetCaptureStackTraceForUncaughtExceptions(
        bool capture,
        int frame_limit = 10,
        StackTrace::StackTraceOptions options = StackTrace::kOverview);
    static void SetFlagsFromString(const char* str, int length);
    static void SetFlagsFromCommandLine(int* argc,
                                        char** argv,
                                        bool remove_flags);
    static void SetCounterFunction(CounterLookupCallback);
    static void SetCreateHistogramFunction(CreateHistogramCallback);
    static void SetAddHistogramSampleFunction(AddHistogramSampleCallback);
    static void EnableSlidingStateWindow();
    static void SetFailedAccessCheckCallbackFunction(FailedAccessCheckCallback);
    static void AddGCPrologueCallback(
        GCPrologueCallback callback, GCType gc_type_filter = kGCTypeAll);
    static void RemoveGCPrologueCallback(GCPrologueCallback callback);
    static void SetGlobalGCPrologueCallback(GCCallback);
    static void AddGCEpilogueCallback(
        GCEpilogueCallback callback, GCType gc_type_filter = kGCTypeAll);
    static void RemoveGCEpilogueCallback(GCEpilogueCallback callback);
    static void AddMemoryAllocationCallback(MemoryAllocationCallback callback,
                                            ObjectSpace space,
                                            AllocationAction action);
    static void RemoveMemoryAllocationCallback(MemoryAllocationCallback callback);
    static void AddCallCompletedCallback(CallCompletedCallback callback);
    static void RemoveCallCompletedCallback(CallCompletedCallback callback);
    static void AddObjectGroup(Persistent<Value>* objects,
                               size_t length,
                               RetainedObjectInfo* info = NULL);
    static void AddImplicitReferences(Persistent<Object> parent,
                                      Persistent<Value>* children,
                                      size_t length);
    static void SetEntropySource(EntropySource source);
    static void SetReturnAddressLocationResolver(
        ReturnAddressLocationResolver return_address_resolver);
    static bool SetFunctionEntryHook(FunctionEntryHook entry_hook);
    static void SetJitCodeEventHandler(JitCodeEventOptions options,
                                       JitCodeEventHandler event_handler);
    static intptr_t AdjustAmountOfExternalAllocatedMemory(
        intptr_t change_in_bytes);
    static void PauseProfiler();
    static void ResumeProfiler();
    static bool IsProfilerPaused();
    static void TerminateExecution(int thread_id);
    static void TerminateExecution(Isolate* isolate = NULL);
    static bool IsExecutionTerminating(Isolate* isolate = NULL);
    static void GetHeapStatistics(HeapStatistics* heap_statistics);
    static void VisitExternalResources(ExternalResourceVisitor* visitor);
    static void VisitHandlesWithClassIds(PersistentHandleVisitor* visitor);
    static bool IdleNotification(int hint = 1000);
    static void LowMemoryNotification();
    static int ContextDisposedNotification();
   private:
    V8();
    static internal::Object** GlobalizeReference(internal::Object** handle);
    static void DisposeGlobal(internal::Object** global_handle);
    static void MakeWeak(internal::Object** global_handle,
                         void* data,
                         WeakReferenceCallback);
    static void ClearWeak(internal::Object** global_handle);
    static void MarkIndependent(internal::Object** global_handle);
    static bool IsGlobalIndependent(internal::Object** global_handle);
    static bool IsGlobalNearDeath(internal::Object** global_handle);
    static bool IsGlobalWeak(internal::Object** global_handle);
    static void SetWrapperClassId(internal::Object** global_handle,
                                  uint16_t class_id);
    static uint16_t GetWrapperClassId(internal::Object** global_handle);

    template <class T> friend class Handle;
    template <class T> friend class Local;
    template <class T> friend class Persistent;
    friend class Context;
    */
    private:
      // This is just a utility class, so should not be constructible
      V8();
      static const char* version_string;
  };
}

#endif
