#include "v8.h"
#include "v8monkey_common.h"
#include "V8MonkeyTest.h"


V8MONKEY_TEST(DeathInternal001, "V8::IsDead returns true after internal error (no init)") {
  v8::V8Monkey::V8MonkeyCommon::TriggerFatalError();
  V8MONKEY_CHECK(v8::V8::IsDead(), "Dead after internal error");
}


V8MONKEY_TEST(DeathInternal002, "V8::IsDead returns true after internal error (with init)") {
  v8::V8::Initialize();
  v8::V8Monkey::V8MonkeyCommon::TriggerFatalError();
  V8MONKEY_CHECK(v8::V8::IsDead(), "Dead after internal error");
  v8::V8::Dispose();
}
