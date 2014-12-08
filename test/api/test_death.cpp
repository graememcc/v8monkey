#include "v8.h"
#include "V8MonkeyTest.h"


using namespace v8;


V8MONKEY_TEST(Death001, "V8::IsDead normally returns false (no init)") {
  V8MONKEY_CHECK(!V8::IsDead(), "I ain't dead yet!");
}


V8MONKEY_TEST(Death002, "V8::IsDead normally returns false (with init)") {
  V8::Initialize();
  V8MONKEY_CHECK(!V8::IsDead(), "I ain't dead yet!");

  // Leave the default isolate (we entered in the initialize call)
  Isolate::GetCurrent()->Exit();

  V8::Dispose();
}


V8MONKEY_TEST(Death003, "V8::IsDead returns true after disposal") {
  V8::Initialize();
  // Leave the default isolate (we entered in the initialize call)
  Isolate::GetCurrent()->Exit();

  V8::Dispose();
  V8MONKEY_CHECK(V8::IsDead(), "V8 is dead after disposal");
}
