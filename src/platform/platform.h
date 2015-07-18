#ifndef V8MONKEY_PLATFORM_H
#define V8MONKEY_PLATFORM_H

// memcpy
#include <cstring>

// EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"

// V8MONKEY_ASSERT
#include "utils/V8MonkeyCommon.h"


/*
 * This file abstracts away platform specific details, though at time of writing the only implementation is for Linux
 *
 */


namespace v8 {
  namespace V8Platform {
    // Platform-agnostic one-shot functions (e.g. pthread_once on POSIX, InitOnceExecuteOnce on Windows)
    using OneTimeFunction = void (*)();

    // Platform-agnostic thread execution functions
    using ThreadFunction = void* (*)(void*);


    /*
     * Platform-agnostic wrapper around the platform's thread-local storage mechanism
     *
     */

    class EXPORT_FOR_TESTING_ONLY PlatformTLSKey {
      protected:
        void* privateData {nullptr};

        // Attempt to set the value: returns false if the native implementation failed, true otherwise
        bool InternalSet(void* data);
        void* InternalGet();

        PlatformTLSKey(void (*destructorFn)(void*) = nullptr);

        ~PlatformTLSKey();

        PlatformTLSKey(PlatformTLSKey& other) = delete;
        PlatformTLSKey(PlatformTLSKey&& other) = default;
        PlatformTLSKey& operator=(PlatformTLSKey& other) = delete;
        PlatformTLSKey& operator=(PlatformTLSKey&& other) = default;
    };


    /*
     * Although unlikely, there are no guarantees that casting an arbitrary small type to void* and back won't lose
     * information. Thus we only admit keys that are of pointer type.
     *
     */

    template <typename T>
    class EXPORT_FOR_TESTING_ONLY TLSKey : private PlatformTLSKey {
      public:
        // Constructing a TLSKey can call std::terminate if key creation failed
        TLSKey(void (*destructorFn)(void*) = nullptr) : PlatformTLSKey(destructorFn) {}

        // Attempt to set the value: returns false if the native implementation failed, true otherwise
        bool Set(T* data) {
          void* asVoid {reinterpret_cast<void*>(data)};
          return InternalSet(asVoid);
        }

        T* Get() {
          void* asVoid {InternalGet()};
          return reinterpret_cast<T*>(asVoid);
        }

        ~TLSKey() = default;
        TLSKey(TLSKey& other) = delete;
        TLSKey(TLSKey&& other) = default;
        TLSKey& operator=(TLSKey& other) = delete;
        TLSKey& operator=(TLSKey&& other) = default;
    };


    class EXPORT_FOR_TESTING_ONLY Platform {
      public:
        // Print to stderr
        static void PrintError(const char* message);

        // Exit with error
        static void ExitWithError(const char* message);
    };


    // RAII class for handling functions that should only be executed once per thread
    class EXPORT_FOR_TESTING_ONLY OneShot {
      public:
        OneShot(OneTimeFunction f);

        ~OneShot();

        // Returns true if the function was invoked succesfully, false if the native function returned an error
        bool Run();

        OneShot(OneShot&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Moving in to non-null OneShot?");
          void* otherData {other.privateData};
          other.privateData = nullptr;
          privateData = otherData;
        }

        OneShot& operator=(OneShot&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Move assigning in to non-null OneShot?");
          void* otherData {other.privateData};
          other.privateData = nullptr;
          privateData = otherData;
          return *this;
        }

        OneShot(const OneShot& other) = delete;
        OneShot& operator=(const OneShot& other) = delete;

      private:
        OneTimeFunction oneTimeFunction;
        void* privateData;
    };


    // RAII class for handling OS Mutexes
    class EXPORT_FOR_TESTING_ONLY Mutex {
      public:
        // Construction can invoke std::terminate if native mutex construction fails
        Mutex();

        ~Mutex();

        // Lock and Unlock return true if the native operation completes successfully, false otherwise
        bool Lock();
        bool Unlock();

        Mutex(Mutex&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Moving in to non-null Mutex?");
          void* otherData {other.privateData};
          other.privateData = nullptr;
          privateData = otherData;
        }

        Mutex& operator=(Mutex&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Move assigning in to non-null Mutex?");
          void* otherData {other.privateData};
          other.privateData = nullptr;
          privateData = otherData;
          return *this;
        }

        Mutex(const Mutex& other) = delete;
        Mutex& operator=(const Mutex& other) = delete;

      private:
        void* privateData {nullptr};
    };


    // RAII class for handling OS Threads
    class EXPORT_FOR_TESTING_ONLY Thread {
      public:
        Thread(ThreadFunction fn) : threadFunction {fn} {}

        ~Thread();

        bool HasExecuted() { return hasExecuted; }

        // Attempt to spawn the thread: returns false if the native implementation failed, true otherwise
        bool Run(void* arg = nullptr);

        // std::terminate may be called if the native join attempt fails
        void* Join();

        Thread(Thread&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Moving in to non-null Thread?");
          void* otherData {other.privateData};
          other.privateData = nullptr;
          privateData = otherData;
        }

        Thread& operator=(Thread&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Move assigning in to non-null Thread?");
          void* otherData {other.privateData};
          other.privateData = nullptr;
          privateData = otherData;
          return *this;
        }

        Thread(const Thread& other) = delete;
        Thread& operator=(const Thread& other) = delete;

      private:
        ThreadFunction threadFunction;
        void* privateData {nullptr};
        bool hasExecuted {false};
    };
  }
}


#endif
