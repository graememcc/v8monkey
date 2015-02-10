#ifndef V8MONKEY_ENCODING_H
#define V8MONKEY_ENCODING_H

// std::transform
#include <algorithm>

// begin, end, distance, iterator_traits
#include <iterator>

// vector
#include <vector>


namespace v8 {
  // XXX Move out of V8Monkey namespace ?
  namespace V8Monkey {
    namespace UTF8 {
      using UTF16Encoded = std::vector<char16_t>;

      constexpr char16_t replacementChar {0xfffdu};
      constexpr unsigned int continuationMask {0x3fu};


      // TODO: Can we hide this implementation detail from includers?
      void utf8toSurrogatePair(const unsigned char c1, const unsigned char c2,
                               const unsigned char c3, const unsigned char c4, UTF16Encoded& dest) {
        auto ls_bottom6 = c4 & continuationMask;
        auto ls_top4 = (c3 & 0x0fu) << 6;
        auto ls_base = ls_bottom6 | ls_top4;

        auto hs_bottom2 = (c3 & 0x30u) >> 4;
        auto hs_middle6 = (c2 & continuationMask) << 2;
        auto hs_top4 = (c1 & 0x0fu) << 8;
        // XXX For safety is it worth asserting that (hs_bottom2 | hs_middle6 | hs_top4) >= 0x40?
        // XXX And is that bound even right? Is it >= 0x40? Check computation vs wiki / standard
        auto hs_base = (hs_bottom2 | hs_middle6 | hs_top4) - 0x40u;

        auto ls_modifier = 0xdc00u;
        auto hs_modifier = 0xd800u;

        dest.emplace_back(static_cast<char16_t>(hs_modifier + hs_base));
        dest.emplace_back(static_cast<char16_t>(ls_modifier + ls_base));
      }


      template <typename T>
      bool isDegenerate(T begin, T end) {
        return begin == end;
      }


      template <typename T>
      bool isDegenerate(T* begin, T* end) {
        return begin == end || !begin || !end;
      }


      // TODO: Can we hide this implementation detail from includers?
      template <typename T, typename U = typename std::iterator_traits<T>::value_type, bool isTwoByte = sizeof(U) == 2>
      struct UTF16Encoder;


      // TODO: Can we hide this implementation detail from includers?
      // Note: Assumes T and U are bidirectional iterators 
      template <typename T, typename U> struct UTF16Encoder<T, U, true> {
        static UTF16Encoded encode(T begin, T end) {
          // Handle degenerate iterators first. Note: the iterators could be pointers, and hence nullptrs
          if (isDegenerate(begin, end)) {
            return UTF16Encoded {0x0000u};
          }

          bool needsTerminatingZeroAdded { *(end - 1) != 0x0000u };
          UTF16Encoded result {begin, end};
          if (needsTerminatingZeroAdded) {
            result.emplace_back(0x0000u);
          }

          return result;
        }
      };


      // TODO: Find a suitable way to hide this implementation detail from includers
      template <typename T, typename U> struct UTF16Encoder<T, U, false> {
        static UTF16Encoded encode(T begin, T end) {
          // Handle degenerate iterators first
          if (isDegenerate(begin, end)) {
            return UTF16Encoded {0x0000u};
          }

          bool needsTerminatingZeroAdded { *(end - 1) != 0x00u };

          UTF16Encoded result {};
          // We have an upper bound on the possible size of the result
          UTF16Encoded::size_type numberOfCodeUnits = static_cast<UTF16Encoded::size_type>(std::distance(begin, end));
          result.reserve(numberOfCodeUnits + (needsTerminatingZeroAdded ? 1 : 0));

          for (auto it = begin; it != end; ++it) {
            unsigned char c {static_cast<unsigned char>(*it)};

            // ASCII can be copied as-is
            if (!(c & 0x80u)) {
              result.emplace_back(static_cast<char16_t>(c));
              continue;
            }

            // At this point, we know that c >= 0x80
            if (!(c & 0x40u)) {
              result.emplace_back(replacementChar);
              continue;
            }

            // We have now established that the code-unit is not a continuation code-unit. Next we check for invalid leading
            // values that are independent of later code-units
            if (c < 0xc2u || c > 0xf4u) {
              result.emplace_back(replacementChar);
              continue;
            }

            // Note: we test != end above, so we cannot move the iterator until we know incrementing it won't reach
            //       beyond the end. In addition, we don't want to move the iterator if the next code-unit is not a
            //       valid continuation code-unit: we'll encode it on the next pass
            auto next = it + 1;
            if (next == end || (*next & 0xc0) != 0x80) {
              result.emplace_back(replacementChar);
              continue;
            }

            unsigned char c2 {static_cast<unsigned char>(*next)};

            // We can now weed out any ill-formed encodings based on the second code-unit.
            if ((c == 0xe0u && c2 < 0xa0u) || (c == 0xedu && !(c2 < 0xa0u)) ||
                (c == 0xf0u && c2 < 0x90u) || (c == 0xf4u && !(c2 < 0x90u))) {
              result.emplace_back(replacementChar);
              continue;
            }

            if (c < 0xe0u) {
              it = next;
              result.emplace_back(((c & 0x1fu) << 6) | (c2 & continuationMask));
              continue;
            }

            // Having reached this point, we have a code-point encoded in 3 or 4 code-units
            ++next;
            if (next == end || (*next & 0xc0) != 0x80) {
              result.emplace_back(replacementChar);
              continue;
            }

            unsigned char c3 {static_cast<unsigned char>(*next)};

            if (c < 0xf0u) {
              it = next;
              result.emplace_back(((c & 0xfu) << 12) | ((c2 & continuationMask) << 6) | (c3 & continuationMask));
              continue;
            }

            // We must have a code-point encoded in 4 code-units
            ++next;
            if (next == end || (*next & 0xc0) != 0x80) {
              result.emplace_back(replacementChar);
              continue;
            }

            unsigned char c4 {static_cast<unsigned char>(*next)};
            it = next;

            // Transform to a surrogate pair
            utf8toSurrogatePair(c, c2, c3, c4, result);
          }

          if (needsTerminatingZeroAdded) {
            result.emplace_back(0x0000);
          }

          // We used the worst-case bound when reserving space
          result.shrink_to_fit();
          return result;
        }
      };


      template <typename T>
      UTF16Encoded EncodeToUTF16(T begin, T end) {
        return UTF16Encoder<T>::encode(begin, end);
      }
    }
  }
}


#endif
