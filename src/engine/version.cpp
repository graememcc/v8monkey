// V8 class definition
#include "v8.h"


// We could get the SpiderMonkey version from the JS_GetImplementationVersion call, however this would require some
// string manipulation. It's easier to simply bake it in at build time.

#define VERSION_STRING V8COMPAT " V8 API provided by V8Monkey (SpiderMonkey " SMVERSION ")"


/*
namespace {
  const char* versionString = VERSION_STRING;
}

namespace v8 {
  const char* V8::GetVersion() {
    return versionString;
  }
  // XXX Check compatability: does real V8 have a similar member?
  //const char* V8::version_string = VERSION_STRING;
}
*/
