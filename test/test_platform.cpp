#include <pthread.h>

#include "V8MonkeyTest.h"
#include "platform.h"


using namespace v8::V8Monkey;


// XXX This should probably be checked at configuration time, rather than getting this far
// XXX And if not, we at least want to factor out the platform specific bit
V8MONKEY_TEST(Plat001, "Platform-specific TLS key fits in a pointer") {
  V8MONKEY_CHECK(Platform::GetTLSKeySize() <= sizeof(TLSKey*), "Opaque type fits in a pointer");
}


V8MONKEY_TEST(Plat002, "TLS Key get initially returns null") {
  TLSKey* key = Platform::CreateTLSKey();
  V8MONKEY_CHECK(Platform::GetTLSData(key) == NULL, "Key value is initially null");
  Platform::DeleteTLSKey(key);
}


V8MONKEY_TEST(Plat003, "TLS Key get returns correct value") {
  TLSKey* key = Platform::CreateTLSKey();
  intptr_t value = 42;
  Platform::StoreTLSData(key, reinterpret_cast<void*>(value));
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(Platform::GetTLSData(key)) == value, "Retrieved key is correct");
  Platform::DeleteTLSKey(key);
}


// Support function for thread result test
#define THREAD_RESULT 100
void*
thread_join_main(void* arg)
{
  return reinterpret_cast<void*>(THREAD_RESULT);
}


V8MONKEY_TEST(Plat004, "Thread joining returns correct value") {
  Thread* t = Platform::CreateThread(thread_join_main);
  t->Run(NULL);
  void* result = t->Join();
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(result) == THREAD_RESULT, "Value returned by thread join is correct");
}


// Support function for thread result test
#define THREAD_ARG 7
void*
thread_arg_main(void* arg)
{
  return arg;
}


V8MONKEY_TEST(Plat005, "Thread argument passing works correctly") {
  Thread* t = Platform::CreateThread(thread_arg_main);
  t->Run(reinterpret_cast<void*>(THREAD_ARG));
  void* result = t->Join();
  V8MONKEY_CHECK(reinterpret_cast<intptr_t>(result) == THREAD_ARG, "Value returned shows arg was passed correctly");
}


// Support function for thread noparam test
void*
thread_noarg_main(void* arg)
{
  return arg;
}


V8MONKEY_TEST(Plat006, "Run called with no arguments calls thread function with NULL") {
  Thread* t = Platform::CreateThread(thread_noarg_main);
  t->Run();
  void* result = t->Join();
  V8MONKEY_CHECK(result == NULL, "Run was called with null pointer");
}
