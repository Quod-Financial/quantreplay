#include <gmock/gmock.h>

#include <optional>

#include "api/models/price_tick_range.hpp"
#include "ih/common/exceptions.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct DataLayerPriceTickRange : public Test {
  PriceTickRange::Patch patch;
};

TEST_F(DataLayerPriceTickRange, IsNotCreatedWithoutTickSize) {
  patch.with_upper_limit(10.0);

  EXPECT_THROW((void)PriceTickRange::create(patch), RequiredAttributeMissing);
}

TEST_F(DataLayerPriceTickRange, IsCreatedWithTickSize) {
  patch.with_tick_size(0.01);

  const auto range = PriceTickRange::create(patch);
  EXPECT_DOUBLE_EQ(range.tick_size(), 0.01);
}

TEST_F(DataLayerPriceTickRange, IsCreatedWithUpperLimit) {
  patch.with_tick_size(0.01).with_upper_limit(10.0);

  const auto range = PriceTickRange::create(patch);
  EXPECT_THAT(range.upper_limit(), Optional(DoubleEq(10.0)));
}

TEST_F(DataLayerPriceTickRange, IsCreatedWithoutUpperLimit) {
  patch.with_tick_size(0.01);

  const auto range = PriceTickRange::create(patch);
  EXPECT_EQ(range.upper_limit(), std::nullopt);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test
