#ifndef SIMULATOR_CORE_TOOLS_PRICE_TICK_RANGES_HPP_
#define SIMULATOR_CORE_TOOLS_PRICE_TICK_RANGES_HPP_

#include <fmt/base.h>

#include <initializer_list>
#include <optional>
#include <vector>

namespace simulator::core {

class PriceTickRanges final {
 public:
  struct Range {
    double tick_size = 0.0;
    std::optional<double> upper_limit = std::nullopt;

    [[nodiscard]]
    auto operator==(const Range&) const -> bool = default;
  };

  PriceTickRanges() = default;

  explicit PriceTickRanges(std::vector<Range> rows);

  PriceTickRanges(std::initializer_list<Range> rows);

  [[nodiscard]]
  auto ranges() const noexcept -> const std::vector<Range>&;

  [[nodiscard]]
  auto tick_of(double price) const -> std::optional<double>;

  [[nodiscard]]
  auto operator==(const PriceTickRanges&) const -> bool = default;

 private:
  std::vector<Range> ranges_;
};

}  // namespace simulator::core

template <>
struct fmt::formatter<simulator::core::PriceTickRanges::Range> {
  using formattable = simulator::core::PriceTickRanges::Range;

  constexpr static auto parse(format_parse_context& ctx) { return ctx.begin(); }

  static auto format(const formattable& range, format_context& context)
      -> decltype(context.out());
};

template <>
struct fmt::formatter<simulator::core::PriceTickRanges> {
  using formattable = simulator::core::PriceTickRanges;

  constexpr static auto parse(format_parse_context& ctx) { return ctx.begin(); }

  static auto format(const formattable& ranges, format_context& context)
      -> decltype(context.out());
};

#endif  // SIMULATOR_CORE_TOOLS_PRICE_TICK_RANGES_HPP_
