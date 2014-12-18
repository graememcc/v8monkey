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
        V8MonkeyObject() : refCount(0), weakCount(0), callbackList(nullptr) {}

        virtual ~V8MonkeyObject() {
          // XXX Tear down weak ref list
        }


        void AddRef() {
          refCount++;
        }


        void Release() {
          if (--refCount == 0 && weakCount == 0) {
            delete this;
          }
        }


        // XXX Use V8 Typedef
        void MakeWeak(void* requester, void* parameters, void (*callback)(void*, void*)) {
          WeakRefListElem* cbListElem = FindForRequester(requester);

          // If the caller has already weakened this object, simply update callback information
          if (cbListElem) {
            cbListElem->params = parameters;
            cbListElem->callback = callback;
            return;
          }

          weakCount++;
          refCount--;

          cbListElem = new WeakRefListElem;
          cbListElem->requester = requester;
          cbListElem->params = parameters;
          cbListElem->callback = callback;
          cbListElem->prev = nullptr;
          cbListElem->next = callbackList;
          callbackList = cbListElem;
        }


        void ClearWeakness(void* requester) {
          WeakRefListElem* cbListElem = FindForRequester(requester);

          if (cbListElem) {
            if (cbListElem->prev) {
              cbListElem->prev->next = cbListElem->next;
            }
            if (cbListElem->next) {
              cbListElem->next->prev = cbListElem->prev;
            }

            ++refCount;
            --weakCount;
          }

          // If the requester was not present, then it never made this object weak, so don't adjust refcounts
        }


        // Customised version of Release required due to the weak pointer semantics afforded by Persistents.
        // Essentially, we have to decide whether the given Persistent is a weak ref or not, and decrement
        // the correct counter
        void PersistentRelease(void* requester) {
          WeakRefListElem* cbListElem = FindForRequester(requester);

          if (cbListElem) {
            --weakCount;
            if (refCount == 0 && weakCount == 0) {
              delete this;
            }
          } else {
            // Value has not been made weak. Safe to Release
            Release();
          }
        }


        // This function is used for implementing the V8 API weak reference mechanisms. Derived classes that wrap
        // SpiderMonkey objects should call this method before tracing the native objects. This method will check
        // if there are only weak references left: in that case, it will only report true after invoking all
        // registered weak reference callbacks
        bool ShouldTrace() {
          if (refCount > 0) {
            return true;
          }

          if (!callbackList) {
            return false;
          }

          WeakRefListElem* current = callbackList;
          while (current) {
            WeakRefListElem* next = current->next;
            if (current->callback) {
              current->callback(current->requester, current->params);
            }
            current = next;
          }

          return refCount > 0;
        }


        virtual void Trace(JSRuntime* runtime, JSTracer* tracer) = 0;


        #ifdef V8MONKEY_INTERNAL_TEST
        int RefCount() { return refCount; }

        int WeakCount() { return weakCount; }
        #endif


      private:
        int refCount;
        int weakCount;


        struct WeakRefListElem {
          void* requester;
          void* params;
          void (*callback)(void*, void*);
          WeakRefListElem* prev;
          WeakRefListElem* next;
        };


        WeakRefListElem* callbackList;


        WeakRefListElem* FindForRequester(void* requester) {
          WeakRefListElem* cbListElem = nullptr;

          if (callbackList) {
            WeakRefListElem* current = callbackList;
            while (current && !cbListElem) {
              if (current->requester == requester) {
                cbListElem = current;
              } else {
                current = current->next;
              }
            }
          }

          return cbListElem;
        }
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
    class DeletionObject: public V8MonkeyObject {
      public:
        DeletionObject(): index(-1), ptr(nullptr) {}
        DeletionObject(bool* boolPtr): index(-1), ptr(boolPtr) {}

        DeletionObject(bool* boolPtr, int i): index(i), ptr(boolPtr) {}

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
    #endif
  }
}


#endif
