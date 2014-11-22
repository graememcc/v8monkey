#include "v8.h"


// We could get the SpiderMonkey version from the JS_GetImplementationVersion call, however this would require some
// string manipulation. It's easier to simply bake it in at build time.

#define VERSION_STRING V8COMPAT " V8 API provided by V8Monkey (SpiderMonkey " SMVERSION ")"


namespace v8 {
  const char* V8::version_string = VERSION_STRING;
}
