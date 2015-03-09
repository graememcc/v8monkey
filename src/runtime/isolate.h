#ifndef V8MONKEY_ISOLATE_H
#define V8MONKEY_ISOLATE_H

// int64_t
#include <cinttypes>

// ObjectBlock
#include "types/objectblock.h"

// shared_ptr
#include "memory"

// Mutex
#include "platform/platform.h"

// Object
#include "types/base_types.h"

// EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"

// V8MONKEY_ASSERT
#include "utils/V8MonkeyCommon.h"

// FatalErrorCallback, Isolate, kNumIsolateDataSlots
#include "v8.h"

// vector
#include <vector>


class JSTracer;


namespace v8 {
  namespace internal {

    /*
     * Isolates and HandleScopes cooperate in the management of local handles. This struct encapsulates the
     * information required for HandleScope construction / destruction.
     *
     * Note: the limit field is currently unused, but is required for V8 API compatability.
     *
     * TODO: I'm not in love with the name.
     *
     */

    struct LocalHandleLimits {
      Object** next;
      Object** limit;
    };


//    /*
//     * An internal variant of the public facing Isolate class. A central repository for a large amount of book-keeping
//     * data. See the comments in isolate.cpp for further information on how this relates to SpiderMonkey structures.
//     *
//     */

    class EXPORT_FOR_TESTING_ONLY Isolate {
      using LocalHandles = Object::ObjectContainer;

      public:
        // XXX Put an initializer list here once we have the shape of InternalIsolate nailed down
        Isolate() : embedderData {nullptr}, hasFatalError {false}, previousIsolates {}, localHandleData {},
                    localHandleLimits {nullptr, nullptr}, isDisposed {false}, isInitted {false},
                    isRegisteredForGC {false}, fatalErrorHandler {nullptr}, lockingThread {0}, lockingMutex{},
                    GCMutex{} {}

//        InternalIsolate() : isDisposed(false), isRegisteredForGC(false), fatalErrorHandler(nullptr), threadData(nullptr),
//                            embedderData(nullptr), lockingThread(0), isInitted(false) {
//          handleScopeData.Initialize();
//          persistentData.Initialize();
//        }

        ~Isolate();

        // XXX temp for version bump
        // XXX Make constructor private and friend public Isolate
        Isolate(const Isolate& other) = default;
        Isolate(Isolate&& other) = default;
        Isolate& operator=(const Isolate& other) = default;
        Isolate& operator=(Isolate&& other) = default;

        /*
         * Enter the given isolate.
         *
         */

        void Enter();

        /*
         * Exit the given isolate.
         *
         */

        void Exit();

        /*
         * Dispose any remaning resources held by this Isolate, and delete the isolate.
         *
         */

        void Dispose(bool fromDestructor = false);

        /*
         * Returns a pointer to the currently entered isolate for the calling thread. May be null.
         *
         */

        static Isolate* GetCurrent();

        /*
         * Reports whether any threads have entered this isolate and not yet exited.
         *
         */

        bool ContainsThreads() const { return !previousIsolates.empty(); }
        //bool ContainsThreads() const { return threadData != nullptr; }

//        // Many API functions will implicitly enter the default isolate if required. To that end, this function returns
//        // the current internal isolate if non-null, otherwise enters the default isolate and returns that
//        // XXX Look for opportunities to use this
//        static InternalIsolate* EnsureInIsolate();

        /*
         * Reports whether the calling thread has entered the given isolate.
         *
         */

        static bool IsEntered(Isolate* i);

        // XXX Clarify the death conditions
        /*
         * Reports whether this isolate is dead (through a fatal error or other condition).
         *
         */

        bool IsDead() const { return hasFatalError; }

        /*
         * Allow various parts of V8Monkey to signal a problem has occurred in this isolate.
         *
         */

        void SignalFatalError() { hasFatalError = true; }

        // XXX I know SetFatalError... is V8 API but who uses Get? Can this be private with the
        // relevant friends
        /*
         * V8 API: Set the client callback that will be invoked when problems occur in this isolate.
         *
         */

        void SetFatalErrorHandler(FatalErrorCallback fn) { fatalErrorHandler = fn; }

        /*
         * Get a function pointer to the client callback for fatal errors in this isolate. Will be null if the
         * client has not supplied a callback via the relevant API function.
         *
         */

        FatalErrorCallback GetFatalErrorHandler() const { return fatalErrorHandler; }

