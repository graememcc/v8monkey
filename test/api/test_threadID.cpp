/*
// TODO: If multi-threaded tests cannot be re-enabled, this header can be removed
// Thread
#include "platform/platform.h"

// V8::GetCurrentThreadID Isolate::GetCurrent V8::Initialize
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"
// 
// 
// using namespace v8;
// using namespace v8::V8Platform;
// 
// 
// 
// V8MONKEY_TEST(ThreadID001, "Main thread gets thread id of 1 (no explicit engine initialization)") {
//   int threadID = V8::GetCurrentThreadId();
//   V8MONKEY_CHECK(threadID == 1, "First thread is assigned correct ID");
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(ThreadID002, "Main thread gets thread id of 1 (explicit engine initialization)") {
//   V8::Initialize();
//   int threadID = V8::GetCurrentThreadId();
//   V8MONKEY_CHECK(threadID == 1, "First thread is assigned correct ID");
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
//  * Tests disabled after threading support disabled
//  *
// namespace {
//   V8MONKEY_TEST_HELPER(ReturnOwnThreadID) {
//     int threadID;
// 
//     // API requirement: one must hold a locker
//     {
//       Locker l;
//       threadID = V8::GetCurrentThreadId();
//     }
// 
//     return reinterpret_cast<void*>(threadID);
//   }
// }
// 
// 
// V8MONKEY_TEST(ThreadID003, "Child threads are assigned thread ids greater than 1") {
//   Thread child(ReturnOwnThreadID);
//   child.Run();
// 
//   intptr_t threadID = reinterpret_cast<intptr_t>(child.Join());
// 
//   V8MONKEY_CHECK(threadID > 1, "Child thread assigned correct ID");
// }
// 
// 
// V8MONKEY_TEST(ThreadID004, "Child threads are assigned distinct ids") {
//   Thread child1(ReturnOwnThreadID);
//   Thread child2(ReturnOwnThreadID);
// 
//   child1.Run();
//   child2.Run();
// 
//   intptr_t threadID1 = reinterpret_cast<intptr_t>(child1.Join());
//   intptr_t threadID2 = reinterpret_cast<intptr_t>(child2.Join());
// 
//   V8MONKEY_CHECK(threadID1 > 1, "Child thread 1 assigned valid ID");
//   V8MONKEY_CHECK(threadID2 > 1, "Child thread 2 assigned valid ID");
//   V8MONKEY_CHECK(threadID1 != threadID2, "Child threads assigned distinct IDs");
// }
// 
// */
