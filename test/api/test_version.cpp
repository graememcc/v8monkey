// strlen strstr
#include <cstring>

// GetVersion
#include "v8.h"

// Unit-testing support
#include "V8MonkeyTest.h"


V8MONKEY_TEST(Vers001, "Version string contains more than just V8 version") {
  const char* version = v8::V8::GetVersion();
  V8MONKEY_CHECK(strlen(version) > strlen(V8COMPAT), "Length is acceptable");
}


V8MONKEY_TEST(Vers002, "Version string starts with V8 API version") {
  const char* version = v8::V8::GetVersion();
  V8MONKEY_CHECK(strstr(version, V8COMPAT) == version, "V8 API version is at correct position");
}


V8MONKEY_TEST(Vers003, "Version string mentions V8Monkey") {
  const char* version = v8::V8::GetVersion();
  V8MONKEY_CHECK(strstr(version, "V8Monkey") != 0, "V8Monkey version string makes explicit V8Monkey is running");
}


V8MONKEY_TEST(Vers004, "Version string mentions SpiderMonkey") {
  const char* version = v8::V8::GetVersion();
  V8MONKEY_CHECK(strstr(version, "SpiderMonkey") != 0,
                        "V8Monkey version string makes explicit second version number relates to SpiderMonkey");
}


V8MONKEY_TEST(Vers005, "Version string mentions SpiderMonkey version") {
  const char* version = v8::V8::GetVersion();
  V8MONKEY_CHECK(strstr(version, SMVERSION) != 0, "V8Monkey version string contains SpiderMonkey version");
}
