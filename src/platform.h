#ifndef V8MONKEY_PLATFORM_H
#define V8MONKEY_PLATFORM_H

/*
 * This file abstracts away platform specific details, though at time of writing the only implementation is for Linux
 *
 */

namespace v8 {
namespace V8Monkey {


  // Class representing a simple mutex
  class Mutex {
    public:
      virtual ~Mutex() {}
      virtual int Lock() = 0;
      virtual int Unlock() = 0;
  };


  // Platform-agnostic one-shot functions (e.g. pthread_once on POSIX, InitOnceExecuteOnce on Windows)
  typedef void (*OneTimeFunction)();
  class OneTimeFunctionControl {
    public:
      OneTimeFunctionControl(OneTimeFunction f) : fn(f) {}

      virtual ~OneTimeFunctionControl() {}
      virtual void Run() = 0;


    protected:
      OneTimeFunction fn;
  };


  // Platform-agnostic wrapper around a TLS key
  class TLSKey {
    private:
      TLSKey() {}
      ~TLSKey() {}
  };


  // Platform-agnostic wrapper around a thread
  typedef void* (*ThreadFunction)(void*);
  class Thread {
    public:
      Thread(ThreadFunction tf) : fn(tf), hasRan(false) {}

      virtual ~Thread() {}

      // Start the given thread. Undefined if the thread is already running or has already completed.
      virtual void Run(void* arg) = 0;
      void Run() {Run(NULL); }
      
      // Has this thread ever started?
      bool HasRan() { return hasRan; }

      virtual void* Join() = 0;

    protected:
      ThreadFunction fn;
      bool hasRan;
  };


  class Platform {
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

      #ifdef V8MONKEY_INTERNAL_TEST
      static size_t GetTLSKeySize();
      #endif

      // Create and initialize a Thread
      static Thread* CreateThread(ThreadFunction tf);
  };


} // namespace V8Monkey
} // namespace v8
#endif
