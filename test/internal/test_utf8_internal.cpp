// std::equal, std::transform
#include <algorithm>

// initializer_list
#include <initializer_list>

// std::begin, std::end, std::distance
#include <iterator>

// std::vector
#include <vector>

// UTF8 class
#include "utils/Encoding.h"

// Int type definitions
#include "v8stdint.h"

// Unit-testing support
#include "V8MonkeyTest.h"


namespace {
  // Key for below:
  // CB1 = First continuation code-unit, CB2 = Second continuation code-unit, CB3 = Third continuation code-unit
  // V = Valid, M = Missing I = Invalid, 2=Is 2-byte starting byte, 3=Is 3 byte-starting byte, 4=Is 4 byte starting byte

  // Note: this array contains erroneous input where the subsequent bytes themseles cannot form an acceptable sequence
  const std::initializer_list<char> invalidUTF8[] =
    {{'\xbf', '\xbf', '\x00'},                  // Continuation code-units only
     {'\xc2', '\x00'},                          // 2 code-units: CB1: M
     {'\xc3', '\x20', '\x00'},                  // 2 code-units: CB1: I
     {'\xc4', '\xd3', '\x00'},                  // 2 code-units: CB1: 2
     {'\xc5', '\xe0', '\x00'},                  // 2 code-units: CB1: 3
     {'\xc6', '\xf0', '\x00'},                  // 2 code-units: CB1: 4
     {'\xe0', '\x00'},                          // 3 code-units: CB1: M, CB2: M
     {'\xe0', '\xbf', '\x00'},                  // 3 code-units: CB1: V, CB2: M
     {'\xe1', '\xbf', '\x20', '\x00'},          // 3 code-units: CB1: V, CB2: I
     {'\xe2', '\xbf', '\xd6', '\x00'},          // 3 code-units: CB1: V, CB2: 2
     {'\xe3', '\xbf', '\xe2', '\x00'},          // 3 code-units: CB1: V, CB2: 3
     {'\xe4', '\xbf', '\xf1', '\x00'},          // 3 code-units: CB1: V, CB2: 4
     {'\xe0', '\x24', '\x00'},                  // 3 code-units: CB1: I, CB2: M
     {'\xe5', '\x20', '\x25', '\x00'},          // 3 code-units: CB1: I, CB2: I
     {'\xe6', '\x31', '\xd4', '\x00'},          // 3 code-units: CB1: I, CB2: 2
     {'\xe7', '\x41', '\xe8', '\x00'},          // 3 code-units: CB1: I, CB2: 3
     {'\xea', '\x59', '\xf2', '\x00'},          // 3 code-units: CB1: I, CB2: 4
     {'\xe0', '\xc2', '\x00'},                  // 3 code-units: CB1: 2, CB2: M
     {'\xe5', '\xc3', '\x46', '\x00'},          // 3 code-units: CB1: 2, CB2: I
     {'\xe6', '\xc4', '\xd2', '\x00'},          // 3 code-units: CB1: 2, CB2: 2
     {'\xe7', '\xdd', '\xea', '\x00'},          // 3 code-units: CB1: 2, CB2: 3
     {'\xea', '\xdf', '\xf3', '\x00'},          // 3 code-units: CB1: 2, CB2: 4
     {'\xe0', '\xe2', '\x00'},                  // 3 code-units: CB1: 3, CB2: M
     {'\xe5', '\xe3', '\x48', '\x00'},          // 3 code-units: CB1: 3, CB2: I
     {'\xe6', '\xc4', '\xd0', '\x00'},          // 3 code-units: CB1: 3, CB2: 2
     {'\xe7', '\xdd', '\xeb', '\x00'},          // 3 code-units: CB1: 3, CB2: 3
     {'\xea', '\xdf', '\xf4', '\x00'},          // 3 code-units: CB1: 3, CB2: 4
     {'\xe0', '\xf1', '\x00'},                  // 3 code-units: CB1: 4, CB2: M
     {'\xe8', '\xf2', '\x48', '\x00'},          // 3 code-units: CB1: 4, CB2: I
     {'\xe9', '\xf3', '\xd0', '\x00'},          // 3 code-units: CB1: 4, CB2: 2
     {'\xeb', '\xf4', '\xeb', '\x00'},          // 3 code-units: CB1: 4, CB2: 3
     {'\xec', '\xf0', '\xf0', '\x00'},          // 3 code-units: CB1: 4, CB2: 4
     {'\xf0', '\x00'},                          // 4 code-units: CB1: M, CB2: M, CB3: M
     {'\xf1', '\xbf', '\x00'},                  // 4 code-units: CB1: V, CB2: M, CB3: M
     {'\xf2', '\x20', '\x00'},                  // 4 code-units: CB1: I, CB2: M, CB3: M
     {'\xf3', '\xd3', '\x00'},                  // 4 code-units: CB1: 2, CB2: M, CB3: M
     {'\xf4', '\xe0', '\x00'},                  // 4 code-units: CB1: 3, CB2: M, CB3: M
     {'\xf0', '\xf1', '\x00'},                  // 4 code-units: CB1: 4, CB2: M, CB3: M
     {'\xf1', '\xbf', '\xbf', '\x00'},          // 4 code-units: CB1: V, CB2: V, CB3: M
     {'\xf2', '\xbf', '\xbf', '\x21', '\x00'},  // 4 code-units: CB1: V, CB2: V, CB3: I
     {'\xf3', '\xbf', '\xbf', '\xd0', '\x00'},  // 4 code-units: CB1: V, CB2: V, CB3: 2
     {'\xf4', '\x8f', '\xbf', '\xe4', '\x00'},  // 4 code-units: CB1: V, CB2: V, CB3: 3
     {'\xf0', '\xbf', '\xbf', '\xf0', '\x00'},  // 4 code-units: CB1: V, CB2: V, CB3: 4
     {'\xf1', '\xbf', '\x29', '\x00'},          // 4 code-units: CB1: V, CB2: I, CB3: M
     {'\xf2', '\xbf', '\x29', '\xbf', '\x00'},  // 4 code-units: CB1: V, CB2: I, CB3: V
     {'\xf3', '\xbf', '\x29', '\x22', '\x00'},  // 4 code-units: CB1: V, CB2: I, CB3: I
     {'\xf4', '\x8f', '\x29', '\xc4', '\x00'},  // 4 code-units: CB1: V, CB2: I, CB3: 2
     {'\xf0', '\xbf', '\x29', '\xed', '\x00'},  // 4 code-units: CB1: V, CB2: I, CB3: 3
     {'\xf1', '\xbf', '\x29', '\xf1', '\x00'},  // 4 code-units: CB1: V, CB2: I, CB3: 4
     {'\xf2', '\xbf', '\xc2', '\x00'},          // 4 code-units: CB1: V, CB2: 2, CB3: M
     {'\xf4', '\x8e', '\xc2', '\x31', '\x00'},  // 4 code-units: CB1: V, CB2: 2, CB3: I
     {'\xf0', '\xbf', '\xc2', '\xc3', '\x00'},  // 4 code-units: CB1: V, CB2: 2, CB3: 2
     {'\xf1', '\xbf', '\xc2', '\xe7', '\x00'},  // 4 code-units: CB1: V, CB2: 2, CB3: 3
     {'\xf2', '\xbf', '\xc2', '\xf2', '\x00'},  // 4 code-units: CB1: V, CB2: 2, CB3: 4
     {'\xf3', '\xbf', '\xe0', '\x00'},          // 4 code-units: CB1: V, CB2: 3, CB3: M
     {'\xf4', '\x8d', '\xe1', '\xbf', '\x00'},  // 4 code-units: CB1: V, CB2: 3, CB3: V
     {'\xf0', '\xbf', '\xe2', '\x32', '\x00'},  // 4 code-units: CB1: V, CB2: 3, CB3: I
     {'\xf1', '\xbf', '\xe3', '\xc4', '\x00'},  // 4 code-units: CB1: V, CB2: 3, CB3: 2
     {'\xf2', '\xbf', '\xe4', '\xe8', '\x00'},  // 4 code-units: CB1: V, CB2: 3, CB3: 3
     {'\xf3', '\xbf', '\xe5', '\xf3', '\x00'},  // 4 code-units: CB1: V, CB2: 3, CB3: 4
     {'\xf4', '\x8c', '\xf1', '\x00'},          // 4 code-units: CB1: V, CB2: 4, CB3: M
     {'\xf0', '\xbf', '\xf2', '\xbf', '\x00'},  // 4 code-units: CB1: V, CB2: 4, CB3: V
     {'\xf1', '\xbf', '\xf3', '\x33', '\x00'},  // 4 code-units: CB1: V, CB2: 4, CB3: I
     {'\xf2', '\xbf', '\xf4', '\xc5', '\x00'},  // 4 code-units: CB1: V, CB2: 4, CB3: 2
     {'\xf3', '\xbf', '\xf0', '\xe1', '\x00'},  // 4 code-units: CB1: V, CB2: 4, CB3: 3
     {'\xf4', '\x8b', '\xf1', '\xf4', '\x00'},  // 4 code-units: CB1: V, CB2: 4, CB3: 4
     {'\xf0', '\x20', '\xbf', '\x00'},          // 4 code-units: CB1: I, CB2: V, CB3: M
     {'\xf1', '\x21', '\xbf', '\x21', '\x00'},  // 4 code-units: CB1: I, CB2: V, CB3: I
     {'\xf2', '\x22', '\xbf', '\xd0', '\x00'},  // 4 code-units: CB1: I, CB2: V, CB3: 2
     {'\xf3', '\x23', '\xbf', '\xe4', '\x00'},  // 4 code-units: CB1: I, CB2: V, CB3: 3
     {'\xf4', '\x24', '\xbf', '\xf0', '\x00'},  // 4 code-units: CB1: I, CB2: V, CB3: 4
     {'\xf0', '\x25', '\x29', '\x00'},          // 4 code-units: CB1: I, CB2: I, CB3: M
     {'\xf1', '\x26', '\x29', '\xbf', '\x00'},  // 4 code-units: CB1: I, CB2: I, CB3: V
     {'\xf2', '\x27', '\x29', '\x22', '\x00'},  // 4 code-units: CB1: I, CB2: I, CB3: I
     {'\xf3', '\x28', '\x29', '\xc4', '\x00'},  // 4 code-units: CB1: I, CB2: I, CB3: 2
     {'\xf4', '\x29', '\x29', '\xed', '\x00'},  // 4 code-units: CB1: I, CB2: I, CB3: 3
     {'\xf0', '\x2a', '\x29', '\xf1', '\x00'},  // 4 code-units: CB1: I, CB2: I, CB3: 4
     {'\xf1', '\x2b', '\xc2', '\x00'},          // 4 code-units: CB1: I, CB2: 2, CB3: M
     {'\xf3', '\x2d', '\xc2', '\x31', '\x00'},  // 4 code-units: CB1: I, CB2: 2, CB3: I
     {'\xf4', '\x2e', '\xc2', '\xc3', '\x00'},  // 4 code-units: CB1: I, CB2: 2, CB3: 2
     {'\xf0', '\x2f', '\xc2', '\xe7', '\x00'},  // 4 code-units: CB1: I, CB2: 2, CB3: 3
     {'\xf1', '\x30', '\xc2', '\xf2', '\x00'},  // 4 code-units: CB1: I, CB2: 2, CB3: 4
     {'\xf2', '\x31', '\xe0', '\x00'},          // 4 code-units: CB1: I, CB2: 3, CB3: M
     {'\xf3', '\x32', '\xe1', '\xbf', '\x00'},  // 4 code-units: CB1: I, CB2: 3, CB3: V
     {'\xf4', '\x33', '\xe2', '\x32', '\x00'},  // 4 code-units: CB1: I, CB2: 3, CB3: I
     {'\xf0', '\x34', '\xe3', '\xc4', '\x00'},  // 4 code-units: CB1: I, CB2: 3, CB3: 2
     {'\xf1', '\x35', '\xe4', '\xe8', '\x00'},  // 4 code-units: CB1: I, CB2: 3, CB3: 3
     {'\xf2', '\x36', '\xe5', '\xf3', '\x00'},  // 4 code-units: CB1: I, CB2: 3, CB3: 4
     {'\xf3', '\x37', '\xf2', '\x00'},          // 4 code-units: CB1: I, CB2: 4, CB3: M
     {'\xf4', '\x38', '\xf3', '\xbf', '\x00'},  // 4 code-units: CB1: I, CB2: 4, CB3: V
     {'\xf0', '\x39', '\xf4', '\x33', '\x00'},  // 4 code-units: CB1: I, CB2: 4, CB3: I
     {'\xf1', '\x3a', '\xf0', '\xc5', '\x00'},  // 4 code-units: CB1: I, CB2: 4, CB3: 2
     {'\xf2', '\x3b', '\xf1', '\xe1', '\x00'},  // 4 code-units: CB1: I, CB2: 4, CB3: 3
     {'\xf3', '\x3c', '\xf2', '\xf4', '\x00'},  // 4 code-units: CB1: I, CB2: 4, CB3: 4
     {'\xf4', '\xc7', '\x29', '\x00'},          // 4 code-units: CB1: 2, CB2: I, CB3: M
     {'\xf0', '\xc8', '\x29', '\xbf', '\x00'},  // 4 code-units: CB1: 2, CB2: I, CB3: V
     {'\xf1', '\xc9', '\x29', '\x22', '\x00'},  // 4 code-units: CB1: 2, CB2: I, CB3: I
     {'\xf2', '\xca', '\x29', '\xc4', '\x00'},  // 4 code-units: CB1: 2, CB2: I, CB3: 2
     {'\xf3', '\xcb', '\x29', '\xed', '\x00'},  // 4 code-units: CB1: 2, CB2: I, CB3: 3
     {'\xf4', '\xcc', '\x29', '\xf1', '\x00'},  // 4 code-units: CB1: 2, CB2: I, CB3: 4
     {'\xf0', '\xcd', '\xc2', '\x00'},          // 4 code-units: CB1: 2, CB2: 2, CB3: M
     {'\xf2', '\xcf', '\xc2', '\x31', '\x00'},  // 4 code-units: CB1: 2, CB2: 2, CB3: I
     {'\xf3', '\xd0', '\xc2', '\xc3', '\x00'},  // 4 code-units: CB1: 2, CB2: 2, CB3: 2
     {'\xf4', '\xd1', '\xc2', '\xe7', '\x00'},  // 4 code-units: CB1: 2, CB2: 2, CB3: 3
     {'\xf0', '\xd2', '\xc2', '\xf2', '\x00'},  // 4 code-units: CB1: 2, CB2: 2, CB3: 4
     {'\xf1', '\xd3', '\xe0', '\x00'},          // 4 code-units: CB1: 2, CB2: 3, CB3: M
     {'\xf2', '\xd4', '\xe1', '\xbf', '\x00'},  // 4 code-units: CB1: 2, CB2: 3, CB3: V
     {'\xf3', '\xd5', '\xe2', '\x32', '\x00'},  // 4 code-units: CB1: 2, CB2: 3, CB3: I
     {'\xf4', '\xd6', '\xe3', '\xc4', '\x00'},  // 4 code-units: CB1: 2, CB2: 3, CB3: 2
     {'\xf0', '\xd7', '\xe4', '\xe8', '\x00'},  // 4 code-units: CB1: 2, CB2: 3, CB3: 3
     {'\xf1', '\xd8', '\xe5', '\xf3', '\x00'},  // 4 code-units: CB1: 2, CB2: 3, CB3: 4
     {'\xf2', '\xd9', '\xf3', '\x00'},          // 4 code-units: CB1: 2, CB2: 4, CB3: M
     {'\xf3', '\xda', '\xf4', '\xbf', '\x00'},  // 4 code-units: CB1: 2, CB2: 4, CB3: V
     {'\xf4', '\xdb', '\xf0', '\x33', '\x00'},  // 4 code-units: CB1: 2, CB2: 4, CB3: I
     {'\xf0', '\xdc', '\xf1', '\xc5', '\x00'},  // 4 code-units: CB1: 2, CB2: 4, CB3: 2
     {'\xf1', '\xdd', '\xf2', '\xe1', '\x00'},  // 4 code-units: CB1: 2, CB2: 4, CB3: 3
     {'\xf2', '\xde', '\xf3', '\xf4', '\x00'},  // 4 code-units: CB1: 2, CB2: 4, CB3: 4
     {'\xf3', '\xe0', '\xbf', '\x00'},          // 4 code-units: CB1: 3, CB2: V, CB3: M
     {'\xf4', '\xe1', '\xbf', '\x21', '\x00'},  // 4 code-units: CB1: 3, CB2: V, CB3: I
     {'\xf0', '\xe2', '\xbf', '\xd0', '\x00'},  // 4 code-units: CB1: 3, CB2: V, CB3: 2
     {'\xf1', '\xe3', '\xbf', '\xe4', '\x00'},  // 4 code-units: CB1: 3, CB2: V, CB3: 3
     {'\xf2', '\xe4', '\xbf', '\xf0', '\x00'},  // 4 code-units: CB1: 3, CB2: V, CB3: 4
     {'\xf3', '\xe5', '\x29', '\x00'},          // 4 code-units: CB1: 3, CB2: I, CB3: M
     {'\xf4', '\xe6', '\x29', '\xbf', '\x00'},  // 4 code-units: CB1: 3, CB2: I, CB3: V
     {'\xf0', '\xe7', '\x29', '\x22', '\x00'},  // 4 code-units: CB1: 3, CB2: I, CB3: I
     {'\xf1', '\xe8', '\x29', '\xc4', '\x00'},  // 4 code-units: CB1: 3, CB2: I, CB3: 2
     {'\xf2', '\xe9', '\x29', '\xed', '\x00'},  // 4 code-units: CB1: 3, CB2: I, CB3: 3
     {'\xf3', '\xea', '\x29', '\xf1', '\x00'},  // 4 code-units: CB1: 3, CB2: I, CB3: 4
     {'\xf4', '\xeb', '\xc2', '\x00'},          // 4 code-units: CB1: 3, CB2: 2, CB3: M
     {'\xf1', '\xed', '\xc2', '\x31', '\x00'},  // 4 code-units: CB1: 3, CB2: 2, CB3: I
     {'\xf2', '\xee', '\xc2', '\xc3', '\x00'},  // 4 code-units: CB1: 3, CB2: 2, CB3: 2
     {'\xf3', '\xef', '\xc2', '\xe7', '\x00'},  // 4 code-units: CB1: 3, CB2: 2, CB3: 3
     {'\xf4', '\xe0', '\xc2', '\xf2', '\x00'},  // 4 code-units: CB1: 3, CB2: 2, CB3: 4
     {'\xf0', '\xe1', '\xe0', '\x00'},          // 4 code-units: CB1: 3, CB2: 3, CB3: M
     {'\xf1', '\xe2', '\xe1', '\xbf', '\x00'},  // 4 code-units: CB1: 3, CB2: 3, CB3: V
     {'\xf2', '\xe3', '\xe2', '\x32', '\x00'},  // 4 code-units: CB1: 3, CB2: 3, CB3: I
     {'\xf3', '\xe4', '\xe3', '\xc4', '\x00'},  // 4 code-units: CB1: 3, CB2: 3, CB3: 2
     {'\xf4', '\xe5', '\xe4', '\xe8', '\x00'},  // 4 code-units: CB1: 3, CB2: 3, CB3: 3
     {'\xf0', '\xe6', '\xe5', '\xf3', '\x00'},  // 4 code-units: CB1: 3, CB2: 3, CB3: 4
     {'\xf1', '\xe7', '\xf4', '\x00'},          // 4 code-units: CB1: 3, CB2: 4, CB3: M
     {'\xf2', '\xe8', '\xf0', '\xbf', '\x00'},  // 4 code-units: CB1: 3, CB2: 4, CB3: V
     {'\xf3', '\xe9', '\xf1', '\x33', '\x00'},  // 4 code-units: CB1: 3, CB2: 4, CB3: I
     {'\xf4', '\xea', '\xf2', '\xc5', '\x00'},  // 4 code-units: CB1: 3, CB2: 4, CB3: 2
     {'\xf0', '\xeb', '\xf3', '\xe1', '\x00'},  // 4 code-units: CB1: 3, CB2: 4, CB3: 3
     {'\xf1', '\xec', '\xf4', '\xf4', '\x00'},  // 4 code-units: CB1: 3, CB2: 4, CB3: 4
     {'\xf2', '\xf2', '\xbf', '\x00'},          // 4 code-units: CB1: 4, CB2: V, CB3: M
     {'\xf3', '\xf3', '\xbf', '\x21', '\x00'},  // 4 code-units: CB1: 4, CB2: V, CB3: I
     {'\xf4', '\xf4', '\xbf', '\xd0', '\x00'},  // 4 code-units: CB1: 4, CB2: V, CB3: 2
     {'\xf0', '\xf0', '\xbf', '\xe4', '\x00'},  // 4 code-units: CB1: 4, CB2: V, CB3: 3
     {'\xf1', '\xf1', '\xbf', '\xf0', '\x00'},  // 4 code-units: CB1: 4, CB2: V, CB3: 4
     {'\xf2', '\xf2', '\x29', '\x00'},          // 4 code-units: CB1: 4, CB2: I, CB3: M
     {'\xf3', '\xf3', '\x29', '\xbf', '\x00'},  // 4 code-units: CB1: 4, CB2: I, CB3: V
     {'\xf4', '\xf4', '\x29', '\x22', '\x00'},  // 4 code-units: CB1: 4, CB2: I, CB3: I
     {'\xf0', '\xf0', '\x29', '\xc4', '\x00'},  // 4 code-units: CB1: 4, CB2: I, CB3: 2
     {'\xf1', '\xf1', '\x29', '\xed', '\x00'},  // 4 code-units: CB1: 4, CB2: I, CB3: 3
     {'\xf2', '\xf2', '\x29', '\xf1', '\x00'},  // 4 code-units: CB1: 4, CB2: I, CB3: 4
     {'\xf3', '\xf3', '\xc2', '\x00'},          // 4 code-units: CB1: 4, CB2: 2, CB3: M
     {'\xf0', '\xf0', '\xc2', '\x31', '\x00'},  // 4 code-units: CB1: 4, CB2: 2, CB3: I
     {'\xf1', '\xf1', '\xc2', '\xc3', '\x00'},  // 4 code-units: CB1: 4, CB2: 2, CB3: 2
     {'\xf2', '\xf2', '\xc2', '\xe7', '\x00'},  // 4 code-units: CB1: 4, CB2: 2, CB3: 3
     {'\xf3', '\xf3', '\xc2', '\xf2', '\x00'},  // 4 code-units: CB1: 4, CB2: 2, CB3: 4
     {'\xf4', '\xf4', '\xe0', '\x00'},          // 4 code-units: CB1: 4, CB2: 3, CB3: M
     {'\xf0', '\xf0', '\xe1', '\xbf', '\x00'},  // 4 code-units: CB1: 4, CB2: 3, CB3: V
     {'\xf1', '\xf1', '\xe2', '\x32', '\x00'},  // 4 code-units: CB1: 4, CB2: 3, CB3: I
     {'\xf2', '\xf2', '\xe3', '\xc4', '\x00'},  // 4 code-units: CB1: 4, CB2: 3, CB3: 2
     {'\xf3', '\xf3', '\xe4', '\xe8', '\x00'},  // 4 code-units: CB1: 4, CB2: 3, CB3: 3
     {'\xf4', '\xf4', '\xe5', '\xf3', '\x00'},  // 4 code-units: CB1: 4, CB2: 3, CB3: 4
     {'\xf0', '\xf0', '\xf0', '\x00'},          // 4 code-units: CB1: 4, CB2: 4, CB3: M
     {'\xf1', '\xf1', '\xf1', '\xbf', '\x00'},  // 4 code-units: CB1: 4, CB2: 4, CB3: V
     {'\xf2', '\xf2', '\xf2', '\x33', '\x00'},  // 4 code-units: CB1: 4, CB2: 4, CB3: I
     {'\xf3', '\xf3', '\xf3', '\xc5', '\x00'},  // 4 code-units: CB1: 4, CB2: 4, CB3: 2
     {'\xf4', '\xf4', '\xf4', '\xe1', '\x00'},  // 4 code-units: CB1: 4, CB2: 4, CB3: 3
     {'\xf0', '\xf0', '\xf0', '\xf4', '\x00'}}; // 4 code-units: CB1: 4, CB2: 4, CB3: 4


