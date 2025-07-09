#include <gtest/gtest.h>

#include "ih/sequences/numeric_sequence.hpp"

namespace simulator::trading_system::idgen::test {
namespace {

// NOLINTBEGIN(*-magic-numbers)

TEST(IdgenNumericSequence, MakeNewNumericSequence) {
  using Sequence = numeric::Sequence<std::uint8_t, 1, 2>;

  const auto sequence = make_sequence<Sequence>();

  EXPECT_EQ(sequence.current(), 1);
}

TEST(IdgenNumericSequence, MakeNewNumericSequenceWithEqualStartLastValues) {
  using Sequence = numeric::Sequence<std::uint8_t, 1, 1>;

  const auto sequence = make_sequence<Sequence>();

  EXPECT_EQ(sequence.current(), 1);
}

TEST(IdgenNumericSequence, IncrementNumericSequence) {
  using Sequence = numeric::Sequence<std::uint8_t, 1, 2>;
  const auto original = make_sequence<Sequence>();

  const auto incremented = increment(original);
  ASSERT_TRUE(incremented.has_value());

  EXPECT_EQ(incremented->current(), 2);
}

TEST(IdgenNumericSequence, IncrementNumericSequenceAfterLastValueReached) {
  using Sequence = numeric::Sequence<std::uint8_t, 1, 2>;
  const auto original = make_sequence<Sequence>();
  const auto firstly_incremented = increment(original);
  ASSERT_TRUE(firstly_incremented.has_value());

  const auto secondly_incremented = increment(*firstly_incremented);
  ASSERT_FALSE(secondly_incremented.has_value());

  EXPECT_EQ(secondly_incremented.error(), numeric::SequenceError::Overflow);
}

TEST(IdgenNumericSequence, IncrementNumericSequenceWithEqualStartLastValues) {
  using Sequence = numeric::Sequence<std::uint8_t, 1, 1>;
  const auto original = make_sequence<Sequence>();

  const auto incremented = increment(original);
  ASSERT_FALSE(incremented.has_value());

  EXPECT_EQ(incremented.error(), numeric::SequenceError::Overflow);
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::idgen::test