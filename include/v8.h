#ifndef V8MONKEY_V8_H
#define V8MONKEY_V8_H


#include <stddef.h>
#include <stdint.h>

#include <stdio.h>

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


  template<class T> class Persistent;
  class Value;
  class Number;
  class Integer;
  class Int32;
  class Uint32;
  class Isolate;
  class V8;


  typedef void (*WeakReferenceCallback)(Persistent<Value> object, void* parameter);


  template <class T> class Handle {
    public:
      inline Handle() : val(0) {}

      inline explicit Handle(T* v) : val(v) {}

      template <class S> inline Handle(Handle<S> that)
          : val(reinterpret_cast<T*>(*that)) {}

      inline bool IsEmpty() const { return val == 0; }

      inline void Clear() { val = 0; }

      inline T* operator->() const { return val; }

      inline T* operator*() const { return val; }

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

    protected:
      T* val;
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


  template <class T> class Persistent : public Handle<T> {
    public:
      inline Persistent() {}

      template <class S> inline Persistent(Persistent<S> that);

      template <class S> inline Persistent(S* that);

      template <class S> explicit inline Persistent(Handle<S> that);

      // The copy-constructor and assignment operators don't appear in the original V8 header, but are required for
      // ensuring the correctness of our ref-counting mechanism
      inline Persistent(const Persistent<T>& that);

      inline Persistent<T>& operator=(const Persistent<T>& that);

      template <class S> inline Persistent<T>& operator=(const Persistent<S>& that);

      template <class S> inline Persistent<T>& operator=(const Handle<S>& that);

      template <class S> inline Persistent<T>& operator=(const S* that);

      ~Persistent() {
        Dispose();
      }
      
      template <class S> static inline Persistent<T> Cast(Persistent<S> that) {
        return Persistent<T>(T::Cast(*that));
      }

      template <class S> inline Persistent<S> As() {
        return Persistent<S>::Cast(*this);
      }

      inline static Persistent<T> New(Handle<T> that);

      inline void Dispose();

      inline void MakeWeak(void* parameters, WeakReferenceCallback callback);

      inline void ClearWeak();

      inline void MarkIndependent() {
        // TODO Object groups not implemented
      }

      inline bool IsIndependent() const {
        // TODO Object groups not implemented
        return false;
      }

      inline bool IsNearDeath() const;

      inline bool IsWeak() const;

      inline void SetWrapperClassId(uint16_t class_id) {
        // TODO Profiling not implemented
      }

      inline uint16_t WrapperClassId() const {
        // TODO Profiling not implemented
        return 0;
      }

    private:
      // XXX Needed?
      friend class V8Monkey::V8MonkeyObject;

      // XXX Needed?
      friend class ImplementationUtilities;
      friend class ObjectTemplate;
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

      void Leave();

      V8Monkey::InternalIsolate* isolate;

      V8Monkey::V8MonkeyObject** prevNext;

      V8Monkey::V8MonkeyObject** prevLimit;

      bool isClosed;

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



  class APIEXPORT Value : public Data {
    public:
      bool IsUndefined() const;

      bool IsNull() const;

      bool IsTrue() const;

      bool IsFalse() const;

      bool IsString() const;

      bool IsFunction() const;

      bool IsArray() const;

      bool IsObject() const;

      bool IsBoolean() const;

      bool IsNumber() const;

      bool IsExternal() const;

      bool IsInt32() const;

      bool IsUint32() const;

      bool IsDate() const;

      bool IsBooleanObject() const;

      bool IsNumberObject() const;

      bool IsStringObject() const;

      bool IsNativeError() const;

      bool IsRegExp() const;

/*
      Local<Boolean> ToBoolean() const;
*/

      Local<Number> ToNumber() const;

/*
      Local<String> ToString() const;

      Local<String> ToDetailString() const;

      Local<Object> ToObject() const;
*/

      Local<Integer> ToInteger() const;

      Local<Uint32> ToUint32() const;

      Local<Int32> ToInt32() const;

      Local<Uint32> ToArrayIndex() const;

/*
      bool BooleanValue() const;

      double NumberValue() const;

      int64_t IntegerValue() const;

      uint32_t Uint32Value() const;

      int32_t Int32Value() const;

      bool Equals(Handle<Value> that) const;

      bool StrictEquals(Handle<Value> that) const;
*/

    private:
      // XXX Needed?
      inline bool QuickIsUndefined() const;

      // XXX Needed?
      inline bool QuickIsNull() const;

      // XXX Needed?
      inline bool QuickIsString() const;

      // XXX Needed?
      bool FullIsUndefined() const;

      // XXX Needed?
      bool FullIsNull() const;

      // XXX Needed?
      bool FullIsString() const;
  };


  class Primitive : public Value {};


/*
  class APIEXPORT Boolean : public Primitive {
    public:
      bool Value() const;

      static inline Handle<Boolean> New(bool value);
  };
*/


  class APIEXPORT Number : public Primitive {
    public:
      double Value() const;

      static Local<Number> New(double value);

      static inline Number* Cast(v8::Value* obj);

    private:
      Number();

      // XXX Needed?
      static void CheckCast(v8::Value* obj);
  };


  class APIEXPORT Integer : public Number {
    public:
      static Local<Integer> New(int32_t value);

      static Local<Integer> NewFromUnsigned(uint32_t value);

      static Local<Integer> New(int32_t value, Isolate*);

      static Local<Integer> NewFromUnsigned(uint32_t value, Isolate*);

      int64_t Value() const;

      static inline Integer* Cast(v8::Value* obj);
    private:
      Integer();

      // XXX Needed?
      static void CheckCast(v8::Value* obj);
  };


  class Int32 : public Integer {
    public:
      int32_t Value() const;
    private:
      Int32();
  };


  class APIEXPORT Uint32 : public Integer {
    public:
      uint32_t Value() const;
    private:
      Uint32();
  };


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
      friend class Context;
      */
      private:
        // This is just a utility class, so should not be constructible
        V8();

        static V8Monkey::V8MonkeyObject** MakePersistent(V8Monkey::V8MonkeyObject** val);

        static void DeletePersistent(V8Monkey::V8MonkeyObject** objSlot, V8Monkey::V8MonkeyObject* obj);

        static bool IsPersistentWeak(V8Monkey::V8MonkeyObject** objSlot, V8Monkey::V8MonkeyObject* obj);

        static bool IsPersistentNearDeath(V8Monkey::V8MonkeyObject* obj);

        static void MakePersistentWeak(V8Monkey::V8MonkeyObject** objSlot, V8Monkey::V8MonkeyObject* obj,
                                       void* parameters, WeakReferenceCallback callback);

        static void ClearPersistentWeakness(V8Monkey::V8MonkeyObject** objSlot, V8Monkey::V8MonkeyObject* obj);

        static const char* version_string;

        template <class T> friend class Persistent;
  };


  template<class T>
  Local<T> Local<T>::New(Handle<T> that) {
    if (that.IsEmpty()) {
      return Local<T>();
    }

    V8Monkey::V8MonkeyObject** internalObj = reinterpret_cast<V8Monkey::V8MonkeyObject**>(*that);
    return Local<T>(reinterpret_cast<T*>(HandleScope::CreateHandle(*internalObj)));
  }


  template<class T>
  Local<T> HandleScope::Close(Handle<T> value) {
    return Local<T>(reinterpret_cast<T*>(InternalClose(reinterpret_cast<V8Monkey::V8MonkeyObject**>(*value))));
  }


  template <class T> Persistent<T>::Persistent(const Persistent<T>& that) : Handle<T>() {
    V8Monkey::V8MonkeyObject** obj = V8::MakePersistent(reinterpret_cast<V8Monkey::V8MonkeyObject**>(*that));
    this->val = reinterpret_cast<T*>(obj);
  }


  template <class T> template <class S> Persistent<T>::Persistent(Persistent<S> that) : Handle<T>() {
    V8Monkey::V8MonkeyObject** obj = V8::MakePersistent(reinterpret_cast<V8Monkey::V8MonkeyObject**>(*that));
    this->val = reinterpret_cast<T*>(obj);
  }


  template <class T> template <class S> Persistent<T>::Persistent(S* that) : Handle<T>() {
    V8Monkey::V8MonkeyObject** obj = V8::MakePersistent(reinterpret_cast<V8Monkey::V8MonkeyObject**>(that));
    this->val = reinterpret_cast<T*>(obj);
  }


  template <class T> template <class S> Persistent<T>::Persistent(Handle<S> that) : Handle<T>() {
    V8Monkey::V8MonkeyObject** obj = V8::MakePersistent(reinterpret_cast<V8Monkey::V8MonkeyObject**>(*that));
    this->val = reinterpret_cast<T*>(obj);
  }


  template <class T> Persistent<T>& Persistent<T>::operator=(const Persistent<T>& that) {
    // We need to be watchful for self-assignment here
    if (that == *this) {
      return *this;
    }

    printf("ASSIGNMENT FROM SAME TYPE OF PERSISTENT!\n");
    // Delete our old pointer
    Dispose();

    // Unwrap the other persistent
    V8Monkey::V8MonkeyObject** slot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(that.val);
    V8Monkey::V8MonkeyObject** newSlot = V8::MakePersistent(slot);

    this->val = reinterpret_cast<T*>(newSlot);
    return *this;
  }


  template <class T> template <class S> Persistent<T>& Persistent<T>::operator=(const Persistent<S>& that) {
    printf("ASSIGNMENT FROM OTHER PERSISTENT!\n");
    // Delete our old pointer
    Dispose();

    // Unwrap the other persistent
    V8Monkey::V8MonkeyObject** slot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(*that);
    V8Monkey::V8MonkeyObject** newSlot = V8::MakePersistent(slot);

    this->val = reinterpret_cast<T*>(newSlot);
    return *this;
  }


  template <class T> template <class S> Persistent<T>& Persistent<T>::operator=(const Handle<S>& that) {
    printf("ASSIGNMENT FROM HANDLE!\n");
    // Delete our old pointer
    Dispose();

    // Unwrap the other handle
    V8Monkey::V8MonkeyObject** slot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(*that);
    V8Monkey::V8MonkeyObject** newSlot = V8::MakePersistent(slot);

    this->val = reinterpret_cast<T*>(newSlot);
    return *this;
  }


  template <class T> template <class S> Persistent<T>& Persistent<T>::operator=(const S* that) {
    // Cast away the const (we need to get at the underlying object) and get the underlying object
    S* v8Obj = const_cast<S*>(that); 
    V8Monkey::V8MonkeyObject** slot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(v8Obj);

    // Watch out for self-assignment!
    V8Monkey::V8MonkeyObject** currentSlot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(this->val);
    if (slot == currentSlot) {
      return *this;
    }

    printf("ASSIGNMENT FROM OTHER POINTER!\n");
    // Delete our old pointer
    Dispose();

    V8Monkey::V8MonkeyObject** newSlot = V8::MakePersistent(slot);

    this->val = reinterpret_cast<T*>(newSlot);
    return *this;
  }


  template <class T> Persistent<T> Persistent<T>::New(Handle<T> that) {
    return Persistent(that);
  }


  template <class T> void Persistent<T>::Dispose() {
    V8Monkey::V8MonkeyObject** objSlot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(this->val);

    if (this->val == nullptr || *objSlot == nullptr) {
      return;
    }

    V8::DeletePersistent(objSlot, *objSlot);
  }


  template <class T> void Persistent<T>::MakeWeak(void* parameters, WeakReferenceCallback callback) {
    V8Monkey::V8MonkeyObject** objSlot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(this->val);
    if (this->val == nullptr || objSlot == nullptr) {
      return;
    }

    V8::MakePersistentWeak(objSlot, *objSlot, parameters, callback);
  }


  template <class T> bool Persistent<T>::IsWeak() const {
    V8Monkey::V8MonkeyObject** objSlot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(this->val);
    if (this->val == nullptr || *objSlot == nullptr) {
      return false;
    }

    return V8::IsPersistentWeak(objSlot, *objSlot);
  }


  template <class T> bool Persistent<T>::IsNearDeath() const {
    V8Monkey::V8MonkeyObject** objSlot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(this->val);
    if (this->val == nullptr || *objSlot == nullptr) {
      return false;
    }

    return V8::IsPersistentNearDeath(*objSlot);
  }


  template <class T> void Persistent<T>::ClearWeak() {
    V8Monkey::V8MonkeyObject** objSlot = reinterpret_cast<V8Monkey::V8MonkeyObject**>(this->val);
    if (this->val == nullptr || *objSlot == nullptr) {
      return;
    }

    V8::ClearPersistentWeakness(objSlot, *objSlot);
  }


  Number* Number::Cast(v8::Value* obj) {
    return static_cast<Number*>(obj);
  }


  Integer* Integer::Cast(v8::Value* obj) {
    return static_cast<Integer*>(obj);
  }
}

#endif
