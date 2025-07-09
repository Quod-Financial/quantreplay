#include <gtest/gtest.h>

#include "ih/sequences/chrono_sequence.hpp"

namespace simulator::trading_system::idgen::test {
namespace {

// NOLINTBEGIN(*-magic-numbers)

TEST(IdgenChronoSequence, MakeDateSequenceWithNegativeTick) {
  using namespace std::chrono_literals;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Date>;

  const auto tick = -1ns;

  const auto sequence = make_sequence<Sequence>(tick);
  ASSERT_FALSE(sequence.has_value());

  EXPECT_EQ(sequence.error(), chrono::SequenceError::NegativeTick);
}

TEST(IdgenChronoSequence, MakeTimestampSequenceWithNegativeTick) {
  using namespace std::chrono_literals;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Timestamp>;

  const auto tick = -1ns;

  const auto sequence = make_sequence<Sequence>(tick);
  ASSERT_FALSE(sequence.has_value());

  EXPECT_EQ(sequence.error(), chrono::SequenceError::NegativeTick);
}

TEST(IdgenChronoSequence, MakeTimestampSequenceWithZeroTick) {
  using namespace std::chrono_literals;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Timestamp>;

  // UNIX: 1970-01-01 00:00:00.000000000 GMT
  const auto ticks = 0ns;

  const auto sequence = make_sequence<Sequence>(ticks);
  ASSERT_TRUE(sequence.has_value());

  EXPECT_EQ(sequence->tick(), floor<std::chrono::seconds>(ticks));
  EXPECT_EQ(sequence->current(), 700101000000);
}

TEST(IdgenChronoSequence, MakeTimestampSequenceWithPositiveTick) {
  using namespace std::chrono_literals;
  using std::chrono::floor;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Timestamp>;

  // UNIX: 2023-12-11 08:07:26.111222333 GMT
  const auto ticks = 1702282046111222333ns;

  const auto sequence = make_sequence<Sequence>(ticks);
  ASSERT_TRUE(sequence.has_value());

  EXPECT_EQ(sequence->tick(), floor<std::chrono::seconds>(ticks));
  EXPECT_EQ(sequence->current(), 231211080726);
}

TEST(IdgenChronoSequence, MakeTimestampSequenceWithRealtimeTick) {
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Timestamp>;

  const auto sequence = make_sequence<Sequence>();
  ASSERT_TRUE(sequence.has_value());

  EXPECT_TRUE(sequence.has_value());
}

TEST(IdgenNumericSequence, MakeDateSequenceWithNegativeTick) {
  using namespace std::chrono_literals;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Date>;

  const auto tick = -1ns;

  const auto sequence = make_sequence<Sequence>(tick);
  ASSERT_FALSE(sequence.has_value());

  EXPECT_EQ(sequence.error(), chrono::SequenceError::NegativeTick);
}

TEST(IdgenChronoSequence, MakeDateSequenceWithZeroTick) {
  using namespace std::chrono_literals;
  using std::chrono::floor;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Date>;

  // UNIX: 1970-01-01 00:00:00.000000000 GMT
  const auto ticks = 0ns;

  const auto sequence = make_sequence<Sequence>(ticks);
  ASSERT_TRUE(sequence.has_value());

  EXPECT_EQ(sequence->tick(), floor<std::chrono::days>(ticks));
  EXPECT_EQ(sequence->current(), 700101);
}

TEST(IdgenChronoSequence, MakeDateSequenceWithPositiveTick) {
  using namespace std::chrono_literals;
  using std::chrono::floor;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Date>;

  // UNIX: 2023-12-11 08:07:26.111222333 GMT
  const auto ticks = 1702282046111222333ns;

  const auto sequence = make_sequence<Sequence>(ticks);
  ASSERT_TRUE(sequence.has_value());

  EXPECT_EQ(sequence->tick(), floor<std::chrono::days>(ticks));
  EXPECT_EQ(sequence->current(), 231211);
}

TEST(IdgenChronoSequence, MakeDateSequenceWithRealtimeTick) {
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Date>;

  const auto sequence = make_sequence<Sequence>();

  EXPECT_TRUE(sequence.has_value());
}

TEST(IdgenChronoSequence, IncrementSequenceWithLowerTick) {
  using namespace std::chrono_literals;
  using std::chrono::days;
  using std::chrono::floor;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Date>;

  const auto orig_tick = floor<days>(1702252800s);  // UNIX: 2023-12-11 GMT
  const auto tick = floor<days>(1702166400s);       // UNIX: 2023-12-10 GMT
  const auto orig_sequence = make_sequence<Sequence>(orig_tick);
  ASSERT_TRUE(orig_sequence.has_value());

  const auto sequence = increment(*orig_sequence, tick);
  ASSERT_TRUE(sequence.has_value());

  EXPECT_EQ(sequence->current(), 231211);
  EXPECT_EQ(sequence->tick(), orig_tick);
}

TEST(IdgenChronoSequence, IncrementSequenceWithTickEqualToCached) {
  using namespace std::chrono_literals;
  using std::chrono::days;
  using std::chrono::floor;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Date>;

  const auto tick = floor<days>(1702252800s);  // UNIX: 2023-12-11 GMT
  const auto orig_sequence = make_sequence<Sequence>(tick);
  ASSERT_TRUE(orig_sequence.has_value());

  const auto sequence = increment(*orig_sequence, tick);
  ASSERT_TRUE(sequence.has_value());

  EXPECT_EQ(sequence->current(), 231211);
  EXPECT_EQ(sequence->tick(), tick);
}

TEST(IdgenChronoSequence, IncrementSequenceWithGreaterTick) {
  using namespace std::chrono_literals;
  using std::chrono::days;
  using std::chrono::floor;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Date>;

  const auto orig_tick = floor<days>(1702252800s);  // UNIX: 2023-12-11 GMT
  const auto tick = floor<days>(1702339200s);       // UNIX: 2023-12-12 GMT
  const auto orig_sequence = make_sequence<Sequence>(orig_tick);
  ASSERT_TRUE(orig_sequence.has_value());

  const auto sequence = increment(*orig_sequence, tick);
  ASSERT_TRUE(sequence.has_value());

  EXPECT_EQ(sequence->current(), 231212);
  EXPECT_EQ(sequence->tick(), tick);
}

TEST(IdgenChronoSequence, IncrementSequenceWithRealtimeTick) {
  using namespace std::chrono_literals;
  using std::chrono::days;
  using std::chrono::floor;
  using std::chrono::system_clock;
  using Sequence = chrono::Sequence<chrono::SequenceFormat::Date>;

  const auto orig_tick = floor<days>(system_clock::now().time_since_epoch());
  const auto orig_sequence = make_sequence<Sequence>(orig_tick);
  ASSERT_TRUE(orig_sequence.has_value());

  const auto sequence = increment(*orig_sequence);
  ASSERT_TRUE(sequence.has_value());

  EXPECT_GE(sequence->tick(), orig_tick);
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::trading_system::idgen::test