  // Bogus UTF-8 sequences whose subsequences may encode to valid UTF-8
  const std::initializer_list<char> encodableInvalidUTF8[] =
    {{'\xf3', '\xbf', '\xc2', '\xbf', '\x00'},  // 4 code-units: CB1: V, CB2: 2
     {'\xf2', '\x2c', '\xc2', '\xbf', '\x00'},  // 4 code-units: CB1: I, CB2: 2, CB3: V
     {'\xf4', '\xc2', '\xbf', '\x00'},          // 4 code-units: CB1: 2, CB2: V, CB3: M
     {'\xf0', '\xc3', '\xbf', '\x21', '\x00'},  // 4 code-units: CB1: 2, CB2: V, CB3: I
     {'\xf1', '\xc4', '\xbf', '\xd0', '\x00'},  // 4 code-units: CB1: 2, CB2: V, CB3: 2
     {'\xf2', '\xc5', '\xbf', '\xe4', '\x00'},  // 4 code-units: CB1: 2, CB2: V, CB3: 3
     {'\xf3', '\xc6', '\xbf', '\xf0', '\x00'},  // 4 code-units: CB1: 2, CB2: V, CB3: 4
     {'\xf1', '\xce', '\xc2', '\xbf', '\x00'},  // 4 code-units: CB1: 2, CB2: 2, CB3: V
     {'\xf4', '\xe1', '\xbf', '\xbf', '\x00'},  // 4 code-units: CB1: 3, CB2: V, CB3: V
     {'\xf0', '\xec', '\xc2', '\xbf', '\x00'},  // 4 code-units: CB1: 3, CB2: 2, CB3: V
     {'\xf4', '\xf4', '\xc2', '\xbf', '\x00'}}; // 4 code-units: CB1: 4, CB2: 2, CB3: V


