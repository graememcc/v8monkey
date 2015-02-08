// TestUtils
#include "utils/test.h"

// Initialize, SetFatalErrorHandler
#include "v8.h"

// TriggerFatalError
#include "utils/V8MonkeyCommon.h"

// Unit-testing support
#include "V8MonkeyTest.h"
// 
// 
// namespace {
//   // Suppress error arbortions
//   void dummyFatalErrorHandler(const char*, const char*) {return;}
// }
// 
// 
// V8MONKEY_TEST(DeathInternal001, "V8::IsDead returns true after internal error (no init)") {
//   v8::V8Monkey::TestUtils::AutoTestCleanup ac;
// 
//   v8::V8::SetFatalErrorHandler(dummyFatalErrorHandler);
//   v8::V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);
//   V8MONKEY_CHECK(v8::V8::IsDead(), "Dead after internal error");
// }
// 
// 
// V8MONKEY_TEST(DeathInternal002, "V8::IsDead returns true after internal error (with init)") {
//   v8::V8Monkey::TestUtils::AutoTestCleanup ac;
// 
//   v8::V8::Initialize();
//   v8::V8::SetFatalErrorHandler(dummyFatalErrorHandler);
//   v8::V8Monkey::V8MonkeyCommon::TriggerFatalError(nullptr, nullptr);
//   V8MONKEY_CHECK(v8::V8::IsDead(), "Dead after internal error");
// }
