// fprintf
#include <cstdio>

// exit
#include <cstdlib>

// memcpy
#include <cstring>

// pthread_key_(create|delete|get_specific|set_specific|t) pthread_(create|join|t)
// pthread_mutex_(destroy|init|lock|t|unlock) pthread_once
#include <pthread.h>

// Class definition
#include "platform.h"


namespace {
  pthread_mutex_t mutexFromRaw(void* raw) {
    pthread_mutex_t p {};
    std::memcpy(reinterpret_cast<char*>(&p), reinterpret_cast<char*>(&raw), sizeof(pthread_mutex_t));
    return p;
  }


  pthread_mutex_t* mutexPtrFromRaw(void* raw) {
    pthread_mutex_t* p {reinterpret_cast<pthread_mutex_t*>(raw)};
    return p;
  }
}


namespace v8 {
  namespace V8Platform {
    Mutex::Mutex() {
      if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        pthread_mutex_t* platformMutex {new pthread_mutex_t};
        // XXX Should we abort on failure?
        pthread_mutex_init(platformMutex, nullptr);
        privateData = reinterpret_cast<void*>(platformMutex);
      } else {
        pthread_mutex_t platformMutex {};
        // XXX Should we abort on failure?
        pthread_mutex_init(&platformMutex, nullptr);
        std::memcpy(reinterpret_cast<char*>(privateData), reinterpret_cast<char*>(&platformMutex),
                    sizeof(pthread_mutex_t));
      }
    }


    Mutex::~Mutex() {
      if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        pthread_mutex_t* platformMutex {mutexPtrFromRaw(privateData)};
        // XXX assert not null
        // XXX Should we abort on failure?
        pthread_mutex_destroy(platformMutex);
        delete platformMutex;
      } else {
        pthread_mutex_t platformMutex = mutexFromRaw(privateData);
        // XXX Should we abort on failure?
        pthread_mutex_destroy(&platformMutex);
      }
    }


    void Mutex::Lock() {
      if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        pthread_mutex_t* platformMutex {mutexPtrFromRaw(privateData)};
        // XXX assert not null
        // XXX Should we abort on failure?
        pthread_mutex_unlock(platformMutex);
      } else {
        pthread_mutex_t platformMutex = mutexFromRaw(privateData);
        // XXX Should we abort on failure?
        pthread_mutex_unlock(&platformMutex);
      }
    }


    void Mutex::Unlock() {
      if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        pthread_mutex_t* platformMutex {mutexPtrFromRaw(privateData)};
        // XXX assert not null
        // XXX Should we abort on failure?
        pthread_mutex_unlock(platformMutex);
      } else {
        pthread_mutex_t platformMutex = mutexFromRaw(privateData);
        // XXX Should we abort on failure?
        pthread_mutex_unlock(&platformMutex);
      }
    }


    OneShot::OneShot(OneTimeFunction f) : oneTimeFunction {f}, privateData {nullptr} {
      if (sizeof(pthread_once_t) > sizeof(void*)) {
        pthread_once_t* once {new pthread_once_t};
        *once = PTHREAD_ONCE_INIT;
        privateData = reinterpret_cast<void*>(once);
      } else {
        pthread_once_t once {PTHREAD_ONCE_INIT};
        std::memcpy(reinterpret_cast<char*>(&privateData), reinterpret_cast<char*>(&once), sizeof(pthread_once_t));
      }
    }


    OneShot::~OneShot() {
      if (sizeof(pthread_once_t) > sizeof(void*)) {
        pthread_once_t* once {reinterpret_cast<pthread_once_t*>(privateData)};
        delete once;
      }
    }


    void OneShot::Run() {
      if (sizeof(pthread_once_t) <= sizeof(void*)) {
        pthread_once(reinterpret_cast<pthread_once_t*>(&privateData), oneTimeFunction);
      } else {
        pthread_once(reinterpret_cast<pthread_once_t*>(privateData), oneTimeFunction);
      }
    }


    Thread::~Thread() {
      if (sizeof(pthread_t) > sizeof(void*)) {
        pthread_t* thread {reinterpret_cast<pthread_t*>(privateData)};
        delete thread;
      }
    }


    void Thread::Run(void* arg) {
      // XXX Assert not been executed
      if (sizeof(pthread_t) <= sizeof(void*)) {
        pthread_create(reinterpret_cast<pthread_t*>(&privateData), nullptr, threadFunction, arg);
      } else {
        pthread_t* thread {new pthread_t};
        pthread_create(thread, nullptr, threadFunction, arg);
        privateData = reinterpret_cast<void*>(thread);
      }

      hasExecuted = true;
    }


    void* Thread::Join() {
      // XXX Assert we've been executed
      void* resultPtr;

      if (sizeof(pthread_t) <= sizeof(void*)) {
        pthread_t thread {};
        std::memcpy(reinterpret_cast<char*>(&thread), reinterpret_cast<char*>(&privateData), sizeof(pthread_t));
        pthread_join(thread, &resultPtr);
      } else {
        pthread_t thread {*reinterpret_cast<pthread_t*>(privateData)};
        pthread_join(thread, &resultPtr);
      }

      return resultPtr;
    }


    TLSKey* Platform::CreateTLSKey(void (*destructorFn)(void*)) {
      static_assert(sizeof(pthread_key_t) <= sizeof(TLSKey*), "pthread_key_t won't fit in a pointer; cannot type-pun");

      // We don't actually create an object, we just pretend whatever pthread_key_create gives us is a pointer.
      // A test in test_platform checks that pthread_key_t fits, however, as noted there, I'd like to check this earlier,
      // e.g. perhaps at the configure stage if we go down the configure/make route
      pthread_key_t key;

      // XXX Should we abort on non-zero return code?
      pthread_key_create(&key, destructorFn);
      TLSKey* result {nullptr};
      std::memcpy(reinterpret_cast<char*>(&result), reinterpret_cast<char*>(&key), sizeof(pthread_key_t));

      return result;
    }


    void Platform::DeleteTLSKey(TLSKey* k) {
      pthread_key_t key;
      std::memcpy(reinterpret_cast<char*>(&key), reinterpret_cast<char*>(&k), sizeof(pthread_key_t));

      // XXX Should we abort on non-zero return code?
      pthread_key_delete(key);
    }


    void Platform::StoreTLSData(TLSKey* k, void* value) {
      pthread_key_t key;
      std::memcpy(reinterpret_cast<char*>(&key), reinterpret_cast<char*>(&k), sizeof(pthread_key_t));

      // XXX Should we abort on non-zero return code?
      pthread_setspecific(key, value);
    }


    void* Platform::GetTLSData(TLSKey* k) {
      pthread_key_t key;
      std::memcpy(reinterpret_cast<char*>(&key), reinterpret_cast<char*>(&k), sizeof(pthread_key_t));

      return pthread_getspecific(key);
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
