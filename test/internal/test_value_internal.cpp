// Required for ISOLATE_INIT_TESTS
#include "runtime/isolate.h"

// ISOLATE_INIT_TESTS TestUtils
#include "utils/test.h"

// Unit-testing support
#include "V8MonkeyTest.h"

// HandleScope Integer Int32 Local Number Uint32 Value V8
#include "v8.h"

// TriggerFatalError
#include "utils/V8MonkeyCommon.h"
// 
// 
// using namespace v8;
// using namespace v8::V8Monkey;
// 
// 
// namespace {
//   // Suppress error abortions
//   void dummyFatalErrorHandler(const char*, const char*) {return;}
// 
// 
//   bool errorCallbackCalled = false;
// 
//   void errorCallback(const char*, const char*) {
//     errorCallbackCalled = true;
//   }
// }
// 
// 
// #define TRIGGERS_ERROR_TEST(num, method) \
// V8MONKEY_TEST(IntValue##num, #method " triggers error if V8 dead") { \
//   TestUtils::AutoTestCleanup ac;\
//   V8::Initialize();\
// \
//   { \
//     HandleScope h; \
//     Local<Value> n = Number::New(123.45); \
//  \
//     TestUtils::SetHandlerAndKill(errorCallback); \
//     errorCallbackCalled = false; \
//     n->method();\
// \
//     V8MONKEY_CHECK(errorCallbackCalled, "Error triggered");\
//   }\
// }
// 
// 
// #define RETURNS_FALSE_TEST(num, method) \
// V8MONKEY_TEST(IntValue##num, #method " returns false if V8 dead") { \
//   TestUtils::AutoTestCleanup ac;\
//   V8::Initialize();\
// \
//   { \
//     HandleScope h; \
//     Local<Value> n = Number::New(123.45); \
//  \
//     TestUtils::SetHandlerAndKill(dummyFatalErrorHandler); \
//     V8MONKEY_CHECK(!n->method(), "Returned false"); \
//   }\
// }
// 
// #define BASE_TESTS(num1, num2, method) \
//   TRIGGERS_ERROR_TEST(num1, method) \
//   RETURNS_FALSE_TEST(num2, method)
// 
// 
// // First, we test the base implementations supplied by Value, then below, we will test the individual type overloads
// BASE_TESTS(001, 002, IsUndefined)
// BASE_TESTS(003, 004, IsNull)
// BASE_TESTS(005, 006, IsTrue)
// BASE_TESTS(007, 008, IsTrue)
// BASE_TESTS(009, 010, IsFalse)
// BASE_TESTS(011, 012, IsString)
// BASE_TESTS(013, 014, IsFunction)
// BASE_TESTS(015, 016, IsArray)
// BASE_TESTS(017, 018, IsObject)
// BASE_TESTS(019, 020, IsBoolean)
// BASE_TESTS(021, 022, IsExternal)
// BASE_TESTS(023, 024, IsInt32)
// BASE_TESTS(025, 026, IsUint32)
// BASE_TESTS(027, 028, IsDate)
// BASE_TESTS(029, 030, IsBooleanObject)
// BASE_TESTS(031, 032, IsNumberObject)
// BASE_TESTS(033, 034, IsStringObject)
// BASE_TESTS(035, 036, IsNativeError)
// BASE_TESTS(037, 038, IsRegExp)
// #undef BASE_TESTS
// #undef RETURNS_FALSE_TEST
// #undef TRIGGERS_ERROR_TEST
// 
// 
// V8MONKEY_TEST(IntNumber001, "Number::IsNumber triggers error if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Value> n = Number::New(123.45);
// 
//     TestUtils::SetHandlerAndKill(errorCallback);
//     errorCallbackCalled = false;
//     n->IsNumber();
// 
//     V8MONKEY_CHECK(errorCallbackCalled, "Error triggered");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntNumber002, "Number::IsNumber triggers error if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Value> n = Number::New(123.45);
// 
//     TestUtils::SetHandlerAndKill(dummyFatalErrorHandler);
//     V8MONKEY_CHECK(!n->IsNumber(), "Returned false");
//   }
// }
// 
// 
// ISOLATE_INIT_TESTS(IntNumber, 003, 004, 005, {
//   {
//     HandleScope h;
//     // Deliberate lack of assignment
//     Number::New(123.45);
//   }
// })
// 
// 
// V8MONKEY_TEST(IntNumber006, "Number::Value returns 0 if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
// 
//     Local<Number> l = Number::New(123.45);
// 
//     V8::SetFatalErrorHandler(dummyFatalErrorHandler);
//     V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);
// 
//     V8MONKEY_CHECK(l->Value() == 0, "Zero returned");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntNumber007, "Number::Value triggers error if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
// 
//     Local<Number> l = Number::New(123.45);
// 
//     TestUtils::SetHandlerAndKill(errorCallback);
//     errorCallbackCalled = false;
// 
//     l->Value();
//     V8MONKEY_CHECK(errorCallbackCalled, "Error triggered");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntNumber008, "Equals fails if V8 dead") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 123.0;
//     Local<Value> n = Number::New(value);
//     Local<Value> n2 = Number::New(value);
//     TestUtils::SetHandlerAndKill(errorCallback);
// 
//     V8MONKEY_CHECK(!n->Equals(n), "Self-equality returns false");
//     V8MONKEY_CHECK(!n->Equals(n2), "Equals returns correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(IntNumber009, "StrictEquals fails if V8 dead") {
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     double value = 123.0;
//     Local<Value> n = Number::New(value);
//     Local<Value> n2 = Number::New(value);
//     TestUtils::SetHandlerAndKill(errorCallback);
// 
//     V8MONKEY_CHECK(!n->StrictEquals(n), "Self-equality returns false");
//     V8MONKEY_CHECK(!n->StrictEquals(n2), "StrictEquals returns correctly");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(IntInteger001, "Integer::IsInt32 triggers error if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Value> n = Integer::New(123);
// 
//     TestUtils::SetHandlerAndKill(errorCallback);
//     errorCallbackCalled = false;
//     n->IsInt32();
// 
//     V8MONKEY_CHECK(errorCallbackCalled, "Error triggered");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntInteger002, "Integer::IsInt32 returns false if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Value> n = Integer::New(123);
// 
//     TestUtils::SetHandlerAndKill(dummyFatalErrorHandler);
//     V8MONKEY_CHECK(!n->IsInt32(), "Returned false");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntInteger003, "Integer::IsUint32 triggers error if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Value> n = Integer::New(123);
// 
//     TestUtils::SetHandlerAndKill(errorCallback);
//     errorCallbackCalled = false;
//     n->IsUint32();
// 
//     V8MONKEY_CHECK(errorCallbackCalled, "Error triggered");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntInteger004, "Integer::IsUint32 returns false if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
//     Local<Value> n = Integer::New(123);
// 
//     TestUtils::SetHandlerAndKill(dummyFatalErrorHandler);
//     V8MONKEY_CHECK(!n->IsUint32(), "Returned false");
//   }
// }
// 
// 
// ISOLATE_INIT_TESTS(IntInteger, 005, 006, 007, {
//   {
//     HandleScope h;
//     // Lack of assignment is deliberate here
//     Integer::New(123);
//   }
// })
// 
// 
// ISOLATE_INIT_TESTS(IntInteger, 008, 009, 010, {
//   {
//     HandleScope h;
//     // Lack of assignment is deliberate here
//     Integer::NewFromUnsigned(123);
//   }
// })
// 
// 
// V8MONKEY_TEST(IntInteger011, "Integer::Value returns 0 if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
// 
//     Local<Integer> l = Integer::New(123);
// 
//     V8::SetFatalErrorHandler(dummyFatalErrorHandler);
//     V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);
// 
//     V8MONKEY_CHECK(l->Value() == 0, "Zero returned");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntInteger012, "Integer::Value triggers error if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
// 
//     Local<Integer> l = Integer::New(123);
// 
//     TestUtils::SetHandlerAndKill(errorCallback);
//     errorCallbackCalled = false;
// 
//     l->Value();
//     V8MONKEY_CHECK(errorCallbackCalled, "Error triggered");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntInteger013, "Int32::Value returns 0 if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
// 
//     Local<Int32> l = Integer::New(123);
// 
//     V8::SetFatalErrorHandler(dummyFatalErrorHandler);
//     V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);
// 
//     V8MONKEY_CHECK(l->Value() == 0, "Zero returned");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntInteger014, "Int32::Value triggers error if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
// 
//     Local<Int32> l = Integer::New(123);
// 
//     TestUtils::SetHandlerAndKill(errorCallback);
//     errorCallbackCalled = false;
// 
//     l->Value();
//     V8MONKEY_CHECK(errorCallbackCalled, "Error triggered");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntInteger015, "Uint32::Value returns 0 if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
// 
//     Local<Uint32> l = Integer::New(123);
// 
//     V8::SetFatalErrorHandler(dummyFatalErrorHandler);
//     V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);
// 
//     V8MONKEY_CHECK(l->Value() == 0, "Zero returned");
//   }
// }
// 
// 
// V8MONKEY_TEST(IntInteger016, "Uint32::Value triggers error if V8 dead") {
//   TestUtils::AutoTestCleanup ac;
//   V8::Initialize();
// 
//   {
//     HandleScope h;
// 
//     Local<Uint32> l = Integer::NewFromUnsigned(123);
// 
//     TestUtils::SetHandlerAndKill(errorCallback);
//     errorCallbackCalled = false;
// 
//     l->Value();
//     V8MONKEY_CHECK(errorCallbackCalled, "Error triggered");
//   }
// }
// 
// 
// // XXX Add value tests for IsNumber once we have implemented a non-number type
// // XXX Do we need similar tests for Boolean Null Undefined String?
// /*
//       Local<Boolean> ToBoolean() const;
// */
// 
//       //Local<Number> ToNumber() const;
// 
// /*
//       Local<String> ToString() const;
// */
