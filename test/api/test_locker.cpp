// Thread
#include "platform.h"

// Class definition, Isolate::New
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;


namespace {
  void* CheckIsLockedStatus(void* iso) {
    Isolate* i = reinterpret_cast<Isolate*>(iso);
    return reinterpret_cast<void*>(Locker::IsLocked(i));
  }
}


V8MONKEY_TEST(Locker001, "IsActive is false if no lockers ever constructed") {
   V8MONKEY_CHECK(!Locker::IsActive(), "IsActive reported false");
}


V8MONKEY_TEST(Locker002, "IsActive reports true when locker constructed") {
  Isolate* i {Isolate::New()};

  {
    Locker l {i};
  }

  V8MONKEY_CHECK(Locker::IsActive(), "IsActive reported true");
  i->Dispose();
}


V8MONKEY_TEST(Locker003, "IsActive remains true after locker destruction") {
  Isolate* i {Isolate::New()};

  {
    Locker l {i};
  }

  V8MONKEY_CHECK(Locker::IsActive(), "IsActive reported true");

  i->Dispose();
}


V8MONKEY_TEST(Locker004, "IsLocked reports false for isolate when we haven't locked it") {
  Isolate* i {Isolate::New()};
  V8MONKEY_CHECK(!Locker::IsLocked(i), "Isolate not locked");
  i->Dispose();
}


V8MONKEY_TEST(Locker005, "IsLocked reports true for isolate when locked") {
  Isolate* i {Isolate::New()};

  {
    Locker l {i};
    V8MONKEY_CHECK(Locker::IsLocked(i), "Isolate locked");
  }

  i->Dispose();
}


V8MONKEY_TEST(Locker006, "IsLocked reports false for isolate when locker destroyed") {
  Isolate* i {Isolate::New()};

  {
    Locker l {i};
  }

  V8MONKEY_CHECK(!Locker::IsLocked(i), "Isolate not locked");

  i->Dispose();
}


V8MONKEY_TEST(Locker007, "IsLocked reports true if multiple locks created for same isolate, and only some destroyed") {
  Isolate* i {Isolate::New()};

  {
    Locker l1 {i};

    {
      Locker l2 {i};

      {
        Locker l3 {i};
      }

      V8MONKEY_CHECK(Locker::IsLocked(i), "Isolate still locked with mutiple lockers");
    }

    V8MONKEY_CHECK(Locker::IsLocked(i), "Isolate still locked with mutiple lockers");
  }
}


V8MONKEY_TEST(Locker008, "IsLocked reports false if multiple locks created for same isolate, and all destroyed") {
  Isolate* i {Isolate::New()};

  {
    Locker l1 {i};

    {
      Locker l2 {i};

      {
        Locker l3 {i};
      }
    }
  }

  V8MONKEY_CHECK(!Locker::IsLocked(i), "Isolate not locked after mutiple lockers");
}


V8MONKEY_TEST(Locker009, "IsLocked reports false if unlocker constructed for single lock") {
  Isolate* i {Isolate::New()};

  Locker l {i};
  Unlocker u {i};
  V8MONKEY_CHECK(!Locker::IsLocked(i), "Value correct after unlocker construction");
}


V8MONKEY_TEST(Locker010, "IsLocked reports false if unlocker constructed for multiple locks") {
  Isolate* i {Isolate::New()};

  Locker l {i};
  {
    Locker m {i};

    {
      Locker n {i};
      Unlocker u {i};
      V8MONKEY_CHECK(!Locker::IsLocked(i), "Value correct after unlocker construction");
    }
  }
}


V8MONKEY_TEST(Locker011, "IsLocked reports true after unlocker constructed deep in multiple locks") {
  Isolate* i {Isolate::New()};

  {

    Locker l {i};

    {
      Locker m {i};

      {

        Locker n {i};

        {
          Unlocker u {i};
        }

        V8MONKEY_CHECK(Locker::IsLocked(i), "Value correct after unlocker destroyed");
      }

      V8MONKEY_CHECK(Locker::IsLocked(i), "Value correct after lock 3 destroyed");
    }

    V8MONKEY_CHECK(Locker::IsLocked(i), "Value correct after lock 2 destroyed");
  }

  V8MONKEY_CHECK(!Locker::IsLocked(i), "Value correct after lock 1 destroyed");
}


V8MONKEY_TEST(Locker012, "IsLocked reports false when queried from different thread") {
  // The game here is that we lock the default isolate, and pass that as an argument
  // to the thread which then queries it

  Isolate* i {Isolate::New()};
  Locker l {i};
  V8Platform::Thread t {CheckIsLockedStatus};
  t.Run(i);
  V8MONKEY_CHECK(!t.Join(), "Thread found isolate was not locked");
}


V8MONKEY_TEST(Locker013, "Locker destruction unlocks correct isolate") {
  Isolate* a {Isolate::New()};
  Isolate* b {Isolate::New()};

  {
    Locker l {b};
    a->Enter();
    V8MONKEY_CHECK(Isolate::GetCurrent() == a, "Sanity check");
  }

  V8MONKEY_CHECK(!Locker::IsLocked(b), "Correct isolate unlocked");

  b->Dispose();
  a->Exit();
  a->Dispose();
}


V8MONKEY_TEST(Locker014, "Unlocker destruction locks correct isolate") {
  Isolate* a {Isolate::New()};
  Isolate* b {Isolate::New()};

  {
    Locker l {b};

    {
      a->Enter();
      Unlocker u {b};
      V8MONKEY_CHECK(Isolate::GetCurrent() == a, "Sanity check");
    }

    V8MONKEY_CHECK(!Locker::IsLocked(a), "Correct isolate unlocked");
    V8MONKEY_CHECK(Locker::IsLocked(b), "Correct isolate relocked");
  }

  b->Dispose();
  a->Exit();
  a->Dispose();
}


V8MONKEY_TEST(Locker015, "Locking doesn't affect isolate entry status (1)") {
  Isolate* i {Isolate::New()};

  {
    Locker l {i};
    V8MONKEY_CHECK(Isolate::GetCurrent() != i, "Locking didn't enter isolate");
  }

  i->Dispose();
}


V8MONKEY_TEST(Locker016, "Locking doesn't affect isolate entry status (2)") {
  // Tests that locking doesn't add new entries
  Isolate* original {Isolate::GetCurrent()};
  Isolate* i {Isolate::New()};
  i->Enter();

  {
    Locker l {i};
    i->Exit();
    V8MONKEY_CHECK(Isolate::GetCurrent() == original, "Locking didn't enter isolate");
  }

  i->Dispose();
}


V8MONKEY_TEST(Locker017, "Locking doesn't affect isolate entry status (3)") {
  Isolate* i {Isolate::New()};
  i->Enter();
  Isolate* j {Isolate::New()};

  {
    Locker l {j};
    V8MONKEY_CHECK(Isolate::GetCurrent() == i, "Locking didn't enter isolate");
  }

  j->Dispose();
  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(Locker018, "Unlocking doesn't affect isolate entry status") {
  Isolate* i {Isolate::New()};

  {
    Locker l {i};
    i->Enter();

    {
      Unlocker u {i};
      V8MONKEY_CHECK(Isolate::GetCurrent() == i, "Unlocking didn't exit isolate");
    }
  }

  i->Dispose();
}
