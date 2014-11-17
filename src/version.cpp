#include "../include/v8.h"


#define VERSION_STRING "V8Monkey (SpiderMonkey " SMVERSION ") V8 API: " V8COMPAT


namespace v8 {
  const char* V8::version_string = VERSION_STRING;
}