  // This array must be kept in sync with encodableInvalidUTF8 above!
  const std::initializer_list<char16_t> encodableInvalidAsUTF16[] =
    {{0xfffd, 0xfffd, 0x00bf, 0x0000},  // 4 code-units: CB1: V, CB2: 2
     {0xfffd, 0x002c, 0x00bf, 0x0000},  // 4 code-units: CB1: I, CB2: 2, CB3: V
     {0xfffd, 0x00bf, 0x0000},          // 4 code-units: CB1: 2, CB2: V, CB3: M
     {0xfffd, 0x00ff, 0x0021, 0x0000},  // 4 code-units: CB1: 2, CB2: V, CB3: I
     {0xfffd, 0x013f, 0xfffd, 0x0000},  // 4 code-units: CB1: 2, CB2: V, CB3: 2
     {0xfffd, 0x017f, 0xfffd, 0x0000},  // 4 code-units: CB1: 2, CB2: V, CB3: 3
     {0xfffd, 0x01bf, 0xfffd, 0x0000},  // 4 code-units: CB1: 2, CB2: V, CB3: 4
     {0xfffd, 0xfffd, 0x00bf, 0x0000},  // 4 code-units: CB1: 2, CB2: 2, CB3: V
     {0xfffd, 0x1fff, 0x0000},          // 4 code-units: CB1: 3, CB2: V, CB3: V
     {0xfffd, 0xfffd, 0x00bf, 0x0000},  // 4 code-units: CB1: 3, CB2: 2, CB3: V
     {0xfffd, 0xfffd, 0x00bf, 0x0000}}; // 4 code-units: CB1: 4, CB2: 2, CB3: V
}


