#include "v8.h"

#include "isolate.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8;
using namespace v8::V8Monkey;


V8MONKEY_TEST(IntLocker001, "Locking default isolate implicitly initializes") {
  TestUtils::AutoTestCleanup ac;
  {
    Locker l;
    V8MONKEY_CHECK(V8Monkey::TestUtils::IsV8Initialized(), "V8Monkey was initialized");
  }
}


V8MONKEY_TEST(IntLocker002, "Locking default isolate implicitly enters it (1)") {
  TestUtils::AutoTestCleanup ac;
  {
    Locker l(Isolate::GetCurrent());
    V8MONKEY_CHECK(InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Locker construction enters default isolate");
  }
}


V8MONKEY_TEST(IntLocker003, "Locking default isolate implicitly enters it (2)") {
  TestUtils::AutoTestCleanup ac;
  {
    Locker l;
    V8MONKEY_CHECK(InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Locker construction enters default isolate");
  }
}


V8MONKEY_TEST(IntLocker004, "Locking non-default isolate does not enter it") {
  TestUtils::AutoTestCleanup ac;
  Isolate* i = Isolate::New();
  {
    Locker l(i);
    V8MONKEY_CHECK(!InternalIsolate::IsEntered(reinterpret_cast<InternalIsolate*>(i)), "Locker construction didn't enter isolate");
  }

  // We didn't enter the isolate, so need to manually delete it
  i->Dispose();
}


V8MONKEY_TEST(IntLocker005, "Locking default isolate exits it on destruction") {
  TestUtils::AutoTestCleanup ac;
  // We want to avoid the implicit V8 init in locker construction, so do it here, and exit the isolate
  V8::Initialize();
  Isolate::GetCurrent()->Exit();

  {
    Locker l;
  }
  V8MONKEY_CHECK(!InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Locker destruction exits default isolate");
}


V8MONKEY_TEST(IntLocker006, "Locker exits default on destruction even if it was already entered before construction") {
  TestUtils::AutoTestCleanup ac;
  // Initting V8 enters the default isolate
  V8::Initialize();
  V8MONKEY_CHECK(InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Sanity check");
  {
    Locker l;
  }
  V8MONKEY_CHECK(!InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Default isolate exited");
}


V8MONKEY_TEST(IntLocker007, "Previously entered default isolate not exited on locker destruction when locker didn't lock") {
  TestUtils::AutoTestCleanup ac;
  // Initting V8 enters the default isolate
  V8::Initialize();
  V8MONKEY_CHECK(InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Sanity check");
  {
    Locker l;
    {
      Locker m;
    }
    V8MONKEY_CHECK(InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Default isolate remains entered");
  }
}


V8MONKEY_TEST(IntLocker008, "Destruction of locker of non-default isolate doesn't exit it") {
  TestUtils::AutoTestCleanup ac;
  Isolate* i = Isolate::New();
  i->Enter();
  {
    Locker l(i);
  }
  V8MONKEY_CHECK(InternalIsolate::IsEntered(reinterpret_cast<InternalIsolate*>(i)), "Locker construction didn't enter isolate");
  V8::Dispose();
}


V8MONKEY_TEST(IntLocker009, "Unlocking default isolate implicitly exits it (1)") {
  TestUtils::AutoTestCleanup ac;
  // Perform V8 init separately: it implicitly enters, so we need to exit to prevent stacking (we'd have two entries, so would still
  // be entered after the unlock)
  V8::Initialize();
  Isolate::GetCurrent()->Exit();
  {
    Locker l(Isolate::GetCurrent());
    {
      Unlocker u(Isolate::GetCurrent());
      V8MONKEY_CHECK(!InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Unlocker construction exits default isolate");
    }
  }
}


V8MONKEY_TEST(IntLocker010, "Unlocking default isolate implicitly exits it (2)") {
  TestUtils::AutoTestCleanup ac;
  // Perform V8 init separately: it implicitly enters, so we need to exit to prevent stacking (we'd have two entries, so would still
  // be entered after the unlock)
  V8::Initialize();
  Isolate::GetCurrent()->Exit();
  {
    Locker l;
    Unlocker u;
    V8MONKEY_CHECK(!InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Unlocker construction exits default isolate");
  }
}


V8MONKEY_TEST(IntLocker011, "Unlocking non-default isolate does not exit it") {
  TestUtils::AutoTestCleanup ac;
  Isolate* i = Isolate::New();
  {
    Locker l(i);
    i->Enter();
    Unlocker u(i);
    V8MONKEY_CHECK(InternalIsolate::IsEntered(reinterpret_cast<InternalIsolate*>(i)), "Unlocker construction didn't exit isolate");
  }
}


V8MONKEY_TEST(IntLocker012, "Unlocker destruction for default isolate implicitly re-enters it") {
  TestUtils::AutoTestCleanup ac;
  // Perform V8 init separately: it implicitly enters, so we need to exit to prevent stacking (we'd have two entries, so would still
  // be entered after the unlock)
  V8::Initialize();
  Isolate::GetCurrent()->Exit();
  {
    Locker l(Isolate::GetCurrent());
    {
      Unlocker u(Isolate::GetCurrent());
    }
    V8MONKEY_CHECK(InternalIsolate::IsEntered(InternalIsolate::GetDefaultIsolate()), "Unlocker destruction re-enters default isolate");
  }
}


V8MONKEY_TEST(IntLocker013, "Unlocker destruction for non-default doesn't re-enter it") {
  TestUtils::AutoTestCleanup ac;
  Isolate* i = Isolate::New();
  {
    Locker l(i);
    {
      Unlocker u(i);
    }
    V8MONKEY_CHECK(!InternalIsolate::IsEntered(reinterpret_cast<InternalIsolate*>(i)), "Unlocker destruction doesn't enter isolate");
  }

  // Manually delete i, as we didn't enter it
  i->Dispose();
}
