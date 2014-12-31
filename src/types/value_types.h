#ifndef V8MONKEY_VALUETYPES_H
#define V8MONKEY_VALUETYPES_H

#include "v8.h"

#include "jsapi.h"

#include "types/base_types.h"


#define CONVERT_FROM_API(APIType, Type, val) *(reinterpret_cast<V8Monkey::Type**>(const_cast<APIType*>(val)))

namespace v8 {
  namespace V8Monkey {


    /*
     * The base class of all V8 values that wrap values from SpiderMonkey
     *
     */

    class EXPORT_FOR_TESTING_ONLY SMValue: public V8Value {
      public:
        SMValue(JSRuntime* runtime, JS::Handle<JS::Value> val) : numberTag(UNKNOWN), rt(runtime), jsValue(val) {}

        JSRuntime* Runtime() const { return rt; }

        const JS::Heap<JS::Value>& GetRawValue() const { return jsValue; }

        enum NumberTags {UNKNOWN, NUMBER, INT32, UINT32};

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

          return numberTag == INT32;
        }

        bool IsUint32() const {
          if (!IsNumber()) {
            return false;
          }

          return numberTag == UINT32;
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
