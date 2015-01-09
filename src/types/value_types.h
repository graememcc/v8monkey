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

        void Trace(JSRuntime* runtime, JSTracer* tracer) {}

      private:
        static void classifyNumber();

        double value;
        NumberTag type;
    };



    /*
     * The base class of all V8 values that wrap values from SpiderMonkey
     *
     */

    class EXPORT_FOR_TESTING_ONLY SMValue: public V8Value {
      public:
        SMValue(JSRuntime* runtime, JS::Handle<JS::Value> val) : numberTag(UNKNOWN), rt(runtime), jsValue(val) {}

        JSRuntime* Runtime() const { return rt; }

        const JS::Heap<JS::Value>& GetRawValue() const { return jsValue; }

        // VAL32 represents UINT32 values that are small enough to fit into INT32s without changing sign
        enum NumberTags {UNKNOWN, NUMBER, INT32, UINT32, VAL32};

        #define SMVALUE_IMPL(name, smMethod) bool name() const { return jsValue.smMethod(); }

        SMVALUE_IMPL(IsUndefined, isUndefined)
        SMVALUE_IMPL(IsNull, isNull)
        SMVALUE_IMPL(IsTrue, isTrue)
        SMVALUE_IMPL(IsFalse, isFalse)
        SMVALUE_IMPL(IsBoolean, isBoolean)
        SMVALUE_IMPL(IsNumber, isNumber)

        bool IsInt32() const {
          if (!IsNumber()) {
            return false;
          }

          return numberTag == VAL32 || numberTag == INT32;
        }

        bool IsUint32() const {
          if (!IsNumber()) {
            return false;
          }

          return numberTag == VAL32 || numberTag == UINT32;
        }


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

        NumberTags numberTag;

      private:
        JSRuntime* rt;
        JS::Heap<JS::Value> jsValue;
    };
  }
}



#endif
