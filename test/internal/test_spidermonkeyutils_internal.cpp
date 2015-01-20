// The class under test
#include "utils/SpiderMonkeyUtils.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8::V8Monkey;


V8MONKEY_TEST(IntSMUtils001, "Current compartment initially null") {
  V8MONKEY_CHECK(!SpiderMonkeyUtils::GetCurrentCompartment(), "Current compartment initially null");
}


V8MONKEY_TEST(IntSMUtils002, "Previous compartment initially null") {
  V8MONKEY_CHECK(!SpiderMonkeyUtils::GetPreviousCompartment(), "Previous compartment initially null");
}


V8MONKEY_TEST(IntSMUtils003, "JSRuntime initially null") {
  RTCXData rtcxData = SpiderMonkeyUtils::GetJSRuntimeAndJSContext();
  V8MONKEY_CHECK(!rtcxData.rt, "Runtime initially null");
}


V8MONKEY_TEST(IntSMUtils004, "JSContext initially null") {
  RTCXData rtcxData = SpiderMonkeyUtils::GetJSRuntimeAndJSContext();
  V8MONKEY_CHECK(!rtcxData.cx, "Context initially null");
}
