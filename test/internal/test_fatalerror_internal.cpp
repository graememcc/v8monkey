#include "v8.h"

#include "isolate.h"
#include "test.h"
#include "v8monkey_common.h"
#include "V8MonkeyTest.h"


namespace {
  // To verify that the fatal error handlers we supply are called, we will have them set these variables, and then
  // test for equality

  int callback1Called = 0;
  int callback2Called = 0;
  int callback3Called = 0;
  int callback4Called = 0;
  int callback5Called = 0;


  // Our first specimen is just a dummy; the first test checks for the effect on entering isolates
  void callback0(const char* location, const char* message) {}


  void callback1(const char* location, const char* message) {
    callback1Called = 1;
  }


  void callback2(const char* location, const char* message) {
    callback2Called = 2;
  }


  void callback3(const char* location, const char* message) {
    callback3Called = 3;
  }


  void callback4(const char* location, const char* message) {
    callback4Called = 4;
  }


  void callback5(const char* location, const char* message) {
    callback5Called = 5;
  }


  v8::V8Monkey::InternalIsolate* AsInternal(v8::Isolate* i) {
    return reinterpret_cast<v8::V8Monkey::InternalIsolate*>(i);
  }
}


using namespace v8;


V8MONKEY_TEST(FatalHandler001, "Calling SetFatalErrorHandler implicitly enters default isolate") {
  V8::SetFatalErrorHandler(callback0);
  V8MONKEY_CHECK(V8Monkey::InternalIsolate::IsEntered(AsInternal(Isolate::GetCurrent())), "Correctly entered default isolate");
  Isolate::GetCurrent()->Exit();
}


V8MONKEY_TEST(FatalHandler002, "Calling SetFatalErrorHandler doesn't change isolate if already entered") {
  Isolate* i = Isolate::New();
  i->Enter();
  V8::SetFatalErrorHandler(callback0);
  V8MONKEY_CHECK(V8Monkey::InternalIsolate::IsEntered(AsInternal(i)), "Correctly stayed in isolate");
  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(FatalHandler003, "Specified fatal error handler called") {
  V8::SetFatalErrorHandler(callback1);
  V8Monkey::V8MonkeyCommon::TriggerFatalError(NULL, NULL);
  V8MONKEY_CHECK(callback1Called == 1, "Handler called");
}


V8MONKEY_TEST(FatalHandler004, "Specified fatal error handler is isolate specific (1)") {
  Isolate* i = Isolate::New();
  i->Enter();
  V8::SetFatalErrorHandler(callback2);

  Isolate* j = Isolate::New();
  j->Enter();
  V8::SetFatalErrorHandler(callback3);

  V8Monkey::V8MonkeyCommon::TriggerFatalError(NULL, NULL);
  V8MONKEY_CHECK(callback2Called == 0, "Old handler not called");
  V8MONKEY_CHECK(callback3Called == 3, "Handler called");

  j->Exit();
  j->Dispose();

  i->Exit();
  i->Dispose();
}


V8MONKEY_TEST(FatalHandler005, "Specified fatal error handler is isolate specific (2)") {
  Isolate* i = Isolate::New();
  i->Enter();
  V8::SetFatalErrorHandler(callback4);

  Isolate* j = Isolate::New();
  j->Enter();
  V8::SetFatalErrorHandler(callback5);
  j->Exit();

  V8Monkey::V8MonkeyCommon::TriggerFatalError(NULL, NULL);
  V8MONKEY_CHECK(callback5Called == 0, "Old handler not called");
  V8MONKEY_CHECK(callback4Called == 4, "Handler called");

  j->Dispose();

  i->Exit();
  i->Dispose();
}
