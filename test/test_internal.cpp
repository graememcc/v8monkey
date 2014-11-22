#include "../include/v8.h"
#include "V8MonkeyTest.h"
#include "testAPI.h"


V8MONKEY_TEST(Inte001, "Internal dummy test") {
  V8MONKEY_CHECK(v8::V8Test::IsInternal(), "This works!");
}
