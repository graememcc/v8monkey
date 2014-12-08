#include "v8.h"

#include "isolate.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;


IMPLICIT_INIT_TESTS(IntThreadID, 001, 002, {
  V8::GetCurrentThreadId();
})