        // XXX Might want to make Lock/Unlock private, and friend Locker/Unlocker
        /*
         * API for client API Locker objects. Signals that the calling thread has locked this isolate. Per API
         * requirements, other threads are not permitted to enter and/or manipulate this isolate without holding
         * this lock.
         *
         */

        void Lock();

        /*
         * API for client API Locker objects. Signals that the calling thread has finished with this isolate, and
         * other threads are now free to acquire the lock and enter or manipulate it.
        // XXX Should the caller have exited the isolate? Assert if so.
         *
         */

        void Unlock();

        /*
         * Returns true if a Locker has locked this isolate on behalf of some thread. Largely for future use.
        // XXX Does anybody use this?
         *
         */

        bool IsLocked() const {
          // This assumes that a thread's ID can never be zero. (Note: storing thread IDs in TLS would be broken were
          // this not the case).
          return lockingThread != 0;
        }


        /*
         * Reports whether the calling thread has locked this isolate.
         *
         */

        bool IsLockedForThisThread() const;

        // XXX
        //   We might want to make GetLocalHandleLimits and co private, and make HandleScope a friend class
        //   This would include LockGCMutex etc. Note that it is the HS that must lock: it must record the
        //   info before it has a chance to change

        // XXX The bit about no need for mutex will only be true if this is private and HandleScope is a friend
        /*
         * Return a copy of the local handle limits for this isolate. There is no need to hold the GCMutex during this
         * call: a GC iteration will not change the values, and the only caller is expected to the HandleScope
         * XXX...(FIX THIS SENTENCE: CONSTRUCTOR OR INITIALIZER?), and the V8 API requires correct locking in place
         * HandleScope construction.
         *
         */

        LocalHandleLimits GetLocalHandleLimits() const {
          return localHandleLimits;
        }

        /*
         * Returns the number of local handles managed by this isolate.
         *
         */

        unsigned long LocalHandleCount() const {
          return localHandleData.NumberOfItems();
        }

        /*
         * Adds the given object to the set of local handles managed by this isolate. Returns a Object** pointer representing
         * the slot where the handle was stored.
         *
         */

        Object** AddLocalHandle(Object* obj) {
          V8MONKEY_ASSERT(obj, "Attempting to add nullptr?");

          AutoGCMutex {this};
          auto result = localHandleData.Add(obj);
          localHandleLimits.next = result.next;
          localHandleLimits.limit = result.limit;
          return result.objectAddress;
        }


        /*
         * Delete all local handles from the given slot onwards. The supplied slot is assumed to have been a previously returned
         * LocalHandleLimit
         *
         */

        void DeleteLocalHandleSlots(Object** slot) {
          auto result = localHandleData.Delete(slot);
          localHandleLimits.next = result.next;
          localHandleLimits.limit = result.limit;
        }

        /*
         * Reports whether the isolate has been initialized. Required for V8 compatability: some API calls should assert
         * when called if the the isolate has not been initialized
         *
         */

        bool IsInitted() const { return isInitted; }


        /*
         * Initialize the isolate in a manner expected by the V8 header.
         *
         */

        void Init() {
          if (isInitted) {
            return;
          }

          doInit();
          isInitted = true;
        }

//        // XXX Check need to hold GCMutex in dispose/destructor
//        // Copy the given HandleData into our own. The caller must hold the GC Mutex.
//        // XXX Check need to hold it in dispose/destructor
//        void SetLocalHandleData(HandleData& hd);

//        // Return a copy of the persistent data for this isolate. If manipulating this data, the caller must hold the
//        // GC Mutex.
//        // XXX Check need to hold it in dispose/destructor
//        HandleData GetPersistentHandleData();

//        // Copy the given HandleData into our persistent data. The caller must hold the GC Mutex.
//        // XXX Check need to hold it in dispose/destructor
//        void SetPersistentHandleData(HandleData& hd);

        /*
         * Lock the GC Mutex to allow the caller to safely mutate or iterate over the handles belonging to this Isolate.
         *
         */

        void LockGCMutex() {
          GCMutex.Lock();
        }


        /*
         * Signal that the caller has finished iterating or mutating the handles stored herein.
         *
         */

        void UnlockGCMutex() {
          GCMutex.Unlock();
        }

        /*
         * RAII helper for managing the GC Mutex during handle iteration / mutation.
         *
         */

        class AutoGCMutex {
          public:
            AutoGCMutex(Isolate* i) : isolate(i) {
              isolate->LockGCMutex();
            }

