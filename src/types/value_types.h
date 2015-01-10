#ifndef V8MONKEY_VALUETYPES_H
#define V8MONKEY_VALUETYPES_H

#include "v8.h"

#include "jsapi.h"

#include "types/base_types.h"


#define CONVERT_FROM_API(APIType, Type, val) *(reinterpret_cast<V8Monkey::Type**>(const_cast<APIType*>(val)))

namespace v8 {
  namespace V8Monkey {


    class EXPORT_FOR_TESTING_ONLY V8Number: public V8Value {
      public:
        enum NumberTag {UNKNOWN, NUMBER, INT32, UINT32, VAL32};

        V8Number(double val) : value(val) {
          classifyNumber();
        }

        double Value() {
          return value;
        }

        bool IsNumber() const {
          return true;
        }

        bool IsRealDouble() const {
          return type == NUMBER;
        }

        bool IsVal32() const {
          return type == VAL32;
        }

        bool IsInt32() const {
          return type == VAL32 || type == INT32;
        }

        bool IsUint32() const {
          return type == VAL32 || type == UINT32;
        }

      private:
        void classifyNumber();

        double value;
        NumberTag type;
    };


    class EXPORT_FOR_TESTING_ONLY V8SpecialValue: public V8Value {
      public:
        V8SpecialValue(bool isNull, bool isUndefined) : isNull(isNull), isUndefined(isUndefined) {}

        bool IsNull() const {
          return isNull;
        }

        bool IsUndefined() const {
          return isUndefined;
        }

      private:
        bool isNull;
        bool isUndefined;
    };


    class EXPORT_FOR_TESTING_ONLY V8Boolean: public V8Value {
      public:
        V8Boolean(bool val) : value(val) {}

        bool Value() {
          return value;
        }

        bool IsBoolean() const {
          return true;
        }

        bool IsTrue() const {
          return value;
        }

        bool IsFalse() const {
          return !value;
        }

      private:
        bool value;
    };


    /*
     * The base class of all V8 values that wrap values from SpiderMonkey
     *
     */

    class EXPORT_FOR_TESTING_ONLY SMValue: public V8Value {
      public:
        SMValue(JSRuntime* runtime, JS::Handle<JS::Value> val) : rt(runtime), jsValue(val) {}

        JSRuntime* Runtime() const { return rt; }

        const JS::Heap<JS::Value>& GetRawValue() const { return jsValue; }

        #define SMVALUE_IMPL(name, smMethod) bool name() const { return jsValue.smMethod(); }

        SMVALUE_IMPL(IsUndefined, isUndefined)
        SMVALUE_IMPL(IsNull, isNull)
        SMVALUE_IMPL(IsTrue, isTrue)
        SMVALUE_IMPL(IsFalse, isFalse)
        SMVALUE_IMPL(IsBoolean, isBoolean)
        SMVALUE_IMPL(IsNumber, isNumber)

        void Trace(JSRuntime* runtime, JSTracer* tracer) {
          printf("Value being traced %p\n", this);
          if (runtime != rt || !ShouldTrace()) {
          printf("No tracing required\n");
            return;
          }

          printf("SM Tracing\n");
          JS_CallValueTracer(tracer, &jsValue, "V8Monkey manual rooting");
          printf("Done value trace. Returning\n");
        }

      private:
        JSRuntime* rt;
        JS::Heap<JS::Value> jsValue;
    };
  }
}



#endif
