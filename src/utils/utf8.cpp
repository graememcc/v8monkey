/*
// ptrdiff_t
#include <cstddef>

// size_t
#include <cstdlib>

// std::logic_error
#include <stdexcept>

// UTF8 definition
#include "utils/Encoding.h"

namespace {
  enum ByteClassification {ASCII, LATIN1, TWOBYTE, THREEBYTE, FOURBYTE, INVALID};

  // The following arrays should be kept in sync with the enum above

  // On finding a value with the given classification, how many characters forward should we move to get to the next
  // character?
  int offsetForClassification[] = {1, 2, 2, 3, 4, 1};

  // How many char16_ts do we require to store a value of the given classification?
  size_t utf16CountForClassification[] = {1, 1, 1, 1, 2, 1};
*/


  /*
   * Consumes one or two bytes at the beginning of the the input string, and returns a value denoting the
   * representability of the character in Latin1:
   *   ASCII     - the code unit corresponds to plain ASCII (i.e. is in the range 0x00 - 0x7f)
   *   LATIN1    - the code unit corresponds to a Latin1 code unit in the range 0x80 - 0xff
   *   TWOBYTE   - the code unit corresponds to a Unicode code unit in the range 0x100 - 0x7ff
   *   THREEBYTE - the code unit corresponds to a Unicode code unit in the range 0x800 - 0xffff
   *   FOURBYTE  - the code unit corresponds to a Unicode code unit in the range 0x10000 - 0x10ffff
   *   INVALID   - the UTF-8 code unit is invalid (it is followed by the wrong number of continuation bytes,
   *               the continuation bytes are missing, or the value corresponds to high/low surrogates)
   *
   */

/*
  ByteClassification classifyCodeUnit(const char* s, const char* end) {
    ptrdiff_t limit = end - s;

    unsigned char c = s[0];

    if (!(c & 0x80)) {
      return ASCII;
    }

    if ((c & 0xc0) != 0xc0 || !(c & 0x3e) || limit < 2) {
      return INVALID;
    }

    unsigned char c2 = s[1];

    if ((c2 & 0xc0) != 0x80) {
      return INVALID;
    }

    if (!(c & 0x20)) {
      return (c & 0x1c) ? TWOBYTE : LATIN1;
    }

    if ((c == 0xe0 && !(c2 & 0x20)) || (c == 0xed && !(c2 & 0x40)) || limit < 3) {
      return INVALID;
    }

    unsigned char c3 = s[2];

    if ((c3 & 0xc0) != 0x80) {
      return INVALID;
    }

    if (!(c & 0x10)) {
      return THREEBYTE;
    }

    if (c > 0xf4 || limit < 4) {
      return INVALID;
    }

    unsigned char c4 = s[3];

    if ((c4 & 0xc0) != 0x80) {
      return INVALID;
    }

    if (!(c & 0x0F) && !(c2 & 0x70)) {
      return INVALID;
    }

    if ((c & 0x4) && (c2 & 0x70)) {
      return INVALID;
    }

    return FOURBYTE;
  }
*/


  /*
   * Given the two bytes of a UTF-8 code unit, return the equivalent Latin1 code unit. The caller is assumed
   * to have checked that the given code unit can be translated to Latin1
   *
   */

/*
  char Utf8ToLatin1(const char first, const char second) {
    char result = second & 0x3f;
    result |= static_cast<char>((first & 0x3) << 6);
    return result;
  }
*/


  /*
   * Returns true if the UTF-8 BOM is present. The caller must have verified that the length of the string from the
   * supplied pointer is greater than or equal to 3.
   *
   */

/*
  bool hasBOM(const char* s) {
    char bom1 = static_cast<char>(0xef);
    char bom2 = static_cast<char>(0xbb);
    char bom3 = static_cast<char>(0xbf);

    return s[0] == bom1 && s[1] == bom2 && s[2] == bom3;
  }
*/


  /*
   * Given an address and a classification, produce the non-surrogate UTF-16 code point for the UTF-8 code units at the
   * address. The caller is assumed to have checked that the buffer contains sufficient continuation bytes for the
   * type.
   *
   */

/*
  char16_t utf8ToUtf16(const char* s, ByteClassification classification) {
    char16_t top, middle, bottom;

    switch (classification) {
      case ASCII:
        return s[0];
        break;

      case LATIN1:
      case TWOBYTE:
        top = static_cast<char16_t>((s[0] & 0x1f) << 6);
        bottom = static_cast<char16_t>(s[1] & 0x3f);
        return top | bottom;
        break;

      case THREEBYTE:
        top = static_cast<char16_t>((s[0] & 0x0f) << 12);
        middle = static_cast<char16_t>((s[1] & 0x3f) << 6);
        bottom = static_cast<char16_t>(s[2] & 0x3f);
        return top | middle | bottom;
        break;

      case FOURBYTE:
      case INVALID:
      default:
        return 0xfffd;
    }
  }
*/


  /*
   * Given a source address and a destination, produce the UTF-16 surrogate pair for the 4-byte UTF-8 code point at the
   * source address. The caller is assumed to have checked that the buffer contains sufficient continuation bytes.
   *
   */

