#include "v8.h"

#include "runtime/isolate.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;


ISOLATE_INIT_TESTS(IntThreadID, 001, 002, 003, {
  V8::GetCurrentThreadId();
})
