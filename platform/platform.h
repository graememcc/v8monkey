#ifndef V8MONKEY_PLATFORM_H
#define V8MONKEY_PLATFORM_H

// size_t
#include <cstddef>

/*
 * This file abstracts away platform specific details, though at time of writing the only implementation is for Linux
 *
 */


#ifndef APIEXPORT
  #define APIEXPORT __attribute__ ((visibility("default")))
#endif


namespace v8 {
  namespace V8Platform {
    class Once;
    class Thread;


    // Platform-agnostic one-shot functions (e.g. pthread_once on POSIX, InitOnceExecuteOnce on Windows)
    typedef void (*OneTimeFunction)();
    class OSOnce {
      public:
        OSOnce(OneTimeFunction f) : fn(f) {}

        virtual ~OSOnce() {}
        virtual void Run() = 0;


      protected:
        OneTimeFunction fn;
    };


    // Platform-agnostic wrapper around a TLS key
    class APIEXPORT TLSKey {
      public:
        TLSKey() = delete;
        TLSKey(TLSKey& other) = delete;
        TLSKey(TLSKey&& other) = delete;
        TLSKey& operator=(TLSKey& other) = delete;
        TLSKey& operator=(TLSKey&& other) = delete;
        ~TLSKey() = delete;
    };


    // Platform-agnostic wrapper around a thread
    typedef void* (*ThreadFunction)(void*);
    class OSThread {
      public:
        OSThread(ThreadFunction tf) : fn(tf), hasRan(false) {}

        virtual ~OSThread() {}

        // Start the given thread. Undefined if the thread is already running or has already completed.
        virtual void Run(void* arg = nullptr) = 0;

        // Has this thread ever started?
        bool HasRan() { return hasRan; }

        virtual void* Join() = 0;

      protected:
        ThreadFunction fn;
        bool hasRan;
    };



    class APIEXPORT Platform {
      public:
        // Create a thread-local storage key with the optional destructor
        static TLSKey* CreateTLSKey(void (*destructorFn)(void*) = nullptr);

        // Delete a thread-local storage key
        static void DeleteTLSKey(TLSKey* k);

        // Store a value in thread-local storage
        static void StoreTLSData(TLSKey* k, void* value);

        // Retrieve a value from TLS
        static void* GetTLSData(TLSKey* k);

        // Print to stderr
        static void PrintError(const char* message);

        // Exit with error
        static void ExitWithError(const char* message);

      private:
        // Create and initialize a Thread
        static OSThread* CreateThread(ThreadFunction tf);

        // Create and initialize a one-time function
        static OSOnce* CreateOneShotFunction(OneTimeFunction f);

      friend class OneShot;
      friend class Mutex;
      friend class Thread;
    };


    // RAII class for handling OSOnce pointers
    class APIEXPORT OneShot {
      public:
        OneShot(OneTimeFunction f) : oneShot (Platform::CreateOneShotFunction(f)) {}
        ~OneShot() { delete oneShot; }

        void Run() {
          oneShot->Run();
        }

      private:
        OSOnce* oneShot;
        // XXX Delete copy constructor
    };


    // RAII class for handling OSMutex pointers
    class APIEXPORT Mutex {
      public:
        Mutex();

        ~Mutex();

        void Lock();

        void Unlock();

        Mutex(Mutex&& other) {
          // XXX Assert privateData is null
          void* otherData = other.privateData;
          other.privateData = nullptr;
          privateData = otherData;
        }

        Mutex& operator=(Mutex&& other) {
          // XXX Assert privateData is null
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
          // XXX Assert privateData is null
          void* otherData = other.privateData;
          other.privateData = nullptr;
          privateData = otherData;
        }

        Thread& operator=(Thread&& other) {
          // XXX Assert privateData is null
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
