#ifndef V8MONKEY_SMUTILS_H
#define V8MONKEY_SMUTILS_H

// JSTraceDataOp
#include "jsapi.h"

// EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"


namespace v8 {

  namespace internal {
    class Isolate;
  }


  namespace SpiderMonkey {
    using RooterCallback = JSTraceDataOp;

    /*
     * In this namespace, we wrap some common JSAPI calls, particularly where there is a need to perform additional
     * internal book-keeping in relation to the particular operation.
     *
     */

    struct SpiderMonkeyData {
      JSRuntime* rt;
      JSContext* cx;
    };


    /*
     * Checks if SpiderMonkey is initialized, and if not, performs that initialization in a thread-safe fashion
     *
     */

    void EnsureSpiderMonkey();


    /*
     * Shuts down SpiderMonkey if safe to do so, otherwise aborts with an error
     *
     */

    void TearDownSpiderMonkey();


    /*
     * Create a JSRuntime and JSContext for the given thread, and store those values in TLS.
     *
     * Aborts if creating either of those objects fails, as most of our assumptions will be hopelessly broken.
     *
     * // XXX Write a test for the below (if possible)
     * Calling this function on a particular thread more than once has no effect: the JSRuntime and JSContext
     * will be assigned by the first call, and will not be changed by later calls.
     *
     */

    EXPORT_FOR_TESTING_ONLY void EnsureRuntimeAndContext();


    /*
     * Returns a SpiderMonkeyData struct containing the JSRuntime and JSContext for the current thread. Either entry in
     * the struct may be null.
     *
     */

    SpiderMonkeyData GetJSRuntimeAndJSContext();


    /*
     * Return the assigned JSRuntime for this thread. May be null if one has not yet been assigned.
     *
     */

    EXPORT_FOR_TESTING_ONLY JSRuntime* GetJSRuntimeForThread();


    /*
     * Return the assigned JSContext for this thread. May be null if one has not yet been assigned.
     *
     */

    EXPORT_FOR_TESTING_ONLY JSContext* GetJSContextForThread();


    /*
     * Add the given callback function as a GC roots tracer, for the given isolate. The function will be called with
     * the supplied data during SpiderMonkey garbage collection. (Note in particular, that if the function needs to
     * refer to the isolate, then the calling isolate must include a self-pointer in the supplied data; this function
     * will not add it).
     *
     * It is assumed that the calling thread has been allocated a JSRuntime, and it is this JSRuntime that the isolate
     * should be added as a rooter of.
     *
     * This function is a no-op if the isolate has already been added for the given runtime.
     *
     */

    void AddIsolateRooter(::v8::internal::Isolate* isolate, RooterCallback callback, void* data);


    /*
     * Remove the given isolate as a rooter of any JSRuntimes it has previously registered to. It is intended that this
     * function should only be called on client-initiated teardown, or isolate destruction.
     *
     * This is a no-op if the given isolate was not associated with any JSRuntimes.
     *
     */

    void RemoveRooter(::v8::internal::Isolate*);


    #ifdef V8MONKEY_INTERNAL_TEST
    EXPORT_FOR_TESTING_ONLY void ForceGC();
    #endif
  }


//   namespace V8Monkey {
//    class V8MonkeyCommon;
//
//
//    /*
//     * The most common pieces of information required by SpiderMonkey API callers are the JSRuntime and JSContext pointers.
//     * Each thread should be assigned these the first time it enters an isolate. This POD structure is returned when
//     * querying the values for the current thread.
//     *
//     */
//
//
//    class EXPORT_FOR_TESTING_ONLY SpiderMonkeyUtils {
//      public:
//
//        /*
//         * Return the current JSCompartment for this thread. If the thread has not yet entered a compartment, then this
//         * might return null.
//         *
//         */
//
//        static JSCompartment* GetCurrentCompartment();
//
//        /*
//         * Return the JSCompartment that this thread will exit to when it leaves the current compartment. This value may
//         * be null if the thread has only entered one compartment (i.e. has not entered a second compartment from the
//         * first). Note, however that it is also possible for this to be non-null, and the current compartment to be
//         * null: this situation will occur when entering a new isolate when already in an isolate.
//         */
//
//        static JSCompartment* GetPreviousCompartment();
//
//        // XXX Tests:
//        //  - GetJSRuntimeAndJSContext returns a null rtcxdata initially
//        //  - GetCurrentCompartment returns a null ptr initally
//        //  - GetPreviousCompartment returns null ptr initally
//        // NOTES:
//        //   We need to use a list of previous compartments, as we could be entering leaving more than one
//        //   (Related: on exit, we will need to exit every compartment
//        //
//
//    };
//  }
}


#endif