using namespace v8;
using namespace V8Monkey;


// XXX Need to research exactly what V8 does with 16-bit data, particularly invalid surrogates


V8MONKEY_TEST(IntUtf8_001, "Empty uint16_t array yields empty zero-terminated container") {
  const uint16_t input[] = {0x0000};
  const auto encoded = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(encoded.size() == 1, "Container of correct size returned");
  V8MONKEY_CHECK(encoded[0] == 0x0000, "Container zero-terminated");
}


V8MONKEY_TEST(IntUtf8_002, "Degenerate uint16_t iterators yield empty zero-terminated container") {
  const uint16_t input[] = {0x0000};
  const auto encoded = UTF8::EncodeToUTF16(std::begin(input), std::begin(input));

  V8MONKEY_CHECK(encoded.size() == 1, "Container of correct size returned");
  V8MONKEY_CHECK(encoded[0] == 0x0000, "Container zero-terminated");
}


V8MONKEY_TEST(IntUtf8_003, "16-bit data copied correctly") {
  const uint16_t input[] = {0x0061u, 0x00c8u, 0x01b0u, 0x08a0u, 0xd801u, 0xdc37u, 0x0000};
  const auto inputBegin = std::begin(input);
  const auto encoded = UTF8::EncodeToUTF16(inputBegin, std::end(input));

  V8MONKEY_CHECK(std::equal(std::begin(encoded), std::end(encoded), inputBegin), "16-bit data copied");
}


