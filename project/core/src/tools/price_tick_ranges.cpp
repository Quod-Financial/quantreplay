#include "core/tools/price_tick_ranges.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <initializer_list>
#include <optional>
#include <utility>
#include <vector>

#include "core/common/std_formatter.hpp"
#include "core/tools/format.hpp"
#include "core/tools/numeric.hpp"

namespace simulator::core {
namespace {

[[nodiscard]]
auto ends_below(const PriceTickRanges::Range& left,
                const PriceTickRanges::Range& right) -> bool {
  return left.upper_limit.has_value() &&
         (!right.upper_limit.has_value() ||
          *left.upper_limit < *right.upper_limit);
}

[[nodiscard]]
auto reaches(const PriceTickRanges::Range& range, double price) -> bool {
  return !range.upper_limit.has_value() ||
         price - *range.upper_limit < AbsoluteTolerance;
}

[[nodiscard]]
auto make_ranges(std::vector<PriceTickRanges::Range> ranges)
    -> std::vector<PriceTickRanges::Range> {
  std::ranges::stable_sort(ranges, ends_below);
  const auto ignored_ranges =
      std::ranges::unique(ranges, {}, &PriceTickRanges::Range::upper_limit);
  ranges.erase(ignored_ranges.begin(), ignored_ranges.end());

  if (!ranges.empty()) {
    ranges.back().upper_limit = std::nullopt;
  }
  return ranges;
}

}  // namespace

PriceTickRanges::PriceTickRanges(std::vector<Range> rows)
    : ranges_{make_ranges(std::move(rows))} {}

PriceTickRanges::PriceTickRanges(std::initializer_list<Range> rows)
    : ranges_{make_ranges(rows)} {}

auto PriceTickRanges::ranges() const noexcept -> const std::vector<Range>& {
  return ranges_;
}

auto PriceTickRanges::tick_of(double price) const -> std::optional<double> {
  const auto range = std::ranges::find_if(
      ranges_,
      [price](const Range& candidate) { return reaches(candidate, price); });
  return range != ranges_.end() ? std::make_optional(range->tick_size)
                                : std::nullopt;
}

}  // namespace simulator::core

auto fmt::formatter<simulator::core::PriceTickRanges::Range>::format(
    const formattable& range, format_context& context)
    -> decltype(context.out()) {
  return format_to(context.out(),
                   "{{ TickSize={}, UpperLimit={} }}",
                   range.tick_size,
                   range.upper_limit);
}

auto fmt::formatter<simulator::core::PriceTickRanges>::format(
    const formattable& ranges, format_context& context)
    -> decltype(context.out()) {
  return format_to(
      context.out(), "{}", simulator::core::format_collection(ranges.ranges()));
}
