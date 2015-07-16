/*
// std::isnan
#include <cmath>

// std::numeric_limits
#include <limits>

// Boolean Integer Int32 Handle HandleScope Isolate Local Null Number Primitive Uint32 Undefined V8 Value
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;


#define BOOLEAN_IS_TEST(num, fn) \
V8MONKEY_TEST(Boolean##num, "Is" #fn " works correctly") { \
  Isolate* i {Isolate::New()}; \
  i->Enter(); \
  V8::Initialize(); \
\
  { \
    HandleScope h {i}; \
\
    Handle<Boolean> t {Boolean::New(i, true)}; \
    V8MONKEY_CHECK(!t->Is##fn(), "Is" #fn " reported correct result for true"); \
\
    Handle<Boolean> f {Boolean::New(i, false)}; \
    V8MONKEY_CHECK(!f->Is##fn(), "Is" #fn " reported correct result for false"); \
  } \
\
  i->Exit(); \
  i->Dispose(); \
  V8::Dispose(); \
}


// XXX Add test for IsUndefined once implemented
// XXX Add test for IsNull once implemented
// XXX Add test for IsString once implemented

BOOLEAN_IS_TEST(001, Symbol)
BOOLEAN_IS_TEST(002, Function)
BOOLEAN_IS_TEST(003, Array)
BOOLEAN_IS_TEST(004, Object)
BOOLEAN_IS_TEST(005, Number)
BOOLEAN_IS_TEST(006, External)
BOOLEAN_IS_TEST(007, Int32)
BOOLEAN_IS_TEST(008, Uint32)
BOOLEAN_IS_TEST(009, Date)
BOOLEAN_IS_TEST(010, BooleanObject)
BOOLEAN_IS_TEST(011, NumberObject)
BOOLEAN_IS_TEST(012, StringObject)
BOOLEAN_IS_TEST(013, SymbolObject)
BOOLEAN_IS_TEST(014, NativeError)
BOOLEAN_IS_TEST(015, RegExp)
BOOLEAN_IS_TEST(016, Promise)
BOOLEAN_IS_TEST(017, ArrayBuffer)
BOOLEAN_IS_TEST(018, ArrayBufferView)
BOOLEAN_IS_TEST(019, TypedArray)
BOOLEAN_IS_TEST(020, Uint8Array)
BOOLEAN_IS_TEST(021, Uint8ClampedArray)
BOOLEAN_IS_TEST(022, Int8Array)
BOOLEAN_IS_TEST(023, Uint16Array)
BOOLEAN_IS_TEST(024, Int16Array)
BOOLEAN_IS_TEST(025, Uint32Array)
BOOLEAN_IS_TEST(026, Int32Array)
BOOLEAN_IS_TEST(027, Float32Array)
BOOLEAN_IS_TEST(028, Float64Array)
BOOLEAN_IS_TEST(029, DataView)
#undef BOOLEAN_IS_TEST


V8MONKEY_TEST(Boolean030, "IsBoolean works correctly") {
  Isolate* i {Isolate::New()}; 
  i->Enter(); 
  V8::Initialize(); 

  { 
    HandleScope h {i}; 

    Handle<Boolean> t {Boolean::New(i, true)}; 
    V8MONKEY_CHECK(t->IsBoolean(), "IsBoolean reported correct result for true"); 

    Handle<Boolean> f {Boolean::New(i, false)}; 
    V8MONKEY_CHECK(f->IsBoolean(), "IsBoolean reported correct result for false"); 
  } 

  i->Exit(); 
  i->Dispose(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Boolean031, "IsTrue works correctly") {
  Isolate* i {Isolate::New()}; 
  i->Enter(); 
  V8::Initialize(); 

  { 
    HandleScope h {i}; 

    Handle<Boolean> t {Boolean::New(i, true)}; 
    V8MONKEY_CHECK(t->IsTrue(), "IsTrue reported correct result for true"); 

    Handle<Boolean> f {Boolean::New(i, false)}; 
    V8MONKEY_CHECK(!f->IsTrue(), "IsTrue reported correct result for false"); 
  } 

  i->Exit(); 
  i->Dispose(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Boolean032, "IsFalse works correctly") {
  Isolate* i {Isolate::New()}; 
  i->Enter(); 
  V8::Initialize(); 

  { 
    HandleScope h {i}; 

    Handle<Boolean> t {Boolean::New(i, true)}; 
    V8MONKEY_CHECK(!t->IsFalse(), "IsFalse reported correct result for true"); 

    Handle<Boolean> f {Boolean::New(i, false)}; 
    V8MONKEY_CHECK(f->IsFalse(), "IsFalse reported correct result for false"); 
  } 

  i->Exit(); 
  i->Dispose(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Boolean033, "Value works correctly") {
  Isolate* i {Isolate::New()}; 
  i->Enter(); 
  V8::Initialize(); 

  { 
    HandleScope h {i}; 

    Handle<Boolean> t {Boolean::New(i, true)}; 
    V8MONKEY_CHECK(t->Value(), "Value reported correct result for true"); 

    Handle<Boolean> f {Boolean::New(i, false)}; 
    V8MONKEY_CHECK(!f->Value(), "Value reported correct result for false"); 
  } 

  i->Exit(); 
  i->Dispose(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Boolean034, "BooleanValue works correctly") {
  Isolate* i {Isolate::New()}; 
  i->Enter(); 
  V8::Initialize(); 

  { 
    HandleScope h {i}; 

    Handle<Value> t {Boolean::New(i, true)}; 
    V8MONKEY_CHECK(t->BooleanValue(), "BooleanValue reported correct result for true"); 

    Handle<Value> f {Boolean::New(i, false)}; 
    V8MONKEY_CHECK(!f->BooleanValue(), "BooleanValue reported correct result for false"); 
  } 

  i->Exit(); 
  i->Dispose(); 
  V8::Dispose(); 
}


V8MONKEY_TEST(Boolean035, "Boolean statics works correctly") {
  Isolate* i {Isolate::New()}; 
  i->Enter(); 
  V8::Initialize(); 

  { 
    HandleScope h {i}; 

    Handle<Boolean> t {True(i)};
    V8MONKEY_CHECK(t->BooleanValue(), "BooleanValue reported correct result for true"); 

    Handle<Value> f {False(i)};
    V8MONKEY_CHECK(!f->BooleanValue(), "BooleanValue reported correct result for false"); 
  } 

  i->Exit(); 
  i->Dispose(); 
  V8::Dispose(); 
}


// XXX ToBoolean


//#define BOOLEANNUMERICVALUETEST(testNumber, variant, val, method, expected) \
//V8MONKEY_TEST(Boolean##testNumber, #method " works correctly (" #variant ")") { \
//  V8::Initialize(); \
//\
//  { \
//    HandleScope h; \
//    bool value = val; \
//    Handle<Value> b = Boolean::New(value); \
//\
//    V8MONKEY_CHECK(b->method() == expected, "Correct value returned"); \
//  } \
// \
//  Isolate::GetCurrent()->Exit(); \
//  V8::Dispose(); \
//}
//
//
//BOOLEANNUMERICVALUETEST(028, 1, true, NumberValue, 1)
//BOOLEANNUMERICVALUETEST(029, 2, false, NumberValue, 0.0)
//BOOLEANNUMERICVALUETEST(030, 1, true, IntegerValue, 1)
//BOOLEANNUMERICVALUETEST(031, 2, false, IntegerValue, 0)
//BOOLEANNUMERICVALUETEST(032, 1, true, Int32Value, 1)
//BOOLEANNUMERICVALUETEST(033, 2, false, Int32Value, 0)
//BOOLEANNUMERICVALUETEST(034, 1, true, Uint32Value, 1)
//BOOLEANNUMERICVALUETEST(035, 2, false, Uint32Value, 0)
//#undef BOOLEANNUMERICVALUETEST
//
//
//#define BOOLEANTOBOOLEANTEST(testNumber, variant, val) \
//V8MONKEY_TEST(Boolean##testNumber, "ToBoolean works correctly (" #variant ")") { \
//  V8::Initialize(); \
//\
//  { \
//    HandleScope h; \
//    bool value = val; \
//    Handle<Value> b = Boolean::New(value); \
//    Local<Boolean> n = b->ToBoolean(); \
//\
//    V8MONKEY_CHECK(n->Value() == b->BooleanValue(), "Correct value returned"); \
//    V8MONKEY_CHECK(n == b, "Same object returned"); \
//  } \
// \
//  Isolate::GetCurrent()->Exit(); \
//  V8::Dispose(); \
//}
//
//BOOLEANTOBOOLEANTEST(036, 1, true)
//BOOLEANTOBOOLEANTEST(037, 2, false)
//
//
//#define BOOLEANTOTYPETEST(testNumber, variant, val, type) \
//V8MONKEY_TEST(Boolean##testNumber, "To" #type " works correctly (" #variant ")") { \
//  V8::Initialize(); \
//\
//  { \
//    HandleScope h; \
//    bool value = val; \
//    Handle<Value> b = Boolean::New(value); \
//    Local<type> n = b->To##type(); \
//\
//    V8MONKEY_CHECK(n->Value() == b->type##Value(), "Correct value returned"); \
//  } \
// \
//  Isolate::GetCurrent()->Exit(); \
//  V8::Dispose(); \
//}
//
//
//BOOLEANTOTYPETEST(038, 1, true, Number)
//BOOLEANTOTYPETEST(039, 2, false, Number)
//BOOLEANTOTYPETEST(040, 1, true, Integer)
//BOOLEANTOTYPETEST(041, 2, false, Integer)
//BOOLEANTOTYPETEST(042, 1, true, Int32)
//BOOLEANTOTYPETEST(043, 2, false, Int32)
//BOOLEANTOTYPETEST(044, 1, true, Uint32)
//BOOLEANTOTYPETEST(045, 2, false, Uint32)
//#undef BOOLEANTOTYPETEST
//
//
//#define BOOLEANTOARRAYINDEXTEST(testNumber, variant, val, expected) \
//V8MONKEY_TEST(Boolean##testNumber, "ToArrayIndex returns correct value (" #variant ")") { \
//  V8::Initialize(); \
//\
//  { \
//    HandleScope h; \
//    bool value = val; \
//    Handle<Value> b = Boolean::New(value); \
//    Local<Uint32> indexHandle = b->ToArrayIndex(); \
//\
//    V8MONKEY_CHECK(indexHandle->Value() == expected, "Correct value returned"); \
//  } \
// \
//  Isolate::GetCurrent()->Exit(); \
//  V8::Dispose(); \
//}
//
//
//BOOLEANTOARRAYINDEXTEST(046, 1, true, 1)
//BOOLEANTOARRAYINDEXTEST(047, 2, false, 0)
//#undef BOOLEANTOARRAYINDEXTEST


//#define BOOLEANEQUALITYTEST(testNumber, variant, val, method) \
//V8MONKEY_TEST(Boolean##testNumber, #method " works correctly (" #variant ")") { \
//  V8::Initialize(); \
//\
//  { \
//    HandleScope h; \
//    bool value = val; \
//    Handle<Value> b = Boolean::New(value); \
//    Handle<Value> b2 = Boolean::New(value); \
//    Handle<Value> b3 = Boolean::New(!value); \
//\
//    V8MONKEY_CHECK(b->method(b), "Self-equality works correctly"); \
//    V8MONKEY_CHECK(b->method(b2), "Equality works correctly"); \
//    V8MONKEY_CHECK(b2->method(b), "Equality is symmetric"); \
//    V8MONKEY_CHECK(!b->method(b3), "Inequality is correct"); \
//  } \
// \
//  Isolate::GetCurrent()->Exit(); \
//  V8::Dispose(); \
//}
//
//
//BOOLEANEQUALITYTEST(052, 1, true, Equals)
//BOOLEANEQUALITYTEST(053, 2, false, Equals)
//
//
//V8MONKEY_TEST(Boolean054, "Equals works correctly (3)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Local<Value> n = Number::New(1);
//
//    V8MONKEY_CHECK(b->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean055, "Equals works correctly (4)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Local<Value> n = Number::New(0.0);
//
//    V8MONKEY_CHECK(b->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean056, "Equals works correctly (5)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Local<Value> n = Number::New(-0.0);
//
//    V8MONKEY_CHECK(b->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean057, "Equals works correctly (6)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Local<Value> n = Number::New(0.0);
//
//    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean058, "Equals works correctly (6)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Local<Value> n = Number::New(-0.0);
//
//    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean059, "Equals works correctly (7)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Local<Value> n = Number::New(123.45);
//
//    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean060, "Equals works correctly (8)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Local<Value> n = Number::New(123.45);
//
//    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean061, "Equals works correctly (9)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Handle<Value> u = Undefined();
//
//    V8MONKEY_CHECK(!b->Equals(u), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean062, "Equals works correctly (10)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Handle<Value> u = Undefined();
//
//    V8MONKEY_CHECK(!b->Equals(u), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean063, "Equals works correctly (11)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Handle<Value> n = Null();
//
//    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean064, "Equals works correctly (12)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Handle<Value> n = Null();
//
//    V8MONKEY_CHECK(!b->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//BOOLEANEQUALITYTEST(065, 1, true, StrictEquals)
//BOOLEANEQUALITYTEST(066, 2, false, StrictEquals)
//#undef BOOLEANEQUALITYTEST
//
//
//V8MONKEY_TEST(Boolean067, "StrictEquals works correctly (3)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Local<Value> n = Number::New(1);
//
//    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean068, "StrictEquals works correctly (4)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Local<Value> n = Number::New(0.0);
//
//    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean069, "StrictEquals works correctly (5)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Local<Value> n = Number::New(-0.0);
//
//    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean070, "StrictEquals works correctly (6)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Local<Value> n = Number::New(0.0);
//
//    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean071, "StrictEquals works correctly (6)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Local<Value> n = Number::New(-0.0);
//
//    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean072, "StrictEquals works correctly (7)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Local<Value> n = Number::New(123.45);
//
//    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean073, "StrictEquals works correctly (8)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Local<Value> n = Number::New(123.45);
//
//    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean074, "StrictEquals works correctly (9)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Handle<Value> u = Undefined();
//
//    V8MONKEY_CHECK(!b->StrictEquals(u), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean075, "StrictEquals works correctly (10)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Handle<Value> u = Undefined();
//
//    V8MONKEY_CHECK(!b->StrictEquals(u), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean076, "StrictEquals works correctly (11)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(true);
//    Handle<Value> n = Null();
//
//    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Boolean077, "StrictEquals works correctly (12)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> b = Boolean::New(false);
//    Handle<Value> n = Null();
//
//    V8MONKEY_CHECK(!b->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined001, "IsUndefined works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(u->IsUndefined(), "IsUndefined reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined002, "IsNull works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsNull(), "IsNull reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined003, "IsString works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsString(), "IsString reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined004, "IsFunction works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsFunction(), "IsFunction reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined005, "IsArray works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsArray(), "IsArray reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined006, "IsObject works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsObject(), "IsObject reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined007, "IsNumber works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsNumber(), "IsNumber reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//}
//
//
//V8MONKEY_TEST(Undefined008, "IsExternal works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsExternal(), "IsExternal reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined009, "IsDate works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsDate(), "IsDate reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined010, "IsBooleanObject works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsBooleanObject(), "IsUndefinedObject reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined011, "IsNumberObject works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsNumberObject(), "IsNumberObject reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined012, "IsStringObject works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsStringObject(), "IsStringObject reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined013, "IsNativeError works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsNativeError(), "IsNativeError reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined014, "IsRegExp works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsRegExp(), "IsRegExp reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined015, "IsInt32 works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsInt32(), "IsInt32 reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined016, "IsUint32 works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsUint32(), "IsUint32 reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined017, "IsBoolean works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsBoolean(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined018, "IsTrue works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsTrue(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined019, "IsFalse works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined();
//
//    V8MONKEY_CHECK(!u->IsFalse(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined020, "NumberValue works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//
//    V8MONKEY_CHECK(std::isnan(u->NumberValue()), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//#define UNDEFINEDVALUETEST(testNumber, variant, method, expected) \
//V8MONKEY_TEST(Undefined##testNumber, #method " works correctly (" #variant ")") { \
//  V8::Initialize(); \
//\
//  { \
//    HandleScope h; \
//    Handle<Value> u = Undefined(); \
//\
//    V8MONKEY_CHECK(u->method() == expected, "Correct value returned"); \
//  } \
// \
//  Isolate::GetCurrent()->Exit(); \
//  V8::Dispose(); \
//}
//
//
//UNDEFINEDVALUETEST(021, 1, IntegerValue, 0)
//UNDEFINEDVALUETEST(022, 1, Int32Value, 0)
//UNDEFINEDVALUETEST(023, 1, Uint32Value, 0)
//UNDEFINEDVALUETEST(024, 1, BooleanValue, false)
//#undef UNDEFINEDVALUETEST
//
//
//V8MONKEY_TEST(Undefined025, "ToNumber works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Local<Number> n = u->ToNumber();
//
//    V8MONKEY_CHECK(std::isnan(n->Value()), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//#define UNDEFINEDTOTYPETEST(testNumber, variant, type) \
//V8MONKEY_TEST(Undefined##testNumber, "To" #type " works correctly (" #variant ")") { \
//  V8::Initialize(); \
//\
//  { \
//    HandleScope h; \
//    Handle<Value> u = Undefined(); \
//    Local<type> n = u->To##type(); \
//\
//    V8MONKEY_CHECK(n->Value() == u->type##Value(), "Correct value returned"); \
//  } \
// \
//  Isolate::GetCurrent()->Exit(); \
//  V8::Dispose(); \
//}
//
//
//UNDEFINEDTOTYPETEST(026, 1, Integer)
//UNDEFINEDTOTYPETEST(027, 1, Int32)
//UNDEFINEDTOTYPETEST(028, 1, Uint32)
//UNDEFINEDTOTYPETEST(029, 1, Boolean)
//#undef UNDEFINEDTOTYPETEST
//
//
//V8MONKEY_TEST(Undefined030, "ToArrayIndex returns correct value") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Local<Uint32> indexHandle = u->ToArrayIndex();
//
//    V8MONKEY_CHECK(indexHandle.IsEmpty(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined031, "Undefined with isolate returns correct value") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> u = Undefined(Isolate::GetCurrent());
//
//    V8MONKEY_CHECK(u == Undefined(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined032, "Equals works correctly (1)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//
//    V8MONKEY_CHECK(u->Equals(u), "Self-equality works correctly");
//    V8MONKEY_CHECK(u->Equals(Undefined()), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined033, "Equals works correctly (2)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Local<Value> n = Number::New(1);
//
//    V8MONKEY_CHECK(!u->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined034, "Equals works correctly (3)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Local<Value> n = Number::New(std::numeric_limits<double>::quiet_NaN());
//
//    V8MONKEY_CHECK(!u->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined035, "Equals works correctly (4)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Handle<Boolean> b = Boolean::New(true);
//
//    V8MONKEY_CHECK(!u->Equals(b), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined036, "Equals works correctly (5)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Handle<Boolean> b = Boolean::New(false);
//
//    V8MONKEY_CHECK(!u->Equals(b), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined037, "Equals works correctly (6)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Handle<Value> n = Null();
//
//    V8MONKEY_CHECK(u->Equals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined038, "StrictEquals works correctly (1)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//
//    V8MONKEY_CHECK(u->StrictEquals(u), "Self-equality works correctly");
//    V8MONKEY_CHECK(u->StrictEquals(Undefined()), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined039, "StrictEquals works correctly (2)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Local<Value> n = Number::New(1);
//
//    V8MONKEY_CHECK(!u->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined040, "StrictEquals works correctly (3)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Local<Value> n = Number::New(std::numeric_limits<double>::quiet_NaN());
//
//    V8MONKEY_CHECK(!u->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined041, "StrictEquals works correctly (4)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Handle<Boolean> b = Boolean::New(true);
//
//    V8MONKEY_CHECK(!u->StrictEquals(b), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined042, "StrictEquals works correctly (5)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Handle<Boolean> b = Boolean::New(false);
//
//    V8MONKEY_CHECK(!u->StrictEquals(b), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Undefined043, "StrictEquals works correctly (6)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> u = Undefined();
//    Handle<Value> n = Null();
//
//    V8MONKEY_CHECK(!u->StrictEquals(n), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null001, "IsNull works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(n->IsNull(), "IsNull reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null002, "IsUndefined works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsUndefined(), "IsUndefined reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null003, "IsString works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsString(), "IsString reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null004, "IsFunction works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsFunction(), "IsFunction reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null005, "IsArray works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsArray(), "IsArray reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null006, "IsObject works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsObject(), "IsObject reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null007, "IsNumber works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsNumber(), "IsNumber reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//}
//
//
//V8MONKEY_TEST(Null008, "IsExternal works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsExternal(), "IsExternal reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null009, "IsDate works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsDate(), "IsDate reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null010, "IsBooleanObject works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsBooleanObject(), "IsNullObject reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null011, "IsNumberObject works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsNumberObject(), "IsNumberObject reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null012, "IsStringObject works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsStringObject(), "IsStringObject reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null013, "IsNativeError works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsNativeError(), "IsNativeError reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null014, "IsRegExp works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsRegExp(), "IsRegExp reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null015, "IsInt32 works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsInt32(), "IsInt32 reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null016, "IsUint32 works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsUint32(), "IsUint32 reports correct result");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null017, "IsBoolean works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsBoolean(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null018, "IsTrue works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsTrue(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null019, "IsFalse works correctly") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null();
//
//    V8MONKEY_CHECK(!n->IsFalse(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//#define NULLVALUETEST(testNumber, variant, method, expected) \
//V8MONKEY_TEST(Null##testNumber, #method " works correctly (" #variant ")") { \
//  V8::Initialize(); \
//\
//  { \
//    HandleScope h; \
//    Handle<Value> n = Null(); \
//\
//    V8MONKEY_CHECK(n->method() == expected, "Correct value returned"); \
//  } \
// \
//  Isolate::GetCurrent()->Exit(); \
//  V8::Dispose(); \
//}
//
//
//NULLVALUETEST(020, 1, NumberValue, 0)
//NULLVALUETEST(021, 1, IntegerValue, 0)
//NULLVALUETEST(022, 1, Int32Value, 0)
//NULLVALUETEST(023, 1, Uint32Value, 0)
//NULLVALUETEST(024, 1, BooleanValue, false)
//#undef NULLVALUETEST
//
//
//#define NULLTOTYPETEST(testNumber, variant, type) \
//V8MONKEY_TEST(Null##testNumber, "To" #type " works correctly (" #variant ")") { \
//  V8::Initialize(); \
//\
//  { \
//    HandleScope h; \
//    Handle<Value> n = Null(); \
//    Local<type> other = n->To##type(); \
//\
//    V8MONKEY_CHECK(other->Value() == n->type##Value(), "Correct value returned"); \
//  } \
// \
//  Isolate::GetCurrent()->Exit(); \
//  V8::Dispose(); \
//}
//
//
//NULLTOTYPETEST(025, 1, Number)
//NULLTOTYPETEST(026, 1, Integer)
//NULLTOTYPETEST(027, 1, Int32)
//NULLTOTYPETEST(028, 1, Uint32)
//NULLTOTYPETEST(029, 1, Boolean)
//#undef NULLTOTYPETEST
//
//
//V8MONKEY_TEST(Null030, "ToArrayIndex returns correct value") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Local<Uint32> indexHandle = n->ToArrayIndex();
//
//    V8MONKEY_CHECK(indexHandle.IsEmpty(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null031, "Null with isolate returns correct value") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Primitive> n = Null(Isolate::GetCurrent());
//
//    V8MONKEY_CHECK(n == Null(), "Correct value returned");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null032, "Equals works correctly (1)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//
//    V8MONKEY_CHECK(n->Equals(n), "Self-equality works correctly");
//    V8MONKEY_CHECK(n->Equals(Null()), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null033, "Equals works correctly (2)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Local<Value> other = Number::New(1);
//
//    V8MONKEY_CHECK(!n->Equals(other), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null034, "Equals works correctly (3)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Local<Value> other = Number::New(std::numeric_limits<double>::quiet_NaN());
//
//    V8MONKEY_CHECK(!n->Equals(other), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null035, "Equals works correctly (4)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Handle<Boolean> b = Boolean::New(true);
//
//    V8MONKEY_CHECK(!n->Equals(b), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null036, "Equals works correctly (5)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Handle<Boolean> b = Boolean::New(false);
//
//    V8MONKEY_CHECK(!n->Equals(b), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null037, "Equals works correctly (6)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Handle<Value> u = Undefined();
//
//    V8MONKEY_CHECK(n->Equals(u), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null038, "StrictEquals works correctly (1)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//
//    V8MONKEY_CHECK(n->StrictEquals(n), "Self-equality works correctly");
//    V8MONKEY_CHECK(n->StrictEquals(Null()), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null039, "StrictEquals works correctly (2)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Local<Value> other = Number::New(1);
//
//    V8MONKEY_CHECK(!n->StrictEquals(other), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null040, "StrictEquals works correctly (3)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Handle<Boolean> b = Boolean::New(true);
//
//    V8MONKEY_CHECK(!n->StrictEquals(b), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null041, "StrictEquals works correctly (4)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Handle<Boolean> b = Boolean::New(false);
//
//    V8MONKEY_CHECK(!n->StrictEquals(b), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
//
//
//V8MONKEY_TEST(Null042, "StrictEquals works correctly (5)") {
//  V8::Initialize();
//
//  {
//    HandleScope h;
//    Handle<Value> n = Null();
//    Handle<Value> u = Undefined();
//
//    V8MONKEY_CHECK(!n->StrictEquals(u), "Equality works correctly");
//  }
//
//  Isolate::GetCurrent()->Exit();
//  V8::Dispose();
//}
*/
