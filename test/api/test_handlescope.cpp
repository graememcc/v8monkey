#include "v8.h"

#include "V8MonkeyTest.h"


using namespace v8;

namespace {
  int errorCaught = 0;
  void fatalErrorHandler(const char* location, const char* message) {
    errorCaught = 1;
  }
}


V8MONKEY_TEST(HandleScope001, "HandleScope construction fails if locker ever constructed and isolate not locked") {
  Isolate* i = Isolate::New();
  i->Enter();

  {
    Locker l(i);
  }

  errorCaught = 0;
  V8::SetFatalErrorHandler(fatalErrorHandler);
  {
    HandleScope h;
  }

  V8MONKEY_CHECK(V8::IsDead() && errorCaught != 0, "Fatal error if locker constructed and lock not held");
  i->Exit();
  i->Dispose();
}
