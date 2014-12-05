#include "v8.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;


V8MONKEY_TEST(IntThreadID001, "GetCurrentThreadId implicitly initializes") {
  int threadID = V8::GetCurrentThreadId();
  V8MONKEY_CHECK(V8Monkey::TestUtils::IsV8Initialized(), "V8Monkey was initialized");
  V8::Dispose();
}
