/*
// strlen
#include <cstring>

// Local String etc.
#include "v8.h"

// XXX Needed?
#include "jsapi.h"

// XXX Needed?
#include "types/base_types.h"

// SMString
#include "types/value_types.h"

// SMEncodedString definition
#include "utils/Encoding.h"

// TwoByteLength
#include "utils/MiscUtils.h"

// GetJSRuntime/GetJSContext
#include "utils/SpiderMonkeyUtils.h"

// ASSERT
#include "utils/V8MonkeyCommon.h"
*/
//
//
//namespace {
//  using namespace v8;
//  using namespace V8Monkey;
//
//
//  /*
//   * Builds a V8Number which represents the given number.
//   *
//   * Note that, whilst both V8 and SpiderMonkey APIs have notions of arbitrary IEEE 754 double-precision numbers and
//   * Int32s, only V8 additionally exposes Uint32s. This presents a minor book-keeping headache, particularly if we were
//   * to take the approach of wrapping a SpiderMonkey JS::Value, as we would be constantly checking to see if the value
//   * fits in 31 bits, in order to choose the correct accessor. Instead, we just create our own objects, handling such
//   * complications at this level. We lazily create SpiderMonkey equivalents as required.
//   *
//   */
//
//  V8MonkeyObject** handlizeNumber(double value) {
//    // The V8 API specifies that this call implicitly inits V8
//    V8::Initialize();
//
//    V8Monkey::V8Number* number = new V8Monkey::V8Number(value);
//    // After creating a handle, the value will be rooted by the isolate
//    V8Monkey::V8MonkeyObject** handle = HandleScope::CreateHandle(number);
//
//    return handle;
//  }
//}
//
//
//namespace v8 {
//  Local<String> String::New(const char* data, int length) {
//    using namespace V8Monkey;
//
//    if (length == -1) {
//      length = strlen(data);
//    }
//
//    // First, we need to ascertain what type of string we can create
//    // If the thread is in a context, then 
//    bool needsEncoded = UTF8::NeedsEncodingForSpiderMonkey(data, length);
//
//    if (!needsEncoded) {
//      JSRuntime* rt = SpiderMonkeyUtils::GetJSRuntimeForThread();
//      JSContext* cx = SpiderMonkeyUtils::GetJSContextForThread();
//      JSAutoRequest ar(cx);
//
//      JS::RootedString s(cx, JS_NewStringCopyN(cx, data, length));
//      SMString* sm = new SMString(rt, s);
//
//      // XXX Check this assumption
//      // We should be in a handlescope already
//      V8MonkeyObject** handle = HandleScope::CreateHandle(sm);
//      return reinterpret_cast<String*>(handle);
//    }
//
//    // If I don't need encoded, I can go ahead and create the object
//    // Otherwise, create a SMEncodedString
//    return reinterpret_cast<String*>(handlizeNumber(0.0));
//  }
//
//
//  namespace V8Monkey {
//    SMEncodedString::SMEncodedString(char* s, int len) : encoding(UNKNOWN), asLatin1(nullptr), asUtf16(nullptr),
//                                                            length(len) {
//
//      if (len == -1) {
//        length = strlen(s);
//      }
//
//      if (UTF8::CanEncodeToLatin1(s, length)) {
//        encoding = LATIN1;
//        asLatin1 = UTF8::Latin1Encode(s, length);
//        return;
//      }
//
//      encoding = UTF16;
//      asUtf16 = UTF8::Utf16Encode(s, length);
//    }
//
//
//    SMEncodedString::SMEncodedString(uint16_t* s, int len) : encoding(UTF16), asLatin1(nullptr), asUtf16(nullptr),
//                                                                length(len) {
//      if (len == -1) {
//        length = MiscUtils::TwoByteLength(s);
//      }
//
//      asUtf16 = new char16_t[length + 1];
//      for (size_t i = 0; i < length + 1; i++) {
//        asUtf16[i] = s[i];
//      }
//
//      asUtf16[length] = 0x00;
//    }
//
//
//    SMEncodedString::SMEncodedString(SMEncodedString& other) {
//      encoding = other.encoding;
//      length = other.length;
//
//      ASSERT(encoding != UNKNOWN, "SMEncodedString::SMEncodedString (copy)", "Copied from unknown encoding");
//      if (encoding == LATIN1) {
//        asLatin1 = new char[length + 1];
//        for (size_t i = 0; i < length; i++) {
//          asLatin1[i] = other.asLatin1[i];
//        }
//
//        asLatin1[length] = 0x00;
//        asUtf16 = nullptr;
//      } else {
//        asUtf16 = new char16_t[length + 1];
//        for (size_t i = 0; i < length; i++) {
//          asUtf16[i] = other.asUtf16[i];
//        }
//
//        asUtf16[length] = 0x0000;
//        asLatin1 = nullptr;
//      }
//    }
//
//
//    SMEncodedString::SMEncodedString(SMEncodedString&& other) {
//      encoding = other.encoding;
//      length = other.length;
//      asLatin1 = other.asLatin1;
//      asUtf16 = other.asUtf16;
//
//      ASSERT(encoding != UNKNOWN, "SMEncodedString::SMEncodedString (move)", "Moved from unknown encoding");
//      if (encoding == LATIN1) {
//        other.asLatin1 = nullptr;
//      } else {
//        other.asUtf16 = nullptr;
//      }
//      other.encoding = UNKNOWN;
//    }
//
//
//    SMEncodedString::~SMEncodedString() {
//      #ifdef V8MONKEY_INTERNAL_TEST
//      PoisonArrays();
//      #endif
//
//      delete[] asLatin1;
//      delete[] asUtf16;
//    }
//
//
//    #ifdef V8MONKEY_INTERNAL_TEST
//    void SMEncodedString::PoisonArrays() {
//      // "Poison" the buffer changing its value
//      if (encoding == LATIN1) {
//        asLatin1[0] ^= 0x1;
//      } else if (encoding == UTF16) {
//        asUtf16[0] ^= 0x1;
//      }
//    }
//    #endif
//  }
//}
///*
//  Local<Number> Number::New(double value) {
//    return reinterpret_cast<Number*>(handlizeNumber(value));
//  }
//
//
//  double Number::Value() const {
//    if (V8Monkey::V8MonkeyCommon::CheckDeath("Number::Value")) {
//      return 0;
//    }
//
//    V8Monkey::V8Number* v8number = CONVERT_FROM_API(Number, V8Number, this);
//    return v8number->Value();
//  }
//
//
//  Local<Integer> Integer::New(int32_t value) {
//    return reinterpret_cast<Integer*>(handlizeNumber(value));
//  }
//
//
//  Local<Integer> Integer::NewFromUnsigned(uint32_t value) {
//    return reinterpret_cast<Integer*>(handlizeNumber(value));
//  }
//
//
//   * I'm somewhat unclear as to to the point of the Integer constructors that take an isolate. They ultimately end up
//   * in the internal handlescope's CreateHandle method which requires the supplied isolate to be current anyway.
//   * Thus I simply defer to the standard versions
//   * TODO: Should we follow V8 and assert when the isolate != current isolate?
//
//  Local<Integer> Integer::New(int32_t value, Isolate* i) {
//    return Integer::New(value);
//  }
//
//
//  Local<Integer> Integer::NewFromUnsigned(uint32_t value, Isolate* i) {
//    return Integer::NewFromUnsigned(value);
//  }
//
//
//  int64_t Integer::Value() const {
//    if (V8Monkey::V8MonkeyCommon::CheckDeath("Integer::Value")) {
//      return 0;
//    }
//
//    V8Monkey::V8Number* v8number = CONVERT_FROM_API(Integer, V8Number, this);
//    return static_cast<int64_t>(v8number->Value());
//  }
//
//
//  int32_t Int32::Value() const {
//    if (V8Monkey::V8MonkeyCommon::CheckDeath("Int32::Value")) {
//      return 0;
//    }
//
//    V8Monkey::V8Number* v8number = CONVERT_FROM_API(Int32, V8Number, this);
//    return static_cast<int64_t>(v8number->Value());
//  }
//
//
//  uint32_t Uint32::Value() const {
//    if (V8Monkey::V8MonkeyCommon::CheckDeath("Uint32::Value")) {
//      return 0;
//    }
//
//    V8Monkey::V8Number* v8number = CONVERT_FROM_API(Uint32, V8Number, this);
//    return static_cast<int64_t>(v8number->Value());
//  }
//
//
//  namespace V8Monkey {
//    void V8Number::classifyNumber() {
//      // Classify the number. We follow V8, and build a union to capture the bit pattern to distinguish -0.0. The V8
//      // code assumes doubles are IEEE 754 double-precision, so we shall too.
//      union Bits {
//        double dval;
//        uint64_t uval;
//      };
//
//      Bits negZero = {-0.0};
//
//      int classification = std::fpclassify(value);
//      Bits bitRepresentation = {value};
//
//      if (classification == FP_NAN || classification == FP_INFINITE || bitRepresentation.uval == negZero.uval) {
//        type = V8Monkey::V8Number::NUMBER;
//      } else {
//        int32_t asInt32 = static_cast<int32_t>(value);
//        uint32_t asUint32 = static_cast<uint32_t>(value);
//
//        if (asUint32 == value) {
//          type = asUint32 < 0x10000000 ? V8Monkey::V8Number::VAL32 : V8Monkey::V8Number::UINT32;
//        } else if (asInt32 == value) {
//          type = V8Monkey::V8Number::INT32;
//        } else {
//          type = V8Monkey::V8Number::NUMBER;
//        }
//      }
//    }
//  }
//}
//*/
