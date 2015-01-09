#include <cmath>
#include <limits>

#include "v8.h"

#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;


V8MONKEY_TEST(Number001, "Value works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(n->Value() == d, "Value correct");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number002, "IsUndefined works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsUndefined(), "IsUndefined reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number003, "IsNull works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsNull(), "IsNull reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number004, "IsTrue works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsTrue(), "IsTrue reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number005, "IsFalse works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsFalse(), "IsFalse reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number006, "IsString works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsString(), "IsString reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number007, "IsFunction works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsFunction(), "IsFunction reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number008, "IsArray works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsArray(), "IsArray reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number009, "IsObject works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsObject(), "IsObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number010, "IsBoolean works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsBoolean(), "IsBoolean reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number011, "IsNumber works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(n->IsNumber(), "IsNumber reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number012, "IsExternal works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsExternal(), "IsExternal reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number013, "IsDate works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsDate(), "IsDate reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number014, "IsBooleanObject works correctly") {
  double d = 123.45;

  V8::Initialize();
  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsBooleanObject(), "IsBooleanObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number015, "IsNumberObject works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsNumberObject(), "IsNumberObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number016, "IsStringObject works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsStringObject(), "IsStringObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number017, "IsNativeError works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsNativeError(), "IsNativeError reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number018, "IsRegExp works correctly") {
  double d = 123.45;
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsRegExp(), "IsRegExp reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number019, "Works correctly for NaN") {
  double d = std::numeric_limits<double>::quiet_NaN();
  V8::Initialize();

  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(std::isnan(n->Value()), "NaN works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


#define NUMBERISINT32TEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Number##testNumber, "IsInt32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
\
    V8MONKEY_CHECK(n->IsInt32() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERISINT32TEST(020, 1, 123.45, false)
NUMBERISINT32TEST(021, 2, 123.65, false)
NUMBERISINT32TEST(022, 3, -123.45, false)
NUMBERISINT32TEST(023, 4, -123.67, false)
NUMBERISINT32TEST(024, 5, 123, true)
NUMBERISINT32TEST(025, 6, -1, true)
NUMBERISINT32TEST(026, 7, 0xffffffff, false)
NUMBERISINT32TEST(027, 8, -0.0, false)
NUMBERISINT32TEST(028, 9, 0.0, true)
NUMBERISINT32TEST(029, 10, std::numeric_limits<double>::infinity(), false)
NUMBERISINT32TEST(030, 11, std::numeric_limits<double>::quiet_NaN(), false)
#undef NUMBERISINT32TEST


#define NUMBERISUINT32TEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Number##testNumber, "IsUint32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
\
    V8MONKEY_CHECK(n->IsUint32() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERISUINT32TEST(031, 1, 123.45, false)
NUMBERISUINT32TEST(032, 2, 123.65, false)
NUMBERISUINT32TEST(033, 3, -123.45, false)
NUMBERISUINT32TEST(034, 4, -123.67, false)
NUMBERISUINT32TEST(035, 5, 123, true)
NUMBERISUINT32TEST(036, 6, -1, false)
NUMBERISUINT32TEST(037, 7, 0xffffffff, true)
NUMBERISUINT32TEST(038, 8, -0.0, false)
NUMBERISUINT32TEST(039, 9, 0.0, true)
NUMBERISUINT32TEST(040, 10, std::numeric_limits<double>::infinity(), false)
NUMBERISUINT32TEST(041, 11, std::numeric_limits<double>::quiet_NaN(), false)
#undef NUMBERISUINT32TEST


#define NUMBERVALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Number##testNumber, "Value works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Number> n = Number::New(value); \
\
    V8MONKEY_CHECK(n->Value() == value, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERVALUETEST(042, 1, 123.45)
NUMBERVALUETEST(043, 2, 123.65)
NUMBERVALUETEST(044, 3, -123.45)
NUMBERVALUETEST(045, 4, -123.67)
NUMBERVALUETEST(046, 5, 123)
NUMBERVALUETEST(047, 6, -1)
NUMBERVALUETEST(048, 7, 0xffffffff)
NUMBERVALUETEST(049, 8, -0.0)
NUMBERVALUETEST(050, 9, 0.0)
NUMBERVALUETEST(051, 10, std::numeric_limits<double>::infinity())
#undef NUMBERVALUETEST


V8MONKEY_TEST(Number052, "Value works correctly (10)") {
  V8::Initialize();

  {
    HandleScope h;
    double value = std::numeric_limits<double>::quiet_NaN();
    Local<Number> n = Number::New(value);

    V8MONKEY_CHECK(isnan(n->Value()), "Correct value returned");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


#define NUMBERNUMBERVALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Number##testNumber, "NumberValue works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Number> n = Number::New(value); \
\
    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERNUMBERVALUETEST(053, 1, 123.45)
NUMBERNUMBERVALUETEST(054, 2, 123.65)
NUMBERNUMBERVALUETEST(055, 3, -123.45)
NUMBERNUMBERVALUETEST(056, 4, -123.67)
NUMBERNUMBERVALUETEST(057, 5, 123)
NUMBERNUMBERVALUETEST(058, 6, -1)
NUMBERNUMBERVALUETEST(059, 7, 0xffffffff)
NUMBERNUMBERVALUETEST(060, 8, -0.0)
NUMBERNUMBERVALUETEST(061, 9, 0.0)
NUMBERNUMBERVALUETEST(062, 10, std::numeric_limits<double>::infinity())
#undef NUMBERNUMBERVALUETEST


V8MONKEY_TEST(Number063, "NumberValue works correctly (10)") {
  V8::Initialize();

  {
    HandleScope h;
    double value = std::numeric_limits<double>::quiet_NaN();
    Local<Number> n = Number::New(value);

    V8MONKEY_CHECK(isnan(n->NumberValue()), "Correct value returned");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


#define NUMBERTONUMBERTEST(testNumber, variant, val) \
V8MONKEY_TEST(Number##testNumber, "ToNumber works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
    Local<Number> n2 = n->ToNumber(); \
\
    V8MONKEY_CHECK(n2->NumberValue() == n->NumberValue(), "Correct value returned"); \
    V8MONKEY_CHECK(n2 == n, "Same object returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERTONUMBERTEST(064, 1, 123.45)
NUMBERTONUMBERTEST(065, 2, 123.65)
NUMBERTONUMBERTEST(066, 3, -123.45)
NUMBERTONUMBERTEST(067, 4, -123.67)
NUMBERTONUMBERTEST(068, 5, 123)
NUMBERTONUMBERTEST(069, 6, -1)
NUMBERTONUMBERTEST(070, 7, 0xffffffff)
NUMBERTONUMBERTEST(071, 8, -0.0)
NUMBERTONUMBERTEST(072, 9, 0.0)
NUMBERTONUMBERTEST(073, 10, std::numeric_limits<double>::infinity())
#undef NUMBERTONUMBERTEST


V8MONKEY_TEST(Number074, "ToNumber works correctly (10)") {
  V8::Initialize();

  {
    HandleScope h;
    double value = std::numeric_limits<double>::quiet_NaN();
    Local<Value> n = Number::New(value);
    Local<Number> n2 = n->ToNumber();

    V8MONKEY_CHECK(isnan(n2->Value()), "Correct value returned");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


#define NUMBERINTEGERVALUETEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Number##testNumber, "IntegerValue works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
\
    V8MONKEY_CHECK(n->IntegerValue() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERINTEGERVALUETEST(075, 1, 123.45, 123)
NUMBERINTEGERVALUETEST(076, 2, 123.65, 123)
NUMBERINTEGERVALUETEST(077, 3, -123.45, -123)
NUMBERINTEGERVALUETEST(078, 4, -123.67, -123)
NUMBERINTEGERVALUETEST(079, 5, 123, 123)
NUMBERINTEGERVALUETEST(080, 6, -1, -1)
NUMBERINTEGERVALUETEST(081, 7, 0xffffffff, 0xffffffff)
NUMBERINTEGERVALUETEST(082, 8, -0.0, 0)
NUMBERINTEGERVALUETEST(083, 9, 0.0, 0)
NUMBERINTEGERVALUETEST(084, 10, std::numeric_limits<double>::infinity(), static_cast<int64_t>(0x8000000000000000))
NUMBERINTEGERVALUETEST(085, 11, std::numeric_limits<double>::quiet_NaN(), static_cast<int64_t>(0x8000000000000000))
#undef NUMBERINTEGERVALUETEST


#define NUMBERTOINTEGERTEST(testNumber, variant, val, isSame) \
V8MONKEY_TEST(Number##testNumber, "ToInteger works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
    Local<Integer> n2 = n->ToInteger(); \
\
    V8MONKEY_CHECK(n2->Value() == n->IntegerValue(), "Correct value returned"); \
    V8MONKEY_CHECK((n2 == n) == isSame, "New object returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERTOINTEGERTEST(086, 1, 123.45, false)
NUMBERTOINTEGERTEST(087, 2, 123.65, false)
NUMBERTOINTEGERTEST(088, 3, -123.45, false)
NUMBERTOINTEGERTEST(089, 4, -123.67, false)
NUMBERTOINTEGERTEST(090, 5, 123, true)
NUMBERTOINTEGERTEST(091, 6, -1, true)
NUMBERTOINTEGERTEST(092, 7, 0xffffffff, false)
NUMBERTOINTEGERTEST(093, 8, -0.0, false)
NUMBERTOINTEGERTEST(094, 9, 0.0, true)
NUMBERTOINTEGERTEST(095, 10, std::numeric_limits<double>::infinity(), false)
NUMBERTOINTEGERTEST(096, 11, std::numeric_limits<double>::quiet_NaN(), false)
#undef NUMBERTOINTEGERTEST


#define NUMBERINT32VALUETEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Number##testNumber, "Int32Value works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
\
    V8MONKEY_CHECK(n->Int32Value() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERINT32VALUETEST(097, 1, 123.45, 123)
NUMBERINT32VALUETEST(098, 2, 123.65, 123)
NUMBERINT32VALUETEST(099, 3, -123.45, -123)
NUMBERINT32VALUETEST(100, 4, -123.67, -123)
NUMBERINT32VALUETEST(101, 5, 123, 123)
NUMBERINT32VALUETEST(102, 6, -1, -1)
NUMBERINT32VALUETEST(103, 7, 0xffffffff, -1)
NUMBERINT32VALUETEST(104, 8, -0.0, 0)
NUMBERINT32VALUETEST(105, 9, 0.0, 0)
NUMBERINT32VALUETEST(106, 10, std::numeric_limits<double>::infinity(), 0)
NUMBERINT32VALUETEST(107, 11, std::numeric_limits<double>::quiet_NaN(), 0)
#undef NUMBERINT32VALUETEST


#define NUMBERTOINT32TEST(testNumber, variant, val, isSame) \
V8MONKEY_TEST(Number##testNumber, "ToInt32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
    Local<Int32> n2 = n->ToInt32(); \
\
    V8MONKEY_CHECK(n2->Value() == n->Int32Value(), "Correct value returned"); \
    V8MONKEY_CHECK((n2 == n) == isSame, "New object returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERTOINT32TEST(108, 1, 123.45, false)
NUMBERTOINT32TEST(109, 2, 123.65, false)
NUMBERTOINT32TEST(110, 3, -123.45, false)
NUMBERTOINT32TEST(111, 4, -123.67, false)
NUMBERTOINT32TEST(112, 5, 123, true)
NUMBERTOINT32TEST(113, 6, -1, true)
NUMBERTOINT32TEST(114, 7, 0xffffffff, false)
NUMBERTOINT32TEST(115, 8, -0.0, false)
NUMBERTOINT32TEST(116, 9, 0.0, true)
NUMBERTOINT32TEST(117, 10, std::numeric_limits<double>::infinity(), false)
NUMBERTOINT32TEST(118, 11, std::numeric_limits<double>::quiet_NaN(), false)
#undef NUMBERTOINT32TEST


#define NUMBERUINT32VALUETEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Number##testNumber, "Uint32Value works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
\
    V8MONKEY_CHECK(n->Uint32Value() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERUINT32VALUETEST(119, 1, 123.45, 123)
NUMBERUINT32VALUETEST(120, 2, 123.65, 123)
NUMBERUINT32VALUETEST(121, 3, -123.45, 0xffffff85)
NUMBERUINT32VALUETEST(122, 4, -123.67, 0xffffff85)
NUMBERUINT32VALUETEST(123, 5, 123, 123)
NUMBERUINT32VALUETEST(124, 6, -1, 0xffffffff)
NUMBERUINT32VALUETEST(125, 7, 0xffffffff, 0xffffffff)
NUMBERUINT32VALUETEST(126, 8, -0.0, 0)
NUMBERUINT32VALUETEST(127, 9, 0.0, 0)
NUMBERUINT32VALUETEST(128, 10, std::numeric_limits<double>::infinity(), 0)
NUMBERUINT32VALUETEST(129, 11, std::numeric_limits<double>::quiet_NaN(), 0)
#undef NUMBERUINT32VALUETEST


#define NUMBERTOUINT32TEST(testNumber, variant, val, isSame) \
V8MONKEY_TEST(Number##testNumber, "ToUint32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
    Local<Uint32> n2 = n->ToUint32(); \
\
    V8MONKEY_CHECK(n2->Value() == n->Uint32Value(), "Correct value returned"); \
    V8MONKEY_CHECK((n2 == n) == isSame, "New object returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERTOUINT32TEST(130, 1, 123.45, false)
NUMBERTOUINT32TEST(131, 2, 123.65, false)
NUMBERTOUINT32TEST(132, 3, -123.45, false)
NUMBERTOUINT32TEST(133, 4, -123.67, false)
NUMBERTOUINT32TEST(134, 5, 123, true)
NUMBERTOUINT32TEST(135, 6, -1, true)
NUMBERTOUINT32TEST(136, 7, 0xffffffff, false)
NUMBERTOUINT32TEST(137, 8, -0.0, false)
NUMBERTOUINT32TEST(138, 9, 0.0, true)
NUMBERTOUINT32TEST(139, 10, std::numeric_limits<double>::infinity(), false)
NUMBERTOUINT32TEST(140, 11, std::numeric_limits<double>::quiet_NaN(), false)
#undef NUMBERTOUINT32TEST


#define NUMBERTOARRAYINDEXEMPTYTEST(testNumber, variant, val, shouldBeEmpty) \
V8MONKEY_TEST(Number##testNumber, "ToArrayIndex works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
    Local<Uint32> indexHandle = n->ToArrayIndex(); \
\
    V8MONKEY_CHECK(indexHandle.IsEmpty() == shouldBeEmpty, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERTOARRAYINDEXEMPTYTEST(141, 1, 123.45, true)
NUMBERTOARRAYINDEXEMPTYTEST(142, 2, 123.65, true)
NUMBERTOARRAYINDEXEMPTYTEST(143, 3, -123.45, true)
NUMBERTOARRAYINDEXEMPTYTEST(144, 4, -123.67, true)
NUMBERTOARRAYINDEXEMPTYTEST(145, 5, 123, false)
NUMBERTOARRAYINDEXEMPTYTEST(146, 6, -1, true)
NUMBERTOARRAYINDEXEMPTYTEST(147, 7, 0xffffffff, false)
NUMBERTOARRAYINDEXEMPTYTEST(148, 8, -0.0, false)
NUMBERTOARRAYINDEXEMPTYTEST(149, 9, 0.0, false)
NUMBERTOARRAYINDEXEMPTYTEST(150, 10, std::numeric_limits<double>::infinity(), true)
NUMBERTOARRAYINDEXEMPTYTEST(151, 11, std::numeric_limits<double>::quiet_NaN(), true)
#undef NUMBERTOARRAYINDEXTEST


#define NUMBERTOARRAYINDEXTEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Number##testNumber, "ToArrayIndex returns correct value (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
    Local<Uint32> indexHandle = n->ToArrayIndex(); \
\
    V8MONKEY_CHECK(indexHandle->Value() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERTOARRAYINDEXTEST(152, 1, 123, 123)
NUMBERTOARRAYINDEXTEST(153, 2, 0xffffffff, 0xffffffff)
NUMBERTOARRAYINDEXTEST(154, 3, -0.0, 0)
NUMBERTOARRAYINDEXTEST(155, 4, 0.0, 0)
#undef NUMBERTOARRAYINDEXTEST


#define NUMBERTOARRAYINDEXSAMETEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Number##testNumber, "ToArrayIndex returns correct slot (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    double value = val; \
    Local<Value> n = Number::New(value); \
    Local<Uint32> indexHandle = n->ToArrayIndex(); \
\
    V8MONKEY_CHECK((*indexHandle == *n) == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


NUMBERTOARRAYINDEXSAMETEST(156, 1, 123, true)
NUMBERTOARRAYINDEXSAMETEST(157, 2, 0xffffffff, false)
NUMBERTOARRAYINDEXSAMETEST(158, 3, -0.0, false)
NUMBERTOARRAYINDEXSAMETEST(159, 4, 0.0, true)
#undef NUMBERTOARRAYINDEXSAMETEST


V8MONKEY_TEST(Integer001, "IsUndefined works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsUndefined(), "IsUndefined reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer002, "IsNull works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsNull(), "IsNull reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer003, "IsTrue works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsTrue(), "IsTrue reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer004, "IsFalse works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsFalse(), "IsFalse reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer005, "IsString works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsString(), "IsString reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer006, "IsFunction works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsFunction(), "IsFunction reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer007, "IsArray works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsArray(), "IsArray reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer008, "IsObject works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsObject(), "IsObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer009, "IsBoolean works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsBoolean(), "IsBoolean reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer010, "IsExternal works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsExternal(), "IsExternal reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer011, "IsDate works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsDate(), "IsDate reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer012, "IsBooleanObject works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsBooleanObject(), "IsBooleanObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer013, "IsNumberObject works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsNumberObject(), "IsNumberObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer014, "IsStringObject works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsStringObject(), "IsStringObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer015, "IsNativeError works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsNativeError(), "IsNativeError reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer016, "IsRegExp works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(!n->IsRegExp(), "IsRegExp reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


#define INTEGERISINT32TEST(testNumber, variant, val, constructor, expected) \
V8MONKEY_TEST(Integer##testNumber, "IsInt32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    int32_t value = val; \
    Local<Value> n = Integer::constructor(value); \
\
    V8MONKEY_CHECK(n->IsInt32() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


INTEGERISINT32TEST(018, 1, 123, New, true)
INTEGERISINT32TEST(019, 2, -1, New, true)
INTEGERISINT32TEST(020, 3, 0, New, true)
INTEGERISINT32TEST(021, 4, 123, NewFromUnsigned, true)
INTEGERISINT32TEST(022, 5, 0xffffffff, NewFromUnsigned, false)
INTEGERISINT32TEST(023, 6, 0, NewFromUnsigned, true)
#undef INTEGERISINT32TEST


#define INTEGERISUINT32TEST(testNumber, variant, val, constructor, expected) \
V8MONKEY_TEST(Integer##testNumber, "IsUint32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    int32_t value = val; \
    Local<Value> n = Integer::constructor(value); \
\
    V8MONKEY_CHECK(n->IsUint32() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


INTEGERISUINT32TEST(024, 1, 123, New, true)
INTEGERISUINT32TEST(025, 2, -1, New, false)
INTEGERISUINT32TEST(026, 3, 0, New, true)
INTEGERISUINT32TEST(027, 4, 123, NewFromUnsigned, true)
INTEGERISUINT32TEST(028, 5, 0xffffffff, NewFromUnsigned, true)
INTEGERISUINT32TEST(029, 6, 0, NewFromUnsigned, true)
#undef INTEGERISUINT32TEST


#define INTEGERVALUETEST(testNumber, variant, val, type, constructor) \
V8MONKEY_TEST(Integer##testNumber, "Value works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    type value = val; \
    Local<Integer> n = Integer::constructor(value); \
\
    V8MONKEY_CHECK(n->Value() == value, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


INTEGERVALUETEST(030, 1, 123, int32_t, New)
INTEGERVALUETEST(031, 2, -1, int32_t, New)
INTEGERVALUETEST(032, 3, 0, int32_t, New)
INTEGERVALUETEST(033, 4, 123, uint32_t, NewFromUnsigned)
INTEGERVALUETEST(034, 5, 0xffffffff, uint32_t, NewFromUnsigned)
INTEGERVALUETEST(035, 6, 0, uint32_t, NewFromUnsigned)
#undef INTEGERVALUETEST


#define INTEGERNUMBERVALUETEST(testNumber, variant, val, constructor) \
V8MONKEY_TEST(Integer##testNumber, "NumberValue works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    int32_t value = val; \
    Local<Integer> n = Integer::constructor(value); \
\
    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


INTEGERNUMBERVALUETEST(036, 1, 123, New)
INTEGERNUMBERVALUETEST(037, 2, -1, New)
INTEGERNUMBERVALUETEST(038, 3, 0, New)
INTEGERNUMBERVALUETEST(039, 4, 123, NewFromUnsigned)
INTEGERNUMBERVALUETEST(040, 5, 0xffffffff, NewFromUnsigned)
INTEGERNUMBERVALUETEST(041, 6, 0, NewFromUnsigned)
#undef INTEGERNUMBERVALUETEST


V8MONKEY_TEST(Integer042, "NumberValue works correctly (7)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::infinity();
    Local<Value> temp = Number::New(value); 
    Local<Integer> n = temp->ToInteger(); 

    V8MONKEY_CHECK(n->NumberValue() == value, "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Integer043, "NumberValue works correctly (8)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::quiet_NaN();
    Local<Value> temp = Number::New(value); 
    Local<Integer> n = temp->ToInteger(); 

    V8MONKEY_CHECK(std::isnan(n->NumberValue()), "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}


#define INTEGERINTEGERVALUETEST(testNumber, variant, val, constructor) \
V8MONKEY_TEST(Integer##testNumber, "IntegerValue works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    int32_t value = val; \
    Local<Integer> n = Integer::constructor(value); \
\
    V8MONKEY_CHECK(n->IntegerValue() == n->Value(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


INTEGERINTEGERVALUETEST(044, 1, 123, New)
INTEGERINTEGERVALUETEST(045, 2, -1, New)
INTEGERINTEGERVALUETEST(046, 3, 0, New)
INTEGERINTEGERVALUETEST(047, 4, 123, NewFromUnsigned)
INTEGERINTEGERVALUETEST(048, 5, 0xffffffff, NewFromUnsigned)
INTEGERINTEGERVALUETEST(049, 6, 0, NewFromUnsigned)
#undef INTEGERINTEGERVALUETEST


V8MONKEY_TEST(Integer050, "Isolate variant works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::New(d, Isolate::GetCurrent());

    V8MONKEY_CHECK(n->Value() == d, "Value correct");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer051, "Unsigned isolate variant works correctly") {
  uint32_t d = 0xffffffff;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> n = Integer::NewFromUnsigned(d, Isolate::GetCurrent());
    V8MONKEY_CHECK(n->Value() == d, "IsUint32 reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_001, "IsUndefined works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsUndefined(), "IsUndefined reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_002, "IsNull works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsNull(), "IsNull reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_003, "IsTrue works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsTrue(), "IsTrue reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_004, "IsFalse works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsFalse(), "IsFalse reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_005, "IsString works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsString(), "IsString reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_006, "IsFunction works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsFunction(), "IsFunction reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_007, "IsArray works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsArray(), "IsArray reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_008, "IsObject works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsObject(), "IsObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_009, "IsBoolean works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsBoolean(), "IsBoolean reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_010, "IsExternal works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsExternal(), "IsExternal reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_011, "IsDate works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsDate(), "IsDate reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_012, "IsBooleanObject works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsBooleanObject(), "IsBooleanObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_013, "IsNumberObject works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsNumberObject(), "IsNumberObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_014, "IsStringObject works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsStringObject(), "IsStringObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_015, "IsNativeError works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsNativeError(), "IsNativeError reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Int32_016, "IsRegExp works correctly") {
  int32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::New(d);
    Local<Int32> n = temp->ToInt32();

    V8MONKEY_CHECK(!n->IsRegExp(), "IsRegExp reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


#define INT32ISINT32TEST(testNumber, variant, val) \
V8MONKEY_TEST(Int32_##testNumber, "IsInt32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    int32_t value = val; \
    Local<Integer> temp = Integer::New(value); \
    Local<Value> n = temp->ToInt32(); \
\
    V8MONKEY_CHECK(n->IsInt32(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


INT32ISINT32TEST(017, 1, 123)
INT32ISINT32TEST(018, 2, -1)
INT32ISINT32TEST(019, 3, 0)
#undef INT32ISINT32TEST


#define INT32ISUINT32TEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Int32_##testNumber, "IsUint32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    int32_t value = val; \
    Local<Integer> temp = Integer::New(value); \
    Local<Value> n = temp->ToInt32(); \
\
    V8MONKEY_CHECK(n->IsUint32() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


INT32ISUINT32TEST(020, 1, 123, true)
INT32ISUINT32TEST(021, 2, -1, false)
INT32ISUINT32TEST(022, 3, 0, true)
#undef INT32ISUINT32TEST


#define INT32VALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Int32_##testNumber, "Value works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    int32_t value = val; \
    Local<Integer> temp = Integer::New(value); \
    Local<Int32> n = temp->ToInt32(); \
\
    V8MONKEY_CHECK(n->Value() == value, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


INT32VALUETEST(023, 1, 123)
INT32VALUETEST(024, 2, -1)
INT32VALUETEST(025, 3, 0)
#undef INT32VALUETEST


#define INT32NUMBERVALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Int32_##testNumber, "NumberValue works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    int32_t value = val; \
    Local<Integer> temp = Integer::New(value); \
    Local<Int32> n = temp->ToInt32(); \
\
    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


INT32NUMBERVALUETEST(026, 1, 123)
INT32NUMBERVALUETEST(027, 2, -1)
INT32NUMBERVALUETEST(028, 3, 0)
#undef INT32NUMBERVALUETEST


V8MONKEY_TEST(Int32_029, "NumberValue works correctly (4)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::infinity();
    Local<Value> temp = Number::New(value); 
    Local<Int32> n = temp->ToInt32(); 

    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Int32_030, "NumberValue works correctly (5)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::quiet_NaN();
    Local<Value> temp = Number::New(value); 
    Local<Int32> n = temp->ToInt32(); 

    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}


#define INT32INT32VALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Int32_##testNumber, "Int32Value works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    int32_t value = val; \
    Local<Integer> temp = Integer::New(value); \
    Local<Int32> n = temp->ToInt32(); \
\
    V8MONKEY_CHECK(n->Int32Value() == n->Value(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}



INT32INT32VALUETEST(031, 1, 123)
INT32INT32VALUETEST(032, 2, -1)
INT32INT32VALUETEST(033, 3, 0)
#undef INTEGERINT32VALUETEST


V8MONKEY_TEST(Int32_034, "Conversions of special values work as expected (1)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::infinity();
    Local<Value> temp = Number::New(value); 
    Local<Int32> n = temp->ToInt32(); 
    Local<Number> n2 = n->ToNumber();

    V8MONKEY_CHECK(n2->Value() == n->Value(), "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Int32_035, "Conversions of special values work as expected (2)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::quiet_NaN();
    Local<Value> temp = Number::New(value); 
    Local<Int32> n = temp->ToInt32(); 
    Local<Number> n2 = n->ToNumber();

    V8MONKEY_CHECK(n2->Value() == n->Value(), "Correct value returned"); 
    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Uint32_001, "IsUndefined works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsUndefined(), "IsUndefined reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_002, "IsNull works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsNull(), "IsNull reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_003, "IsTrue works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsTrue(), "IsTrue reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_004, "IsFalse works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsFalse(), "IsFalse reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_005, "IsString works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsString(), "IsString reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_006, "IsFunction works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsFunction(), "IsFunction reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_007, "IsArray works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsArray(), "IsArray reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_008, "IsObject works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsObject(), "IsObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_009, "IsBoolean works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsBoolean(), "IsBoolean reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_010, "IsExternal works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsExternal(), "IsExternal reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_011, "IsDate works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsDate(), "IsDate reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_012, "IsBooleanObject works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsBooleanObject(), "IsBooleanObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_013, "IsNumberObject works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsNumberObject(), "IsNumberObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_014, "IsStringObject works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsStringObject(), "IsStringObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_015, "IsNativeError works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsNativeError(), "IsNativeError reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Uint32_016, "IsRegExp works correctly") {
  uint32_t d = 123;
  V8::Initialize();

  {
    HandleScope h;
    Local<Integer> temp = Integer::NewFromUnsigned(d);
    Local<Uint32> n = temp->ToUint32();

    V8MONKEY_CHECK(!n->IsRegExp(), "IsRegExp reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


#define UINT32ISINT32TEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Uint32_##testNumber, "IsInt32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    uint32_t value = val; \
    Local<Integer> temp = Integer::NewFromUnsigned(value); \
    Local<Value> n = temp->ToUint32(); \
\
    V8MONKEY_CHECK(n->IsInt32() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


UINT32ISINT32TEST(017, 1, 123, true)
UINT32ISINT32TEST(018, 2, 0xffffffff, false)
UINT32ISINT32TEST(019, 3, 0, true)
#undef UINT32ISINT32TEST


#define UINT32ISUINT32TEST(testNumber, variant, val) \
V8MONKEY_TEST(Uint32_##testNumber, "IsUint32 works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    uint32_t value = val; \
    Local<Integer> temp = Integer::NewFromUnsigned(value); \
    Local<Value> n = temp->ToUint32(); \
\
    V8MONKEY_CHECK(n->IsUint32(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


UINT32ISUINT32TEST(020, 1, 123)
UINT32ISUINT32TEST(021, 2, 0xffffffff)
UINT32ISUINT32TEST(022, 3, 0)
#undef UINT32ISUINT32TEST


#define UINT32VALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Uint32_##testNumber, "Value works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    uint32_t value = val; \
    Local<Integer> temp = Integer::NewFromUnsigned(value); \
    Local<Uint32> n = temp->ToUint32(); \
\
    V8MONKEY_CHECK(n->Value() == value, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


UINT32VALUETEST(023, 1, 123)
UINT32VALUETEST(024, 2, 0xffffffff)
UINT32VALUETEST(025, 3, 0)
#undef UINT32VALUETEST


#define UINT32NUMBERVALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Uint32_##testNumber, "NumberValue works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    uint32_t value = val; \
    Local<Integer> temp = Integer::NewFromUnsigned(value); \
    Local<Uint32> n = temp->ToUint32(); \
\
    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


UINT32NUMBERVALUETEST(026, 1, 123)
UINT32NUMBERVALUETEST(027, 2, 0xffffffff)
UINT32NUMBERVALUETEST(028, 3, 0)
#undef UINT32NUMBERVALUETEST


V8MONKEY_TEST(Uint32_029, "NumberValue works correctly (4)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::infinity();
    Local<Value> temp = Number::New(value); 
    Local<Uint32> n = temp->ToUint32(); 

    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Uint32_030, "NumberValue works correctly (5)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::quiet_NaN();
    Local<Value> temp = Number::New(value); 
    Local<Uint32> n = temp->ToUint32(); 

    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}


#define UINT32UINT32VALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Uint32_##testNumber, "Uint32Value works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    uint32_t value = val; \
    Local<Integer> temp = Integer::NewFromUnsigned(value); \
    Local<Uint32> n = temp->ToUint32(); \
\
    V8MONKEY_CHECK(n->Uint32Value() == n->Value(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}



UINT32UINT32VALUETEST(031, 1, 123)
UINT32UINT32VALUETEST(032, 2, 0xffffffff)
UINT32UINT32VALUETEST(033, 3, 0)
#undef INTEGERUINT32VALUETEST


V8MONKEY_TEST(Uint32_034, "Conversions of special values work as expected (1)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::infinity();
    Local<Value> temp = Number::New(value); 
    Local<Uint32> n = temp->ToUint32(); 
    Local<Number> n2 = n->ToNumber();

    V8MONKEY_CHECK(n2->Value() == n->Value(), "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Uint32_035, "Conversions of special values work as expected (2)") {
  V8::Initialize(); 

  { 
    HandleScope h; 
    double value = std::numeric_limits<double>::quiet_NaN();
    Local<Value> temp = Number::New(value); 
    Local<Uint32> n = temp->ToUint32(); 
    Local<Number> n2 = n->ToNumber();

    V8MONKEY_CHECK(n2->Value() == n->Value(), "Correct value returned"); 
    V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); 
  } 
 
  Isolate::GetCurrent()->Exit(); 
  V8::Dispose(); 
}