V8MONKEY_TEST(IntUtf8_004, "Copied 16-bit data is not aliased") {
  uint16_t input[] = {0x0061u, 0x0000};
  const auto inputBegin = std::begin(input);
  const auto encoded = UTF8::EncodeToUTF16(inputBegin, std::end(input));
  input[0] = 0x0062u;

  V8MONKEY_CHECK(!std::equal(std::begin(encoded), std::end(encoded), inputBegin), "16-bit data was copied");
}


V8MONKEY_TEST(IntUtf8_005, "Copied 16-bit data is zero-terminated if necessary") {
  const uint16_t input[] = {0x0061u};
  const auto inputBegin = std::begin(input);
  const auto inputEnd = std::end(input);
  const auto inputSize = static_cast<UTF8::UTF16Encoded::size_type>(std::distance(inputBegin, inputEnd));

  const auto encoded = UTF8::EncodeToUTF16(inputBegin, inputEnd);
  const auto length = encoded.size();

  V8MONKEY_CHECK(length == inputSize + 1, "Size correct");
  V8MONKEY_CHECK(std::equal(std::begin(encoded), std::end(encoded) - 1, inputBegin), "Character data was copied");
  V8MONKEY_CHECK(encoded[length - 1] == 0x0000, "Data was zero-terminated");
}


