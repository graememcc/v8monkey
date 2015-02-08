// ConvertFromAPI V8Boolean V8SpecialValue
#include "types/value_types.h"

// Boolean Handle Null Primitive Undefined
#include "v8.h"

// InitPrimitiveSingletons TearDownPrimitiveSingletons
#include "utils/V8MonkeyCommon.h"


/*
 * This file, and the V8Boolean and V8SpecialValue types in value_types.h, provide the implementation of booleans,
 * undefined and null.
 *
 * Whilst we could use the SpiderMonkey versions, the API documentation makes no guarantees that the given values are
 * constants (although I believe they are), and reminds clients not to rely on any internal details of JS::Values.
 * By rolling our own, we can guarantee that they are singletons, and we avoid a complication if/when we get threads
 * working: by extension, we wouldn't be able to rely on the SpiderMonkey versions being constant across JSRuntimes,
 * so would need to create fresh versions for every thread.
 *
 */
//
//namespace {
//  using namespace v8;
//  using namespace V8Monkey;
//
//
//  V8MonkeyObject* trueValue;
//  V8MonkeyObject* falseValue;
//  V8MonkeyObject** truePtr = &trueValue;
//  V8MonkeyObject** falsePtr = &falseValue;
//  V8MonkeyObject* undefinedValue;
//  V8MonkeyObject** undefinedPtr = &undefinedValue;
//  V8MonkeyObject* nullValue;
//  V8MonkeyObject** nullPtr = &nullValue;
//}
//
//
//// XXX Do we want a ConvertToAPI equivalent of ConvertFromAPI?
//
//
//namespace v8 {
//  Handle<Boolean> True() {
//    Handle<Boolean> h(reinterpret_cast<Boolean*>(truePtr));
//    return h;
//  }
//
//
//  Handle<Boolean> False() {
//    Handle<Boolean> h(reinterpret_cast<Boolean*>(falsePtr));
//    return h;
//  }
//
//
//  bool Boolean::Value() const {
//    V8Monkey::V8Boolean* v8bool = V8Monkey::V8Value::ConvertFromAPI<V8Boolean, Boolean>(this);
//    return v8bool->Value();
//  }
//
//
//  Handle<Primitive> Undefined() {
//    Handle<Primitive> h(reinterpret_cast<Primitive*>(undefinedPtr));
//    return h;
//  }
//
//
//  Handle<Primitive> Null() {
//    Handle<Primitive> h(reinterpret_cast<Primitive*>(nullPtr));
//    return h;
//  }
//
//
//  namespace V8Monkey {
//    void V8MonkeyCommon::InitPrimitiveSingletons() {
//      // Booleans
//      trueValue = new V8Monkey::V8Boolean(true);
//      falseValue = new V8Monkey::V8Boolean(false);
//      trueValue->AddRef();
//      falseValue->AddRef();
//
//      // Undefined
//      undefinedValue = new V8Monkey::V8SpecialValue(false, true);
//      undefinedValue->AddRef();
//
//      // Null
//      nullValue = new V8Monkey::V8SpecialValue(true, false);
//      nullValue->AddRef();
//    }
//
//
//    void V8MonkeyCommon::TearDownPrimitiveSingletons() {
//      trueValue->Release();
//      falseValue->Release();
//      undefinedValue->Release();
//      nullValue->Release();
//    }
//  }
//}
