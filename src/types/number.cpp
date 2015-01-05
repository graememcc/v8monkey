#include <stdint.h>

#include "v8.h"

#include "jsapi.h"

#include "runtime/isolate.h"
#include "types/base_types.h"
#include "types/value_types.h"


// XXX Casting?
namespace v8 {
  Local<Number> Number::New(double value) {
    // The V8 API specifies that this call implicitly inits V8
    V8::Initialize();

    JSRuntime* rt = V8Monkey::InternalIsolate::GetJSRuntimeForThread();
    JS::RootedValue numVal(rt, JS::NumberValue(value));

    V8Monkey::SMValue* smValue = new V8Monkey::SMValue(rt, numVal);
    V8Monkey::V8MonkeyObject** handle = HandleScope::CreateHandle(smValue);
    // Value is now rooted by isolate

    return reinterpret_cast<Number*>(handle);
  }


  double Number::Value() const {
    if (V8Monkey::V8MonkeyCommon::CheckDeath("Number::Value")) {
      return 0;
    }

    V8Monkey::SMValue* smValue = CONVERT_FROM_API(Number, SMValue, this);
    JSRuntime* rt = smValue->Runtime();
    JS::RootedValue rooted(rt, smValue->GetRawValue());
    return rooted.toDouble();
  }


  Local<Integer> Integer::New(int32_t value) {
    // The V8 API specifies that this call implicitly inits V8
    V8::Initialize();

    JSRuntime* rt = V8Monkey::InternalIsolate::GetJSRuntimeForThread();
    JS::RootedValue numVal(rt, JS::NumberValue(value));

    V8Monkey::SMValue* smValue = new V8Monkey::SMValue(rt, numVal);
    smValue->numberTag = V8Monkey::SMValue::INT32;

    V8Monkey::V8MonkeyObject** handle = HandleScope::CreateHandle(smValue);
    // Value is now rooted by isolate

    return reinterpret_cast<Integer*>(handle);
  }


  Local<Integer> Integer::NewFromUnsigned(uint32_t value) {
    // The V8 API specifies that this call implicitly inits V8
    V8::Initialize();

    JSRuntime* rt = V8Monkey::InternalIsolate::GetJSRuntimeForThread();

    // Cast it to fool SpiderMonkey
    int32_t asInt32 = static_cast<int32_t>(value);
    JS::RootedValue numVal(rt, JS::NumberValue(asInt32));

    V8Monkey::SMValue* smValue = new V8Monkey::SMValue(rt, numVal);
    smValue->numberTag = V8Monkey::SMValue::UINT32;

    V8Monkey::V8MonkeyObject** handle = HandleScope::CreateHandle(smValue);
    // Value is now rooted by isolate

    return reinterpret_cast<Integer*>(handle);
  }


  /*
   * I'm somewhat unclear as to to the point of the Integer constructors that take an isolate. They ultimately end up
   * in the internal handlescope's CreateHandle method which requires the supplied isolate to be current anyway.
   * Thus I simply defer to the standard versions
   * TODO: Should we follow V8 and assert when the isolate != current isolate?
   */

  Local<Integer> Integer::New(int32_t value, Isolate* i) {
    return Integer::New(value);
  }


  Local<Integer> Integer::NewFromUnsigned(uint32_t value, Isolate* i) {
    return Integer::NewFromUnsigned(value);
  }


  int64_t Integer::Value() const {
    if (V8Monkey::V8MonkeyCommon::CheckDeath("Integer::Value")) {
      return 0;
    }

    V8Monkey::SMValue* smValue = CONVERT_FROM_API(Integer, SMValue, this);
    JSRuntime* rt = smValue->Runtime();
    JS::RootedValue rooted(rt, smValue->GetRawValue());

    int32_t fromSpiderMonkey = rooted.toInt32();
    if (fromSpiderMonkey < 0 && smValue->numberTag == V8Monkey::SMValue::UINT32) {
      return static_cast<uint32_t>(fromSpiderMonkey);
    }

    return fromSpiderMonkey;
  }


  int32_t Int32::Value() const {
    if (V8Monkey::V8MonkeyCommon::CheckDeath("Int32::Value")) {
      return 0;
    }

    V8Monkey::SMValue* smValue = CONVERT_FROM_API(Int32, SMValue, this);
    JSRuntime* rt = smValue->Runtime();
    JS::RootedValue rooted(rt, smValue->GetRawValue());

    int32_t fromSpiderMonkey = rooted.toInt32();
    return static_cast<int32_t>(fromSpiderMonkey);
  }


  uint32_t Uint32::Value() const {
    if (V8Monkey::V8MonkeyCommon::CheckDeath("Uint32::Value")) {
      return 0;
    }

    V8Monkey::SMValue* smValue = CONVERT_FROM_API(Uint32, SMValue, this);
    JSRuntime* rt = smValue->Runtime();
    JS::RootedValue rooted(rt, smValue->GetRawValue());

    int32_t fromSpiderMonkey = rooted.toInt32();
    return static_cast<uint32_t>(fromSpiderMonkey);
  }
}
