/*
#ifndef V8MONKEY_BASETYPES_H
#define V8MONKEY_BASETYPES_H

// atomic_uint, atomic_fetch_{add,subtract}
#include <atomic>

// is_base_of
#include <type_traits>

// GetJSRuntimeForThread
#include "utils/SpiderMonkeyUtils.h"

// EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"

// Value
#include "v8.h"


struct JSRuntime;
class JSTracer;


namespace v8 {
  namespace DataStructures {
    template <unsigned int SlabSize = 1024u>
    class ObjectBlock;
  }


  namespace internal {
*/


    /*
     * It is assumed that objects are stored in an ObjectBlock, in shared_ptrs. Thus, whenever a handle is
     * dereferenced, instead of an internal::Object**, what one actually has is a shared_ptr<Object>*, cast
     * to internal::Object** to fit V8 API requirements. The function templates below deal with getting useful types
     * out of those wrapped values.
     *
     */

    //Object
    //using 
    /*
     * The base class of all internal refcounted objects (i.e. anything that can be stored in a Local or Persistent
     * handle).
     *
* XXX Check the below at end of reinstatement, see if it is still true
     * As noted in the comments in persistent.cpp, there is some machinery here to implement weak handles for
     * for Persistents, which ultimately can only be implemented when SpiderMonkey root tracing occurs. The key point
     * is that all derived classes must call ShouldTrace in their Trace implementation. If they participate in
     * SpiderMonkey root tracing, they should only call the relevant SpiderMonkey tracing method if ShouldTrace returns
     * true. If ShouldTrace returns false, callers should assume the object has been destructed.
     *
     */

/*
    class EXPORT_FOR_TESTING_ONLY Object {
      public:
        Object() : refCount {0}, weakCount{0}, owningRuntime {::v8::SpiderMonkey::GetJSRuntimeForThread()},
                   callbackList {nullptr} {}

        virtual ~Object() {}
*/

        /*
         * Bumps this object's strong reference count.
         *
         */

/*
        void AddRef() {
          std::atomic_fetch_add(&refCount, 1u);
        }
*/

        /*
         * Decrements this object's strong reference count. Deletes the object if the decremented refcount is zero.
         *
         * The parameter anticipates the future reestablishment of Persistent support.
         *
         */

/*
        void Release(Object**) {
          std::atomic_fetch_sub(&refCount, 1u);

          if (refCount == 0) {
            delete this;
          }
        }
*/

        // Support for weak Persistent handles. See persistent.cpp/v8monkeyobject.cpp for details
//        void MakeWeak(V8MonkeyObject** slotPtr, void* parameters, WeakReferenceCallback callback);
//        void ClearWeakness(V8MonkeyObject** slotPtr);
//        bool IsWeak(V8MonkeyObject** slotPtr);
//        bool IsNearDeath();
//        void PersistentRelease(V8MonkeyObject** slotPtr);

        // Called when the SpiderMonkey garbage collector requests a trace
/*
        void Trace(JSRuntime* runtime, JSTracer* tracer) {
          // Watch out for cases where a different runtime is being traced
          if (!ignoreRuntime && (runtime != owningRuntime)) {
            return;
          }

          // XXX Note: we may need some mechanism for custom semantics (i.e. shouldTrace might be false, but the
          //           value might be an external string that we cannot delete without SpiderMonkey's sayso.
          //           Also, will depend on how we do iteration. There may be a smartptr keeping us alive.
          if (ShouldTrace()) {
            DoTrace(runtime, tracer);
          }
        }

        #ifdef V8MONKEY_INTERNAL_TEST
        unsigned int RefCount() { return refCount; }

        int WeakCount() { return weakCount; }
        #endif

        Object(const Object& other) = delete;
        Object(Object&& other) = delete;
        Object& operator=(const Object& other) = delete;
        Object& operator=(Object&& other) = delete;

        using ObjectContainer = ::v8::DataStructures::ObjectBlock<>;

      protected:
        bool ignoreRuntime {false};

      private:
        std::atomic_uint refCount {0};
        int weakCount {0};
//        bool isNearDeath;
//
//        struct WeakRefListElem {
//          V8MonkeyObject** slotPtr;
//          void* params;
//          WeakReferenceCallback callback;
//          WeakRefListElem* prev;
//          WeakRefListElem* next;
//        };
//
//        WeakRefListElem* callbackList;
        JSRuntime* owningRuntime {nullptr};

        void* callbackList {nullptr};

        bool ShouldTrace() { return true; }

        virtual void DoTrace(JSRuntime*, JSTracer*) = 0;
//
//        // Find the callback information for the given slot
//        WeakRefListElem* FindForSlot(V8MonkeyObject** slotPtr);
//
//        // Remove the given callback list data
//        void RemoveFromWeakList(WeakRefListElem* cbListElem);
//
//        // Decrement one of the refcounters
//        void RemoveRef(V8MonkeyObject** slotPtr, int* counter);
//
//        // Used for isolate teardown
//        void IsolateRelease();
    };


    using ObjectContainer = ::v8::DataStructures::ObjectBlock<>;
*/

    /*
     *  The base class of all objects that implement value types for the V8 API
     *
     */

