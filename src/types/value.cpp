#include <stdint.h>

#include "v8.h"

#include "jsapi.h"

#include "runtime/isolate.h"
#include "types/value_types.h"


// XXX Ensure initialized/isdead checking
// XXX Casting?
namespace v8 {
  #define FORWARD_TO_INTERNAL(name) \
  bool Value::name() const {\
    V8Monkey::V8Value* val = *(reinterpret_cast<V8Monkey::V8Value**>(const_cast<Value*>(this)));\
    return val->name();\
  }

  FORWARD_TO_INTERNAL(IsUndefined)

  FORWARD_TO_INTERNAL(IsNull)

  FORWARD_TO_INTERNAL(IsTrue)

  FORWARD_TO_INTERNAL(IsFalse)

  FORWARD_TO_INTERNAL(IsString)

  FORWARD_TO_INTERNAL(IsFunction)

  FORWARD_TO_INTERNAL(IsArray)

  FORWARD_TO_INTERNAL(IsObject)

  FORWARD_TO_INTERNAL(IsBoolean)

  FORWARD_TO_INTERNAL(IsNumber)

  FORWARD_TO_INTERNAL(IsExternal)

  FORWARD_TO_INTERNAL(IsInt32)

  FORWARD_TO_INTERNAL(IsUint32)

  FORWARD_TO_INTERNAL(IsDate)

  FORWARD_TO_INTERNAL(IsBooleanObject)

  FORWARD_TO_INTERNAL(IsNumberObject)

  FORWARD_TO_INTERNAL(IsStringObject)

  FORWARD_TO_INTERNAL(IsNativeError)

  FORWARD_TO_INTERNAL(IsRegExp)

  #undef FORWARD_TO_INTERNAL
}
