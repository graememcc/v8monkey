#ifndef V8MONKEY_ISOLATE_H
#define V8MONKEY_ISOLATE_H 


#include "test.h"


namespace v8 {
  namespace V8Monkey {
    // An internal variant of the public facing Isolate class
    class EXPORT_FOR_TESTING_ONLY InternalIsolate {
      public:
        // Enter the given isolate
        void Enter();

        // Exit the given isolate
        void Exit();

        // Dispose of the given isolate
        void Dispose();

        // Is the given isolate the special default isolate?
        static bool IsDefaultIsolate(InternalIsolate* i);

        // Return the default isolate
        static InternalIsolate* GetDefaultIsolate();

        // Find the current InternalIsolate for the given thread
        static InternalIsolate* GetCurrent();

        // Create the default isolate if necessary
        static void CreateDefaultIsolate();

        // Reports whether any threads are active in this isolate
        bool ContainsThreads() { return threadData != NULL; }

        // Checks to see if the current thread is in an isolate. If not, the default isolate is entered
        static void EnsureInIsolate();

        #ifdef V8MONKEY_INTERNAL_TEST
          // Has the current thread entered the given isolate?
          static bool IsEntered(InternalIsolate* i);
        #endif

        // Isolates stack, can be entered multiple times, and can be used by multiple threads. As V8 allows threads to
        // "unlock" themselves to yield the isolate, we can't even be sure that threads will enter and exit in a LIFO
        // order-the ordering will be at the mercy of the locking mechanism. Thus we need some way of answering the
        // following questions:
        //   Which threads have entered us?
        //   How many times has a given thread entered us?
        //   When a thread leaves, which isolate is it returning to?
        // We use this class to answer such questions
        struct ThreadData {
          int entryCount;
          int threadID;
          InternalIsolate* previousIsolate;
          ThreadData* prev;
          ThreadData* next;

          ThreadData(int id, InternalIsolate* previous, ThreadData* previousElement, ThreadData* nextElement) :
            entryCount(0), threadID(id), previousIsolate(previous), prev(previousElement), next(nextElement) {}
        };

      private:
        // Each thread has a reference to its current isolate stored within the thread
        static InternalIsolate* GetIsolateFromTLS();
        static void SetIsolateInTLS(InternalIsolate* i);

        // In a single threaded application, there is no need to explicitly construct an isolate. V8 constructs a
        // "default" isolate automatically, and use it where necessary.
        static InternalIsolate* defaultIsolate;

        // Our linked list of data about active threads in this isolate
        ThreadData* threadData;

        // Linked list manipulations
        ThreadData* FindOrCreateThreadData(int threadID, InternalIsolate* previousIsolate);
        ThreadData* FindThreadData(int threadID);
        void DeleteThreadData(ThreadData* data);
    };
  }
}


#endif
