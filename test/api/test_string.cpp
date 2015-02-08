#include "v8.h"

#include "utils/test.h"
#include "V8MonkeyTest.h"
// 
// 
// using namespace v8;
// 
// 
// V8MONKEY_TEST(String001, "IsUndefined works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsUndefined(), "IsUndefined reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String002, "IsNull works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsNull(), "IsNull reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String003, "IsTrue works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsTrue(), "IsTrue reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String004, "IsFalse works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsFalse(), "IsFalse reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String005, "IsFunction works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsFunction(), "IsFunction reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String006, "IsArray works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsArray(), "IsArray reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String007, "IsObject works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsObject(), "IsObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String008, "IsBoolean works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsBoolean(), "IsBoolean reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
// }
// 
// 
// V8MONKEY_TEST(String009, "IsExternal works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsExternal(), "IsExternal reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String010, "IsDate works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsDate(), "IsDate reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String011, "IsBooleanObject works correctly") {
//   char data[] = "";
// 
//   V8::Initialize();
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsBooleanObject(), "IsBooleanObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String012, "IsNumberObject works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsNumberObject(), "IsNumberObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String013, "IsStringObject works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsStringObject(), "IsStringObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String014, "IsNativeError works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsNativeError(), "IsNativeError reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String015, "IsRegExp works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsRegExp(), "IsRegExp reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String016, "IsNumber works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsNumber(), "IsNumber reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String017, "IsInt32 works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsInt32(), "IsInt32 reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String018, "IsUint32 works correctly") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(!s->IsUint32(), "IsUint32 reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String019, "IsString works correctly (1)") {
//   char data[] = "";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(s->IsString(), "IsString reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String020, "IsString works correctly (2)") {
//   char data[] = "abc";
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(s->IsString(), "IsString reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String021, "IsString works correctly (3)") {
//   char data[] = {0xc3, 0xa8, 0x00};
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(s->IsString(), "IsString reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String022, "IsString works correctly (4)") {
//   char data[] = {0xc4, 0xbf, 0x00};
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(s->IsString(), "IsString reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String023, "IsString works correctly (5)") {
//   char data[] = {0xe4, 0x90, 0xaf, 0x00};
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(s->IsString(), "IsString reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String024, "IsString works correctly (6)") {
//   char data[] = {0xf1, 0x89, 0x90, 0xaf, 0x00};
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<String> s = String::New(data);
// 
//   //  V8MONKEY_CHECK(s->IsString(), "IsString reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// /*
// 
// 
// V8MONKEY_TEST(Number##testNumber, #method " works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Value> n = Number::New(value); \
// \
//   //  V8MONKEY_CHECK(n->method() == expected, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERISTEST(017, 1, 123.45, IsNumber, true)
// NUMBERISTEST(018, 2, 123.65, IsNumber, true)
// NUMBERISTEST(019, 3, -123.45, IsNumber, true)
// NUMBERISTEST(020, 4, -123.67, IsNumber, true)
// NUMBERISTEST(021, 5, 123, IsNumber, true)
// NUMBERISTEST(022, 6, -1, IsNumber, true)
// NUMBERISTEST(023, 7, 0xffffffff, IsNumber, true)
// NUMBERISTEST(024, 8, -0.0, IsNumber, true)
// NUMBERISTEST(025, 9, 0.0, IsNumber, true)
// NUMBERISTEST(026, 10, std::numeric_limits<double>::infinity(), IsNumber, true)
// NUMBERISTEST(027, 11, std::numeric_limits<double>::quiet_NaN(), IsNumber, true)
// 
// 
// NUMBERISTEST(028, 1, 123.45, IsInt32, false)
// NUMBERISTEST(029, 2, 123.65, IsInt32, false)
// NUMBERISTEST(030, 3, -123.45, IsInt32, false)
// NUMBERISTEST(031, 4, -123.67, IsInt32, false)
// NUMBERISTEST(032, 5, 123, IsInt32, true)
// NUMBERISTEST(033, 6, -1, IsInt32, true)
// NUMBERISTEST(034, 7, 0xffffffff, IsInt32, false)
// NUMBERISTEST(035, 8, -0.0, IsInt32, false)
// NUMBERISTEST(036, 9, 0.0, IsInt32, true)
// NUMBERISTEST(037, 10, std::numeric_limits<double>::infinity(), IsInt32, false)
// NUMBERISTEST(038, 11, std::numeric_limits<double>::quiet_NaN(), IsInt32, false)
// 
// 
// NUMBERISTEST(039, 1, 123.45, IsUint32, false)
// NUMBERISTEST(040, 2, 123.65, IsUint32, false)
// NUMBERISTEST(041, 3, -123.45, IsUint32, false)
// NUMBERISTEST(042, 4, -123.67, IsUint32, false)
// NUMBERISTEST(043, 5, 123, IsUint32, true)
// NUMBERISTEST(044, 6, -1, IsUint32, false)
// NUMBERISTEST(045, 7, 0xffffffff, IsUint32, true)
// NUMBERISTEST(046, 8, -0.0, IsUint32, false)
// NUMBERISTEST(047, 9, 0.0, IsUint32, true)
// NUMBERISTEST(048, 10, std::numeric_limits<double>::infinity(), IsUint32, false)
// NUMBERISTEST(049, 11, std::numeric_limits<double>::quiet_NaN(), IsUint32, false)
// #undef NUMBERISTEST
// */
// /*
// #include <cmath>
// #include <limits>
// 
// #include "v8.h"
// 
// #include "utils/test.h"
// #include "V8MonkeyTest.h"
// 
// 
// using namespace v8;
// 
// 
// 
// 
// #define NUMBERVALUETEST(testNumber, variant, val) \
// V8MONKEY_TEST(Number##testNumber, "Value works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Number> n = Number::New(value); \
// \
//   //  V8MONKEY_CHECK(n->Value() == value, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERVALUETEST(050, 1, 123.45)
// NUMBERVALUETEST(051, 2, 123.65)
// NUMBERVALUETEST(052, 3, -123.45)
// NUMBERVALUETEST(053, 4, -123.67)
// NUMBERVALUETEST(054, 5, 123)
// NUMBERVALUETEST(055, 6, -1)
// NUMBERVALUETEST(056, 7, 0xffffffff)
// NUMBERVALUETEST(057, 8, -0.0)
// NUMBERVALUETEST(058, 9, 0.0)
// NUMBERVALUETEST(059, 10, std::numeric_limits<double>::infinity())
// #undef NUMBERVALUETEST
// 
// 
// V8MONKEY_TEST(String060, "Value works correctly (10)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Number> n = Number::New(value);
// 
//   //  V8MONKEY_CHECK(isnan(n->Value()), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define NUMBERNUMBERVALUETEST(testNumber, variant, val) \
// V8MONKEY_TEST(Number##testNumber, "NumberValue works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Number> n = Number::New(value); \
// \
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERNUMBERVALUETEST(061, 1, 123.45)
// NUMBERNUMBERVALUETEST(062, 2, 123.65)
// NUMBERNUMBERVALUETEST(063, 3, -123.45)
// NUMBERNUMBERVALUETEST(064, 4, -123.67)
// NUMBERNUMBERVALUETEST(065, 5, 123)
// NUMBERNUMBERVALUETEST(066, 6, -1)
// NUMBERNUMBERVALUETEST(067, 7, 0xffffffff)
// NUMBERNUMBERVALUETEST(068, 8, -0.0)
// NUMBERNUMBERVALUETEST(069, 9, 0.0)
// NUMBERNUMBERVALUETEST(070, 10, std::numeric_limits<double>::infinity())
// #undef NUMBERNUMBERVALUETEST
// 
// 
// V8MONKEY_TEST(String071, "NumberValue works correctly (10)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Number> n = Number::New(value);
// 
//   //  V8MONKEY_CHECK(isnan(n->NumberValue()), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define NUMBERTONUMBERTEST(testNumber, variant, val) \
// V8MONKEY_TEST(Number##testNumber, "ToNumber works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Value> n = Number::New(value); \
//     Local<Number> n2 = n->ToNumber(); \
// \
//   //  V8MONKEY_CHECK(n2->NumberValue() == n->NumberValue(), "Correct value returned"); \
//   //  V8MONKEY_CHECK(n2 == n, "Same object returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERTONUMBERTEST(072, 1, 123.45)
// NUMBERTONUMBERTEST(073, 2, 123.65)
// NUMBERTONUMBERTEST(074, 3, -123.45)
// NUMBERTONUMBERTEST(075, 4, -123.67)
// NUMBERTONUMBERTEST(076, 5, 123)
// NUMBERTONUMBERTEST(077, 6, -1)
// NUMBERTONUMBERTEST(078, 7, 0xffffffff)
// NUMBERTONUMBERTEST(079, 8, -0.0)
// NUMBERTONUMBERTEST(080, 9, 0.0)
// NUMBERTONUMBERTEST(081, 10, std::numeric_limits<double>::infinity())
// #undef NUMBERTONUMBERTEST
// 
// 
// V8MONKEY_TEST(String082, "ToNumber works correctly (10)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Value> n = Number::New(value);
//     Local<Number> n2 = n->ToNumber();
// 
//   //  V8MONKEY_CHECK(isnan(n2->Value()), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define NUMBERNUMERICVALUETEST(testNumber, variant, val, method, expected) \
// V8MONKEY_TEST(Number##testNumber, #method " works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Number> n = Number::New(value); \
// \
//   //  V8MONKEY_CHECK(n->method() == expected, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERNUMERICVALUETEST(083, 1, 123.45, IntegerValue, 123)
// NUMBERNUMERICVALUETEST(084, 2, 123.65, IntegerValue, 123)
// NUMBERNUMERICVALUETEST(085, 3, -123.45, IntegerValue, -123)
// NUMBERNUMERICVALUETEST(086, 4, -123.67, IntegerValue, -123)
// NUMBERNUMERICVALUETEST(087, 5, 123, IntegerValue, 123)
// NUMBERNUMERICVALUETEST(088, 6, -1, IntegerValue, -1)
// NUMBERNUMERICVALUETEST(089, 7, 0xffffffff, IntegerValue, 0xffffffff)
// NUMBERNUMERICVALUETEST(090, 8, -0.0, IntegerValue, 0)
// NUMBERNUMERICVALUETEST(091, 9, 0.0, IntegerValue, 0)
// NUMBERNUMERICVALUETEST(092, 10, std::numeric_limits<double>::infinity(), IntegerValue, static_cast<int64_t>(0x8000000000000000))
// NUMBERNUMERICVALUETEST(093, 11, std::numeric_limits<double>::quiet_NaN(), IntegerValue, static_cast<int64_t>(0x8000000000000000))
// 
// 
// NUMBERNUMERICVALUETEST(094, 1, 123.45,Int32Value,  123)
// NUMBERNUMERICVALUETEST(095, 2, 123.65, Int32Value, 123)
// NUMBERNUMERICVALUETEST(096, 3, -123.45, Int32Value, -123)
// NUMBERNUMERICVALUETEST(097, 4, -123.67, Int32Value, -123)
// NUMBERNUMERICVALUETEST(098, 5, 123, Int32Value, 123)
// NUMBERNUMERICVALUETEST(099, 6, -1, Int32Value, -1)
// NUMBERNUMERICVALUETEST(100, 7, 0xffffffff, Int32Value, -1)
// NUMBERNUMERICVALUETEST(101, 8, -0.0, Int32Value, 0)
// NUMBERNUMERICVALUETEST(102, 9, 0.0, Int32Value,  0)
// NUMBERNUMERICVALUETEST(103, 10, std::numeric_limits<double>::infinity(), Int32Value, 0)
// NUMBERNUMERICVALUETEST(104, 11, std::numeric_limits<double>::quiet_NaN(), Int32Value, 0)
// 
// 
// NUMBERNUMERICVALUETEST(105, 1, 123.45, Uint32Value, 123)
// NUMBERNUMERICVALUETEST(106, 2, 123.65, Uint32Value, 123)
// NUMBERNUMERICVALUETEST(107, 3, -123.45, Uint32Value, 0xffffff85)
// NUMBERNUMERICVALUETEST(108, 4, -123.67, Uint32Value, 0xffffff85)
// NUMBERNUMERICVALUETEST(109, 5, 123, Uint32Value, 123)
// NUMBERNUMERICVALUETEST(110, 6, -1, Uint32Value, 0xffffffff)
// NUMBERNUMERICVALUETEST(111, 7, 0xffffffff, Uint32Value, 0xffffffff)
// NUMBERNUMERICVALUETEST(112, 8, -0.0, Uint32Value, 0)
// NUMBERNUMERICVALUETEST(113, 9, 0.0, Uint32Value, 0)
// NUMBERNUMERICVALUETEST(114, 10, std::numeric_limits<double>::infinity(), Uint32Value, 0)
// NUMBERNUMERICVALUETEST(115, 11, std::numeric_limits<double>::quiet_NaN(), Uint32Value, 0)
// 
// 
// NUMBERNUMERICVALUETEST(116, 1, 123.45, BooleanValue, true)
// NUMBERNUMERICVALUETEST(117, 2, 123.65, BooleanValue, true)
// NUMBERNUMERICVALUETEST(118, 3, -123.45, BooleanValue, true)
// NUMBERNUMERICVALUETEST(119, 4, -123.67, BooleanValue, true)
// NUMBERNUMERICVALUETEST(120, 5, 123, BooleanValue, true)
// NUMBERNUMERICVALUETEST(121, 6, -1, BooleanValue, true)
// NUMBERNUMERICVALUETEST(122, 7, 0xffffffff, BooleanValue, true)
// NUMBERNUMERICVALUETEST(123, 8, -0.0, BooleanValue, false)
// NUMBERNUMERICVALUETEST(124, 9, 0.0, BooleanValue, false)
// NUMBERNUMERICVALUETEST(125, 10, std::numeric_limits<double>::infinity(), BooleanValue, true)
// NUMBERNUMERICVALUETEST(126, 11, std::numeric_limits<double>::quiet_NaN(), BooleanValue, 0)
// #undef NUMBERNUMERICVALUETEST
// 
// 
// #define NUMBERTOTYPETEST(testNumber, variant, val, type, isSame) \
// V8MONKEY_TEST(Number##testNumber, "To" #type " works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Value> n = Number::New(value); \
//     Local<type> n2 = n->To##type(); \
// \
//   //  V8MONKEY_CHECK(n2->Value() == n->type##Value(), "Correct value returned"); \
//   //  V8MONKEY_CHECK((n2 == n) == isSame, "New object returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERTOTYPETEST(127, 1, 123.45, Integer, false)
// NUMBERTOTYPETEST(128, 2, 123.65, Integer, false)
// NUMBERTOTYPETEST(129, 3, -123.45, Integer, false)
// NUMBERTOTYPETEST(130, 4, -123.67, Integer, false)
// NUMBERTOTYPETEST(131, 5, 123, Integer, true)
// NUMBERTOTYPETEST(132, 6, -1, Integer, true)
// NUMBERTOTYPETEST(133, 7, 0xffffffff, Integer, false)
// NUMBERTOTYPETEST(134, 8, -0.0, Integer, false)
// NUMBERTOTYPETEST(135, 9, 0.0, Integer, true)
// NUMBERTOTYPETEST(136, 10, std::numeric_limits<double>::infinity(), Integer, false)
// NUMBERTOTYPETEST(137, 11, std::numeric_limits<double>::quiet_NaN(), Integer, false)
// 
// 
// NUMBERTOTYPETEST(138, 1, 123.45, Int32, false)
// NUMBERTOTYPETEST(139, 2, 123.65, Int32, false)
// NUMBERTOTYPETEST(140, 3, -123.45, Int32, false)
// NUMBERTOTYPETEST(141, 4, -123.67, Int32, false)
// NUMBERTOTYPETEST(142, 5, 123, Int32, true)
// NUMBERTOTYPETEST(143, 6, -1, Int32, true)
// NUMBERTOTYPETEST(144, 7, 0xffffffff, Int32, false)
// NUMBERTOTYPETEST(145, 8, -0.0, Int32, false)
// NUMBERTOTYPETEST(146, 9, 0.0, Int32, true)
// NUMBERTOTYPETEST(147, 10, std::numeric_limits<double>::infinity(), Int32, false)
// NUMBERTOTYPETEST(148, 11, std::numeric_limits<double>::quiet_NaN(), Int32, false)
// 
// 
// NUMBERTOTYPETEST(149, 1, 123.45, Uint32, false)
// NUMBERTOTYPETEST(150, 2, 123.65, Uint32, false)
// NUMBERTOTYPETEST(151, 3, -123.45, Uint32, false)
// NUMBERTOTYPETEST(152, 4, -123.67, Uint32, false)
// NUMBERTOTYPETEST(153, 5, 123, Uint32, true)
// NUMBERTOTYPETEST(154, 6, -1, Uint32, true)
// NUMBERTOTYPETEST(155, 7, 0xffffffff, Uint32, false)
// NUMBERTOTYPETEST(156, 8, -0.0, Uint32, false)
// NUMBERTOTYPETEST(157, 9, 0.0, Uint32, true)
// NUMBERTOTYPETEST(158, 10, std::numeric_limits<double>::infinity(), Uint32, false)
// NUMBERTOTYPETEST(159, 11, std::numeric_limits<double>::quiet_NaN(), Uint32, false)
// 
// 
// NUMBERTOTYPETEST(160, 1, 123.45, Boolean, false)
// NUMBERTOTYPETEST(161, 2, 123.65, Boolean, false)
// NUMBERTOTYPETEST(162, 3, -123.45, Boolean, false)
// NUMBERTOTYPETEST(163, 4, -123.67, Boolean, false)
// NUMBERTOTYPETEST(164, 5, 123, Boolean, false)
// NUMBERTOTYPETEST(165, 6, -1, Boolean, false)
// NUMBERTOTYPETEST(166, 7, 0xffffffff, Boolean, false)
// NUMBERTOTYPETEST(167, 8, -0.0, Boolean, false)
// NUMBERTOTYPETEST(168, 9, 0.0, Boolean, false)
// NUMBERTOTYPETEST(169, 10, std::numeric_limits<double>::infinity(), Boolean, false)
// NUMBERTOTYPETEST(170, 11, std::numeric_limits<double>::quiet_NaN(), Boolean, false)
// #undef NUMBERTOTYPETEST
// 
// 
// #define NUMBERTOARRAYINDEXEMPTYTEST(testNumber, variant, val, shouldBeEmpty) \
// V8MONKEY_TEST(Number##testNumber, "ToArrayIndex works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Value> n = Number::New(value); \
//     Local<Uint32> indexHandle = n->ToArrayIndex(); \
// \
//   //  V8MONKEY_CHECK(indexHandle.IsEmpty() == shouldBeEmpty, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERTOARRAYINDEXEMPTYTEST(171, 1, 123.45, true)
// NUMBERTOARRAYINDEXEMPTYTEST(172, 2, 123.65, true)
// NUMBERTOARRAYINDEXEMPTYTEST(173, 3, -123.45, true)
// NUMBERTOARRAYINDEXEMPTYTEST(174, 4, -123.67, true)
// NUMBERTOARRAYINDEXEMPTYTEST(175, 5, 123, false)
// NUMBERTOARRAYINDEXEMPTYTEST(176, 6, -1, true)
// NUMBERTOARRAYINDEXEMPTYTEST(177, 7, 0xffffffff, false)
// NUMBERTOARRAYINDEXEMPTYTEST(178, 8, -0.0, false)
// NUMBERTOARRAYINDEXEMPTYTEST(179, 9, 0.0, false)
// NUMBERTOARRAYINDEXEMPTYTEST(180, 10, std::numeric_limits<double>::infinity(), true)
// NUMBERTOARRAYINDEXEMPTYTEST(181, 11, std::numeric_limits<double>::quiet_NaN(), true)
// #undef NUMBERTOARRAYINDEXEMPTYTEST
// 
// 
// #define NUMBERTOARRAYINDEXTEST(testNumber, variant, val, expected) \
// V8MONKEY_TEST(Number##testNumber, "ToArrayIndex returns correct value (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Value> n = Number::New(value); \
//     Local<Uint32> indexHandle = n->ToArrayIndex(); \
// \
//   //  V8MONKEY_CHECK(indexHandle->Value() == expected, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERTOARRAYINDEXTEST(182, 1, 123, 123)
// NUMBERTOARRAYINDEXTEST(183, 2, 0xffffffff, 0xffffffff)
// NUMBERTOARRAYINDEXTEST(184, 3, -0.0, 0)
// NUMBERTOARRAYINDEXTEST(185, 4, 0.0, 0)
// #undef NUMBERTOARRAYINDEXTEST
// 
// 
// #define NUMBERTOARRAYINDEXSAMETEST(testNumber, variant, val, expected) \
// V8MONKEY_TEST(Number##testNumber, "ToArrayIndex returns correct slot (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Value> n = Number::New(value); \
//     Local<Uint32> indexHandle = n->ToArrayIndex(); \
// \
//   //  V8MONKEY_CHECK((*indexHandle == *n) == expected, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERTOARRAYINDEXSAMETEST(186, 1, 123, true)
// NUMBERTOARRAYINDEXSAMETEST(187, 2, 0xffffffff, false)
// NUMBERTOARRAYINDEXSAMETEST(188, 3, -0.0, false)
// NUMBERTOARRAYINDEXSAMETEST(189, 4, 0.0, true)
// #undef NUMBERTOARRAYINDEXSAMETEST
// 
// 
// #define NUMBEREQUALTEST(testNumber, variant, val) \
// V8MONKEY_TEST(Number##testNumber, "Equals works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Value> n = Number::New(value); \
//     Local<Value> n2 = Number::New(value); \
//     Local<Value> n3 = Number::New(value - 1.0); \
// \
//   //  V8MONKEY_CHECK(n->Equals(n), "Self-equality works correctly"); \
//   //  V8MONKEY_CHECK(n->Equals(n2), "Equality works correctly"); \
//   //  V8MONKEY_CHECK(n2->Equals(n), "Equality is symmetric"); \
//   //  V8MONKEY_CHECK(!n->Equals(n3), "Inequality is correct"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBEREQUALTEST(190, 1, 123.45)
// NUMBEREQUALTEST(191, 2, 123.65)
// NUMBEREQUALTEST(192, 3, -123.45)
// NUMBEREQUALTEST(193, 4, -123.67)
// NUMBEREQUALTEST(194, 5, 123)
// NUMBEREQUALTEST(195, 6, -1)
// NUMBEREQUALTEST(196, 7, 0xffffffff)
// NUMBEREQUALTEST(197, 8, -0.0)
// NUMBEREQUALTEST(198, 9, 0.0)
// #undef NUMBEREQUALTEST
// 
// 
// V8MONKEY_TEST(String199, "Equals works correctly (10)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::infinity();
//     Local<Value> n = Number::New(value);
//     Local<Value> n2 = Number::New(value);
//     Local<Value> n3 = Number::New(123);
// 
//   //  V8MONKEY_CHECK(n->Equals(n), "Self-equality works correctly");
//   //  V8MONKEY_CHECK(n->Equals(n2), "Equality works correctly");
//   //  V8MONKEY_CHECK(!n->Equals(n3), "Inequality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String200, "Equals works correctly (11)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Value> n = Number::New(value);
//     Local<Value> n2 = Number::New(value);
// 
//   //  V8MONKEY_CHECK(!n->Equals(n), "Self-NaN equality works correctly");
//   //  V8MONKEY_CHECK(!n->Equals(n2), "NaN Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String201, "Equals works correctly (12)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 1.0;
//     Local<Value> n = Number::New(value);
//     Handle<Value> n2 = Boolean::New(true);
// 
//   //  V8MONKEY_CHECK(n->Equals(n2), "Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String202, "Equals works correctly (13)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 2.0;
//     Local<Value> n = Number::New(value);
//     Handle<Value> n2 = Boolean::New(true);
// 
//   //  V8MONKEY_CHECK(!n->Equals(n2), "Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String203, "Equals works correctly (14)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 0.0;
//     Local<Value> n = Number::New(value);
//     Handle<Value> n2 = Boolean::New(false);
// 
//   //  V8MONKEY_CHECK(n->Equals(n2), "Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String204, "Equals works correctly (15)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 1.0;
//     Local<Value> n = Number::New(value);
//     Handle<Value> n2 = Boolean::New(false);
// 
//   //  V8MONKEY_CHECK(!n->Equals(n2), "Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String205, "Equals works correctly (16)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 0.0;
//     Local<Value> n = Number::New(value);
//     Handle<Value> u = Undefined();
// 
//   //  V8MONKEY_CHECK(!n->Equals(u), "Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String206, "Equals works correctly (17)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 0.0;
//     Local<Value> n = Number::New(value);
//     Handle<Value> other = Null();
// 
//   //  V8MONKEY_CHECK(!n->Equals(other), "Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define NUMBERSTRICTEQUALTEST(testNumber, variant, val) \
// V8MONKEY_TEST(Number##testNumber, "StrictEquals works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     double value = val; \
//     Local<Value> n = Number::New(value); \
//     Local<Value> n2 = Number::New(value); \
//     Local<Value> n3 = Number::New(value - 1.0); \
// \
//   //  V8MONKEY_CHECK(n->StrictEquals(n), "Self-strict equality works correctly"); \
//   //  V8MONKEY_CHECK(n->StrictEquals(n2), "Strict equality works correctly"); \
//   //  V8MONKEY_CHECK(n2->StrictEquals(n), "Strict quality is symmetric"); \
//   //  V8MONKEY_CHECK(!n->StrictEquals(n3), "Inequality is correct"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// NUMBERSTRICTEQUALTEST(207, 1, 123.45)
// NUMBERSTRICTEQUALTEST(208, 2, 123.65)
// NUMBERSTRICTEQUALTEST(209, 3, -123.45)
// NUMBERSTRICTEQUALTEST(210, 4, -123.67)
// NUMBERSTRICTEQUALTEST(211, 5, 123)
// NUMBERSTRICTEQUALTEST(212, 6, -1)
// NUMBERSTRICTEQUALTEST(213, 7, 0xffffffff)
// NUMBERSTRICTEQUALTEST(214, 8, -0.0)
// NUMBERSTRICTEQUALTEST(215, 9, 0.0)
// #undef NUMBERSTRICTEQUALTEST
// 
// 
// V8MONKEY_TEST(String216, "StrictEquals works correctly (10)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::infinity();
//     Local<Value> n = Number::New(value);
//     Local<Value> n2 = Number::New(value);
//     Local<Value> n3 = Number::New(123);
// 
//   //  V8MONKEY_CHECK(n->StrictEquals(n), "Self-equality works correctly");
//   //  V8MONKEY_CHECK(n->StrictEquals(n2), "Strict equality works correctly");
//   //  V8MONKEY_CHECK(!n->StrictEquals(n3), "Strict inequality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String217, "StrictEquals works correctly (11)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Value> n = Number::New(value);
//     Local<Value> n2 = Number::New(value);
// 
//   //  V8MONKEY_CHECK(!n->StrictEquals(n), "Self-NaN equality works correctly");
//   //  V8MONKEY_CHECK(!n->StrictEquals(n2), "NaN Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String218, "StrictEquals works correctly (12)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 1;
//     Local<Value> n = Number::New(value);
//     Handle<Value> n2 = Boolean::New(true);
// 
//   //  V8MONKEY_CHECK(!n->StrictEquals(n2), "Strict equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String219, "StrictEquals works correctly (13)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 0;
//     Local<Value> n = Number::New(value);
//     Handle<Value> n2 = Boolean::New(false);
// 
//   //  V8MONKEY_CHECK(!n->StrictEquals(n2), "Strict Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String220, "StrictEquals works correctly (14)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 0.0;
//     Local<Value> n = Number::New(value);
//     Handle<Value> u = Undefined();
// 
//   //  V8MONKEY_CHECK(!n->StrictEquals(u), "Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(String221, "StrictEquals works correctly (15)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 0.0;
//     Local<Value> n = Number::New(value);
//     Handle<Value> other = Null();
// 
//   //  V8MONKEY_CHECK(!n->StrictEquals(other), "Equality works correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer001, "IsUndefined works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsUndefined(), "IsUndefined reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer002, "IsNull works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsNull(), "IsNull reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer003, "IsTrue works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsTrue(), "IsTrue reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer004, "IsFalse works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsFalse(), "IsFalse reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer005, "IsString works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsString(), "IsString reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer006, "IsFunction works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsFunction(), "IsFunction reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer007, "IsArray works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsArray(), "IsArray reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer008, "IsObject works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsObject(), "IsObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer009, "IsBoolean works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsBoolean(), "IsBoolean reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer010, "IsExternal works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsExternal(), "IsExternal reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer011, "IsDate works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsDate(), "IsDate reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer012, "IsBooleanObject works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsBooleanObject(), "IsBooleanObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer013, "IsNumberObject works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsNumberObject(), "IsNumberObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer014, "IsStringObject works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsStringObject(), "IsStringObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer015, "IsNativeError works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsNativeError(), "IsNativeError reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer016, "IsRegExp works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d);
// 
//   //  V8MONKEY_CHECK(!n->IsRegExp(), "IsRegExp reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define INTEGERISTEST(testNumber, variant, val, constructor, method, expected) \
// V8MONKEY_TEST(Integer##testNumber, "Is" #method " works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     int32_t value = val; \
//     Local<Value> n = Integer::constructor(value); \
// \
//   //  V8MONKEY_CHECK(n->Is##method() == expected, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// INTEGERISTEST(018, 1, 123, New, Number, true)
// INTEGERISTEST(019, 2, -1, New, Number, true)
// INTEGERISTEST(020, 3, 0, New, Number, true)
// INTEGERISTEST(021, 4, 123, NewFromUnsigned, Number, true)
// INTEGERISTEST(022, 5, 0xffffffff, NewFromUnsigned, Number, true)
// INTEGERISTEST(023, 6, 0, NewFromUnsigned, Number, true)
// 
// 
// INTEGERISTEST(024, 1, 123, New, Int32, true)
// INTEGERISTEST(025, 2, -1, New, Int32, true)
// INTEGERISTEST(026, 3, 0, New, Int32, true)
// INTEGERISTEST(027, 4, 123, NewFromUnsigned, Int32, true)
// INTEGERISTEST(028, 5, 0xffffffff, NewFromUnsigned, Int32, false)
// INTEGERISTEST(029, 6, 0, NewFromUnsigned, Int32, true)
// 
// 
// INTEGERISTEST(030, 1, 123, New, Uint32, true)
// INTEGERISTEST(031, 2, -1, New, Uint32, false)
// INTEGERISTEST(032, 3, 0, New, Uint32, true)
// INTEGERISTEST(033, 4, 123, NewFromUnsigned, Uint32, true)
// INTEGERISTEST(034, 5, 0xffffffff, NewFromUnsigned, Uint32, true)
// INTEGERISTEST(035, 6, 0, NewFromUnsigned, Uint32, true)
// #undef INTEGERISTEST
// 
// 
// #define INTEGERVALUETEST(testNumber, variant, val, type, constructor) \
// V8MONKEY_TEST(Integer##testNumber, "Value works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     type value = val; \
//     Local<Integer> n = Integer::constructor(value); \
// \
//   //  V8MONKEY_CHECK(n->Value() == value, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// INTEGERVALUETEST(036, 1, 123, int32_t, New)
// INTEGERVALUETEST(037, 2, -1, int32_t, New)
// INTEGERVALUETEST(038, 3, 0, int32_t, New)
// INTEGERVALUETEST(039, 4, 123, uint32_t, NewFromUnsigned)
// INTEGERVALUETEST(040, 5, 0xffffffff, uint32_t, NewFromUnsigned)
// INTEGERVALUETEST(041, 6, 0, uint32_t, NewFromUnsigned)
// #undef INTEGERVALUETEST
// 
// 
// #define INTEGERNUMBERVALUETEST(testNumber, variant, val, constructor) \
// V8MONKEY_TEST(Integer##testNumber, "NumberValue works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     int32_t value = val; \
//     Local<Integer> n = Integer::constructor(value); \
// \
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// INTEGERNUMBERVALUETEST(042, 1, 123, New)
// INTEGERNUMBERVALUETEST(043, 2, -1, New)
// INTEGERNUMBERVALUETEST(044, 3, 0, New)
// INTEGERNUMBERVALUETEST(045, 4, 123, NewFromUnsigned)
// INTEGERNUMBERVALUETEST(046, 5, 0xffffffff, NewFromUnsigned)
// INTEGERNUMBERVALUETEST(047, 6, 0, NewFromUnsigned)
// #undef INTEGERNUMBERVALUETEST
// 
// 
// V8MONKEY_TEST(Integer048, "NumberValue works correctly (7)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::infinity();
//     Local<Value> temp = Number::New(value);
//     Local<Integer> n = temp->ToInteger();
// 
//   //  V8MONKEY_CHECK(n->NumberValue() == value, "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer049, "NumberValue works correctly (8)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Value> temp = Number::New(value);
//     Local<Integer> n = temp->ToInteger();
// 
//   //  V8MONKEY_CHECK(std::isnan(n->NumberValue()), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define INTEGERINTEGERVALUETEST(testNumber, variant, val, constructor) \
// V8MONKEY_TEST(Integer##testNumber, "IntegerValue works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     int32_t value = val; \
//     Local<Integer> n = Integer::constructor(value); \
// \
//   //  V8MONKEY_CHECK(n->IntegerValue() == n->Value(), "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// INTEGERINTEGERVALUETEST(050, 1, 123, New)
// INTEGERINTEGERVALUETEST(051, 2, -1, New)
// INTEGERINTEGERVALUETEST(052, 3, 0, New)
// INTEGERINTEGERVALUETEST(053, 4, 123, NewFromUnsigned)
// INTEGERINTEGERVALUETEST(054, 5, 0xffffffff, NewFromUnsigned)
// INTEGERINTEGERVALUETEST(055, 6, 0, NewFromUnsigned)
// #undef INTEGERINTEGERVALUETEST
// 
// 
// V8MONKEY_TEST(Integer056, "Isolate variant works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::New(d, Isolate::GetCurrent());
// 
//   //  V8MONKEY_CHECK(n->Value() == d, "Value correct");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Integer057, "Unsigned isolate variant works correctly") {
//   uint32_t d = 0xffffffff;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> n = Integer::NewFromUnsigned(d, Isolate::GetCurrent());
//   //  V8MONKEY_CHECK(n->Value() == d, "IsUint32 reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_001, "IsUndefined works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsUndefined(), "IsUndefined reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_002, "IsNull works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsNull(), "IsNull reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_003, "IsTrue works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsTrue(), "IsTrue reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_004, "IsFalse works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsFalse(), "IsFalse reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_005, "IsString works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsString(), "IsString reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_006, "IsFunction works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsFunction(), "IsFunction reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_007, "IsArray works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsArray(), "IsArray reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_008, "IsObject works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsObject(), "IsObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_009, "IsBoolean works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsBoolean(), "IsBoolean reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_010, "IsExternal works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsExternal(), "IsExternal reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_011, "IsDate works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsDate(), "IsDate reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_012, "IsBooleanObject works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsBooleanObject(), "IsBooleanObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_013, "IsNumberObject works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsNumberObject(), "IsNumberObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_014, "IsStringObject works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsStringObject(), "IsStringObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_015, "IsNativeError works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsNativeError(), "IsNativeError reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_016, "IsRegExp works correctly") {
//   int32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::New(d);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(!n->IsRegExp(), "IsRegExp reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define INT32ISINT32TEST(testNumber, variant, val) \
// V8MONKEY_TEST(Int32_##testNumber, "IsInt32 works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     int32_t value = val; \
//     Local<Integer> temp = Integer::New(value); \
//     Local<Value> n = temp->ToInt32(); \
// \
//   //  V8MONKEY_CHECK(n->IsInt32(), "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// INT32ISINT32TEST(017, 1, 123)
// INT32ISINT32TEST(018, 2, -1)
// INT32ISINT32TEST(019, 3, 0)
// #undef INT32ISINT32TEST
// 
// 
// #define INT32ISUINT32TEST(testNumber, variant, val, expected) \
// V8MONKEY_TEST(Int32_##testNumber, "IsUint32 works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     int32_t value = val; \
//     Local<Integer> temp = Integer::New(value); \
//     Local<Value> n = temp->ToInt32(); \
// \
//   //  V8MONKEY_CHECK(n->IsUint32() == expected, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// INT32ISUINT32TEST(020, 1, 123, true)
// INT32ISUINT32TEST(021, 2, -1, false)
// INT32ISUINT32TEST(022, 3, 0, true)
// #undef INT32ISUINT32TEST
// 
// 
// #define INT32VALUETEST(testNumber, variant, val) \
// V8MONKEY_TEST(Int32_##testNumber, "Value works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     int32_t value = val; \
//     Local<Integer> temp = Integer::New(value); \
//     Local<Int32> n = temp->ToInt32(); \
// \
//   //  V8MONKEY_CHECK(n->Value() == value, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// INT32VALUETEST(023, 1, 123)
// INT32VALUETEST(024, 2, -1)
// INT32VALUETEST(025, 3, 0)
// #undef INT32VALUETEST
// 
// 
// #define INT32NUMBERVALUETEST(testNumber, variant, val) \
// V8MONKEY_TEST(Int32_##testNumber, "NumberValue works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     int32_t value = val; \
//     Local<Integer> temp = Integer::New(value); \
//     Local<Int32> n = temp->ToInt32(); \
// \
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// INT32NUMBERVALUETEST(026, 1, 123)
// INT32NUMBERVALUETEST(027, 2, -1)
// INT32NUMBERVALUETEST(028, 3, 0)
// #undef INT32NUMBERVALUETEST
// 
// 
// V8MONKEY_TEST(Int32_029, "NumberValue works correctly (4)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::infinity();
//     Local<Value> temp = Number::New(value);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_030, "NumberValue works correctly (5)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Value> temp = Number::New(value);
//     Local<Int32> n = temp->ToInt32();
// 
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define INT32INT32VALUETEST(testNumber, variant, val) \
// V8MONKEY_TEST(Int32_##testNumber, "Int32Value works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     int32_t value = val; \
//     Local<Integer> temp = Integer::New(value); \
//     Local<Int32> n = temp->ToInt32(); \
// \
//   //  V8MONKEY_CHECK(n->Int32Value() == n->Value(), "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// 
// INT32INT32VALUETEST(031, 1, 123)
// INT32INT32VALUETEST(032, 2, -1)
// INT32INT32VALUETEST(033, 3, 0)
// #undef INTEGERINT32VALUETEST
// 
// 
// V8MONKEY_TEST(Int32_034, "Conversions of special values work as expected (1)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::infinity();
//     Local<Value> temp = Number::New(value);
//     Local<Int32> n = temp->ToInt32();
//     Local<Number> n2 = n->ToNumber();
// 
//   //  V8MONKEY_CHECK(n2->Value() == n->Value(), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Int32_035, "Conversions of special values work as expected (2)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Value> temp = Number::New(value);
//     Local<Int32> n = temp->ToInt32();
//     Local<Number> n2 = n->ToNumber();
// 
//   //  V8MONKEY_CHECK(n2->Value() == n->Value(), "Correct value returned");
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_001, "IsUndefined works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsUndefined(), "IsUndefined reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_002, "IsNull works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsNull(), "IsNull reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_003, "IsTrue works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsTrue(), "IsTrue reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_004, "IsFalse works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsFalse(), "IsFalse reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_005, "IsString works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsString(), "IsString reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_006, "IsFunction works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsFunction(), "IsFunction reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_007, "IsArray works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsArray(), "IsArray reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_008, "IsObject works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsObject(), "IsObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_009, "IsBoolean works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsBoolean(), "IsBoolean reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_010, "IsExternal works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsExternal(), "IsExternal reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_011, "IsDate works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsDate(), "IsDate reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_012, "IsBooleanObject works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsBooleanObject(), "IsBooleanObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_013, "IsNumberObject works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsNumberObject(), "IsNumberObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_014, "IsStringObject works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsStringObject(), "IsStringObject reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_015, "IsNativeError works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsNativeError(), "IsNativeError reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_016, "IsRegExp works correctly") {
//   uint32_t d = 123;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Integer> temp = Integer::NewFromUnsigned(d);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(!n->IsRegExp(), "IsRegExp reports correct result");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define UINT32ISINT32TEST(testNumber, variant, val, expected) \
// V8MONKEY_TEST(Uint32_##testNumber, "IsInt32 works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     uint32_t value = val; \
//     Local<Integer> temp = Integer::NewFromUnsigned(value); \
//     Local<Value> n = temp->ToUint32(); \
// \
//   //  V8MONKEY_CHECK(n->IsInt32() == expected, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// UINT32ISINT32TEST(017, 1, 123, true)
// UINT32ISINT32TEST(018, 2, 0xffffffff, false)
// UINT32ISINT32TEST(019, 3, 0, true)
// #undef UINT32ISINT32TEST
// 
// 
// #define UINT32ISUINT32TEST(testNumber, variant, val) \
// V8MONKEY_TEST(Uint32_##testNumber, "IsUint32 works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     uint32_t value = val; \
//     Local<Integer> temp = Integer::NewFromUnsigned(value); \
//     Local<Value> n = temp->ToUint32(); \
// \
//   //  V8MONKEY_CHECK(n->IsUint32(), "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// UINT32ISUINT32TEST(020, 1, 123)
// UINT32ISUINT32TEST(021, 2, 0xffffffff)
// UINT32ISUINT32TEST(022, 3, 0)
// #undef UINT32ISUINT32TEST
// 
// 
// #define UINT32VALUETEST(testNumber, variant, val) \
// V8MONKEY_TEST(Uint32_##testNumber, "Value works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     uint32_t value = val; \
//     Local<Integer> temp = Integer::NewFromUnsigned(value); \
//     Local<Uint32> n = temp->ToUint32(); \
// \
//   //  V8MONKEY_CHECK(n->Value() == value, "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// UINT32VALUETEST(023, 1, 123)
// UINT32VALUETEST(024, 2, 0xffffffff)
// UINT32VALUETEST(025, 3, 0)
// #undef UINT32VALUETEST
// 
// 
// #define UINT32NUMBERVALUETEST(testNumber, variant, val) \
// V8MONKEY_TEST(Uint32_##testNumber, "NumberValue works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     uint32_t value = val; \
//     Local<Integer> temp = Integer::NewFromUnsigned(value); \
//     Local<Uint32> n = temp->ToUint32(); \
// \
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// UINT32NUMBERVALUETEST(026, 1, 123)
// UINT32NUMBERVALUETEST(027, 2, 0xffffffff)
// UINT32NUMBERVALUETEST(028, 3, 0)
// #undef UINT32NUMBERVALUETEST
// 
// 
// V8MONKEY_TEST(Uint32_029, "NumberValue works correctly (4)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::infinity();
//     Local<Value> temp = Number::New(value);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_030, "NumberValue works correctly (5)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Value> temp = Number::New(value);
//     Local<Uint32> n = temp->ToUint32();
// 
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// #define UINT32UINT32VALUETEST(testNumber, variant, val) \
// V8MONKEY_TEST(Uint32_##testNumber, "Uint32Value works correctly (" #variant ")") { \
//   V8::Initialize(); \
// \
//   { \
//     HandleScope h; \
//     uint32_t value = val; \
//     Local<Integer> temp = Integer::NewFromUnsigned(value); \
//     Local<Uint32> n = temp->ToUint32(); \
// \
//   //  V8MONKEY_CHECK(n->Uint32Value() == n->Value(), "Correct value returned"); \
//   } \
//  \
//   Isolate::GetCurrent()->Exit(); \
//   V8::Dispose(); \
// }
// 
// 
// 
// UINT32UINT32VALUETEST(031, 1, 123)
// UINT32UINT32VALUETEST(032, 2, 0xffffffff)
// UINT32UINT32VALUETEST(033, 3, 0)
// #undef INTEGERUINT32VALUETEST
// 
// 
// V8MONKEY_TEST(Uint32_034, "Conversions of special values work as expected (1)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::infinity();
//     Local<Value> temp = Number::New(value);
//     Local<Uint32> n = temp->ToUint32();
//     Local<Number> n2 = n->ToNumber();
// 
//   //  V8MONKEY_CHECK(n2->Value() == n->Value(), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(Uint32_035, "Conversions of special values work as expected (2)") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = std::numeric_limits<double>::quiet_NaN();
//     Local<Value> temp = Number::New(value);
//     Local<Uint32> n = temp->ToUint32();
//     Local<Number> n2 = n->ToNumber();
// 
//   //  V8MONKEY_CHECK(n2->Value() == n->Value(), "Correct value returned");
//   //  V8MONKEY_CHECK(n->NumberValue() == n->Value(), "Correct value returned");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// */
