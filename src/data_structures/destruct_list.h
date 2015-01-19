#ifndef V8MONKEY_DESTRUCTLIST_H
#define V8MONKEY_DESTRUCTLIST_H


/*
 * A class that wraps a linked list of pointers to a given type T, but which also takes a function that will be called
 * on destruction. This function will be called once for each pointer in the list, and should perform any teardown for
 * the given type. The list will not delete the objects pointed to itself.
 *
 * Pointers can be added multiple times, but will only be stored once. The deletion function is thus guaranteed to only
 * be called once per pointer. Note that NULL pointers are accepted.
 *
 * Constructing without a deletion function will result in a standard linked list.
 *
 * The intention was to use this for JSRuntime and JSContext teardown, however the design didn't work out. I'm leaving
 * this in the tree for the moment, in case I find a use for it later.
 *
 */


namespace v8 {
  namespace V8Monkey {

    template <class T>
    class DestructingList {
      public:
        DestructingList() : destructionFunction(nullptr), head(nullptr) {}
        DestructingList(void (*destructionFn)(T*)) : destructionFunction(destructionFn), head(nullptr) {}

        ~DestructingList() {
          DestructingListElem<T>* current = head;

          while (current != nullptr) {
            DestructingListElem<T>* next = current->next;

            if (destructionFunction) {
              destructionFunction(current->value);
            } else {
              delete current->value;
            }

            delete current;
            current = next;
          }

          head = nullptr;
        }

        void Add(T* value) {
          if (Contains(value)) {
            return;
          }

          DestructingListElem<T>* newVal = new DestructingListElem<T>(value, nullptr, head);
          if (head) {
            head->prev = newVal;
          }
          head = newVal;
        }

        void Delete(T* value) {
          DestructingListElem<T>* val = Find(value);
          if (val == nullptr) {
            return;
          }

          if (val->prev != nullptr) {
            val->prev->next = val->next;
          }

          if (val->next != nullptr) {
            val->next->prev = val->prev;
          }

          if (head == val) {
            head = val->next;
          }

          if (destructionFunction) {
            destructionFunction(val->value);
          } else {
            delete val->value;
          }

          delete val;
        }

        int Count() {
          int count = 0;
          DestructingListElem<T>* val = head;
          while (val != nullptr) {
            count++;
            val = val->next;
          }
          return count;
        }

        bool Contains(T* value) {
          return Find(value) != nullptr ? true : false;
        }

        void Iterate(void (*iterationFunction)(T*)) {
          DestructingListElem<T>* current = head;

          while (current != nullptr) {
            iterationFunction(current->value);
            current = current->next;
          }
        }

      private:
        template <class S>
        class DestructingListElem {
          public:
            DestructingListElem(S* val, DestructingListElem<S>* p, DestructingListElem<S>* n) : value(val), prev(p), next(n) {}
            S* value;
            DestructingListElem<S>* prev;
            DestructingListElem<S>* next;
        };

        void (*destructionFunction)(T*);

        DestructingListElem<T>* head;

        DestructingListElem<T>* Find(T* value) {
          DestructingListElem<T>* current = head;
          while (current != nullptr) {
            if (current->value == value) {
              return current;
            }
            current = current->next;
          }

          return nullptr;
        }
    };
  }
}


#endif
