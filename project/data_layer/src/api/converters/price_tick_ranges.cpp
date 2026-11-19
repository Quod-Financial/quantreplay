#include "api/converters/price_tick_ranges.hpp"

#include <utility>
#include <vector>

#include "api/models/price_tick_range.hpp"
#include "core/tools/price_tick_ranges.hpp"

namespace simulator::data_layer::converter {

auto to_price_tick_ranges(const std::vector<PriceTickRange>& rows)
    -> core::PriceTickRanges {
  std::vector<core::PriceTickRanges::Range> converted_rows;
  converted_rows.reserve(rows.size());
  for (const auto& row : rows) {
    converted_rows.push_back(
        {.tick_size = row.tick_size(), .upper_limit = row.upper_limit()});
  }
  return core::PriceTickRanges{std::move(converted_rows)};
}

}  // namespace simulator::data_layer::converter
