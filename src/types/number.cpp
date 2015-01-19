// int32_t, int64_t, uint32_t
#include <cinttypes>

// fpclassify
#include <cmath>

// Integer Int32 Local Number Uint32
#include "v8.h"

// ConvertFromAPI V8MonkeyObject
#include "types/base_types.h"

// V8Number
#include "types/value_types.h"

// CheckDeath
#include "v8monkey_common.h"


namespace {
  using namespace v8;
  using namespace V8Monkey;


  /*
   * Builds a V8Number which represents the given number.
   *
   * Note that, whilst both V8 and SpiderMonkey APIs have notions of arbitrary IEEE 754 double-precision numbers and
   * Int32s, only V8 additionally exposes Uint32s. This presents a minor book-keeping headache, particularly if we were
   * to take the approach of wrapping a SpiderMonkey JS::Value, as we would be constantly checking to see if the value
   * fits in 31 bits, in order to choose the correct accessor. Instead, we just create our own objects, handling such
   * complications at this level. We lazily create SpiderMonkey equivalents as required.
   *
   */

  V8MonkeyObject** handlizeNumber(double value) {
    // The V8 API specifies that this call implicitly inits V8
    V8::Initialize();

    V8Monkey::V8Number* number = new V8Monkey::V8Number(value);
    V8Monkey::V8MonkeyObject** handle = HandleScope::CreateHandle(number);
    return handle;
  }
}


namespace v8 {
  Local<Number> Number::New(double value) {
    return reinterpret_cast<Number*>(handlizeNumber(value));
  }


  double Number::Value() const {
    if (V8Monkey::V8MonkeyCommon::CheckDeath("Number::Value")) {
      return 0;
    }

    V8Monkey::V8Number* v8number = V8Monkey::V8Value::ConvertFromAPI<V8Number, Number>(this);
    return v8number->Value();
  }


  Local<Integer> Integer::New(int32_t value) {
    return reinterpret_cast<Integer*>(handlizeNumber(value));
  }


  Local<Integer> Integer::NewFromUnsigned(uint32_t value) {
    return reinterpret_cast<Integer*>(handlizeNumber(value));
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


  // XXX Look again at these casts
  int64_t Integer::Value() const {
    if (V8Monkey::V8MonkeyCommon::CheckDeath("Integer::Value")) {
      return 0;
    }

    V8Monkey::V8Number* v8number = V8Monkey::V8Value::ConvertFromAPI<V8Number, Integer>(this);
    return static_cast<int64_t>(v8number->Value());
  }


  int32_t Int32::Value() const {
    if (V8Monkey::V8MonkeyCommon::CheckDeath("Int32::Value")) {
      return 0;
    }

    V8Monkey::V8Number* v8number = V8Monkey::V8Value::ConvertFromAPI<V8Number, Int32>(this);
    return static_cast<int64_t>(v8number->Value());
  }


  uint32_t Uint32::Value() const {
    if (V8Monkey::V8MonkeyCommon::CheckDeath("Uint32::Value")) {
      return 0;
    }

    V8Monkey::V8Number* v8number = V8Monkey::V8Value::ConvertFromAPI<V8Number, Uint32>(this);
    return static_cast<int64_t>(v8number->Value());
  }


  namespace V8Monkey {
    void V8Number::classifyNumber() {
      // Classify the number. We follow V8, and build a union to capture the bit pattern to distinguish -0.0. The V8
      // code assumes doubles are IEEE 754 double-precision; we shall too.
      union Bits {
        double dval;
        uint64_t uval;
      };

      Bits negZero = {-0.0};

      int classification = std::fpclassify(value);
      Bits bitRepresentation = {value};

      if (classification == FP_NAN || classification == FP_INFINITE || bitRepresentation.uval == negZero.uval) {
        type = V8Monkey::V8Number::NUMBER;
      } else {
        int32_t asInt32 = static_cast<int32_t>(value);
        uint32_t asUint32 = static_cast<uint32_t>(value);

        if (asUint32 == value) {
          type = asUint32 < 0x10000000 ? V8Monkey::V8Number::VAL32 : V8Monkey::V8Number::UINT32;
        } else if (asInt32 == value) {
          type = V8Monkey::V8Number::INT32;
        } else {
          type = V8Monkey::V8Number::NUMBER;
        }
      }
    }
  }
}
