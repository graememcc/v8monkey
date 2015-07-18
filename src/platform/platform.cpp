// fprintf
#include <cstdio>

// exit
#include <cstdlib>

// memcpy
#include <cstring>

// terminate
#include <exception>

// pthread_key_(create|delete|get_specific|set_specific|t) pthread_(create|join|t)
// pthread_mutex_(destroy|init|lock|t|unlock) pthread_once
#include <pthread.h>

// Class definition
#include "platform.h"

// V8MONKEY_ASSERT
#include "utils/V8MonkeyCommon.h"


namespace v8 {
  namespace V8Platform {
    Mutex::Mutex() {
      if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        pthread_mutex_t* platformMutex {new pthread_mutex_t};

        int err {pthread_mutex_init(platformMutex, nullptr)};
        // We're not going to be able to do anything useful if we can't create the locking mechanism
        if (err)
          std::terminate();

        privateData = reinterpret_cast<void*>(platformMutex);
      } else {
        pthread_mutex_t platformMutex {};

        int err{pthread_mutex_init(&platformMutex, nullptr)};
        // We're not going to be able to do anything useful if we can't create the locking mechanism
        if (err)
          std::terminate();

        std::memcpy(reinterpret_cast<char*>(&privateData), reinterpret_cast<char*>(&platformMutex),
                    sizeof(pthread_mutex_t));
      }
    }


    Mutex::~Mutex() {
      if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        pthread_mutex_t* platformMutex {reinterpret_cast<pthread_mutex_t*>(privateData)};
        V8MONKEY_ASSERT(platformMutex, "Native mutex is a nullptr");
        // There's nothing that can really be done if the native "destructor" failed
        pthread_mutex_destroy(platformMutex);
        delete platformMutex;
      } else {
        pthread_mutex_t platformMutex;
        std::memcpy(reinterpret_cast<char*>(&platformMutex), reinterpret_cast<char*>(&privateData),
                    sizeof(pthread_mutex_t));
        // There's nothing that can really be done if the native "destructor" failed
        pthread_mutex_destroy(&platformMutex);
      }
    }


    bool Mutex::Lock() {
      if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        pthread_mutex_t* platformMutex {reinterpret_cast<pthread_mutex_t*>(privateData)};
        V8MONKEY_ASSERT(platformMutex, "Native mutex is a nullptr");
        return pthread_mutex_lock(platformMutex) == 0;
      } else {
        pthread_mutex_t platformMutex;
        std::memcpy(reinterpret_cast<char*>(&platformMutex), reinterpret_cast<char*>(&privateData),
                    sizeof(pthread_mutex_t));
        return pthread_mutex_lock(&platformMutex) == 0;
      }
    }


    bool Mutex::Unlock() {
      if (sizeof(pthread_mutex_t) > sizeof(void*)) {
        pthread_mutex_t* platformMutex {reinterpret_cast<pthread_mutex_t*>(privateData)};
        V8MONKEY_ASSERT(platformMutex, "Native mutex is a nullptr");
        return pthread_mutex_unlock(platformMutex) == 0;
      } else {
        pthread_mutex_t platformMutex;
        std::memcpy(reinterpret_cast<char*>(&platformMutex), reinterpret_cast<char*>(&privateData),
                    sizeof(pthread_mutex_t));
        return pthread_mutex_unlock(&platformMutex) == 0;
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


    bool OneShot::Run() {
      if (sizeof(pthread_once_t) <= sizeof(void*)) {
        return pthread_once(reinterpret_cast<pthread_once_t*>(&privateData), oneTimeFunction) == 0;
      } else {
        V8MONKEY_ASSERT(privateData, "Native OneShot is a nullptr");
        return pthread_once(reinterpret_cast<pthread_once_t*>(privateData), oneTimeFunction) == 0;
      }
    }


    Thread::~Thread() {
      if (sizeof(pthread_t) > sizeof(void*)) {
        pthread_t* thread {reinterpret_cast<pthread_t*>(privateData)};
        delete thread;
      }
    }


    bool Thread::Run(void* arg) {
      V8MONKEY_ASSERT(!hasExecuted, "Attempting to execute thread that has already executed");

      hasExecuted = true;
      if (sizeof(pthread_t) <= sizeof(void*)) {
        return pthread_create(reinterpret_cast<pthread_t*>(&privateData), nullptr, threadFunction, arg) == 0;
      } else {
        pthread_t* thread {new pthread_t};
        privateData = reinterpret_cast<void*>(thread);
        return pthread_create(thread, nullptr, threadFunction, arg) == 0;
      }
    }


    void* Thread::Join() {
      V8MONKEY_ASSERT(hasExecuted, "Attempting to join thread that has not been executed");
      void* resultPtr;

      if (sizeof(pthread_t) <= sizeof(void*)) {
        pthread_t thread {};
        std::memcpy(reinterpret_cast<char*>(&thread), reinterpret_cast<char*>(&privateData), sizeof(pthread_t));
        int err {pthread_join(thread, &resultPtr)};

        // Can't really do anything useful if join failed
        if (err)
          std::terminate();
      } else {
        V8MONKEY_ASSERT(privateData, "Native Thread is a nullptr");
        pthread_t thread {*reinterpret_cast<pthread_t*>(privateData)};
        int err {pthread_join(thread, &resultPtr)};

        // Can't really do anything useful if join failed
        if (err)
          std::terminate();
      }

      return resultPtr;
    }


    PlatformTLSKey::PlatformTLSKey(void (*destructorFn)(void*)) {
      if (sizeof(pthread_key_t) > sizeof(void*)) {
        pthread_key_t* platformTLSKey {new pthread_key_t};

        int err {pthread_key_create(platformTLSKey, destructorFn)};
        // Bail if the POSIX call failed
        if (err)
          std::terminate();

        privateData = reinterpret_cast<void*>(platformTLSKey);
      } else {
        pthread_key_t platformTLSKey {};

        int err {pthread_key_create(&platformTLSKey, destructorFn)};
        // Bail if the POSIX call failed
        if (err)
          std::terminate();

        std::memcpy(reinterpret_cast<char*>(&privateData), reinterpret_cast<char*>(&platformTLSKey),
                    sizeof(pthread_key_t));
      }
    }


    PlatformTLSKey::~PlatformTLSKey() {
      if (sizeof(pthread_key_t) > sizeof(void*)) {
        pthread_key_t* platformTLSKey {reinterpret_cast<pthread_key_t*>(privateData)};

        // I don't think there's really much we can do if pthread_key_delete fails.
        pthread_key_delete(*platformTLSKey);
        delete platformTLSKey;
      } else {
        pthread_key_t platformTLSKey {};
        std::memcpy(reinterpret_cast<char*>(&platformTLSKey), reinterpret_cast<char*>(&privateData),
                    sizeof(pthread_key_t));

        // I don't think there's really much we can do if pthread_key_delete fails.
        pthread_key_delete(platformTLSKey);
      }
    }



    bool PlatformTLSKey::InternalSet(void* value) {
      if (sizeof(pthread_key_t) > sizeof(void*)) {
        pthread_key_t* platformTLSKey {reinterpret_cast<pthread_key_t*>(privateData)};

        return pthread_setspecific(*platformTLSKey, value) == 0;
      } else {
        pthread_key_t platformTLSKey {};
        std::memcpy(reinterpret_cast<char*>(&platformTLSKey), reinterpret_cast<char*>(&privateData),
                    sizeof(pthread_key_t));

        return pthread_setspecific(platformTLSKey, value) == 0;
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
