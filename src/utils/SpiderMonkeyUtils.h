#ifndef V8MONKEY_SMUTILS_H
#define V8MONKEY_SMUTILS_H


// EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"


class JSRuntime;
class JSContext;
class JSCompartment;


namespace v8{
  namespace V8Monkey {
    class V8MonkeyCommon;


    /*
     * The most common pieces of information required by SpiderMonkey API callers are the JSRuntime and JSContext pointers.
     * Each thread should be assigned these the first time it enters an isolate. This POD structure is returned when
     * querying the values for the current thread.
     *
     */

    struct RTCXData {
      JSRuntime* rt;
      JSContext* cx;
    };


    class EXPORT_FOR_TESTING_ONLY SpiderMonkeyUtils {
      public:

        /*
         * Create a JSRuntime and JSContext for the given thread, and store those values in TLS.
         *
         * Triggers a fatal error (invoking the user-defined or default error handler) if creating either of those
         * objects fails.
         *
         * // XXX Write a test for the below (if possible)
         * Calling this function on a particular thread more than once has no effect: the JSRuntime and JSContext
         * will be assigned by the first call, and will not be changed by later calls.
         */

        static void AssignJSRuntimeAndJSContext();

        /*
         * Returns a RTCXData struct containing the JSRuntime and JSContext for the current thread. Either entry in
         * the struct may be null.
         *
         */

        static RTCXData GetJSRuntimeAndJSContext();

        /*
         * Return the assigned JSRuntime for this thread. May be null if one has not yet been assigned.
         *
         */

        static JSRuntime* GetJSRuntimeForThread();

        /*
         * Return the assigned JSContext for this thread. May be null if one has not yet been assigned.
         *
         */

        static JSContext* GetJSContextForThread();

        /*
         * Return the current JSCompartment for this thread. If the thread has not yet entered a compartment, then this
         * might return null.
         *
         */

        static JSCompartment* GetCurrentCompartment();

        /*
         * Return the JSCompartment that this thread will exit to when it leaves the current compartment. This value may
         * be null if the thread has only entered one compartment (i.e. has not entered a second compartment from the
         * first). Note, however that it is also possible for this to be non-null, and the current compartment to be
         * null: this situation will occur when entering a new isolate when already in an isolate.
         */

        static JSCompartment* GetPreviousCompartment();

        // XXX Tests:
        //  - GetJSRuntimeAndJSContext returns a null rtcxdata initially
        //  - GetCurrentCompartment returns a null ptr initally
        //  - GetPreviousCompartment returns null ptr initally
        // NOTES:
        //   We need to use a list of previous compartments, as we could be entering leaving more than one
        //   (Related: on exit, we will need to exit every compartment
        //

        private:

          /*
           * Define the TLS keys. Intended to only be called once, by the OneTrueStaticInitializer
           *
           */

          static void InitialiseSpiderMonkeyDataTLSKeys();


        friend class V8MonkeyCommon;
    };
  }
}


#endif