/*
  void utf8toSurrogatePair(const char* s, char16_t* dest) {
    char16_t ls_bottom6 = static_cast<char16_t>(s[3] & 0x3f);
    char16_t ls_top4 = static_cast<char16_t>((s[2] & 0x0f) << 6);
    char16_t ls_base = ls_bottom6 | ls_top4;

    char16_t hs_bottom2 = static_cast<char16_t>((s[2] & 0x30) >> 4);
    char16_t hs_middle6 = static_cast<char16_t>((s[1] & 0x3f) << 2);
    char16_t hs_top4 = static_cast<char16_t>((s[0] & 0x0f) << 8);
    char16_t hs_base = static_cast<char16_t>((hs_bottom2 | hs_middle6  | hs_top4) - 0x40);

    char16_t ls_modifier = 0xdc00;
    char16_t hs_modifier = 0xd800;

    dest[0] = static_cast<char16_t>(hs_modifier + hs_base);
    dest[1] = static_cast<char16_t>(ls_modifier + ls_base);
  }
}


namespace v8 {
  namespace V8Monkey {
    bool UTF8::NeedsEncodingForSpiderMonkey(const char* s, size_t byteLength) {
      // TODO: Optimize

      size_t offset = 0;

      // Skip BOM if present
      if (byteLength >= 3 && hasBOM(s)) {
        offset = 3;
      }

      for(size_t i = offset; i < byteLength; i++) {
        if (s[i] & 0x80) {
          return true;
        }
      }

      return false;
    }


    bool UTF8::CanEncodeToLatin1(const char* s, size_t byteLength) {
      // TODO: Optimize

      const char* end = s + byteLength;
      size_t i = 0;

      // Skip BOM if present
      if (byteLength >= 3 && hasBOM(s)) {
        i = 3;
      }

      while (i < byteLength) {
        ByteClassification classification = classifyCodeUnit(s + i, end);
        if (classification != ASCII && classification != LATIN1) {
          return false;
        }

        i += offsetForClassification[classification];
      }

      return true;
    }


    char* UTF8::Latin1Encode(const char* s, size_t byteLength) {
      // TODO: Optimize
      // First, we will calculate how many bytes we need to request from the free store. Along the way, we will throw
      // if we encounter bogus values: we should have checked that the string was encodable before this method was
      // called

      const char* end = s + byteLength;
      size_t i = 0;
      bool hadBOM = false;

      // Skip BOM if present
      if (byteLength >= 3) {
        hadBOM = hasBOM(s);

        if (hadBOM) {
          i = 3;
        }
      }

      size_t byteCount = 0;

      while (i < byteLength) {
        ByteClassification classification = classifyCodeUnit(s + i, end);

        if (classification != ASCII && classification != LATIN1) {
          throw std::logic_error("Latin1Encode called with unencodable data");
        }

        byteCount += 1;
        i += offsetForClassification[classification];
      }

      char* encoded = new char[byteCount + 1];
      i = 0;
      size_t j = hadBOM ? 3 : 0;

      while (j < byteLength) {
        ByteClassification classification = classifyCodeUnit(s + j, end);

        if (classification == ASCII) {
          encoded[i++] = s[j++];
        } else {
          char first = s[j++];
          char second = s[j++];
          encoded[i++] = Utf8ToLatin1(first, second);
        }
      }

      encoded[byteCount] = 0x00;
      return encoded;
    }


    char16_t* UTF8::Utf16Encode(const char* s, size_t byteLength) {
      const char* end = s + byteLength;
      size_t i = 0;
      bool hadBOM = false;

      // Skip BOM if present
      if (byteLength >= 3) {
        hadBOM = hasBOM(s);

        if (hadBOM) {
          i = 3;
        }
      }

      // First, calculate storage requirements
      size_t char16Count = 0;

      while (i < byteLength) {
        ByteClassification classification = classifyCodeUnit(s + i, end);

        char16Count += utf16CountForClassification[classification];
        i += offsetForClassification[classification];
      }

      char16_t* result = new char16_t[char16Count + 1];

      i = 0;
      size_t j = hadBOM ? 3 : 0;

      while (j < byteLength) {
        ByteClassification classification = classifyCodeUnit(s + j, end);

        if (classification != FOURBYTE) {
          result[i++] = utf8ToUtf16(s + j, classification);
        } else {
          utf8toSurrogatePair(s + j, result + i);
          i += 2;
        }

        j += offsetForClassification[classification];
      }

      result[i] = 0x0000;
      return result;
    }
  }
}
*/
