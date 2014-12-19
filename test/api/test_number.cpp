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


V8MONKEY_TEST(Number013, "IsInt32 works correctly") {
  double d = 123.45;
  V8::Initialize();
  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsInt32(), "IsInt32 reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number014, "IsUint32 works correctly") {
  double d = 123.45;
  V8::Initialize();
  {
    HandleScope h;
    Local<Number> n = Number::New(d);

    V8MONKEY_CHECK(!n->IsUint32(), "IsUint32 reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Number015, "IsDate works correctly") {
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


V8MONKEY_TEST(Number016, "IsBooleanObject works correctly") {
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


V8MONKEY_TEST(Number017, "IsNumberObject works correctly") {
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


V8MONKEY_TEST(Number018, "IsStringObject works correctly") {
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


V8MONKEY_TEST(Number019, "IsNativeError works correctly") {
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


V8MONKEY_TEST(Number020, "IsRegExp works correctly") {
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


V8MONKEY_TEST(Number021, "Works correctly for nan") {
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


V8MONKEY_TEST(Integer001, "Value works correctly") {
  int32_t d = 123;
  V8::Initialize();
  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(n->Value() == d, "Value correct");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer002, "IsUndefined works correctly") {
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


V8MONKEY_TEST(Integer003, "IsNull works correctly") {
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


V8MONKEY_TEST(Integer004, "IsTrue works correctly") {
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


V8MONKEY_TEST(Integer005, "IsFalse works correctly") {
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


V8MONKEY_TEST(Integer006, "IsString works correctly") {
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


V8MONKEY_TEST(Integer007, "IsFunction works correctly") {
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


V8MONKEY_TEST(Integer008, "IsArray works correctly") {
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


V8MONKEY_TEST(Integer009, "IsObject works correctly") {
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


V8MONKEY_TEST(Integer010, "IsBoolean works correctly") {
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


V8MONKEY_TEST(Integer011, "IsNumber works correctly") {
  int32_t d = 123;
  V8::Initialize();
  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(n->IsNumber(), "IsNumber reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer012, "IsExternal works correctly") {
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


V8MONKEY_TEST(Integer013, "IsInt32 works correctly") {
  int32_t d = 123;
  V8::Initialize();
  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);

    V8MONKEY_CHECK(n->IsInt32(), "IsInt32 reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer014, "IsUint32 works correctly") {
  int32_t d = 123;
  V8::Initialize();
  {
    HandleScope h;
    Local<Integer> n = Integer::New(d);
    V8MONKEY_CHECK(!n->IsUint32(), "IsUint32 reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer015, "IsDate works correctly") {
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


V8MONKEY_TEST(Integer016, "IsBooleanObject works correctly") {
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


V8MONKEY_TEST(Integer017, "IsNumberObject works correctly") {
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


V8MONKEY_TEST(Integer018, "IsStringObject works correctly") {
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


V8MONKEY_TEST(Integer019, "IsNativeError works correctly") {
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


V8MONKEY_TEST(Integer020, "IsRegExp works correctly") {
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


V8MONKEY_TEST(Integer021, "Value works correctly when created from unsigned") {
  uint32_t d = 0xffffffff;
  V8::Initialize();
  {
    HandleScope h;
    Local<Integer> n = Integer::NewFromUnsigned(d);

    V8MONKEY_CHECK(n->Value() == d, "Value correct");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer022, "IsInt32 works correctly for value created from unsigned") {
  uint32_t d = 0xffffffff;
  V8::Initialize();
  {
    HandleScope h;
    Local<Integer> n = Integer::NewFromUnsigned(d);

    V8MONKEY_CHECK(!n->IsInt32(), "IsInt32 reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer023, "IsUint32 works correctly when created from unsigned") {
  uint32_t d = 0xffffffff;
  V8::Initialize();
  {
    HandleScope h;
    Local<Integer> n = Integer::NewFromUnsigned(d);
    V8MONKEY_CHECK(n->IsUint32(), "IsUint32 reports correct result");
  }

  Isolate::GetCurrent()->Exit();
  V8::Dispose();
}


V8MONKEY_TEST(Integer024, "Isolate variant works correctly") {
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


V8MONKEY_TEST(Integer025, "Unsigned isolate variant works correctly") {
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
