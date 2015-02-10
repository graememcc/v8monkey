// Ensure we are using correct type definitions
#include "v8stdint.h"

// The class under test
#include "utils/MiscUtils.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using v8::V8Monkey::MiscUtils::TwoByteLength;


V8MONKEY_TEST(IntTwoByteLength001, "Empty string has length 0") {
  const uint16_t data[] {0x0000};
  V8MONKEY_CHECK(TwoByteLength(&data[0]) == 0, "Length is correct");
}


V8MONKEY_TEST(IntTwoByteLength002, "Non-empty string has correct length (1)") {
  const uint16_t data[] {0x0041, 0x0000};
  V8MONKEY_CHECK(TwoByteLength(&data[0]) == 1, "Length is correct");
}


V8MONKEY_TEST(IntTwoByteLength003, "Non-empty string has correct length (2)") {
  const uint16_t data[] {0x0041, 0x0060, 0x0000};
  V8MONKEY_CHECK(TwoByteLength(&data[0]) == 2, "Length is correct");
}


V8MONKEY_TEST(IntTwoByteLength004, "Non-empty string has correct length (3)") {
  const uint16_t data[] {0x0041, 0x0060, 0x0000, 0x0000};
  V8MONKEY_CHECK(TwoByteLength(&data[0]) == 2, "Length is correct");
}
