#ifndef V8MONKEY_V8_H
#define V8MONKEY_V8_H


#include <stddef.h>


#ifndef APIEXPORT
  #define APIEXPORT __attribute__ ((visibility("default")))
#endif



// V8 API Compatibility
#define V8COMPAT "3.14.5.9"


// XXX Make order correct
namespace v8 {
  namespace V8Monkey {
    class V8MonkeyObject;
    class InternalIsolate;
  }


  template <class T> class Handle {
    public:
      inline Handle() : val_(0) {}

      inline explicit Handle(T* val) : val_(val) {}

      template <class S> inline Handle(Handle<S> that)
          : val_(reinterpret_cast<T*>(*that)) {}

      inline bool IsEmpty() const { return val_ == 0; }

      inline void Clear() { val_ = 0; }

      inline T* operator->() const { return val_; }

      inline T* operator*() const { return val_; }

      template <class S> inline bool operator==(Handle<S> that) const {
        V8Monkey::V8MonkeyObject** a = reinterpret_cast<V8Monkey::V8MonkeyObject**>(**this);
        V8Monkey::V8MonkeyObject** b = reinterpret_cast<V8Monkey::V8MonkeyObject**>(*that);
        if (a == 0) return b == 0;
        if (b == 0) return false;
        return *a == *b;
      }

      template <class S> inline bool operator!=(Handle<S> that) const {
        return !operator==(that);
      }

      template <class S> static inline Handle<T> Cast(Handle<S> that) {
        return Handle<T>(T::Cast(*that));
      }

      template <class S> inline Handle<S> As() {
        return Handle<S>::Cast(*this);
      }

    private:
      T* val_;
  };


  template <class T> class Local : public Handle<T> {
    public:
      inline Local() {}
      template <class S> inline Local(Local<S> that)
          : Handle<T>(reinterpret_cast<T*>(*that)) {}

      template <class S> inline Local(S* that) : Handle<T>(that) { }

      template <class S> static inline Local<T> Cast(Local<S> that) {
        return Local<T>(T::Cast(*that));
      }

      template <class S> inline Local<S> As() {
        return Local<S>::Cast(*this);
      }

      inline static Local<T> New(Handle<T> that);
  };


  class APIEXPORT HandleScope {
    public:
      HandleScope();

      ~HandleScope();

      template <class T> Local<T> Close(Handle<T> value);

      static int NumberOfHandles();

      static V8Monkey::V8MonkeyObject** CreateHandle(V8Monkey::V8MonkeyObject* value);

    private:
      HandleScope(const HandleScope&);

      void operator=(const HandleScope&);

      void* operator new(size_t size);

      void operator delete(void*, size_t);

      // XXX Needed?
      void Leave();

      V8Monkey::InternalIsolate* isolate;

      V8Monkey::V8MonkeyObject** prev_next;

      V8Monkey::V8MonkeyObject** prev_limit;

      // XXX Needed ?
      bool is_closed_;

      V8Monkey::V8MonkeyObject** InternalClose(V8Monkey::V8MonkeyObject** value);
  };


  class APIEXPORT Data {
    private:
      Data() {}
  };



