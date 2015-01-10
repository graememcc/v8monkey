#include <cmath>
#include <limits>

#include "v8.h"

#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;


V8MONKEY_TEST(Boolean001, "IsUndefined works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsUndefined(), "IsUndefined reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean002, "IsNull works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsNull(), "IsNull reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean003, "IsString works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsString(), "IsString reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean004, "IsFunction works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsFunction(), "IsFunction reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean005, "IsArray works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsArray(), "IsArray reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean006, "IsObject works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsObject(), "IsObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean007, "IsNumber works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsNumber(), "IsNumber reports correct result");
  }

  Isolate::GetCurrent()->Exit();
}


V8MONKEY_TEST(Boolean008, "IsExternal works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsExternal(), "IsExternal reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean009, "IsDate works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsDate(), "IsDate reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean010, "IsBooleanObject works correctly") {
  bool b = true;

  V8::Initialize();
  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsBooleanObject(), "IsBooleanObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean011, "IsNumberObject works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsNumberObject(), "IsNumberObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean012, "IsStringObject works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsStringObject(), "IsStringObject reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean013, "IsNativeError works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsNativeError(), "IsNativeError reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean014, "IsRegExp works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsRegExp(), "IsRegExp reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean015, "IsInt32 works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsInt32(), "IsInt32 reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean016, "IsUint32 works correctly") {
  bool b = true;
  V8::Initialize();

  {
    HandleScope h;
    Handle<Boolean> n = Boolean::New(b);

    V8MONKEY_CHECK(!n->IsUint32(), "IsUint32 reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


#define BOOLEANISBOOLEANTEST(testNumber, variant, val) \
V8MONKEY_TEST(Boolean##testNumber, "IsBoolean works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Value> b = Boolean::New(value); \
\
    V8MONKEY_CHECK(b->IsBoolean(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANISBOOLEANTEST(017, 1, true)
BOOLEANISBOOLEANTEST(018, 2, false)
#undef BOOLEANISBOOLEANTEST


#define BOOLEANISTRUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Boolean##testNumber, "IsTrue works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Value> b = Boolean::New(value); \
\
    V8MONKEY_CHECK(b->IsTrue() == value, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANISTRUETEST(019, 1, true)
BOOLEANISTRUETEST(020, 2, false)
#undef BOOLEANISFALSETEST


#define BOOLEANISFALSETEST(testNumber, variant, val) \
V8MONKEY_TEST(Boolean##testNumber, "IsFalse works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Value> b = Boolean::New(value); \
\
    V8MONKEY_CHECK(b->IsFalse() == !value, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANISFALSETEST(021, 1, true)
BOOLEANISFALSETEST(022, 2, false)
#undef BOOLEANISFALSETEST


#define BOOLEANVALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Boolean##testNumber, "Value works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Boolean> b = Boolean::New(value); \
\
    V8MONKEY_CHECK(b->Value() == value, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANVALUETEST(023, 1, true)
BOOLEANVALUETEST(024, 2, false)
#undef BOOLEANVALUETEST


#define BOOLEANBOOLEANVALUETEST(testNumber, variant, val) \
V8MONKEY_TEST(Boolean##testNumber, "BooleanValue works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Boolean> b = Boolean::New(value); \
\
    V8MONKEY_CHECK(b->BooleanValue() == b->Value(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANBOOLEANVALUETEST(025, 1, true)
BOOLEANBOOLEANVALUETEST(026, 2, false)
#undef BOOLEANBOOLEANVALUETEST


#define BOOLEANNUMERICVALUETEST(testNumber, variant, val, method, expected) \
V8MONKEY_TEST(Boolean##testNumber, #method " works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Value> b = Boolean::New(value); \
\
    V8MONKEY_CHECK(b->method() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANNUMERICVALUETEST(028, 1, true, NumberValue, 1)
BOOLEANNUMERICVALUETEST(029, 2, false, NumberValue, 0.0)
BOOLEANNUMERICVALUETEST(030, 1, true, IntegerValue, 1)
BOOLEANNUMERICVALUETEST(031, 2, false, IntegerValue, 0)
BOOLEANNUMERICVALUETEST(032, 1, true, Int32Value, 1)
BOOLEANNUMERICVALUETEST(033, 2, false, Int32Value, 0)
BOOLEANNUMERICVALUETEST(034, 1, true, Uint32Value, 1)
BOOLEANNUMERICVALUETEST(035, 2, false, Uint32Value, 0)
#undef BOOLEANNUMERICVALUETEST


#define BOOLEANTOBOOLEANTEST(testNumber, variant, val) \
V8MONKEY_TEST(Boolean##testNumber, "ToBoolean works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Value> b = Boolean::New(value); \
    Local<Boolean> n = b->ToBoolean(); \
\
    V8MONKEY_CHECK(n->Value() == b->BooleanValue(), "Correct value returned"); \
    V8MONKEY_CHECK(n == b, "Same object returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}

BOOLEANTOBOOLEANTEST(036, 1, true)
BOOLEANTOBOOLEANTEST(037, 2, false)


#define BOOLEANTOTYPETEST(testNumber, variant, val, type) \
V8MONKEY_TEST(Boolean##testNumber, "To" #type " works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Value> b = Boolean::New(value); \
    Local<type> n = b->To##type(); \
\
    V8MONKEY_CHECK(n->Value() == b->type##Value(), "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANTOTYPETEST(038, 1, true, Number)
BOOLEANTOTYPETEST(039, 2, false, Number)
BOOLEANTOTYPETEST(040, 1, true, Integer)
BOOLEANTOTYPETEST(041, 2, false, Integer)
BOOLEANTOTYPETEST(042, 1, true, Int32)
BOOLEANTOTYPETEST(043, 2, false, Int32)
BOOLEANTOTYPETEST(044, 1, true, Uint32)
BOOLEANTOTYPETEST(045, 2, false, Uint32)
#undef BOOLEANTOTYPETEST


#define BOOLEANTOARRAYINDEXTEST(testNumber, variant, val, expected) \
V8MONKEY_TEST(Boolean##testNumber, "ToArrayIndex returns correct value (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Value> b = Boolean::New(value); \
    Local<Uint32> indexHandle = b->ToArrayIndex(); \
\
    V8MONKEY_CHECK(indexHandle->Value() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANTOARRAYINDEXTEST(046, 1, true, 1)
BOOLEANTOARRAYINDEXTEST(047, 2, false, 0)
#undef BOOLEANTOARRAYINDEXTEST


#define BOOLEANSTATICTEST(testNumber, variant, method, expected) \
V8MONKEY_TEST(Boolean##testNumber, #method " returns correct value (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    Handle<Boolean> b = method(); \
\
    V8MONKEY_CHECK(b->Value() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANSTATICTEST(048, 1, True, true)
BOOLEANSTATICTEST(049, 2, False, false)
#undef BOOLEANSTATICTEST


#define BOOLEANSTATICISOLATETEST(testNumber, variant, method, expected) \
V8MONKEY_TEST(Boolean##testNumber, #method " with isolate returns correct value (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    Handle<Boolean> b = method(Isolate::GetCurrent()); \
\
    V8MONKEY_CHECK(b->Value() == expected, "Correct value returned"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANSTATICISOLATETEST(050, 1, True, true)
BOOLEANSTATICISOLATETEST(051, 2, False, false)
#undef BOOLEANSTATICISOLATETEST


#define BOOLEANEQUALITYTEST(testNumber, variant, val, method) \
V8MONKEY_TEST(Boolean##testNumber, #method " works correctly (" #variant ")") { \
  V8::Initialize(); \
\
  { \
    HandleScope h; \
    bool value = val; \
    Handle<Value> b = Boolean::New(value); \
    Handle<Value> b2 = Boolean::New(value); \
    Handle<Value> b3 = Boolean::New(!value); \
\
    V8MONKEY_CHECK(b->method(b), "Self-equality works correctly"); \
    V8MONKEY_CHECK(b->method(b2), "Equality works correctly"); \
    V8MONKEY_CHECK(b2->method(b), "Equality is symmetric"); \
    V8MONKEY_CHECK(!b->method(b3), "Inequality is correct"); \
  } \
 \
  Isolate::GetCurrent()->Exit(); \
  V8::Dispose(); \
}


BOOLEANEQUALITYTEST(052, 1, true, Equals)
BOOLEANEQUALITYTEST(053, 2, false, Equals)


V8MONKEY_TEST(Boolean054, "Equals works correctly (3)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(true);
    Local<Value> n = Number::New(1);

    V8MONKEY_CHECK(b->Equals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean055, "Equals works correctly (4)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(false);
    Local<Value> n = Number::New(0.0);

    V8MONKEY_CHECK(b->Equals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean056, "Equals works correctly (5)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(false);
    Local<Value> n = Number::New(-0.0);

    V8MONKEY_CHECK(b->Equals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean057, "Equals works correctly (6)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(true);
    Local<Value> n = Number::New(0.0);

    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean058, "Equals works correctly (6)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(true);
    Local<Value> n = Number::New(-0.0);

    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean059, "Equals works correctly (7)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(true);
    Local<Value> n = Number::New(123.45);

    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean060, "Equals works correctly (8)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(false);
    Local<Value> n = Number::New(123.45);

    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


BOOLEANEQUALITYTEST(061, 1, true, StrictEquals)
BOOLEANEQUALITYTEST(062, 2, false, StrictEquals)
#undef BOOLEANEQUALITYTEST


V8MONKEY_TEST(Boolean063, "StrictEquals works correctly (3)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(true);
    Local<Value> n = Number::New(1);

    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean064, "StrictEquals works correctly (4)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(false);
    Local<Value> n = Number::New(0.0);

    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean065, "StrictEquals works correctly (5)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(false);
    Local<Value> n = Number::New(-0.0);

    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean066, "StrictEquals works correctly (6)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(true);
    Local<Value> n = Number::New(0.0);

    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean067, "StrictEquals works correctly (6)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(true);
    Local<Value> n = Number::New(-0.0);

    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean068, "StrictEquals works correctly (7)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(true);
    Local<Value> n = Number::New(123.45);

    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Boolean069, "StrictEquals works correctly (8)") {
  V8::Initialize();

  {
    HandleScope h;
    Handle<Value> b = Boolean::New(false);
    Local<Value> n = Number::New(123.45);

    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}
