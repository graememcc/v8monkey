#include "v8.h"
#include "V8MonkeyTest.h"
#include "testAPI.h"


V8MONKEY_TEST(Inte001, "Internal dummy test") {
  V8MONKEY_CHECK(v8::V8Test::IsInternal(), "This works!");
}


V8MONKEY_TEST(Inte002, "Internal dummy test 2") {
  V8MONKEY_CHECK(v8::V8Test::IsInternal2() == false, "This works too!");
}


V8MONKEY_TEST(Inte003, "Internal dummy test 3") {
  v8::V8::Initialize();
}


V8MONKEY_TEST(Inte004, "Internal dummy test 3") {
  v8::V8::IsDead();
}
