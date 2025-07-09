#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "ih/random/generators/quantity_generator.hpp"
#include "ih/random/values/quantity_generation_params.hpp"
#include "mocks/random/value_generator.hpp"

namespace simulator::generator::random::test {
namespace {

using namespace ::testing;

class Generator_Random_QuantityGenerator : public testing::Test {
 public:
  auto value_generator() -> mock::ValueGenerator& { return *value_generator_; }

  auto generate(const random::QuantityGenerationParams& params) -> double {
    return qty_generator_->generate_qty(params);
  }

  static auto make_params(double multiplier, double min_qty, double max_qty)
      -> random::QuantityGenerationParams {
    QuantityGenerationParams::Builder qty_params_builder{};
    qty_params_builder.set_quantity_multiplier(multiplier);
    qty_params_builder.set_minimal_quantity(min_qty);
    qty_params_builder.set_maximal_quantity(max_qty);
    return QuantityGenerationParams{qty_params_builder};
  }

 protected:
  auto SetUp() -> void override {
    value_generator_ = std::make_shared<mock::ValueGenerator>();
    qty_generator_ =
        std::make_unique<random::QuantityGeneratorImpl>(value_generator_);
  }

 private:
  std::shared_ptr<mock::ValueGenerator> value_generator_;
  std::unique_ptr<QuantityGenerator> qty_generator_;
};

TEST_F(Generator_Random_QuantityGenerator, AllParamsZero) {
  // In case all params are equal to zero, we'll use default multiplier as qty
  // Default multiplier is used when provided multiplier equals to 0
  // Default multiplier must be equal to 1
  constexpr double expected_generated_qty = 1.0;

  constexpr std::int32_t random_value = 0;
  constexpr double expected_rand_min = 0.0;
  constexpr double expected_rand_max = 0.0;

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::int64_t>(Eq(expected_rand_min)),
                               Matcher<std::int64_t>(Eq(expected_rand_max))))
      .Times(1)
      .WillOnce(Return(random_value));

  const double generated_qty = generate(make_params(0., 0., 0.));
  EXPECT_DOUBLE_EQ(generated_qty, expected_generated_qty);
}

TEST_F(Generator_Random_QuantityGenerator, MultiplierIsOne) {
  constexpr double multiplier = 1.0;
  constexpr double min_qty = 20.0;
  constexpr double max_qty = 1000.0;

  // (max_qty / multiplier) - (min_qty / multiplier)
  constexpr std::int64_t expected_rand_max = 980;
  constexpr std::int64_t expected_rand_min = 0;
  constexpr std::int64_t random_value = 700;

  // (random_value + (min_qty / multiplier)) * multiplier
  constexpr double expected_generated_qty = 720.0;

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::int64_t>(Eq(expected_rand_min)),
                               Matcher<std::int64_t>(Eq(expected_rand_max))))
      .Times(1)
      .WillOnce(Return(random_value));

  const double generated_qty =
      generate(make_params(multiplier, min_qty, max_qty));
  EXPECT_DOUBLE_EQ(generated_qty, expected_generated_qty);
}

TEST_F(Generator_Random_QuantityGenerator, MultiplierIsThree) {
  constexpr double multiplier = 3.0;
  constexpr double min_qty = 30.0;
  constexpr double max_qty = 12000.0;

  // (max_qty / multiplier) - (min_qty / multiplier)
  constexpr std::int64_t expected_rand_max = 3990;
  constexpr std::int64_t expected_rand_min = 0;
  constexpr std::int64_t random_value = 2100;

  // (random_value + (min_qty / multiplier)) * multiplier
  constexpr double expected_generated_qty = 6330.0;

  EXPECT_CALL(value_generator(),
              generate_uniform(Matcher<std::int64_t>(Eq(expected_rand_min)),
                               Matcher<std::int64_t>(Eq(expected_rand_max))))
      .Times(1)
      .WillOnce(Return(random_value));

  const double generated_qty =
      generate(make_params(multiplier, min_qty, max_qty));
  EXPECT_DOUBLE_EQ(generated_qty, expected_generated_qty);
}

}  // namespace
}  // namespace simulator::generator::random::test