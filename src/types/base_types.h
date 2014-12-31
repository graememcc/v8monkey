#ifndef V8MONKEY_BASETYPES_H
#define V8MONKEY_BASETYPES_H

#include "v8.h"

#include "jsapi.h"

#include "test.h"


namespace v8 {
  namespace V8Monkey {

    /*
     * The base class of all internal refcounted objects (i.e. anything that can be stored in a Local or Persistent
     * handle).
     *
     */

    class EXPORT_FOR_TESTING_ONLY V8MonkeyObject {
      public:
        V8MonkeyObject() : refCount(0), weakCount(0), isNearDeath(false), callbackList(nullptr) {}

        virtual ~V8MonkeyObject();

        // Bump the reference count
        void AddRef() {
          refCount++;
        }

        // Decrement the strong reference count. If both refcounts are zero, delete this object
        void Release() {
          RemoveRef(nullptr, &refCount);
        }

        // Support for weak Persistent handles. See v8monkeyobject.cpp for details
        void MakeWeak(V8MonkeyObject** slotPtr, void* parameters, WeakReferenceCallback callback);
        void ClearWeakness(V8MonkeyObject** slotPtr);
        bool IsWeak(V8MonkeyObject** slotPtr);
        bool IsNearDeath();
        void PersistentRelease(V8MonkeyObject** slotPtr);
        bool ShouldTrace();

        // Called when the SpiderMonkey garbage collector requests a trace
        virtual void Trace(JSRuntime* runtime, JSTracer* tracer) = 0;

        #ifdef V8MONKEY_INTERNAL_TEST
        int RefCount() { return refCount; }

        int WeakCount() { return weakCount; }
        #endif

      private:
        int refCount;
        int weakCount;
        bool isNearDeath;

        struct WeakRefListElem {
          V8MonkeyObject** slotPtr;
          void* params;
          WeakReferenceCallback callback;
          WeakRefListElem* prev;
          WeakRefListElem* next;
        };

        WeakRefListElem* callbackList;

        // Find the callback information for the given slot
        WeakRefListElem* FindForSlot(V8MonkeyObject** slotPtr);

        // Remove the given callback list data
        void RemoveFromWeakList(WeakRefListElem* cbListElem);

        // Decrement one of the refcounters
        void RemoveRef(V8MonkeyObject** slotPtr, int* counter);

        // Used for isolate teardown
        void IsolateRelease();
    };


    /*
     *  The base class of all objects that implement types for the V8 API
     *
     */
    class V8Value: public V8MonkeyObject {
      public:
        virtual bool IsUndefined() const { return false; }
        virtual bool IsNull() const { return false; }
        virtual bool IsTrue() const { return false; }
        virtual bool IsFalse() const { return false; }
        virtual bool IsString() const { return false; }
        virtual bool IsObject() const { return false; }
        virtual bool IsBoolean() const { return false; }
        virtual bool IsNumber() const { return false; }
        virtual bool IsInt32() const { return false; }
        virtual bool IsUint32() const { return false; }
        virtual bool IsExternal() const { return false; }
        virtual bool IsNativeError() const { return false; }
        virtual bool IsFunction() const { return false; }
        virtual bool IsArray() const { return false; }
        virtual bool IsDate() const { return false; }
        virtual bool IsBooleanObject() const { return false; }
        virtual bool IsNumberObject() const { return false; }
        virtual bool IsStringObject() const { return false; }
        virtual bool IsRegExp() const { return false; }
        /*
        Local<Boolean> ToBoolean() const;

        Local<Number> ToNumber() const;

        Local<String> ToString() const;

        Local<String> ToDetailString() const;

        Local<Object> ToObject() const;

        Local<Integer> ToInteger() const;

        Local<Uint32> ToUint32() const;

        Local<Int32> ToInt32() const;

        Local<Uint32> ToArrayIndex() const;

        bool BooleanValue() const;

        double NumberValue() const;

        int64_t IntegerValue() const;

        uint32_t Uint32Value() const;

        int32_t Int32Value() const;

        bool Equals(Handle<Value> that) const;

        bool StrictEquals(Handle<Value> that) const;
        */
    };


    /*
     * A dummy implementation of V8MonkeyObject for testing purposes
     *
     */
    #ifdef V8MONKEY_INTERNAL_TEST
    class DummyV8MonkeyObject: public V8MonkeyObject {
      public:
        DummyV8MonkeyObject() {}
        ~DummyV8MonkeyObject() {}
        void Trace(JSRuntime* runtime, JSTracer* tracer) {}

        inline bool operator== (const DummyV8MonkeyObject &other) {
          return &other == this;
        }
    };


    // Many tests seek to test when objects are deleted. To this end, we use special fake objects, that set a flag at
    // the given location if they were deleted
    class DeletionObject: public V8MonkeyObject {
      public:
        DeletionObject() : index(-1), ptr(nullptr) {}
        DeletionObject(bool* boolPtr) : index(-1), ptr(boolPtr) {}

        DeletionObject(bool* boolPtr, int i) : index(i), ptr(boolPtr) {}

        ~DeletionObject() {
          if (ptr) {
            *ptr = true;
          }
        }

        int index;

        void Trace(JSRuntime* runtime, JSTracer* tracer) {}
      private:
        bool* ptr;
    };


    // Some tests need to check that their objects are traced by the garbage collector. This dummy class can be used
    // for that purpose
    class TraceFake : public V8MonkeyObject {
      public:
        TraceFake(bool* boolPtr) : ptr(boolPtr) {}
        ~TraceFake() {}
        void Trace(JSRuntime* runtime, JSTracer* tracer) { *ptr = true; }
      private:
        bool* ptr;
    };

    #endif
  }
}


#endif
