/*
#ifndef V8MONKEY_VALUETYPES_H
#define V8MONKEY_VALUETYPES_H

// JS_CallValueTracer JS::Handle JS::Heap JSRuntime JSTracer JS::Value
#include "jsapi.h"

// EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"

// V8Value
#include "types/base_types.h"
//
//
namespace v8 {
  namespace internal {
*/

    /*
     * Base class for types that wrap a SpiderMonkey JSValue
     *
     */

/*
    class EXPORT_FOR_TESTING_ONLY SMValue : public V8Value {
      public:
        SMValue(JS::Value val) : jsval (val) {}
        SMValue(JS::HandleValue val) : jsval (val) {}

        void DoTrace(JSRuntime*, JSTracer* tracer) override {
          JS_CallValueTracer(tracer, &jsval, "V8Monkey root");
        }

        ~SMValue() = default;
        SMValue(const SMValue& other) = delete;
        SMValue(SMValue&& other) = delete;
        SMValue& operator=(const SMValue& other) = delete;
        SMValue& operator=(const SMValue&& other) = delete;

      protected:
        JS::Heap<JS::Value> jsval;
    };
*/


    /*
     * Booleans
     *
     */

/*
   class SMBoolean : public SMValue {
     public:
        SMBoolean(bool value) : SMValue(JS::BooleanValue(value)) {
          ignoreRuntime = true;
        }

        ~SMBoolean() = default;
        SMBoolean(const SMBoolean& other) = delete;
        SMBoolean(SMBoolean&& other) = delete;
        SMBoolean& operator=(const SMBoolean& other) = delete;
        SMBoolean& operator=(const SMBoolean&& other) = delete;

        bool IsBoolean() const override { return true; }

        bool BooleanValue() const override { return jsval.toBoolean(); }
  };


//    class EXPORT_FOR_TESTING_ONLY V8Number: public V8Value {
//      public:
//        enum NumberTag {UNKNOWN, NUMBER, INT32, UINT32, VAL32};

//        V8Number(double val) : value(val) {
//          classifyNumber();
//        }

//        double Value() {
//          return value;
//        }

//        bool IsNumber() const {
//          return true;
//        }

//        bool IsRealDouble() const {
//          return type == NUMBER;
//        }
//
//        bool IsVal32() const {
//          return type == VAL32;
//        }

//        bool IsInt32() const {
//          return type == VAL32 || type == INT32;
//        }

//        bool IsUint32() const {
//          return type == VAL32 || type == UINT32;
//        }

//      private:
//        void classifyNumber();

//        double value;
//        NumberTag type;
//    };


//    class EXPORT_FOR_TESTING_ONLY V8SpecialValue: public V8Value {
//      public:
//        V8SpecialValue(bool isNull, bool isUndefined) : isNull(isNull), isUndefined(isUndefined) {}

//        bool IsNull() const {
//          return isNull;
//        }

//        bool IsUndefined() const {
//          return isUndefined;
//        }

//      private:
//        bool isNull;
//        bool isUndefined;
//    };
*/


//    // XXX Does anybody use this?
//    /*
//     * The base class of all V8 values that wrap values from SpiderMonkey
//     *
//     */
//
//    class EXPORT_FOR_TESTING_ONLY SMValue: public V8Value {
//      public:
//        SMValue(JSRuntime* runtime, JS::Handle<JS::Value> val) : rt(runtime), jsValue(val) {}
//
//        JSRuntime* Runtime() const { return rt; }
//
//        const JS::Heap<JS::Value>& GetRawValue() const { return jsValue; }
//
//        #define SMVALUE_IMPL(name, smMethod) bool name() const { return jsValue.smMethod(); }
//
//        SMVALUE_IMPL(IsUndefined, isUndefined)
//        SMVALUE_IMPL(IsNull, isNull)
//        SMVALUE_IMPL(IsTrue, isTrue)
//        SMVALUE_IMPL(IsFalse, isFalse)
//        SMVALUE_IMPL(IsBoolean, isBoolean)
//        SMVALUE_IMPL(IsNumber, isNumber)
//
//        void Trace(JSRuntime* runtime, JSTracer* tracer) {
//          printf("Value being traced %p\n", static_cast<void*>(this));
//          if (runtime != rt || !ShouldTrace()) {
//          printf("No tracing required\n");
//            return;
//          }
//
//          printf("SM Tracing\n");
//          JS_CallValueTracer(tracer, &jsValue, "V8Monkey manual rooting");
//          printf("Done value trace. Returning\n");
//        }
//
//      private:
//        JSRuntime* rt;
//        JS::Heap<JS::Value> jsValue;
//    };
/*
  }
}



#endif
*/
