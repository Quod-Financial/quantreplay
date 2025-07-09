#include <gmock/gmock.h>

#include "core/tools/string_checkers.hpp"

namespace simulator::core::test {
namespace {

using namespace testing;  // NOLINT

TEST(CoreRepresentsBase10UnsignedIntCheck, DetectsEmptyString) {
  ASSERT_THAT(represents_base10_unsigned_int(""), IsFalse());
}

TEST(CoreRepresentsBase10UnsignedIntCheck, DetectsMinusSign) {
  ASSERT_THAT(represents_base10_unsigned_int("-123456789"), IsFalse());
}

TEST(CoreRepresentsBase10UnsignedIntCheck, DetectsStringWithSpecialSymbol) {
  ASSERT_THAT(represents_base10_unsigned_int("12345@6789"), IsFalse());
}

TEST(CoreRepresentsBase10UnsignedIntCheck, DetectsStringWithLetter) {
  ASSERT_THAT(represents_base10_unsigned_int("1A23456789"), IsFalse());
}

TEST(CoreRepresentsBase10UnsignedIntCheck, DetectsUnsignedInteger) {
  ASSERT_THAT(represents_base10_unsigned_int("123456789"), IsTrue());
}

}  // namespace
}  // namespace simulator::core::test