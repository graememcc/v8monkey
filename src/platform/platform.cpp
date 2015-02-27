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

// V8MONKEY_ASSERT
#include "utils/V8MonkeyCommon.h"

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
        V8MONKEY_ASSERT(platformMutex, "Native mutex is a nullptr");
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
        V8MONKEY_ASSERT(platformMutex, "Native mutex is a nullptr");
        // XXX Should we abort on failure?
        pthread_mutex_lock(platformMutex);
      } else {
        pthread_mutex_t platformMutex = mutexFromRaw(privateData);
        // XXX Should we abort on failure?
        pthread_mutex_lock(&platformMutex);
      }
    }


    void Mutex::Unlock() {
      if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        pthread_mutex_t* platformMutex {mutexPtrFromRaw(privateData)};
        V8MONKEY_ASSERT(platformMutex, "Native mutex is a nullptr");
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
        V8MONKEY_ASSERT(once, "Native OneShot is a nullptr");
        delete once;
      }
    }


    void OneShot::Run() {
      if (sizeof(pthread_once_t) <= sizeof(void*)) {
        pthread_once(reinterpret_cast<pthread_once_t*>(&privateData), oneTimeFunction);
      } else {
        V8MONKEY_ASSERT(privateData, "Native OneShot is a nullptr");
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
      V8MONKEY_ASSERT(!hasExecuted, "Attempting to execute thread that has already executed");
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
      V8MONKEY_ASSERT(hasExecuted, "Attempting to join thread that has not been executed");
      void* resultPtr;

      if (sizeof(pthread_t) <= sizeof(void*)) {
        pthread_t thread {};
        std::memcpy(reinterpret_cast<char*>(&thread), reinterpret_cast<char*>(&privateData), sizeof(pthread_t));
        pthread_join(thread, &resultPtr);
      } else {
        V8MONKEY_ASSERT(privateData, "Native Thread is a nullptr");
        pthread_t thread {*reinterpret_cast<pthread_t*>(privateData)};
        pthread_join(thread, &resultPtr);
      }

      return resultPtr;
    }


    PlatformTLSKey::PlatformTLSKey(void (*destructorFn)(void*)) {
      if (sizeof(pthread_key_t) > sizeof(void*)) {
        pthread_key_t* platformTLSKey {new pthread_key_t};
        // XXX Need to error check the result
        pthread_key_create(platformTLSKey, destructorFn);
        privateData = reinterpret_cast<void*>(platformTLSKey);
      } else {
        pthread_key_t platformTLSKey {};
        // XXX Need to error check the result
        pthread_key_create(&platformTLSKey, destructorFn);
        std::memcpy(reinterpret_cast<char*>(&privateData), reinterpret_cast<char*>(&platformTLSKey),
                    sizeof(pthread_key_t));
      }
    }


    PlatformTLSKey::~PlatformTLSKey() {
      if (sizeof(pthread_key_t) > sizeof(void*)) {
        pthread_key_t* platformTLSKey {reinterpret_cast<pthread_key_t*>(privateData)};
        // XXX Need to error check the result
        pthread_key_delete(*platformTLSKey);
        delete platformTLSKey;
      } else {
        pthread_key_t platformTLSKey {};
        std::memcpy(reinterpret_cast<char*>(&platformTLSKey), reinterpret_cast<char*>(&privateData),
                    sizeof(pthread_key_t));
        // XXX Need to error check the result
        pthread_key_delete(platformTLSKey);
      }
    }



    void PlatformTLSKey::InternalSet(void* value) {
      if (sizeof(pthread_key_t) > sizeof(void*)) {
        pthread_key_t* platformTLSKey {reinterpret_cast<pthread_key_t*>(privateData)};
        // XXX Need to error check the result
        pthread_setspecific(*platformTLSKey, value);
      } else {
        pthread_key_t platformTLSKey {};
        std::memcpy(reinterpret_cast<char*>(&platformTLSKey), reinterpret_cast<char*>(&privateData),
                    sizeof(pthread_key_t));
        // XXX Need to error check the result
        pthread_setspecific(platformTLSKey, value);
      }
    }


    void* PlatformTLSKey::InternalGet() {
      if (sizeof(pthread_key_t) > sizeof(void*)) {
        pthread_key_t* platformTLSKey {reinterpret_cast<pthread_key_t*>(privateData)};
        return pthread_getspecific(*platformTLSKey);
      } else {
        pthread_key_t platformTLSKey {};
        std::memcpy(reinterpret_cast<char*>(&platformTLSKey), reinterpret_cast<char*>(&privateData),
                    sizeof(pthread_key_t));
        return pthread_getspecific(platformTLSKey);
      }
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
