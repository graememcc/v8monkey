#ifndef V8MONKEY_BASETYPES_H
#define V8MONKEY_BASETYPES_H

// ObjectBlock
#include "data_structures/objectblock.h"

// is_base_of
#include <type_traits>

// JSRuntime JSTracer
#include "jsapi.h"

// EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"

// Value
#include "v8.h"


namespace v8 {
  namespace internal {

    class Object;

    using ObjectContainer = ::v8::DataStructures::ObjectBlock<Object, Object*>;

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

    class EXPORT_FOR_TESTING_ONLY Object {
      public:
        Object() : weakCount{0}, callbackList(nullptr) {}

        virtual ~Object() {}

        // Bump the reference count
//        void AddRef() {
//          refCount++;
//        }
//
//        // Decrement the strong reference count. If both refcounts are zero, delete this object
//        void Release() {
//          RemoveRef(nullptr, &refCount);
//        }
//
        // Support for weak Persistent handles. See persistent.cpp/v8monkeyobject.cpp for details
//        void MakeWeak(V8MonkeyObject** slotPtr, void* parameters, WeakReferenceCallback callback);
//        void ClearWeakness(V8MonkeyObject** slotPtr);
//        bool IsWeak(V8MonkeyObject** slotPtr);
//        bool IsNearDeath();
//        void PersistentRelease(V8MonkeyObject** slotPtr);

        // Called when the SpiderMonkey garbage collector requests a trace
        void Trace(JSRuntime* runtime, JSTracer* tracer) {
          // XXX Note: we may need some mechanism for custom semantics (i.e. shouldTrace might be false, but the
          //           value might be an external string that we cannot delete without SpiderMonkey's sayso.
          //           Also, will depend on how we do iteration. There may be a smartptr keeping us alive.
          if (ShouldTrace()) {
            DoTrace(runtime, tracer);
          }
        }
//
//        #ifdef V8MONKEY_INTERNAL_TEST
//        int RefCount() { return refCount; }
//
//        int WeakCount() { return weakCount; }
//        #endif
//

        Object(const Object& other) = delete;
        Object(Object&& other) = delete;
        Object& operator=(const Object& other) = delete;
        Object& operator=(Object&& other) = delete;

      private:
//        int refCount;
        int weakCount;
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
        void* callbackList;

        bool ShouldTrace() { return false; }

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
//
//
//    /*
//     *  The base class of all objects that implement types for the V8 API
//     *
//     */
//    class V8Value: public V8MonkeyObject {
//      public:
//        virtual bool IsUndefined() const { return false; }
//        virtual bool IsNull() const { return false; }
//        virtual bool IsTrue() const { return false; }
//        virtual bool IsFalse() const { return false; }
//        virtual bool IsString() const { return false; }
//        virtual bool IsObject() const { return false; }
//        virtual bool IsBoolean() const { return false; }
//        virtual bool IsNumber() const { return false; }
//        virtual bool IsInt32() const { return false; }
//        virtual bool IsUint32() const { return false; }
//        virtual bool IsExternal() const { return false; }
//        virtual bool IsNativeError() const { return false; }
//        virtual bool IsFunction() const { return false; }
//        virtual bool IsArray() const { return false; }
//        virtual bool IsDate() const { return false; }
//        virtual bool IsBooleanObject() const { return false; }
//        virtual bool IsNumberObject() const { return false; }
//        virtual bool IsStringObject() const { return false; }
//        virtual bool IsRegExp() const { return false; }
//        /*
//        Local<Boolean> ToBoolean() const;
//
//        Local<Number> ToNumber() const;
//
//        Local<String> ToString() const;
//
//        Local<String> ToDetailString() const;
//
//        Local<Object> ToObject() const;
//
//        Local<Integer> ToInteger() const;
//
//        Local<Uint32> ToUint32() const;
//
//        Local<Int32> ToInt32() const;
//
//        Local<Uint32> ToArrayIndex() const;
//
//        bool BooleanValue() const;
//
//        double NumberValue() const;
//
//        int64_t IntegerValue() const;
//
//        uint32_t Uint32Value() const;
//
//        int32_t Int32Value() const;
//
//        bool Equals(Handle<Value> that) const;
//
//        bool StrictEquals(Handle<Value> that) const;
//        */
//
//        virtual void Trace(JSRuntime*, JSTracer*) {
//          // Need to call ShouldTrace for weak callbacks
//          ShouldTrace();
//        }
////
////        template <typename T, typename U>
////        static T* ConvertFromAPI(const U* val) {
////          static_assert(std::is_base_of<V8Value, T>::value, "ConvertFromAPI target is not a V8Value type");
////          static_assert(std::is_base_of<Value, U>::value, "ConvertFromAPI source is not a V8 API Value type");
////
////          return *(reinterpret_cast<T**>(const_cast<U*>(val)));
////        }
//    };
//
//
    /*
     * A dummy implementation of V8MonkeyObject for testing purposes
     *
     */

    #ifdef V8MONKEY_INTERNAL_TEST
    class EXPORT_FOR_TESTING_ONLY DummyV8MonkeyObject : public Object {
      public:
        inline bool operator== (const DummyV8MonkeyObject &other) {
          return &other == this;
        }

        DummyV8MonkeyObject() = default;
        ~DummyV8MonkeyObject() = default;
        DummyV8MonkeyObject(const DummyV8MonkeyObject& other) = default;
        DummyV8MonkeyObject(DummyV8MonkeyObject&& other) = default;
        DummyV8MonkeyObject& operator=(const DummyV8MonkeyObject& other) = default;
        DummyV8MonkeyObject& operator=(DummyV8MonkeyObject&& other) = default;

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

        DeletionObject(const DeletionObject& other) = default;
        DeletionObject(DeletionObject&& other) = default;
        DeletionObject& operator=(const DeletionObject& other) = default;
        DeletionObject& operator=(DeletionObject&& other) = default;

      private:
        bool* ptr;

        void DoTrace(JSRuntime*, JSTracer*) override {}
    };


    // Some tests need to check that their objects are traced by the garbage collector. This dummy class can be used
    // for that purpose
    class EXPORT_FOR_TESTING_ONLY TraceFake : public Object {
      public:
        TraceFake(bool* boolPtr) : ptr(boolPtr) {}

        ~TraceFake() = default;
        TraceFake(const TraceFake& other) = default;
        TraceFake(TraceFake&& other) = default;
        TraceFake& operator=(const TraceFake& other) = default;
        TraceFake& operator=(TraceFake&& other) = default;

      private:
        bool* ptr;

        void DoTrace(JSRuntime*, JSTracer*) override {
          *ptr = true;
        }
    };

    #endif
  }
}


#endif
