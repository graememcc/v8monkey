#include "v8.h"
#include "V8MonkeyTest.h"


V8MONKEY_TEST(Death001, "V8::IsDead normally returns false (no init)") {
  V8MONKEY_CHECK(!v8::V8::IsDead(), "I ain't dead yet!");
}


V8MONKEY_TEST(Death002, "V8::IsDead normally returns false (with init)") {
  v8::V8::Initialize();
  V8MONKEY_CHECK(!v8::V8::IsDead(), "I ain't dead yet!");
  v8::V8::Dispose();
}


V8MONKEY_TEST(Death003, "V8::IsDead returns true after disposal") {
  v8::V8::Initialize();
  v8::V8::Dispose();
  V8MONKEY_CHECK(v8::V8::IsDead(), "V8 is dead after disposal");
}
