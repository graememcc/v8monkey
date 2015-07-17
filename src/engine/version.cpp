// V8 class definition
#include "v8.h"


namespace {
  // We could get the SpiderMonkey version from the JS_GetImplementationVersion call, however this would require some
  // string manipulation. It's easier to simply bake it in at build time.
  static const char* versionString {V8COMPAT " V8 API provided by V8Monkey (SpiderMonkey " SMVERSION ")"};
}

namespace v8 {
  const char* V8::GetVersion() {
    return versionString;
  }
}
