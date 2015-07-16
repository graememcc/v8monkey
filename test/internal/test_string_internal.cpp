/*
// size_t, strlen
#include <cstring>

// std::move
#include <utility>

#include "v8.h"

// SMEncodedString
#include "utils/Encoding.h"

// TwoByteCmp
#include "utils/MiscUtils.h"

#include "utils/test.h"
#include "utils/V8MonkeyCommon.h"
#include "V8MonkeyTest.h"
// 
// 
// #define C(val) static_cast<char>(val)
// 
// 
// using namespace v8;
// using namespace V8Monkey;
// 
// 
// V8MONKEY_TEST(IntSMEncodedString001, "Empty string encoded correctly") {
//   char empty[] = "";
//   SMEncodedString s(empty);
// 
//   V8MONKEY_CHECK(s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(!s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString002, "Empty string encoding correct") {
//   char empty[] = "";
//   SMEncodedString s(empty);
// 
//   char* latin1Encoding = UTF8::Latin1Encode(empty, strlen(empty));
//   V8MONKEY_CHECK(strcmp(s.Latin1Encoding(), latin1Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Utf16Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] latin1Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString003, "Empty string with BOM encoded correctly") {
//   char empty[] = {C(0xef), C(0xbb), C(0xbf), 0x00};
//   SMEncodedString s(empty);
// 
//   V8MONKEY_CHECK(s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(!s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString004, "Empty string with BOM encoding correct") {
//   char empty[] = {C(0xef), C(0xbb), C(0xbf), 0x00};
//   SMEncodedString s(empty);
// 
//   char* latin1Encoding = UTF8::Latin1Encode(empty, strlen(empty));
//   V8MONKEY_CHECK(strcmp(s.Latin1Encoding(), latin1Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Utf16Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] latin1Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString005, "ASCII string encoded correctly") {
//   char ascii[] = "abc123";
//   SMEncodedString s(ascii);
// 
//   V8MONKEY_CHECK(s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(!s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString006, "ASCII string encoding correct") {
//   char ascii[] = "abc123";
//   SMEncodedString s(ascii);
// 
//   char* latin1Encoding = UTF8::Latin1Encode(ascii, strlen(ascii));
//   V8MONKEY_CHECK(strcmp(s.Latin1Encoding(), latin1Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Utf16Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] latin1Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString007, "ASCII string with BOM encoded correctly") {
//   char ascii[] = {C(0xef), C(0xbb), C(0xbf), 0x41, 0x42, 0x43, 0x00};
//   SMEncodedString s(ascii);
// 
//   V8MONKEY_CHECK(s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(!s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString008, "ASCII string with BOM encoding correct") {
//   char ascii[] = {C(0xef), C(0xbb), C(0xbf), 0x41, 0x42, 0x43, 0x00};
//   SMEncodedString s(ascii);
// 
//   char* latin1Encoding = UTF8::Latin1Encode(ascii, strlen(ascii));
//   V8MONKEY_CHECK(strcmp(s.Latin1Encoding(), latin1Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Utf16Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] latin1Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString009, "Latin1 string encoded correctly") {
//   char utf[] = {C(0xc3), C(0xa9), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(!s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString010, "Latin1 string encoding correct") {
//   char utf[] = {C(0xc3), C(0xa9), 0x00};
//   SMEncodedString s(utf);
// 
//   char* latin1Encoding = UTF8::Latin1Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(strcmp(s.Latin1Encoding(), latin1Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Utf16Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] latin1Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString011, "Latin1 string with BOM encoded correctly") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xc3), C(0xa9), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(!s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString012, "Latin1 string with BOM encoding correct") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xc3), C(0xa9), 0x00};
//   SMEncodedString s(utf);
// 
//   char* latin1Encoding = UTF8::Latin1Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(strcmp(s.Latin1Encoding(), latin1Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Utf16Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] latin1Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString013, "UTF-8 2-byte string encoded correctly") {
//   char utf[] = {C(0xd5), C(0xb6), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString014, "UTF-8 2-byte string encoding correct") {
//   char utf[] = {C(0xd5), C(0xb6), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString015, "UTF-8 2-byte string with BOM encoded correctly") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xd5), C(0xb6), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString016, "UTF-8 2-byte string with BOM encoding correct") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xd5), C(0xb6), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString017, "UTF-8 3-byte string encoded correctly") {
//   char utf[] = {C(0xe4), C(0xac), C(0x93), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString018, "UTF-8 3-byte string encoding correct") {
//   char utf[] = {C(0xe4), C(0xac), C(0x93), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString019, "UTF-8 3-byte string with BOM encoded correctly") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xe4), C(0xac), C(0x93), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString020, "UTF-8 3-byte string with BOM encoding correct") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xe4), C(0xac), C(0x93), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString021, "UTF-8 4-byte string encoded correctly") {
//   char utf[] = {C(0xf3), C(0x9e), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString022, "UTF-8 4-byte string encoding correct") {
//   char utf[] = {C(0xf3), C(0x9e), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString023, "UTF-8 4-byte string with BOM encoded correctly") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xf3), C(0x9e), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString024, "UTF-8 4-byte string with BOM encoding correct") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xf3), C(0x9e), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString025, "Invalid UTF-8 2-byte string encoded correctly") {
//   char utf[] = {C(0xc0), C(0xbf), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString026, "Invalid UTF-8 2-byte string encoding correct") {
//   char utf[] = {C(0xc0), C(0xbf), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString027, "Invalid UTF-8 2-byte string with BOM encoded correctly") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xc0), C(0xbf), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString028, "Invalid UTF-8 2-byte string with BOM encoding correct") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xc0), C(0xbf), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString029, "Invalid UTF-8 3-byte string encoded correctly") {
//   char utf[] = {C(0xe0), C(0x80), C(0x93), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString030, "Invalid UTF-8 3-byte string encoding correct") {
//   char utf[] = {C(0xe0), C(0x80), C(0x93), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString031, "Invalid UTF-8 3-byte string with BOM encoded correctly") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xe0), C(0x80), C(0x93), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString032, "Invalid UTF-8 3-byte string with BOM encoding correct") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xe0), C(0x80), C(0x93), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString033, "Invalid UTF-8 4-byte string encoded correctly (1)") {
//   char utf[] = {C(0xf0), C(0x81), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString034, "Invalid UTF-8 4-byte string encoding correct (1)") {
//   char utf[] = {C(0xf0), C(0x81), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString035, "Invalid UTF-8 4-byte string with BOM encoded correctly (1)") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xf0), C(0x81), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString036, "Invalid UTF-8 4-byte string with BOM encoding correct (1)") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xf0), C(0x81), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString037, "Invalid UTF-8 4-byte string encoded correctly (2)") {
//   char utf[] = {C(0xf4), C(0x91), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString038, "Invalid UTF-8 4-byte string encoding correct (2)") {
//   char utf[] = {C(0xf4), C(0x91), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString039, "Invalid UTF-8 4-byte string with BOM encoded correctly (2)") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xf4), C(0x91), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString040, "Invalid UTF-8 4-byte string with BOM encoding correct (2)") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xf4), C(0x91), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString041, "Invalid UTF-8 4-byte string encoded correctly (3)") {
//   char utf[] = {C(0xf5), C(0x91), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString042, "Invalid UTF-8 4-byte string encoding correct (3)") {
//   char utf[] = {C(0xf5), C(0x91), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString043, "Invalid UTF-8 4-byte string with BOM encoded correctly (3)") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xf5), C(0x91), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString044, "Invalid UTF-8 4-byte string with BOM encoding correct (3)") {
//   char utf[] = {C(0xef), C(0xbb), C(0xbf), C(0xf5), C(0x91), C(0xa5), C(0xb0), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString045, "UTF-8 encoded low surrogate string encoded correctly") {
//   char utf[] = {C(0xed), C(0xb0), C(0xbf), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString046, "UTF-8 encoded low surrogate string encoding correct") {
//   char utf[] = {C(0xed), C(0xb0), C(0xbf), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString047, "UTF-8 encoded high surrogate string encoded correctly") {
//   char utf[] = {C(0xed), C(0x81), C(0xbf), 0x00};
//   SMEncodedString s(utf);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(s.IsUtf16Encoded(), "Correct encoding chosen");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString048, "UTF-8 encoded high surrogate string encoding correct") {
//   char utf[] = {C(0xed), C(0x81), C(0xbf), 0x00};
//   SMEncodedString s(utf);
// 
//   char16_t* utf16Encoding = UTF8::Utf16Encode(utf, strlen(utf));
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf16Encoding) == 0, "Encoded correctly");
//   V8MONKEY_CHECK(s.Latin1Encoding() == nullptr, "Other encoding returns nullptr");
// 
//   delete[] utf16Encoding;
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString049, "String to be encoded does not require zero termination (1)") {
//   char utf[] = {0x41};
//   SMEncodedString s(utf, 0);
// 
//   V8MONKEY_CHECK(s.IsLatin1Encoded() && !s.IsUtf16Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(strcmp(s.Latin1Encoding(), "") == 0, "Encoded correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString050, "String to be encoded does not require zero termination (2)") {
//   char utf[] = {0x41};
//   SMEncodedString s(utf, 1);
// 
//   V8MONKEY_CHECK(s.IsLatin1Encoded() && !s.IsUtf16Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(strcmp(s.Latin1Encoding(), "A") == 0, "Encoded correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString051, "String to be encoded does not require zero termination (3)") {
//   char utf[] = {C(0xc3), C(0xa9)};
//   char expected[] = {C(0xe9), 0x00};
//   SMEncodedString s(utf, 2);
// 
//   V8MONKEY_CHECK(s.IsLatin1Encoded() && !s.IsUtf16Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(strcmp(s.Latin1Encoding(), expected) == 0, "Encoded correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString052, "String to be encoded does not require zero termination (4)") {
//   char utf[] = {C(0xd5), C(0xb6)};
//   char16_t expected[] = {0x0576, 0x0000};
//   SMEncodedString s(utf, 2);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded() && s.IsUtf16Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), expected) == 0, "Encoded correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString053, "String to be encoded does not require zero termination (5)") {
//   char utf[] = {C(0xe4), C(0xac), C(0x93)};
//   char16_t expected[] = {0x4b13, 0x0000};
//   SMEncodedString s(utf, 3);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded() && s.IsUtf16Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), expected) == 0, "Encoded correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString054, "String to be encoded does not require zero termination (6)") {
//   char utf[] = {C(0xf3), C(0x9e), C(0xa5), C(0xb0)};
//   char16_t expected[] = {0xdb3a, 0xdd70, 0x0000};
//   SMEncodedString s(utf, 4);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded() && s.IsUtf16Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), expected) == 0, "Encoded correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString055, "String to be encoded does not require zero termination (7)") {
//   char utf[] = {C(0xd5), C(0xb6)};
//   char16_t expected[] = {0xfffd, 0x0000};
//   SMEncodedString s(utf, 1);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded() && s.IsUtf16Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), expected) == 0, "Encoded correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString056, "String to be encoded does not require zero termination (8)") {
//   char utf[] = {C(0xe4), C(0xac), C(0x93)};
//   char16_t expected[] = {0xfffd, 0xfffd, 0x0000};
//   SMEncodedString s(utf, 2);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded() && s.IsUtf16Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), expected) == 0, "Encoded correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString057, "String to be encoded does not require zero termination (9)") {
//   char utf[] = {C(0xf3), C(0x9e), C(0xa5), C(0xb0)};
//   char16_t expected[] = {0xfffd, 0xfffd, 0xfffd, 0x0000};
//   SMEncodedString s(utf, 3);
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded() && s.IsUtf16Encoded(), "Correct encoding chosen");
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), expected) == 0, "Encoded correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString058, "ASCII that doesn't require re-encoding is copied (1)") {
//   char empty[] = "";
//   SMEncodedString s(empty, 0);
// 
//   V8MONKEY_CHECK(s.Latin1Encoding() != empty, "String data copied");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString059, "ASCII that doesn't require re-encoding is copied (2)") {
//   char ascii[] = "abc";
//   SMEncodedString s(ascii, 0);
// 
//   V8MONKEY_CHECK(s.Latin1Encoding() != ascii, "String data copied");
//   ascii[0] = 'd';
//   V8MONKEY_CHECK(strcmp(ascii, s.Latin1Encoding()) != 0, "String data copied");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString060, "UTF-16 data copied") {
//   uint16_t utf[] = {0xdb3a, 0xdd70, 0x0000};
//   SMEncodedString s(utf, MiscUtils::TwoByteLength(utf));
//   utf[0] = 0xabcd;
// 
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s.Utf16Encoding(), utf) != 0, "Data copied");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString061, "UTF-16 data not re-encoded (1)") {
//   uint16_t utf[] = {0x0000};
//   SMEncodedString s(utf, MiscUtils::TwoByteLength(utf));
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded() && s.IsUtf16Encoded(), "Data not re-encoded");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString062, "UTF-16 data not re-encoded (2)") {
//   uint16_t utf[] = {0x0041, 0x0000};
//   SMEncodedString s(utf, MiscUtils::TwoByteLength(utf));
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded() && s.IsUtf16Encoded(), "Data not re-encoded");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString063, "UTF-16 data not re-encoded (3)") {
//   uint16_t utf[] = {0x00e9, 0x0000};
//   SMEncodedString s(utf, MiscUtils::TwoByteLength(utf));
// 
//   V8MONKEY_CHECK(!s.IsLatin1Encoded() && s.IsUtf16Encoded(), "Data not re-encoded");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString064, "Moving works as expected (1)") {
//   char ascii[] = "abc";
// 
//   SMEncodedString t(ascii);
//   const char* s1 = t.Latin1Encoding();
//   SMEncodedString s = std::move(t);
//   const char* s2 = s.Latin1Encoding();
// 
//   V8MONKEY_CHECK(s1 == s2, "Move worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString065, "Moving works as expected (2)") {
//   char ascii1[] = "abc";
//   char ascii2[] = "def";
// 
//   SMEncodedString s(ascii1);
//   SMEncodedString t(ascii2);
// 
//   const char* s1 = t.Latin1Encoding();
//   s = std::move(t);
//   const char* s2 = s.Latin1Encoding();
// 
//   V8MONKEY_CHECK(s1 == s2, "Move worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString066, "Moving works as expected (3)") {
//   uint16_t utf[] = {0x0104, 0x0000};
// 
//   SMEncodedString t(utf);
//   const char16_t* s1 = t.Utf16Encoding();
//   SMEncodedString s = std::move(t);
//   const char16_t* s2 = s.Utf16Encoding();
// 
//   V8MONKEY_CHECK(s1 == s2, "Move worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString067, "Moving works as expected (4)") {
//   uint16_t utf1[] = {0x0104, 0x0000};
//   uint16_t utf2[] = {0x020a, 0x0000};
// 
//   SMEncodedString s(utf1);
//   SMEncodedString t(utf2);
// 
//   const char16_t* s1 = t.Utf16Encoding();
//   s = std::move(t);
//   const char16_t* s2 = s.Utf16Encoding();
// 
//   V8MONKEY_CHECK(s1 == s2, "Move worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString068, "Moving works as expected (5)") {
//   char ascii1[] = "abc";
//   char ascii2[] = "def";
// 
//   SMEncodedString s(ascii1);
// 
//   {
//     SMEncodedString t(ascii2);
//     s = std::move(t);
//   }
// 
//   // In test builds, t's destruction would have poisoned character 0
//   V8MONKEY_CHECK(strcmp(ascii2, s.Latin1Encoding()) == 0, "Move worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString069, "Moving works as expected (6)") {
//   uint16_t utf1[] = {0x0104, 0x0000};
//   uint16_t utf2[] = {0x020a, 0x0000};
// 
//   SMEncodedString s(utf1);
//   SMEncodedString t(utf2);
// 
//   {
//     SMEncodedString t(utf2);
//     s = std::move(t);
//   }
// 
//   // In test builds, t's destruction would have poisoned character 0
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(utf2, s.Utf16Encoding()) == 0, "Move worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString070, "Moving works as expected (7)") {
//   char ascii1[] = "abc";
// 
//   SMEncodedString s(ascii1);
//   const char* s1 = s.Latin1Encoding();
// 
//   const char c = s1[0];
//   s = std::move(s);
//   const char* s2 = s.Latin1Encoding();
//   const char d = s2[0];
// 
//   // In test builds, move assignment would have poisoned character 0
//   V8MONKEY_CHECK(c == d, "Move worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString071, "Moving works as expected (8)") {
//   uint16_t utf[] = {0x0104, 0x0000};
// 
//   SMEncodedString s(utf);
//   const char16_t* s1 = s.Utf16Encoding();
// 
//   const char16_t c = s1[0];
//   s = std::move(s);
//   const char16_t* s2 = s.Utf16Encoding();
//   const char16_t d = s2[0];
// 
//   // In test builds, move assignment would have poisoned char16_tacter 0
//   V8MONKEY_CHECK(c == d, "Move worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString072, "Copying works as expected (1)") {
//   char ascii[] = "abc";
// 
//   SMEncodedString t(ascii);
//   const char* s1 = t.Latin1Encoding();
//   SMEncodedString s = t;
//   const char* s2 = s.Latin1Encoding();
// 
//   V8MONKEY_CHECK(s1 != s2, "Copy worked correctly");
//   V8MONKEY_CHECK(strcmp(s1, s2) == 0, "Buffers correct");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString073, "Copying works as expected (2)") {
//   char ascii1[] = "abc";
//   char ascii2[] = "def";
// 
//   SMEncodedString s(ascii1);
//   SMEncodedString t(ascii2);
// 
//   const char* s1 = t.Latin1Encoding();
//   s = t;
//   const char* s2 = s.Latin1Encoding();
// 
//   V8MONKEY_CHECK(s1 != s2, "Copy worked correctly");
//   V8MONKEY_CHECK(strcmp(s1, s2) == 0, "Buffers correct");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString074, "Copying works as expected (3)") {
//   uint16_t utf[] = {0x0104, 0x0000};
// 
//   SMEncodedString t(utf);
//   const char16_t* s1 = t.Utf16Encoding();
//   SMEncodedString s = t;
//   const char16_t* s2 = s.Utf16Encoding();
// 
//   V8MONKEY_CHECK(s1 != s2, "Copy worked correctly");
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s1, s2) == 0, "Buffers correct");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString075, "Copying works as expected (4)") {
//   uint16_t utf1[] = {0x0104, 0x0000};
//   uint16_t utf2[] = {0x020a, 0x0000};
// 
//   SMEncodedString s(utf1);
//   SMEncodedString t(utf2);
// 
//   const char16_t* s1 = t.Utf16Encoding();
//   s = t;
//   const char16_t* s2 = s.Utf16Encoding();
// 
//   V8MONKEY_CHECK(s1 != s2, "Copy worked correctly");
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(s1, s2) == 0, "Buffers correct");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString076, "Copying works as expected (5)") {
//   char ascii1[] = "abc";
//   char ascii2[] = "def";
// 
//   SMEncodedString s(ascii1);
// 
//   {
//     SMEncodedString t(ascii2);
//     s = t;
//   }
// 
//   // In test builds, t's destruction would have poisoned character 0
//   V8MONKEY_CHECK(strcmp(ascii2, s.Latin1Encoding()) == 0, "Copy worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString077, "Copying works as expected (6)") {
//   uint16_t utf1[] = {0x0104, 0x0000};
//   uint16_t utf2[] = {0x020a, 0x0000};
// 
//   SMEncodedString s(utf1);
//   SMEncodedString t(utf2);
// 
//   {
//     SMEncodedString t(utf2);
//     s = t;
//   }
// 
//   // In test builds, t's destruction would have poisoned character 0
//   V8MONKEY_CHECK(MiscUtils::TwoByteCmp(utf2, s.Utf16Encoding()) == 0, "Copy worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString078, "Copying works as expected (7)") {
//   char ascii1[] = "abc";
// 
//   SMEncodedString s(ascii1);
//   const char* s1 = s.Latin1Encoding();
// 
//   const char c = s1[0];
//   s = s;
//   const char* s2 = s.Latin1Encoding();
//   const char d = s2[0];
// 
//   // In test builds, copy assignment would have poisoned character 0
//   V8MONKEY_CHECK(c == d, "Copy worked correctly");
// }
// 
// 
// V8MONKEY_TEST(IntSMEncodedString079, "Copying works as expected (8)") {
//   uint16_t utf[] = {0x0104, 0x0000};
// 
//   SMEncodedString s(utf);
//   const char16_t* s1 = s.Utf16Encoding();
// 
//   const char16_t c = s1[0];
//   s = s;
//   const char16_t* s2 = s.Utf16Encoding();
//   const char16_t d = s2[0];
// 
//   // In test builds, copy assignment would have poisoned char16_tacter 0
//   V8MONKEY_CHECK(c == d, "Copy worked correctly");
// }
*/