    /*
      class V8EXPORT ScriptData {
      public:
        virtual ~ScriptData() { }

        static ScriptData* PreCompile(const char* input, int length);

        static ScriptData* PreCompile(Handle<String> source);

        static ScriptData* New(const char* data, int length);

        virtual int Length() = 0;

        virtual const char* Data() = 0;

        virtual bool HasError() = 0;
    };


    class ScriptOrigin {
      public:
        inline ScriptOrigin(
            Handle<Value> resource_name,
            Handle<Integer> resource_line_offset = Handle<Integer>(),
            Handle<Integer> resource_column_offset = Handle<Integer>())
            : resource_name_(resource_name),
              resource_line_offset_(resource_line_offset),
              resource_column_offset_(resource_column_offset) { }
        inline Handle<Value> ResourceName() const;
        inline Handle<Integer> ResourceLineOffset() const;
        inline Handle<Integer> ResourceColumnOffset() const;
       private:
        Handle<Value> resource_name_;
        Handle<Integer> resource_line_offset_;
        Handle<Integer> resource_column_offset_;
    };


    class V8EXPORT Script {
      public:
        static Local<Script> New(Handle<String> source,
                                 ScriptOrigin* origin = NULL,
                                 ScriptData* pre_data = NULL,
                                 Handle<String> script_data = Handle<String>());

        static Local<Script> New(Handle<String> source,
                                 Handle<Value> file_name);

        static Local<Script> Compile(Handle<String> source,
                                     ScriptOrigin* origin = NULL,
                                     ScriptData* pre_data = NULL,
                                     Handle<String> script_data = Handle<String>());

        static Local<Script> Compile(Handle<String> source,
                                     Handle<Value> file_name,
                                     Handle<String> script_data = Handle<String>());

        Local<Value> Run();

        Local<Value> Id();

        void SetData(Handle<String> data);
    };


    class V8EXPORT Message {
       public:
        Local<String> Get() const;

        Local<String> GetSourceLine() const;

        Handle<Value> GetScriptResourceName() const;

        Handle<Value> GetScriptData() const;

        Handle<StackTrace> GetStackTrace() const;

        int GetLineNumber() const;

        int GetStartPosition() const;

        int GetEndPosition() const;

        int GetStartColumn() const;

        int GetEndColumn() const;

        static void PrintCurrentStackTrace(FILE* out);

        static const int kNoLineNumberInfo = 0;

        static const int kNoColumnInfo = 0;
    };


    class V8EXPORT StackTrace {
      public:
        enum StackTraceOptions {
          kLineNumber = 1,
          kColumnOffset = 1 << 1 | kLineNumber,
          kScriptName = 1 << 2,
          kFunctionName = 1 << 3,
          kIsEval = 1 << 4,
          kIsConstructor = 1 << 5,
          kScriptNameOrSourceURL = 1 << 6,
          kOverview = kLineNumber | kColumnOffset | kScriptName | kFunctionName,
          kDetailed = kOverview | kIsEval | kIsConstructor | kScriptNameOrSourceURL
      };

      Local<StackFrame> GetFrame(uint32_t index) const;

      int GetFrameCount() const;

      Local<Array> AsArray();

      static Local<StackTrace> CurrentStackTrace(
          int frame_limit,
          StackTraceOptions options = kOverview);
    };


    class V8EXPORT StackFrame {
      public:
        int GetLineNumber() const;

        int GetColumn() const;

        Local<String> GetScriptName() const;

        Local<String> GetScriptNameOrSourceURL() const;

        Local<String> GetFunctionName() const;

        bool IsEval() const;

        bool IsConstructor() const;
    };
    */



/*
  class Value : public Data {
    public:
      inline bool IsUndefined() const;

      inline bool IsNull() const;

      V8EXPORT bool IsTrue() const;

      V8EXPORT bool IsFalse() const;

      inline bool IsString() const;

      V8EXPORT bool IsFunction() const;

      V8EXPORT bool IsArray() const;

      V8EXPORT bool IsObject() const;

      V8EXPORT bool IsBoolean() const;

      V8EXPORT bool IsNumber() const;

      V8EXPORT bool IsExternal() const;

      V8EXPORT bool IsInt32() const;

      V8EXPORT bool IsUint32() const;

      V8EXPORT bool IsDate() const;

      V8EXPORT bool IsBooleanObject() const;

      V8EXPORT bool IsNumberObject() const;

      V8EXPORT bool IsStringObject() const;

      V8EXPORT bool IsNativeError() const;

      V8EXPORT bool IsRegExp() const;

      V8EXPORT Local<Boolean> ToBoolean() const;

      V8EXPORT Local<Number> ToNumber() const;

      V8EXPORT Local<String> ToString() const;

      V8EXPORT Local<String> ToDetailString() const;

      V8EXPORT Local<Object> ToObject() const;

      V8EXPORT Local<Integer> ToInteger() const;

      V8EXPORT Local<Uint32> ToUint32() const;

      V8EXPORT Local<Int32> ToInt32() const;

      V8EXPORT Local<Uint32> ToArrayIndex() const;

      V8EXPORT bool BooleanValue() const;

      V8EXPORT double NumberValue() const;

      V8EXPORT int64_t IntegerValue() const;

      V8EXPORT uint32_t Uint32Value() const;

      V8EXPORT int32_t Int32Value() const;

      V8EXPORT bool Equals(Handle<Value> that) const;

      V8EXPORT bool StrictEquals(Handle<Value> that) const;

    private:
      // XXX Needed?
      inline bool QuickIsUndefined() const;

      // XXX Needed?
      inline bool QuickIsNull() const;

      // XXX Needed?
      inline bool QuickIsString() const;

      // XXX Needed?
      V8EXPORT bool FullIsUndefined() const;

      // XXX Needed?
      V8EXPORT bool FullIsNull() const;

      // XXX Needed?
      V8EXPORT bool FullIsString() const;
  };


  class Primitive : public Value {};


  class APIEXPORT Boolean : public Primitive {
    public:
      bool Value() const;

      static inline Handle<Boolean> New(bool value);
  };
*/


