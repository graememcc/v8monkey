#include "v8.h"

#include "jsapi.h"

#include "runtime/isolate.h"
#include "types/base_types.h"
#include "types/value_types.h"


namespace {
  using namespace v8;
  using namespace V8Monkey;


  V8MonkeyObject* trueValue;
  V8MonkeyObject* falseValue;
  V8MonkeyObject** truePtr = &trueValue;
  V8MonkeyObject** falsePtr = &falseValue;
}


namespace v8 {
  Handle<Boolean> True() {
    Handle<Boolean> h(reinterpret_cast<Boolean*>(truePtr));
    return h;
  }


  Handle<Boolean> False() {
    Handle<Boolean> h(reinterpret_cast<Boolean*>(falsePtr));
    return h;
  }


  bool Boolean::Value() const {
    V8Monkey::V8Boolean* v8bool = CONVERT_FROM_API(Boolean, V8Boolean, this);
    return v8bool->Value();
  }


  namespace V8Monkey {
    void V8MonkeyCommon::InitPrimitiveSingletons() {
      trueValue = new V8Monkey::V8Boolean(true);
      falseValue = new V8Monkey::V8Boolean(false);
      trueValue->AddRef();
      falseValue->AddRef();
    }


    void V8MonkeyCommon::TearDownPrimitiveSingletons() {
      trueValue->Release();
      falseValue->Release();
    }
  }
}
