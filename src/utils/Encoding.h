#ifndef V8MONKEY_ENCODING_H
#define V8MONKEY_ENCODING_H

// std::transform
#include <algorithm>

// begin, end, distance, iterator_traits
#include <iterator>

// vector
#include <vector>


namespace v8 {
  namespace V8Monkey {
    namespace UTF8 {
      using UTF16Encoded = std::vector<char16_t>;

      constexpr char16_t replacementChar = 0xfffdu;
      constexpr unsigned int continuationMask = 0x3fu;


      // TODO: Find a suitable way to hide this implementation detail from includers
      void utf8toSurrogatePair(const unsigned char c1, const unsigned char c2,
                               const unsigned char c3, const unsigned char c4, UTF16Encoded& dest) {
        char16_t ls_bottom6 = c4 & continuationMask;
        char16_t ls_top4 = static_cast<char16_t>((c3 & 0x0f) << 6);
        char16_t ls_base = ls_bottom6 | ls_top4;

        char16_t hs_bottom2 = static_cast<char16_t>((c3 & 0x30) >> 4);
        char16_t hs_middle6 = static_cast<char16_t>((c2 & continuationMask) << 2);
        char16_t hs_top4 = static_cast<char16_t>((c1 & 0x0f) << 8);
        char16_t hs_base = (hs_bottom2 | hs_middle6 | hs_top4) - 0x40u;

        char16_t ls_modifier = 0xdc00;
        char16_t hs_modifier = 0xd800;

        dest.emplace_back(hs_modifier + hs_base);
        dest.emplace_back(ls_modifier + ls_base);
      }


      // TODO: Find a suitable way to hide this implementation detail from includers
      template <typename T, typename U = typename std::iterator_traits<T>::value_type, bool Is16Bit = sizeof(U) == 2>
      struct UTF16Encoder;


      // TODO: Find a suitable way to hide this implementation detail from includers
      template <typename T, typename U> struct UTF16Encoder<T, U, true> {
        static UTF16Encoded encode(T begin, T end) {
          // Handle degenerate iterators first
          if (begin == end || !begin || !end) {
            return UTF16Encoded(1, 0x0000);
          }

          bool needsTerminatingZeroAdded = *(end - 1) != 0x0000;

          // distance is usually unsigned, vector::size_type is usually signed
          UTF16Encoded::size_type numberOfCodeUnits = static_cast<UTF16Encoded::size_type>(std::distance(begin, end));
          UTF16Encoded result(numberOfCodeUnits + (needsTerminatingZeroAdded ? 1 : 0));

          std::transform(begin, end, std::begin(result), [](U c) {
            return static_cast<char16_t>(c);
          });

          return result;
        }
      };


      // TODO: Find a suitable way to hide this implementation detail from includers
      template <typename T, typename U> struct UTF16Encoder<T, U, false> {
        static UTF16Encoded encode(T begin, T end) {
          static_assert(sizeof(U) == 1, "Template constructed with wrong-size value type");

          // Handle degenerate iterators first
          if (begin == end || !begin || !end) {
            return UTF16Encoded(1, 0x0000);
          }

          bool needsTerminatingZeroAdded = *(end - 1) != 0x0000;

          UTF16Encoded result;
          // We have an upper bound on the possible size of the result
          UTF16Encoded::size_type numberOfCodeUnits = static_cast<UTF16Encoded::size_type>(std::distance(begin, end));
          result.reserve(numberOfCodeUnits + (needsTerminatingZeroAdded ? 1 : 0));

          for (auto it = begin; it != end; ++it) {
            unsigned char c = static_cast<unsigned char>(*it);

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

            unsigned char c2 = static_cast<unsigned char>(*next);

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

            unsigned char c3 = static_cast<unsigned char>(*next);

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

            unsigned char c4 = static_cast<unsigned char>(*next);
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
