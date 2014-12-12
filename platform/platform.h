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
    class Once;
    class Mutex;
    class Thread;


    // Class representing a simple mutex
    class OSMutex {
      public:
        virtual ~OSMutex() {}
        virtual int Lock() = 0;
        virtual int Unlock() = 0;
    };


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
      private:
        TLSKey() {}
        ~TLSKey() {}
    };


    // Platform-agnostic wrapper around a thread
    typedef void* (*ThreadFunction)(void*);
    class OSThread {
      public:
        OSThread(ThreadFunction tf) : fn(tf), hasRan(false) {}

        virtual ~OSThread() {}

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
        // Create a thread-local storage key
        static TLSKey* CreateTLSKey();

        // Create a thread-local storage key with the given destructor
        static TLSKey* CreateTLSKey(void (*destructorFn)(void*));

        // Delete a thread-local storage key
        static void DeleteTLSKey(TLSKey* k);

        // Store a value in thread-local storage
        static void StoreTLSData(TLSKey* k, void* value);

        // Retrieve a value from TLS
        static void* GetTLSData(TLSKey* k);

        // For testing: returns size of platform-specific TLS key type
        static size_t GetTLSKeySize();

        // Print to stderr
        static void PrintError(const char* message);

        // Exit with error
        static void ExitWithError(const char* message);

      private:
        // Create and initialize a platform-specific mutex
        static OSMutex* CreateMutex();

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
        Mutex() : mutex (Platform::CreateMutex()) {}
        ~Mutex() { delete mutex; }

        void Lock() {
          mutex->Lock();
        }

        void Unlock() {
          mutex->Unlock();
        }

      private:
        OSMutex* mutex;
        // XXX Delete copy constructor
    };


    // RAII class for handling OSThreads
    class APIEXPORT Thread {
      public:
        Thread(ThreadFunction fn) : thread(Platform::CreateThread(fn)) {}
        ~Thread() { delete thread; }

        void Run(void* arg = NULL) { thread->Run(arg); }

        bool HasRan() { return thread->HasRan(); }

        void* Join() { return thread->Join(); }

      private:
        OSThread* thread;
        // XXX Delete copy constructor
    };
  }
}


#endif