V8MONKEY_TEST(IntUtf8_006, "Data correctly copied when non-terminating zero code-unit present") {
  constexpr int inputSize = 4;
  const uint16_t input[inputSize] = {0x0061u, 0x0000, 0x0062u, 0x0000};
  const auto inputBegin = std::begin(input);

  const auto encoded = UTF8::EncodeToUTF16(inputBegin, std::end(input));
  const auto length = encoded.size();

  V8MONKEY_CHECK(length == inputSize, "Size correct");
  V8MONKEY_CHECK(std::equal(std::begin(encoded), std::end(encoded), inputBegin), "Character data was copied");
}


V8MONKEY_TEST(IntUtf8_007, "BOM copied correctly") {
  const uint16_t input[] = {0xfeffu, 0x0000};
  const auto inputBegin = std::begin(input);
  const auto encoded = UTF8::EncodeToUTF16(inputBegin, std::end(input));

  V8MONKEY_CHECK(std::equal(std::begin(encoded), std::end(encoded), inputBegin), "BOM was correctly copied");
}


V8MONKEY_TEST(IntUtf8_008, "Endian-ness reversed BOM (invalid character) copied correctly") {
  const uint16_t input[] = {0xfffeu, 0x0000};
  const auto inputBegin = std::begin(input);
  const auto encoded = UTF8::EncodeToUTF16(inputBegin, std::end(input));

  V8MONKEY_CHECK(std::equal(std::begin(encoded), std::end(encoded), inputBegin), "BOM was correctly copied");
}


