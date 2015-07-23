// The class under test
#include "utils/SpiderMonkeyUtils.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8::SpiderMonkey;


V8MONKEY_TEST(IntSMUtils001, "JSRuntime initially null") {
  JSRuntime* rt {GetJSRuntimeForThread()};
  V8MONKEY_CHECK(!rt, "Runtime initially null");
}


V8MONKEY_TEST(IntSMUtils002, "JSContext initially null") {
  JSContext* cx {GetJSContextForThread()};
  V8MONKEY_CHECK(!cx, "Context initially null");
}


V8MONKEY_TEST(IntSMUtils003, "JSRuntime non-null after assignment") {
  JSRuntime* rt {GetJSRuntimeForThread()};
  V8MONKEY_CHECK(!rt, "Sanity check");
  EnsureRuntimeAndContext();
  rt = GetJSRuntimeForThread();
  V8MONKEY_CHECK(rt, "Runtime not null");
}


V8MONKEY_TEST(IntSMUtils004, "JSContext non-null after assignment") {
  JSContext* cx {GetJSContextForThread()};
  V8MONKEY_CHECK(!cx, "Sanity check");
  EnsureRuntimeAndContext();
  cx = GetJSContextForThread();
  V8MONKEY_CHECK(cx, "Context not null");
}
