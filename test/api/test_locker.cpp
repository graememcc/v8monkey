// Thread
#include "platform.h"

// Class definition, Isolate::New
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"
// 
// 
// using namespace v8;
// 
// 
// namespace {
//   void* CheckIsLockedStatus(void* iso) {
//     Isolate* i = reinterpret_cast<Isolate*>(iso);
//     return reinterpret_cast<void*>(Locker::IsLocked(i));
//   }
// }
// 
// 
// V8MONKEY_TEST(Locker001, "IsActive is false if no lockers ever constructed") {
//   V8MONKEY_CHECK(!Locker::IsActive(), "IsActive reported false");
// }
// 
// 
// V8MONKEY_TEST(Locker002, "IsActive reports true when locker constructed") {
//   Isolate* i = Isolate::New();
// 
//   {
//     Locker l(i);
//   }
// 
//   V8MONKEY_CHECK(Locker::IsActive(), "IsActive reported true");
//   i->Dispose();
// }
// 
// 
// V8MONKEY_TEST(Locker003, "IsActive remains true after locker destruction") {
//   Isolate* i = Isolate::New();
//   {
//     Locker l(i);
//   }
// 
//   V8MONKEY_CHECK(Locker::IsActive(), "IsActive reported true");
// 
//   i->Dispose();
// }
// 
// 
// V8MONKEY_TEST(Locker004, "IsLocked reports false for default isolate when not locked") {
//   V8MONKEY_CHECK(!Locker::IsLocked(Isolate::GetCurrent()), "Isolate not locked");
// }
// 
// 
// V8MONKEY_TEST(Locker005, "IsLocked reports false for non-default isolate when not locked") {
//   Isolate* i = Isolate::New();
//   V8MONKEY_CHECK(!Locker::IsLocked(i), "Isolate not locked");
//   i->Dispose();
// }
// 
// 
// V8MONKEY_TEST(Locker006, "IsLocked reports true for default isolate when locked") {
//   Locker l(Isolate::GetCurrent());
//   V8MONKEY_CHECK(Locker::IsLocked(Isolate::GetCurrent()), "Isolate locked");
// }
// 
// 
// V8MONKEY_TEST(Locker007, "IsLocked reports false for default isolate when locker destroyed") {
//   {
//     Locker l(Isolate::GetCurrent());
//   }
// 
//   V8MONKEY_CHECK(!Locker::IsLocked(Isolate::GetCurrent()), "Isolate not locked");
// }
// 
// 
// V8MONKEY_TEST(Locker008, "IsLocked reports true for non-default isolate when locked") {
//   Isolate* i = Isolate::New();
//   {
//     // Run in a new scope to ensure we don't dispose of the isolate before the Locker's destructor runs
//     Locker l(i);
//     V8MONKEY_CHECK(Locker::IsLocked(i), "Isolate locked");
//   }
// 
//   i->Dispose();
// }
// 
// 
// V8MONKEY_TEST(Locker009, "IsLocked reports false for non-default isolate when locker destroyed") {
//   Isolate* i = Isolate::New();
//   {
//     Locker l(i);
//   }
// 
//   V8MONKEY_CHECK(!Locker::IsLocked(i), "Isolate not locked");
// 
//   i->Dispose();
// }
// 
// 
// V8MONKEY_TEST(Locker010, "IsLocked reports true if multiple locks created for same isolate, and only some destroyed") {
//   {
//     Locker l1(Isolate::GetCurrent());
//     {
//       Locker l2(Isolate::GetCurrent());
//       {
//         Locker l3(Isolate::GetCurrent());
//       }
//       V8MONKEY_CHECK(Locker::IsLocked(), "Isolate still locked with mutiple lockers");
//     }
//   }
// }
// 
// 
// V8MONKEY_TEST(Locker011, "IsLocked reports false if multiple locks created for same isolate, and all destroyed") {
//   {
//     Locker l1(Isolate::GetCurrent());
//     {
//       Locker l2(Isolate::GetCurrent());
//       {
//         Locker l3(Isolate::GetCurrent());
//       }
//     }
//   }
// 
//   V8MONKEY_CHECK(!Locker::IsLocked(), "Isolate not locked after mutiple lockers");
// }
// 
// 
// V8MONKEY_TEST(Locker012, "IsLocked tests default isolate if none supplied (1)") {
//   V8MONKEY_CHECK(!Locker::IsLocked(), "Correctly checked default isolate");
// }
// 
// 
// V8MONKEY_TEST(Locker013, "IsLocked tests default isolate if none supplied (2)") {
//   Locker l(Isolate::GetCurrent());
//   V8MONKEY_CHECK(Locker::IsLocked(), "Correctly checked default isolate");
// }
// 
// 
// V8MONKEY_TEST(Locker014, "IsLocked reports false if unlocker constructed for single lock") {
//   Locker l(Isolate::GetCurrent());
//   Unlocker u(Isolate::GetCurrent());
//   V8MONKEY_CHECK(!Locker::IsLocked(), "Value correct after unlocker construction");
// }
// 
// 
// V8MONKEY_TEST(Locker015, "IsLocked reports false if unlocker constructed for multiple locks") {
//   Locker l(Isolate::GetCurrent());
//   {
//     Locker m(Isolate::GetCurrent());
//     {
//       Locker n(Isolate::GetCurrent());
//       Unlocker u(Isolate::GetCurrent());
//       V8MONKEY_CHECK(!Locker::IsLocked(), "Value correct after unlocker construction");
//     }
//   }
// }
// 
// 
// V8MONKEY_TEST(Locker016, "IsLocked reports true after unlocker constructed deep in multiple locks") {
//   {
//     Locker l(Isolate::GetCurrent());
//     {
//       Locker m(Isolate::GetCurrent());
//       {
//         Locker n(Isolate::GetCurrent());
//         {
//           Unlocker u(Isolate::GetCurrent());
//         }
//         V8MONKEY_CHECK(Locker::IsLocked(), "Value correct after unlocker destroyed");
//       }
//       V8MONKEY_CHECK(Locker::IsLocked(), "Value correct after lock 3 destroyed");
//     }
//     V8MONKEY_CHECK(Locker::IsLocked(), "Value correct after lock 2 destroyed");
//   }
//   V8MONKEY_CHECK(!Locker::IsLocked(), "Value correct after lock 1 destroyed");
// }
// 
// 
// V8MONKEY_TEST(Locker017, "IsLocked reports true when queried from different thread") {
//   // The game here is that we lock the default isolate, and pass that as an argument
//   // to the thread which then queries it
//   Locker l(Isolate::GetCurrent());
//   V8Platform::Thread t(CheckIsLockedStatus);
//   t.Run(Isolate::GetCurrent());
//   V8MONKEY_CHECK(t.Join(), "Thread found isolate was locked");
// }
// 
// 
// V8MONKEY_TEST(Locker018, "Locker destruction unlocks correct isolate") {
//   Isolate* a = Isolate::New();
//   Isolate* b = Isolate::New();
// 
//   {
//     Locker l(b);
//     a->Enter();
//     V8MONKEY_CHECK(Isolate::GetCurrent() == a, "Sanity check");
//   }
//   V8MONKEY_CHECK(!Locker::IsLocked(b), "Correct isolate unlocked");
// 
//   b->Dispose();
//   a->Exit();
//   a->Dispose();
// }
// 
// 
// V8MONKEY_TEST(Locker019, "Unlocker destruction locks correct isolate") {
//   Isolate* a = Isolate::New();
//   Isolate* b = Isolate::New();
//   {
//     Locker l(b);
//     {
//       a->Enter();
//       Unlocker u(b);
//       V8MONKEY_CHECK(Isolate::GetCurrent() == a, "Sanity check");
//     }
//     V8MONKEY_CHECK(!Locker::IsLocked(a), "Correct isolate unlocked");
//     V8MONKEY_CHECK(Locker::IsLocked(b), "Correct isolate relocked");
//   }
// 
//   b->Dispose();
//   a->Exit();
//   a->Dispose();
// }
// 
// 
// V8MONKEY_TEST(Locker020, "Unlocker unlocks default isolate if no isolate specified") {
//   Isolate* a = Isolate::New();
//   {
//     Locker aLocker(a);
//     Locker defaultLocker(Isolate::GetCurrent());
//     {
//       Unlocker u();
//       V8MONKEY_CHECK(Locker::IsLocked(a), "Didn't unlock wrong isolate");
//       V8MONKEY_CHECK(Locker::IsLocked(Isolate::GetCurrent()), "Unlocked correct isolate");
//     }
//   }
// 
//   a->Dispose();
// }
