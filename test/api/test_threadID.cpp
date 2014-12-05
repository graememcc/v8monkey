#include "v8.h"
#include "platform.h"
#include "V8MonkeyTest.h"


using namespace v8;
using namespace v8::V8Platform;



V8MONKEY_TEST(ThreadID001, "Main thread gets thread id of 1 (no explicit engine initialization)") {
  int threadID = V8::GetCurrentThreadId();
  V8MONKEY_CHECK(threadID == 1, "First thread is assigned correct ID");
}


V8MONKEY_TEST(ThreadID002, "Main thread gets thread id of 1 (explicit engine initialization)") {
  V8::Initialize();
  int threadID = V8::GetCurrentThreadId();
  V8MONKEY_CHECK(threadID == 1, "First thread is assigned correct ID");
}


namespace {
  V8MONKEY_TEST_HELPER(ReturnOwnThreadID) {
    return reinterpret_cast<void*>(V8::GetCurrentThreadId());
  }
}


V8MONKEY_TEST(ThreadID003, "Child threads are assigned thread ids greater than 1") {
  Thread* child = Platform::Platform::CreateThread(ReturnOwnThreadID);
  child->Run();

  intptr_t threadID = reinterpret_cast<intptr_t>(child->Join());

  V8MONKEY_CHECK(threadID > 1, "Child thread assigned correct ID");
}


V8MONKEY_TEST(ThreadID004, "Child threads are assigned distinct ids") {
  Thread* child1 = Platform::CreateThread(ReturnOwnThreadID);
  Thread* child2 = Platform::CreateThread(ReturnOwnThreadID);

  child1->Run();
  child2->Run();

  intptr_t threadID1 = reinterpret_cast<intptr_t>(child1->Join());
  intptr_t threadID2 = reinterpret_cast<intptr_t>(child2->Join());

  V8MONKEY_CHECK(threadID1 > 1, "Child thread 1 assigned valid ID");
  V8MONKEY_CHECK(threadID2 > 1, "Child thread 2 assigned valid ID");
  V8MONKEY_CHECK(threadID1 != threadID2, "Child threads assigned distinct IDs");
}