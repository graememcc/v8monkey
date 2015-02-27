#ifndef V8MONKEY_PLATFORM_H
#define V8MONKEY_PLATFORM_H

// size_t
#include <cstddef>

// memcpy
#include <cstring>

// conditional, decay, is_same, remove_reference, remove_cv
#include <type_traits>

// EXPORT_FOR_TESTING_ONLY
#include "utils/test.h"

// V8MONKEY_ASSERT
#include "utils/V8MonkeyCommon.h"


/*
 * This file abstracts away platform specific details, though at time of writing the only implementation is for Linux
 *
 */


#ifndef APIEXPORT
  #define APIEXPORT __attribute__ ((visibility("default")))
#endif


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

    class EXPORT_FOR_TESTING_ONLY TLSKey {
      public:
        TLSKey(void (*destructorFn)(void*) = nullptr);

        ~TLSKey();

        void Put(void* data);

        void* Get();

        TLSKey(TLSKey& other) = delete;
        TLSKey(TLSKey&& other) = default;
        TLSKey& operator=(TLSKey& other) = delete;
        TLSKey& operator=(TLSKey&& other) = default;

      private:
        void* privateData {nullptr};
    };


    class APIEXPORT Platform {
      public:
        // Print to stderr
        static void PrintError(const char* message);

        // Exit with error
        static void ExitWithError(const char* message);
    };


    // RAII class for handling OSOnce pointers
    class APIEXPORT OneShot {
      public:
        OneShot(OneTimeFunction f);

        ~OneShot();

        void Run();

        OneShot(OneShot&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Moving in to non-null OneShot?");
          void* otherData = other.privateData;
          other.privateData = nullptr;
          privateData = otherData;
        }

        OneShot& operator=(OneShot&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Move assigning in to non-null OneShot?");
          void* otherData = other.privateData;
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


    // RAII class for handling OSMutex pointers
    class APIEXPORT Mutex {
      public:
        Mutex();

        ~Mutex();

        void Lock();

        void Unlock();

        Mutex(Mutex&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Moving in to non-null Mutex?");
          void* otherData = other.privateData;
          other.privateData = nullptr;
          privateData = otherData;
        }

        Mutex& operator=(Mutex&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Move assigning in to non-null Mutex?");
          void* otherData = other.privateData;
          other.privateData = nullptr;
          privateData = otherData;
          return *this;
        }

        Mutex(const Mutex& other) = delete;
        Mutex& operator=(const Mutex& other) = delete;

      private:
        void* privateData {nullptr};
    };


    // RAII class for handling OSThreads
    class APIEXPORT Thread {
      public:
        Thread(ThreadFunction fn) : threadFunction {fn} {}

        ~Thread();

        bool HasExecuted() { return hasExecuted; }

        void Run(void* arg = nullptr);

        void* Join();

        Thread(Thread&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Moving in to non-null Thread?");
          void* otherData = other.privateData;
          other.privateData = nullptr;
          privateData = otherData;
        }

        Thread& operator=(Thread&& other) {
          V8MONKEY_ASSERT(privateData == nullptr, "Move assigning in to non-null Thread?");
          void* otherData = other.privateData;
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
