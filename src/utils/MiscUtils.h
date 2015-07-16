/*
#ifndef V8MONKEY_MISCUTILS_H
#define V8MONKEY_MISCUTILS_H

// size_t
#include <cstdlib>

// is_integral
#include <type_traits>


namespace v8 {
  namespace V8Monkey {
    namespace MiscUtils {
*/

      /*
       * Takes an array of 16-bit values, assumed to be zero-terminated, and returns the number of non-zero entries.
       *
       */

/*
      template <typename T>
      size_t TwoByteLength(const T* s) {
        static_assert(sizeof(T) == 2, "TwoByteLength template instantiated with wrong-sized type");
        static_assert(std::is_integral<T>::value, "TwoByteLength instantiated with non-integral type");

        size_t i = 0;
        while (s[i] != static_cast<T>(0x0000)) {
          i++;
        }

        return i;
      }


    }
  }
}

#endif
*/
