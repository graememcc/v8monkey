#ifndef V8MONKEY_PLATFORM_H
#define V8MONKEY_PLATFORM_H


/*
 * This file abstracts away platform specific details, though at time of writing the only implementation is for Linux
 *
 */


#ifndef APIEXPORT
  #define APIEXPORT __attribute__ ((visibility("default")))
#endif


namespace v8 {
  namespace V8Platform {
    // Class representing a simple mutex
    class APIEXPORT Mutex {
      public:
        virtual ~Mutex() {}
        virtual int Lock() = 0;
        virtual int Unlock() = 0;
    };


    // Platform-agnostic one-shot functions (e.g. pthread_once on POSIX, InitOnceExecuteOnce on Windows)
    typedef void (*OneTimeFunction)();
    class APIEXPORT OneTimeFunctionControl {
      public:
        OneTimeFunctionControl(OneTimeFunction f) : fn(f) {}

        virtual ~OneTimeFunctionControl() {}
        virtual void Run() = 0;


      protected:
        OneTimeFunction fn;
    };


    // Platform-agnostic wrapper around a TLS key
    class APIEXPORT TLSKey {
      private:
        TLSKey() {}
        ~TLSKey() {}
    };


    // Platform-agnostic wrapper around a thread
    typedef void* (*ThreadFunction)(void*);
    class APIEXPORT Thread {
      public:
        Thread(ThreadFunction tf) : fn(tf), hasRan(false) {}

        virtual ~Thread() {}

        // Start the given thread. Undefined if the thread is already running or has already completed.
        virtual void Run(void* arg = NULL) = 0;
        
        // Has this thread ever started?
        bool HasRan() { return hasRan; }

        virtual void* Join() = 0;

      protected:
        ThreadFunction fn;
        bool hasRan;
    };


    class APIEXPORT Platform {
      public:
        // Create and initialize a platform-specific mutex
        static Mutex* CreateMutex();

        // Create and initialize a one-time function
        static OneTimeFunctionControl* CreateOneShotFunction(OneTimeFunction f);

        // Create a thread-local storage key
        static TLSKey* CreateTLSKey();

        // Delete a thread-local storage key
        static void DeleteTLSKey(TLSKey* k);

        // Store a value in thread-local storage
        static void StoreTLSData(TLSKey* k, void* value);

        // Retrieve a value from TLS
        static void* GetTLSData(TLSKey* k);

        // For testing: returns size of platform-specific TLS key type
        static size_t GetTLSKeySize();

        // Create and initialize a Thread
        static Thread* CreateThread(ThreadFunction tf);
    };
  }
}


#endif