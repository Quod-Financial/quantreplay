#include <gmock/gmock.h>

#include <optional>
#include <vector>

#include "api/converters/price_tick_ranges.hpp"
#include "api/models/price_tick_range.hpp"
#include "core/tools/price_tick_ranges.hpp"

namespace simulator::data_layer::converter::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

[[nodiscard]]
auto make_row(double tick_size, std::optional<double> upper_limit)
    -> PriceTickRange {
  return PriceTickRange::create(
      PriceTickRange::Patch{}.with_tick_size(tick_size).with_upper_limit(
          upper_limit));
}

TEST(DataLayerPriceTickRangesConverter, ConvertsRowsInGivenOrder) {
  const std::vector<PriceTickRange> rows{make_row(0.5, std::nullopt),
                                         make_row(0.25, std::nullopt),
                                         make_row(0.01, 10.0)};

  ASSERT_THAT(
      to_price_tick_ranges(rows),
      Eq(core::PriceTickRanges{{.tick_size = 0.5},
                               {.tick_size = 0.25},
                               {.tick_size = 0.01, .upper_limit = 10.0}}));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::converter::test
