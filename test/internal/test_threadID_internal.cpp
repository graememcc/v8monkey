// InternalIsolate is required for ISOLATE_INIT_TESTS
#include "runtime/isolate.h"

// ISOLATE_INIT_TESTS
#include "utils/test.h"

// GetCurrentThreadID
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;


ISOLATE_INIT_TESTS(IntThreadID, 001, 002, 003, {
  V8::GetCurrentThreadId();
})
