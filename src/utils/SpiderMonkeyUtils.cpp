// TLSKey, CreateTLSKey, GetTLSData, StoreTLSData
// XXX Remove me if we don't need this for compartments
#include "platform.h"

// SpiderMonkeyUtils definition
#include "utils/SpiderMonkeyUtils.h"


// XXX Remove me if we don't need this for compartments
using namespace v8::V8Platform;


namespace v8{
  namespace V8Monkey {
    JSRuntime* SpiderMonkeyUtils::GetJSRuntimeForThread() {
      RTCXData rtcx = GetJSRuntimeAndJSContext();
      return rtcx.rt;
    }


    JSContext* SpiderMonkeyUtils::GetJSContextForThread() {
      RTCXData rtcx = GetJSRuntimeAndJSContext();
      return rtcx.cx;
    }


    JSCompartment* SpiderMonkeyUtils::GetCurrentCompartment() {
      // XXX TODO
      return nullptr;
    }


    JSCompartment* SpiderMonkeyUtils::GetPreviousCompartment() {
      // XXX TODO
      return nullptr;
    }
  }
}
