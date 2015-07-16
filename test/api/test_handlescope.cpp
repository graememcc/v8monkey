/*
// HandleScope, Integer, Isolate, Local, Locker, Number, V8
#include "v8.h"

// XXX Check consistency: "testing support" vs "test support"
// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;
// 
// 
// namespace {
//   int errorCaught = 0;
//   void fatalErrorHandler(const char*, const char*) {
//     errorCaught = 1;
//   }
// }
// 
// 
// V8MONKEY_TEST(HandleScope001, "HandleScope construction fails if locker ever constructed and isolate not locked") {
//   Isolate* i = Isolate::New();
//   i->Enter();
// 
//   {
//     Locker l(i);
//   }
// 
//   errorCaught = 0;
//   V8::SetFatalErrorHandler(fatalErrorHandler);
//   {
//     HandleScope h;
//   }
// 
//   V8MONKEY_CHECK(V8::IsDead() && errorCaught != 0, "Fatal error if locker constructed and lock not held");
//   i->Exit();
//   i->Dispose();
// }
// 
// 
// V8MONKEY_TEST(HandleScope002, "NumberOfHandles initially empty") {
//   {
//     HandleScope h;
//     V8MONKEY_CHECK(HandleScope::NumberOfHandles() == 0, "Number of handles correct");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(HandleScope003, "NumberOfHandles correct after creating handle") {
//   {
//     HandleScope h;
//     Local<Integer> l(Integer::New(42));
//     V8MONKEY_CHECK(HandleScope::NumberOfHandles() == 1, "Number of handles correct");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(HandleScope004, "NumberOfHandles count is additive") {
//   {
//     HandleScope h;
//     Local<Integer> l(Integer::New(42));
//     {
//       HandleScope i;
//       Local<Integer> m(Integer::New(1));
//       Local<Integer> n(Number::New(2.3));
//       V8MONKEY_CHECK(HandleScope::NumberOfHandles() == 3, "Number of handles correct");
//     }
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(HandleScope005, "NumberOfHandles count is per isolate (1)") {
//   {
//     HandleScope h;
//     Local<Integer> l(Integer::New(42));
//     {
//       Isolate* i = Isolate::New();
//       i->Enter();
//       V8MONKEY_CHECK(HandleScope::NumberOfHandles() == 0, "Number of handles correct");
//       i->Exit();
//       i->Dispose();
//     }
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(HandleScope006, "NumberOfHandles count is per isolate (2)") {
//   {
//     HandleScope h;
//     Local<Integer> l(Integer::New(42));
//     {
//       Isolate* i = Isolate::New();
//       i->Enter();
//       i->Exit();
//       i->Dispose();
//     }
//     V8MONKEY_CHECK(HandleScope::NumberOfHandles() == 1, "Number of handles correct");
//   }
// 
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
// 
// 
// V8MONKEY_TEST(HandleScope007, "Attempting to close twice should fail") {
//   {
//     HandleScope h;
//     Local<Integer> l3;
//     Local<Integer> l4;
//     {
//       HandleScope i;
//       Local<Integer> l1(Integer::New(1));
//       Local<Integer> l2(Integer::New(2));
//       errorCaught = 0;
//       V8::SetFatalErrorHandler(fatalErrorHandler);
//       l3 = i.Close(l1);
//       l4 = i.Close(l2);
//     }
//   }
// 
//   V8MONKEY_CHECK(V8::IsDead() && errorCaught != 0, "Fatal error if closing twice");
//   Isolate::GetCurrent()->Exit();
//   V8::Dispose();
// }
*/
