// Isolate V8
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8;


V8MONKEY_TEST(Death001, "V8::IsDead normally returns false (no init)") {
  Isolate* i {Isolate::New()};
  i->Enter();
  V8MONKEY_CHECK(!V8::IsDead(), "V8 is still alive");
  i->Exit();
}


// XXX
//V8MONKEY_TEST(Death002, "V8::IsDead normally returns false (with init)") {
  //V8::Initialize();
  //V8MONKEY_CHECK(!V8::IsDead(), "I ain't dead yet!");
  //XXX V8::Dispose();
//}