  /*
    class Number : public Primitive {
      public:
        V8EXPORT double Value() const;
        V8EXPORT static Local<Number> New(double value);
        static inline Number* Cast(v8::Value* obj);
      private:
        V8EXPORT Number();
        V8EXPORT static void CheckCast(v8::Value* obj);
    };


    class Integer : public Number {
      public:
        V8EXPORT static Local<Integer> New(int32_t value);
        V8EXPORT static Local<Integer> NewFromUnsigned(uint32_t value);
        V8EXPORT static Local<Integer> New(int32_t value, Isolate*);
        V8EXPORT static Local<Integer> NewFromUnsigned(uint32_t value, Isolate*);
        V8EXPORT int64_t Value() const;
        static inline Integer* Cast(v8::Value* obj);
      private:
        V8EXPORT Integer();
        V8EXPORT static void CheckCast(v8::Value* obj);
    };


    class Int32 : public Integer {
      public:
        V8EXPORT int32_t Value() const;
      private:
        V8EXPORT Int32();
    };


    class Uint32 : public Integer {
      public:
        V8EXPORT uint32_t Value() const;
      private:
        V8EXPORT Uint32();
    };
  */

  class APIEXPORT Isolate {
    public:
      class APIEXPORT Scope {
        public:
          explicit Scope(Isolate* isolate) : isolate(isolate) {
            isolate->Enter();
          }

          ~Scope() { isolate->Exit(); }

        private:
          Isolate* const isolate;

          Scope(const Scope&);

          Scope& operator=(const Scope&);
      };

      static Isolate* New();

      static Isolate* GetCurrent();

      void Enter();

      void Exit();

      void Dispose();

      void SetData(void* data);

      void* GetData();

    private:
      Isolate();

      Isolate(const Isolate&);

      ~Isolate();

      Isolate& operator=(const Isolate&);

      void* operator new(size_t);

      void operator delete(void*, size_t);
  };


  class APIEXPORT Unlocker {
    public:
      explicit Unlocker(Isolate* isolate = NULL);

      ~Unlocker();

    private:
      Isolate* isolate;
  };


  class APIEXPORT Locker {
    public:
      explicit Locker(Isolate* isolate = NULL);

      ~Locker();

      static void StartPreemption(int every_n_ms) {
        // TODO: Not implemented
      }

      static void StopPreemption() {
        // TODO Not implemented
      }

      static bool IsLocked(Isolate* isolate = NULL);

      static bool IsActive();

    private:
      bool hasLock;

      // XXX Remove?
      //bool top_level_;

      Isolate* isolate;

      static bool active;

      Locker(const Locker&);

      void operator=(const Locker&);
  };


  typedef void (*FatalErrorCallback)(const char* location, const char* message);


  class APIEXPORT V8 {
    public:
      static bool Initialize();
      static bool Dispose();
      static bool IsDead();
      static const char* GetVersion() { return  version_string; };
      static int GetCurrentThreadId();
      static void SetFatalErrorHandler(FatalErrorCallback that);
      /* TO IMPLEMENT:
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


  template<class T>
  Local<T> HandleScope::Close(Handle<T> value) {
    // XXX FIXME
    return Local<T>();
    //return InternalClose(reinterpret_cast<V8Monkey::V8MonkeyObject**>(*value));
  }

}

#endif
