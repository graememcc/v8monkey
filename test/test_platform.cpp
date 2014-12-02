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
