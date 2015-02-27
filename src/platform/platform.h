#ifndef V8MONKEY_PLATFORM_H
#define V8MONKEY_PLATFORM_H

// size_t
#include <cstddef>

// conditional, decay, is_same, remove_reference, remove_cv
#include <type_traits>

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


    // Platform-agnostic wrapper around a TLS key
    class TLSKey {
      public:
        TLSKey() = delete;
        TLSKey(TLSKey& other) = delete;
        TLSKey(TLSKey&& other) = delete;
        TLSKey& operator=(TLSKey& other) = delete;
        TLSKey& operator=(TLSKey&& other) = delete;
        ~TLSKey() = delete;
    };


    class APIEXPORT Platform {
      public:
        // Create a thread-local storage key with the optional destructor
        static TLSKey* CreateTLSKey(void (*destructorFn)(void*) = nullptr);

        // Delete a thread-local storage key
        static void DeleteTLSKey(TLSKey* k);

        /*
         * TLSKeys generally need to be in static storage in the files that use them, making resource-management tricky:
         * one option is to place them in a smart pointer. To that end, the namespace exports a custom deleter. To aid
         * callers who use that option, we template the Get/Set methods for arbitrary pointer types.
         *
         */

        // Store a value in thread-local storage
        template <typename T>
        static void StoreTLSData(T& key, void* value) {
          typename DeReffable<T>::type deref;
          PutTLSData(deref(key), value);
        }

        // Retrieve a value from TLS
        template <typename T>
        static void* GetTLSData(T& key) {
          typename DeReffable<T>::type deref;
          return FetchTLSData(deref(key));
        }

        // Print to stderr
        static void PrintError(const char* message);

        // Exit with error
        static void ExitWithError(const char* message);

      private:
        static void* FetchTLSData(TLSKey* k);
        static void PutTLSData(TLSKey* k, void* value);

        /*
         * Helpers for metaprogramming StoreTLSKey / GetTLSKey. To avoid over-engineering this thing any further, we
         * assume no cv-qualifier's on the smart pointer's pointer type
         *
         */

        template <typename T>
        struct DeReffable {
          template <typename U>
          struct DeRefSmartPtr {
            TLSKey* operator()(U& key) {
              using Element = typename std::decay<U>::type::pointer;
              static_assert(std::is_same<TLSKey*, Element>::value, "Called with incorrect type of smart pointer");
              return key.get();
            }
          };

          template <typename U>
          struct DeRefNormalPtr {
            TLSKey* operator()(U& key) { return key; }
          };

          using NoRef = std::remove_reference<T>;
          using NoCV = std::remove_cv<typename NoRef::type>;
          static const bool isNormalPtr {std::is_same<TLSKey*, typename NoCV::type>::value};
          using type = typename std::conditional<isNormalPtr, DeRefNormalPtr<T>, DeRefSmartPtr<T>>::type;
        };
    };


    /*
     * Custom deleter that can be supplied to smart pointers like unique_ptr.
     *
     */

    struct TLSKeyDeleter {
      void operator()(TLSKey* key) {
        Platform::DeleteTLSKey(key);
      }
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
