#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>
#include <vector>

#include "ih/random/generators/value_generator_impl.hpp"

namespace simulator::generator::random::test {
namespace {

using SignedTypes =
    testing::Types<std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

template <typename>
class Generator_Random_ValueGenerator_Signed : public testing::Test {};

TYPED_TEST_SUITE(Generator_Random_ValueGenerator_Signed, SignedTypes);

TYPED_TEST(Generator_Random_ValueGenerator_Signed, GenerateUniformInterval) {
  constexpr auto min = static_cast<TypeParam>(-1);
  constexpr auto max = static_cast<TypeParam>(1);

  auto generator = ValueGeneratorImpl::create();
  ASSERT_TRUE(generator);

  const auto random = generator->generate_uniform_value(min, max);
  static_assert(std::is_same_v<std::decay_t<decltype(random)>, TypeParam>);

  EXPECT_GE(random, min);
  EXPECT_LE(random, max);
}

TYPED_TEST(Generator_Random_ValueGenerator_Signed, GenerateMin) {
  constexpr std::int64_t min = std::numeric_limits<std::int64_t>::min();
  constexpr std::int64_t max = min + 1;

  auto generator = ValueGeneratorImpl::create();
  ASSERT_TRUE(generator);

  const auto random = generator->generate_uniform_value(min, max);

  EXPECT_TRUE(random == min || random == max);
}

TYPED_TEST(Generator_Random_ValueGenerator_Signed, GenerateMax) {
  constexpr std::int64_t max = std::numeric_limits<std::int64_t>::max();
  constexpr std::int64_t min = max - 1;

  auto generator = ValueGeneratorImpl::create();
  ASSERT_TRUE(generator);

  const auto random = generator->generate_uniform_value(min, max);

  EXPECT_TRUE(random == max || random == min);
}

using UnsignedTypes =
    testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

template <typename>
class Generator_Random_ValueGenerator_Unsigned : public testing::Test {};

TYPED_TEST_SUITE(Generator_Random_ValueGenerator_Unsigned, UnsignedTypes);

TYPED_TEST(Generator_Random_ValueGenerator_Unsigned, GenerateUniformInterval) {
  constexpr auto min = static_cast<TypeParam>(0);
  constexpr auto max = static_cast<TypeParam>(2);

  auto generator = ValueGeneratorImpl::create();
  ASSERT_TRUE(generator);

  const auto random = generator->generate_uniform_value(min, max);
  static_assert(std::is_same_v<std::decay_t<decltype(random)>, TypeParam>);

  EXPECT_LE(random, max);
  EXPECT_GE(random, min);
}

TYPED_TEST(Generator_Random_ValueGenerator_Unsigned, GenerateMin) {
  constexpr std::uint64_t min = std::numeric_limits<std::uint64_t>::min();
  constexpr std::uint64_t max = min + 1;

  auto generator = ValueGeneratorImpl::create();
  ASSERT_TRUE(generator);

  const auto random = generator->generate_uniform_value(min, max);

  EXPECT_TRUE(random == min || random == max);
}

TYPED_TEST(Generator_Random_ValueGenerator_Unsigned, GenerateMax) {
  constexpr std::uint64_t max = std::numeric_limits<std::uint64_t>::max();
  constexpr std::uint64_t min = max - 1;

  auto generator = ValueGeneratorImpl::create();
  ASSERT_TRUE(generator);

  const auto random = generator->generate_uniform_value(min, max);

  EXPECT_TRUE(random == min || random == max);
}

using FloatingPointTypes = testing::Types<std::float_t, std::double_t>;

template <typename>
class Generator_Random_ValueGenerator_FloatingPoint : public testing::Test {};

TYPED_TEST_SUITE(Generator_Random_ValueGenerator_FloatingPoint,
                 FloatingPointTypes);

TYPED_TEST(Generator_Random_ValueGenerator_FloatingPoint,
           GenerateUniformInterval) {
  constexpr auto min = static_cast<TypeParam>(-0.001);
  constexpr auto max = static_cast<TypeParam>(0.001);

  auto generator = ValueGeneratorImpl::create();
  ASSERT_TRUE(generator);

  const auto random = generator->generate_uniform_value(min, max);
  static_assert(std::is_same_v<std::decay_t<decltype(random)>, TypeParam>);

  EXPECT_LE(random, max);
  EXPECT_GE(random, min);
}

// NOLINTBEGIN(*-magic-numbers)

class Generator_Random_ValueGenerator_Seeded : public testing::Test {
 protected:
  static constexpr std::int64_t RangeMin = -1'000'000;
  static constexpr std::int64_t RangeMax = 1'000'000;
  static constexpr std::size_t SampleSize = 32;

  static auto draw_sequence(ValueGeneratorImpl& generator)
      -> std::vector<std::int64_t> {
    std::vector<std::int64_t> sequence;
    sequence.reserve(SampleSize);
    for (std::size_t i = 0; i < SampleSize; ++i) {
      sequence.push_back(generator.generate_uniform_value(RangeMin, RangeMax));
    }
    return sequence;
  }
};

TEST_F(Generator_Random_ValueGenerator_Seeded,
       SameSeedProducesSameSequenceAcrossInstances) {
  constexpr std::uint64_t seed = 0xA5A5A5A5A5A5A5A5ULL;

  ValueGeneratorImpl first{};
  first.reseed(seed);
  ValueGeneratorImpl second{};
  second.reseed(seed);

  EXPECT_EQ(draw_sequence(first), draw_sequence(second));
}

TEST_F(Generator_Random_ValueGenerator_Seeded,
       DifferentSeedsProduceDifferentSequences) {
  ValueGeneratorImpl first{};
  first.reseed(std::uint64_t{1});
  ValueGeneratorImpl second{};
  second.reseed(std::uint64_t{2});

  EXPECT_NE(draw_sequence(first), draw_sequence(second));
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::generator::random::test