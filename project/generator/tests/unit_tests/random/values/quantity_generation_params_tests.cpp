#include <gtest/gtest.h>

#include <stdexcept>

#include "ih/constants.hpp"
#include "ih/random/values/quantity_generation_params.hpp"

namespace simulator::generator::random::test {
namespace {

struct GeneratorRandomQuantityGenerationParams : public ::testing::Test {
  QuantityGenerationParams::Builder qty_params_builder;
};

TEST_F(GeneratorRandomQuantityGenerationParams,
       SetsZeroValuesWhenBuilderIsEmpty) {
  const QuantityGenerationParams qty_params{qty_params_builder};

  EXPECT_DOUBLE_EQ(qty_params.get_multiplier(), 0.);
  EXPECT_DOUBLE_EQ(qty_params.get_minimal_quantity(), 0.);
  EXPECT_DOUBLE_EQ(qty_params.get_maximal_quantity(), 0.);
}

TEST_F(GeneratorRandomQuantityGenerationParams, SetsQtyMultiplier) {
  constexpr double expected_qty_multiplier = 2.00005;

  qty_params_builder.set_quantity_multiplier(expected_qty_multiplier);
  const QuantityGenerationParams qty_params{qty_params_builder};

  EXPECT_DOUBLE_EQ(qty_params.get_multiplier(), expected_qty_multiplier);
}

TEST_F(GeneratorRandomQuantityGenerationParams,
       ThrowsExceptionWhenMinimalQuantityIsLessThanZero) {
  constexpr double min_quantity = -0.000000001;
  qty_params_builder.set_minimal_quantity(min_quantity);

  EXPECT_THROW(QuantityGenerationParams{qty_params_builder}, std::logic_error);
}

TEST_F(GeneratorRandomQuantityGenerationParams, SetsMinimalQuantity) {
  constexpr double expected_min_qty = 12.333345;
  constexpr double max_quantity = 1000.0;

  qty_params_builder.set_minimal_quantity(expected_min_qty);
  qty_params_builder.set_maximal_quantity(max_quantity);
  const QuantityGenerationParams qty_params{qty_params_builder};

  EXPECT_DOUBLE_EQ(qty_params.get_minimal_quantity(), expected_min_qty);
}

TEST_F(GeneratorRandomQuantityGenerationParams,
       ThrowsExceptionWhenMaximalQuantityIsLessThanZero) {
  constexpr double max_quantity = -0.000000001;
  qty_params_builder.set_maximal_quantity(max_quantity);

  EXPECT_THROW(QuantityGenerationParams{qty_params_builder}, std::logic_error);
}

TEST_F(GeneratorRandomQuantityGenerationParams,
       ThrowsExceptionWhenMaximalQuantityIsLessThanMinimaQuantity) {
  constexpr double min_quantity = 0.000000002;
  constexpr double max_quantity = 0.000000001;
  qty_params_builder.set_minimal_quantity(min_quantity);
  qty_params_builder.set_maximal_quantity(max_quantity);

  EXPECT_THROW(QuantityGenerationParams{qty_params_builder}, std::logic_error);
}

TEST_F(GeneratorRandomQuantityGenerationParams, SetsMaximalQuantity) {
  constexpr double expected_max_qty = 1000000.333652;

  qty_params_builder.set_maximal_quantity(expected_max_qty);
  const QuantityGenerationParams qty_params{qty_params_builder};

  EXPECT_DOUBLE_EQ(qty_params.get_maximal_quantity(), expected_max_qty);
}

}  // namespace
}  // namespace simulator::generator::random::test