// fprintf
#include <cstdio>

// exit
#include <cstdlib>

// pthread_key_(create|delete|get_specific|set_specific|t) pthread_(create|join|t)
// pthread_mutex_(destroy|init|lock|t|unlock) pthread_once
#include <pthread.h>

// Class definition
#include "platform.h"


namespace v8 {
  namespace V8Platform {
    class POSIXMutex: public OSMutex {
      public:
        POSIXMutex() {
          // XXX Should we abort on failure?
          pthread_mutex_init(&platformMutex, nullptr);
        };


        ~POSIXMutex() {
          // XXX Should we abort on failure?
          pthread_mutex_destroy(&platformMutex);
        };


        int Lock() {
          return pthread_mutex_lock(&platformMutex);
        }


        int Unlock() {
          return pthread_mutex_unlock(&platformMutex);
        }


      private:
        pthread_mutex_t platformMutex;
    };


    class POSIXOneTimeFunction: public OSOnce {
      public:
        POSIXOneTimeFunction(OneTimeFunction f): OSOnce (f) {
          // Is it safe to place the macro definition in the initializer list?
          once_control = PTHREAD_ONCE_INIT;
        }


        void Run() {
          // XXX Should we abort on non-zero return code?
          pthread_once(&once_control, fn);
        }


      private:
        pthread_once_t once_control;
    };


    class POSIXThread: public OSThread {
      public:
        POSIXThread(ThreadFunction tf) : OSThread(tf) {}


        void Run(void* arg) {
          // XXX We should exit or abort if we try to run a thread that has already ran. Which?
          hasRan = true;
          pthread_create(&identifier, nullptr, fn, arg);
        }


        void* Join() {
          void* resultPtr;
          pthread_join(identifier, &resultPtr);
          return resultPtr;
        }


      private:
        pthread_t identifier;
    };


    OSMutex* Platform::CreateMutex() {
      return new POSIXMutex();
    }


    OSOnce* Platform::CreateOneShotFunction(OneTimeFunction f) {
      return new POSIXOneTimeFunction(f);
    }


    TLSKey* Platform::CreateTLSKey() {
      // We don't actually create an object, we just pretend whatever pthread_key_create gives us is a pointer.
      // A test in test_platform checks that pthread_key_t fits, however, as noted there, I'd like to check this earlier,
      // e.g. perhaps at the configure stage if we go down the configure/make route
      pthread_key_t key;

      // XXX Should we abort on non-zero return code?
      pthread_key_create(&key, nullptr);

      return reinterpret_cast<TLSKey*>(key);
    }


    TLSKey* Platform::CreateTLSKey(void (*destructorFn)(void*)) {
      // We don't actually create an object, we just pretend whatever pthread_key_create gives us is a pointer.
      // A test in test_platform checks that pthread_key_t fits, however, as noted there, I'd like to check this earlier,
      // e.g. perhaps at the configure stage if we go down the configure/make route
      pthread_key_t key;

      // XXX Should we abort on non-zero return code?
      pthread_key_create(&key, destructorFn);

      return reinterpret_cast<TLSKey*>(key);
    }


    void Platform::DeleteTLSKey(TLSKey* k) {
      // Convince C++ we're not really losing precision
      pthread_key_t key = *reinterpret_cast<pthread_key_t*>(&k);

      // XXX Should we abort on non-zero return code?
      pthread_key_delete(key);
    }


    void Platform::StoreTLSData(TLSKey* k, void* value) {
      // Convince C++ we're not really losing precision
      pthread_key_t key = *reinterpret_cast<pthread_key_t*>(&k);

      // XXX Should we abort on non-zero return code?
      pthread_setspecific(key, value);
    }


    void* Platform::GetTLSData(TLSKey* k) {
      // Convince C++ we're not really losing precision
      pthread_key_t key = *reinterpret_cast<pthread_key_t*>(&k);

      return pthread_getspecific(key);
    }


    size_t Platform::GetTLSKeySize() {
      return sizeof(pthread_key_t);
    }


    OSThread* Platform::CreateThread(ThreadFunction tf) {
      return new POSIXThread(tf);
    }


    void Platform::PrintError(const char* message) {
      fprintf(stderr, "%s", message);
    }

    void ExitWithError(const char* message) {
      fprintf(stderr, "%s\n", message);
      exit(1);
    }
  }
}