            ~AutoGCMutex() {
              isolate->UnlockGCMutex();
            }

            AutoGCMutex(const AutoGCMutex& other) = delete;
            AutoGCMutex(AutoGCMutex&& other) = delete;
            AutoGCMutex& operator=(const AutoGCMutex& other) = delete;
            AutoGCMutex& operator=(AutoGCMutex&& other) = delete;

         private:
            Isolate* isolate;
        };

        /*
         * Trace all SpiderMonkey objects contained in handles in this Isolate that were created in the given
         * JSRuntime.
         *
         * Note: this function must be public, as the callback supplied to SpiderMonkey when participating in rooting is
         * in an anonymous namespace.
         *
         */

        void Trace(JSRuntime* rt, JSTracer* tracer);

//        // Provided for V8 compat
//        bool IsInitted() const { return isInitted; }
//        void Init() { isInitted = true; }

        /*
         * API Isolates are all air: this function casts to a pointer to the real underlying internal::Isolate.
         *
         */

        static Isolate* FromAPIIsolate(::v8::Isolate* i) {
          return reinterpret_cast<Isolate*>(i);
        }

//
//        // Initialize the default isolate and claim thread id 1
//        static void EnsureDefaultIsolateForStaticInitializerThread();

      private:
        /*
         *                 ** V8 Binary compatability **
         *
         * The values below are positioned in order to maintain binary compatability with the V8 header. Ideally, I
         * would static assert that they are in the correct position, however, we're not a POD type, so we cannot
         * safely use offsetof.
         *
         */

        void* embedderData[Internals::kNumIsolateDataSlots] {nullptr};

        // Padding for layout compatability
        int64_t dummyValues[2] {0, 0};
        void* dummyPointer {nullptr};

        // Array of slots for booleans, undefined and null. Must be at this position for V8 compatability
        Object* primitiveValues[10] {nullptr};

        /*
         *                ** End of V8 Binary compatability **
         *
         * Members from this point on have no restriction on their layout
         *
         */

        // Denotes whether this isolate is effectively dead
        bool hasFatalError {false};

        // XXX Temporary?
        // Record the previously entered isolates of all the threads that have entered this isolate.
        std::vector<Isolate*> previousIsolates {};

        // XXX Temporary?
        // Pointers to objects contained in API Local handles
        LocalHandles localHandleData {};

        // Data for HandleScopes to destroy all managed Local handles on destruction
        LocalHandleLimits localHandleLimits {nullptr, nullptr};

//        struct ThreadData;

        // Has this isolate been disposed?
        bool isDisposed {false};

        // Has this isolate been initialized?
        bool isInitted {false};

        // Have we told SpiderMonkey we're a rooter?
        bool isRegisteredForGC {false};

        // Fatal error handler for this isolate
        FatalErrorCallback fatalErrorHandler {nullptr};

//        // Our linked list of data about active threads in this isolate
//        ThreadData* threadData;

//        // Embedder data
//        void* embedderData;

        // ID of the thread that has "locked" this isolate. For future use.
        int lockingThread {0};

        // Thread entry mutex. For future use.
        V8Platform::Mutex lockingMutex {};

        // GC Mutex - should be held during handle manipulation / iteration
        V8Platform::Mutex GCMutex {};

//        // In a single threaded application, there is no need to explicitly construct an isolate. V8 constructs a
//        // "default" isolate automatically, and use it where necessary. Note that this changes in V8 3.29.79
//        static InternalIsolate* defaultIsolate;

//        // HandleScope data for Locals
//        HandleData handleScopeData;

//        // HandleScope data for Persistents
//        HandleData persistentData;

        // XXX Temporary?
        /*
         * This pair of functions should be called when a thread enters/exits the Isolate. They maintain the invariant
         * previousIsolates.back() is the isolate that the most-recently entered thread came from. It seems to be an
         * (undocumented) V8 API requirement that threads enter and exit isolates in a LIFO fashion.
         * XXX Check that assertion.
         *
         */

        void RecordThreadEntry(Isolate* i);
        Isolate* RecordThreadExit();

        /*
         * Perform one-time initialization tasks, such as installing true, false, etc. in their expected locations
         *
         */

        void doInit();

//        // ThreadData linked list manipulations
//        ThreadData* FindOrCreateThreadData(int threadID, InternalIsolate* previousIsolate);
//        ThreadData* FindThreadData(int threadID);
//        void DeleteAndFreeThreadData(ThreadData* data);
    };
  }
}


#endif
