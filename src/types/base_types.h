#ifndef V8MONKEY_BASETYPES_H
#define V8MONKEY_BASETYPES_H

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
        V8MonkeyObject() : refCount(0) {}

        virtual ~V8MonkeyObject() {}

        void AddRef() {
          refCount++;
        }

        void Release() {
          if (--refCount == 0) {
            delete this;
          }
        }

        virtual void Trace(JSRuntime* runtime, JSTracer* tracer) = 0;

        #ifdef V8MONKEY_INTERNAL_TEST
        int RefCount() { return refCount; }
        #endif

        
      private:
        int refCount;
    };


    /*
     * The base class of all V8 API objects that do not wrap objects from SpiderMonkey, and hence do not require
     * rooting for the SpiderMonkey garbage collector.
     *
     */
    class EXPORT_FOR_TESTING_ONLY V8APIObject : public V8MonkeyObject {
      public:
        virtual void Trace(JSRuntime* runtime, JSTracer* tracer) {
          // NO-OP
        }
    };


    /*
     * The base class of all objects that wrap objects from SpiderMonkey.
     *
     */
    class EXPORT_FOR_TESTING_ONLY SMObject : public V8MonkeyObject {};
// Think maybe isNull etc should be here?


    /*
     * The base class of all objects that wrap objects from SpiderMonkey that are GCThings.
     *
     */
    class EXPORT_FOR_TESTING_ONLY SMGCObject : public SMObject {};


    /*
     * The base class of all objects that wrap values from SpiderMonkey
     *
     */
    class EXPORT_FOR_TESTING_ONLY SMValue: public SMObject {
      public:
        SMValue(JSRuntime* runtime, JS::Handle<JS::Value> val) : rt(runtime), jsValue(val) {}

        // Needs more methods?f
        virtual bool isNull() const;
        virtual bool isUndefined() const;
        virtual bool isBoolean() const;
        virtual bool isTrue() const;
        virtual bool isFalse() const;
        virtual bool isInt32() const;
        virtual bool isDouble() const;
        virtual bool isNumber() const;
        virtual bool isString() const;
        virtual bool isObject() const;

        void Trace(JSRuntime* runtime, JSTracer* tracer) {
          if (runtime != rt) {
            return;
          }

          JS_CallValueTracer(tracer, &jsValue, "V8Monkey manual rooting");
        }

      private:
        JSRuntime* rt;
        JS::Heap<JS::Value> jsValue;
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
    class DeletionObject {
      public:
        DeletionObject(bool* boolPtr): index(-1), ptr(boolPtr) {}

        DeletionObject(bool* boolPtr, int i): index(i), ptr(boolPtr) {}

        ~DeletionObject() {
          if (ptr) {
            *ptr = true;
          }
        }

        int index;
      private:
        bool* ptr;
    };
    #endif
  }
}


#endif
