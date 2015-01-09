#include <cmath>
#include <cinttypes>

#include "v8.h"

#include "jsapi.h"

#include "runtime/isolate.h"
#include "types/value_types.h"
#include "v8monkey_common.h"


// XXX Ensure initialized/isdead checking
// XXX Casting?
namespace {

  /*
   * ECMA-262 Section 9.5 compliant To(U)Int32 casting
   *
   */

  uint32_t doubleToUint32(double d) {
    if (std::isnan(d) || !std::isfinite(d) || d == -0.0) {
      return 0;
    }

    return static_cast<uint32_t>(d);
  }


  int32_t doubleToInt32(double d) {
    // Convert to an unsigned first, to avoid undefined behaviour for uint32s that don't fit in an int32
    return static_cast<int32_t>(doubleToUint32(d));
  }
}


namespace v8 {
  #define FORWARD_TO_INTERNAL(name) \
  bool Value::name() const {\
    if (V8Monkey::V8MonkeyCommon::CheckDeath("Value::" #name)) { \
      return false; \
    } \
 \
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


  double Value::NumberValue() const {
    if (IsNumber()) {
      return static_cast<Number*>(const_cast<Value*>(this))->Value();
    }

    return 0xdead;
  }


  int64_t Value::IntegerValue() const {
    if (IsNumber()) {
      Number* asNumber = static_cast<Number*>(const_cast<Value*>(this));

      // Interestingly, the V8 API isn't ECMA262 Section 9.4 compliant here (although the engine presumably is), in that
      // NaN gets cast to int64_t too: ECMA states NaN maps to 0.
      return static_cast<int64_t>(asNumber->Value());
    }

    // XXX Finish
    return 0x3;
  }


  int32_t Value::Int32Value() const {
    if (IsNumber()) {
      Number* asNumber = static_cast<Number*>(const_cast<Value*>(this));

      double doubleValue = asNumber->Value();

      if (IsInt32() || IsUint32()) {
        // Watch out for negative zero!
        if (doubleValue == -0.0) {
          return 0;
        }

        // Shove the value into a uint32_t first, to avoid undefined behaviour when values don't fit
        uint32_t asUnsigned = static_cast<uint32_t>(doubleValue);
        return asUnsigned;
      }

      return doubleToInt32(asNumber->Value());
    }

    // XXX Finish
    return 0x4;
  }


  uint32_t Value::Uint32Value() const {
    if (IsNumber()) {
      Number* asNumber = static_cast<Number*>(const_cast<Value*>(this));

      double doubleValue = asNumber->Value();

      if (IsInt32() || IsUint32()) {
        // Watch out for negative zero!
        if (doubleValue == -0.0) {
          return 0;
        }

        return static_cast<uint32_t>(doubleValue);
      }

      return doubleToInt32(asNumber->Value());
    }

    // XXX Finish
    return 12;
  }


  Local<Number> Value::ToNumber() const {
    // The caller should be in a handlescope already

    if (IsNumber()) {
      return Local<Number>(static_cast<Number*>(const_cast<Value*>(this)));
    }

    // XXX Finish
    Local<Number> l;
    return l;
  }


  Local<Integer> Value::ToInteger() const {
    // The caller should be in a handlescope already

    if (IsNumber()) {
      Number* asNumber = static_cast<Number*>(const_cast<Value*>(this));
      double value = asNumber->Value();

      // In V8, if the number contains an integer that would fit in a 31-bit two's complement integer, then the
      // original object is returned
      if ((IsInt32() || IsUint32()) && value < 0x10000000) {
        return reinterpret_cast<Integer*>(asNumber);
      }

      if (IsUint32()) {
        return Integer::NewFromUnsigned(static_cast<uint32_t>(value));
      }

      // NaN and infinity are tricky. We want an Integer whose value equals the value cast to int64_t, but the API
      // integer constructors take (u)int32_ts. Thus, we fake it.
      if (std::isnan(value) || !std::isfinite(value)) {
        Local<Number> temp = Number::New(value);
        return reinterpret_cast<Integer*>(*temp);
      }

      return Integer::New(static_cast<int32_t>(value));
    }

    // XXX Finish
    Local<Integer> l;
    return l;
  }


  Local<Int32> Value::ToInt32() const {
    // The caller should be in a handlescope already

    if (IsNumber()) {
      Number* asNumber = static_cast<Number*>(const_cast<Value*>(this));
      double value = asNumber->Value();

      // In V8, if the number contains an integer that would fit in a 31-bit two's complement integer, then the
      // original object is returned
      if ((IsInt32() || IsUint32()) && value < 0x10000000) {
        return reinterpret_cast<Int32*>(asNumber);
      }

      Local<Integer> temp = Integer::New(static_cast<int32_t>(doubleToInt32(value)));
      return reinterpret_cast<Int32*>(*temp);
    }

    // XXX Finish
    Local<Integer> l;
    return l;
  }


  Local<Uint32> Value::ToUint32() const {
    // The caller should be in a handlescope already

    if (IsNumber()) {
      Number* asNumber = static_cast<Number*>(const_cast<Value*>(this));
      double value = asNumber->Value();

      // In V8, if the number contains an integer that would fit in a 31-bit two's complement integer, then the
      // original object is returned
      if ((IsInt32() || IsUint32()) && value < 0x10000000) {
        return reinterpret_cast<Uint32*>(asNumber);
      }

      Local<Integer> temp = Integer::NewFromUnsigned(doubleToUint32(value));
      return reinterpret_cast<Uint32*>(*temp);
    }

    // XXX Finish
    Local<Integer> l;
    return l;
  }


  Local<Uint32> Value::ToArrayIndex() const {
    // The caller should be in a handlescope already
    Local<Uint32> empty;

    if (IsNumber()) {
      Number* asNumber = static_cast<Number*>(const_cast<Value*>(this));
      double value = asNumber->Value();

      // Negative zero-as always-complicates matters
      union Bits {
        double dval;
        uint64_t uval;
      };

      Bits negZero = {-0.0};
      Bits bitRepresentation = {value};

      if (bitRepresentation.uval == negZero.uval) {
        Local<Integer> temp = Integer::NewFromUnsigned(doubleToUint32(0));
        return reinterpret_cast<Uint32*>(*temp);
      }

      if (!(IsInt32() || IsUint32()) || value < 0) {
        return empty;
      }

      // Values that fit in a 31-bit integer are returned unscathed
      if (value < 0x10000000) {
        return reinterpret_cast<Uint32*>(asNumber);
      }

      Local<Integer> temp = Integer::NewFromUnsigned(doubleToUint32(value));
      return reinterpret_cast<Uint32*>(*temp);
    }

    // XXX Finish
    Local<Integer> l;
    return l;
  }
}