/*
    class V8Value: public Object {
      public:
        V8Value() {}
        virtual ~V8Value() {}

        virtual bool IsUndefined() const { return false; }
        virtual bool IsNull() const { return false; }
        virtual bool IsTrue() const { return false; }
        virtual bool IsFalse() const { return false; }
        virtual bool IsString() const { return false; }
        virtual bool IsSymbol() const { return false; }
        virtual bool IsFunction() const { return false; }
        virtual bool IsArray() const { return false; }
        virtual bool IsObject() const { return false; }
        virtual bool IsBoolean() const { return false; }
        virtual bool IsNumber() const { return false; }
        virtual bool IsExternal() const { return false; }
        virtual bool IsInt32() const { return false; }
        virtual bool IsUint32() const { return false; }
        virtual bool IsDate() const { return false; }
        virtual bool IsBooleanObject() const { return false; }
        virtual bool IsNumberObject() const { return false; }
        virtual bool IsStringObject() const { return false; }
        virtual bool IsSymbolObject() const { return false; }
        virtual bool IsNativeError() const { return false; }
        virtual bool IsRegExp() const { return false; }
        virtual bool IsPromise() const { return false; }
        virtual bool IsArrayBuffer() const { return false; }
        virtual bool IsArrayBufferView() const { return false; }
        virtual bool IsTypedArray() const { return false; }
        virtual bool IsUint8Array() const { return false; }
        virtual bool IsUint8ClampedArray() const { return false; }
        virtual bool IsInt8Array() const { return false; }
        virtual bool IsUint16Array() const { return false; }
        virtual bool IsInt16Array() const { return false; }
        virtual bool IsUint32Array() const { return false; }
        virtual bool IsInt32Array() const { return false; }
        virtual bool IsFloat32Array() const { return false; }
        virtual bool IsFloat64Array() const { return false; }
        virtual bool IsDataView() const { return false; }

        virtual bool BooleanValue() const = 0;
//        virtual double NumberValue() const = 0;
//        virtual int64_t IntegerValue() const = 0;
//        virtual uint32_t Uint32Value() const = 0;
//        virtual int32_t Int32Value() const = 0;

////        template <typename T, typename U>
////        static T* ConvertFromAPI(const U* val) {
////          static_assert(std::is_base_of<V8Value, T>::value, "ConvertFromAPI target is not a V8Value type");
////          static_assert(std::is_base_of<Value, U>::value, "ConvertFromAPI source is not a V8 API Value type");
////
////          return *(reinterpret_cast<T**>(const_cast<U*>(val)));
////        }

        V8Value(const V8Value& other) = delete;
        V8Value(V8Value&& other) = delete;
        V8Value& operator=(const V8Value& other) = delete;
        V8Value& operator=(V8Value&& other) = delete;
    };
*/


    /*
     * A dummy implementation of V8MonkeyObject for testing purposes
     *
     */

/*
    #ifdef V8MONKEY_INTERNAL_TEST
    class EXPORT_FOR_TESTING_ONLY DummyV8MonkeyObject : public Object {
      public:
        inline bool operator== (const DummyV8MonkeyObject &other) {
          return &other == this;
        }

        DummyV8MonkeyObject() = default;
        ~DummyV8MonkeyObject() = default;
        DummyV8MonkeyObject(const DummyV8MonkeyObject& other) = delete;
        DummyV8MonkeyObject(DummyV8MonkeyObject&& other) = delete;
        DummyV8MonkeyObject& operator=(const DummyV8MonkeyObject& other) = delete;
        DummyV8MonkeyObject& operator=(DummyV8MonkeyObject&& other) = delete;

      private:
        void DoTrace(JSRuntime*, JSTracer*) override {}
    };


    // Many tests seek to test when objects are deleted. To this end, we use special fake objects, that set a flag at
    // the given location if they were deleted
    class EXPORT_FOR_TESTING_ONLY DeletionObject : public Object {
      public:
        DeletionObject() : index {-1}, ptr {nullptr} {}
        DeletionObject(bool* boolPtr) : index(-1), ptr(boolPtr) {}

        DeletionObject(bool* boolPtr, int i) : index(i), ptr(boolPtr) {}

        ~DeletionObject() {
          if (ptr) {
            *ptr = true;
          }
        }

        // XXX What is this used for?!?
        int index;

        DeletionObject(const DeletionObject& other) = delete;
        DeletionObject(DeletionObject&& other) = delete;
        DeletionObject& operator=(const DeletionObject& other) = delete;
        DeletionObject& operator=(DeletionObject&& other) = delete;

      private:
        bool* ptr;

        void DoTrace(JSRuntime*, JSTracer*) override {}
    };


    // Some tests need to check that their objects are traced by the garbage collector. This dummy class can be used
    // for that purpose
    class EXPORT_FOR_TESTING_ONLY TraceFake : public Object {
      public:
        TraceFake(bool* boolPtr, int* traceCount = nullptr) : ptr(boolPtr), traceCountPtr {traceCount} {}

        ~TraceFake() = default;
        TraceFake(const TraceFake& other) = delete;
        TraceFake(TraceFake&& other) = delete;
        TraceFake& operator=(const TraceFake& other) = delete;
        TraceFake& operator=(TraceFake&& other) = delete;

      private:
        bool* ptr {nullptr};
        int* traceCountPtr {nullptr};

        void DoTrace(JSRuntime*, JSTracer*) override {
          *ptr = true;

          if (traceCountPtr) {
            (*traceCountPtr)++;
          }
        }
    };

    #endif
  }
}


#endif
*/