V8MONKEY_TEST(IntUtf8_009, "Empty char array yields empty zero-terminated container") {
  const char input[] = {'\0'};
  const auto encoded = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(encoded.size() == 1, "Container of correct size returned");
  V8MONKEY_CHECK(encoded[0] == 0x0000, "Container zero-terminated");
}


V8MONKEY_TEST(IntUtf8_010, "Degenerate char iterators yield empty zero-terminated container") {
  const char input[] = {'\0'};
  const auto encoded = UTF8::EncodeToUTF16(std::begin(input), std::begin(input));

  V8MONKEY_CHECK(encoded.size() == 1, "Container of correct size returned");
  V8MONKEY_CHECK(encoded[0] == 0x0000, "Container zero-terminated");
}


V8MONKEY_TEST(IntUtf8_011, "ASCII encoded correctly") {
  const char input[] = {'\x61', '\x00'};
  const UTF8::UTF16Encoded expected = {0x0061u, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_012, "ASCII without terminating zero encoded correctly") {
  const char input[] = {'\x61'};
  const UTF8::UTF16Encoded expected = {0x0061u, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_013, "2-code-unit UTF-8 encoded correctly") {
  const char input[] = {'\xc3', '\xa9', '\x00'};
  const UTF8::UTF16Encoded expected = {0x00e9u, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_014, "2-code-unit UTF-8 without terminating zero encoded correctly") {
  const char input[] = {'\xc3', '\xa9'};
  const UTF8::UTF16Encoded expected = {0x00e9u, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_015, "3-code-unit UTF-8 encoded correctly") {
  const char input[] = {'\xe4', '\xac', '\x93', '\x00'};
  const UTF8::UTF16Encoded expected = {0x4b13u, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_016, "3-code-unit UTF-8 without terminating zero encoded correctly") {
  const char input[] = {'\xe4', '\xac', '\x93'};
  const UTF8::UTF16Encoded expected = {0x4b13u, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_017, "4-code-unit UTF-8 encoded correctly") {
  const char input[] = {'\xf3', '\x9e', '\xa5', '\xb0', '\x00'};
  const UTF8::UTF16Encoded expected = {0xdb3au, 0xdd70u, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_018, "4-code-unit UTF-8 without terminating zero encoded correctly") {
  const char input[] = {'\xf3', '\x9e', '\xa5', '\xb0'};
  const UTF8::UTF16Encoded expected = {0xdb3au, 0xdd70u, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_019, "UTF-8 encoded BOM encoded correctly") {
  const char input[] = {'\xef', '\xbb', '\xbf', '\x00'};
  const UTF8::UTF16Encoded expected = {0xfeffu, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_020, "UTF-8 encoded data with BOM as whitespace code-point encoded correctly") {
  const char input[] = {'\x61', '\xef', '\xbb', '\xbf', '\x00'};
  const UTF8::UTF16Encoded expected = {0x0061u, 0xfeffu, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_021, "Zero in UTF-8 data handled correctly") {
  const char input[] = {'\x62', '\xc3', '\xaa', '\x00', '\xe4', '\xac', '\x92', '\x00'};
  const UTF8::UTF16Encoded expected = {0x0062u, 0x00eau, 0x0000, 0x4b12u, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_022, "UTF-8 data with an explicit replacement character is encoded correctly") {
  char input[] = {'\xef', '\xbf', '\xbd', '\x00'};
  const UTF8::UTF16Encoded expected = {0xfffdu, 0x0000};
  const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

  V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
}


V8MONKEY_TEST(IntUtf8_023, "Continuation code-units outside of a multi-byte sequence encode to the replacement character") {
  for (char cont = '\x80'; cont < '\xc0'; cont++) {
    char input[] = {cont, 0x00};
    const UTF8::UTF16Encoded expected = {0xfffdu, 0x0000};
    const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

    V8MONKEY_CHECK(actual == expected, "Continuation code-unit correctly encoded as replacement character");
  }
}


V8MONKEY_TEST(IntUtf8_024, "UTF-8 encoded low surrogates encode to the replacement character") {
  for (char cont1 = '\xb0'; cont1 < '\xc0'; cont1++) {
    for (char cont2 = '\x80'; cont2 < '\xc0'; cont2++) {
      char input[] = {'\xed', cont1, cont2, '\x00'};
      const UTF8::UTF16Encoded expected = {0xfffdu, 0xfffdu, 0xfffdu, 0x0000};
      const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

      V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
    }
  }
}


V8MONKEY_TEST(IntUtf8_025, "UTF-8 encoded high surrogates encode to the replacement character") {
  for (char cont1 = '\xa0'; cont1 < '\xb0'; cont1++) {
    for (char cont2 = '\x80'; cont2 < '\xc0'; cont2++) {
      char input[] = {'\xed', cont1, cont2, '\x00'};
      const UTF8::UTF16Encoded expected = {0xfffdu, 0xfffdu, 0xfffdu, 0x0000};
      const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

      V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
    }
  }
}


V8MONKEY_TEST(IntUtf8_026, "Out-of-range 2-code-unit UTF-8 encodes to replacement characters") {
  for (char leading = '\xc0'; leading < '\xc2'; leading++) {
    for (char cont = '\x80'; cont < '\xc0'; cont++) {
      char input[] = {leading, cont, '\x00'};
      const UTF8::UTF16Encoded expected = {0xfffdu, 0xfffdu, 0x0000};
      const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

      V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
    }
  }
}


V8MONKEY_TEST(IntUtf8_027, "Out-of-range 3-code-unit UTF-8 encodes to replacement characters") {
  for (char leading = '\x80'; leading < '\xa0'; leading++) {
    for (char cont = '\x80'; cont < '\xc0'; cont++) {
      char input[] = {'\xe0', leading, cont, '\x00'};
      const UTF8::UTF16Encoded expected = {0xfffdu, 0xfffdu, 0xfffdu, 0x0000};
      const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

      V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
    }
  }
}


V8MONKEY_TEST(IntUtf8_028, "Out-of-range 4-code-unit UTF-8 encodes to replacement characters (1)") {
  for (char leading = '\x80'; leading < '\x90'; leading++) {
    for (char cont1 = '\x80'; cont1 < '\xc0'; cont1++) {
      for (char cont2 = '\x80'; cont2 < '\xc0'; cont2++) {
        char input[] = {'\xf0', leading, cont1, cont2, '\x00'};
        const UTF8::UTF16Encoded expected = {0xfffdu, 0xfffdu, 0xfffdu, 0xfffdu, 0x0000};
        const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

        V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
      }
    }
  }
}


V8MONKEY_TEST(IntUtf8_029, "Out-of-range 4-code-unit UTF-8 encodes to replacement characters (2)") {
  for (char leading = '\xf4'; leading <= '\xff'; leading++) {
    for (char cont1 = (leading == '\xf4' ? '\x90' : '\x80'); cont1 < '\xc0'; cont1++) {
      for (char cont2 = '\x80'; cont2 < '\xc0'; cont2++) {
        for (char cont3 = '\x80'; cont3 < '\xc0'; cont3++) {
          char input[] = {leading, cont1, cont2, cont3, '\x00'};
          const UTF8::UTF16Encoded expected = {0xfffdu, 0xfffdu, 0xfffdu, 0xfffdu, 0x0000};
          const auto actual = UTF8::EncodeToUTF16(std::begin(input), std::end(input));

          V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
        }
      }
    }
  }
}


V8MONKEY_TEST(IntUtf8_030, "Ill-formed UTF-8 sequences have their contents encoded to replacement characters (1)") {
  for (const auto& utf : invalidUTF8) {
    const std::vector<char> testCase(utf);

    auto testCaseSize = testCase.size();
    const char* inputBegin = testCase.data();
    const char* inputEnd = inputBegin + testCaseSize;
    UTF8::UTF16Encoded expected(testCaseSize);

    const auto expectedBegin = std::begin(expected);
    std::transform(inputBegin, inputEnd, expectedBegin, [](const char c) {
      if (c & 0x80) {
        return static_cast<char16_t>(0xfffdu);
      } else {
        return static_cast<char16_t>(c);
      }
    });

    const auto actual = UTF8::EncodeToUTF16(inputBegin, inputEnd);
    V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
  }
}


V8MONKEY_TEST(IntUtf8_031, "Ill-formed UTF-8 sequences have their contents encoded to replacement characters (2)") {
  auto utfIter = std::begin(encodableInvalidUTF8);
  const auto& utfEnd = std::end(encodableInvalidUTF8);
  auto encodedIter = std::begin(encodableInvalidAsUTF16);
  const auto& encodedEnd = std::end(encodableInvalidAsUTF16);

  // std::distance and std::begin/end aren't constexpr in our case, so we cannot static assert this
  V8MONKEY_CHECK(std::distance(utfIter, utfEnd) == std::distance(encodedIter, encodedEnd),
                 "Length mismatch in encodable invalid utf arrays");

  for ( /* utfIter and encodedIter are initialized above */; utfIter != utfEnd; ++utfIter, ++encodedIter) {
    UTF8::UTF16Encoded expected(*encodedIter);

    const std::vector<char> testCase(*utfIter);
    const char* inputBegin = testCase.data();
    const char* inputEnd = inputBegin + testCase.size();

    const auto actual = UTF8::EncodeToUTF16(inputBegin, inputEnd);
    V8MONKEY_CHECK(actual == expected, "Data correctly encoded");
  }
}


// XXX Reversed surrogates
// XXX Verify everything against V8
