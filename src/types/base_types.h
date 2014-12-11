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
        void Trace(JSRuntime* runtime, JSTracer* tracer) {
          // NO-OP
        }
    };


    /*
     * The base class of all objects that wrap objects from SpiderMonkey.
     *
     */
    class EXPORT_FOR_TESTING_ONLY SMObject : public V8MonkeyObject {};


    /*
     * The base class of all objects that wrap objects from SpiderMonkey that are not GCThings.
     *
     */
    class EXPORT_FOR_TESTING_ONLY SMValue : public SMObject {
      public:
        void Trace(JSRuntime* runtime, JSTracer* tracer) {
          // NO-OP
        }
    };


    /*
     * The base class of all objects that wrap objects from SpiderMonkey that are GCThings.
     *
     */
    class EXPORT_FOR_TESTING_ONLY SMGCObject : public SMObject {};


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
    #endif
  }
}


#endif
