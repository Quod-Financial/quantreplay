#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_IES_PHASE_RECORD_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_IES_PHASE_RECORD_HPP_

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <chrono>
#include <variant>

#include "core/domain/attributes.hpp"

namespace simulator::trading_system::ies {

struct PhaseRecord {
  std::chrono::minutes begin = std::chrono::minutes(0);
  std::chrono::minutes end = std::chrono::minutes(0);
  std::chrono::seconds end_range = std::chrono::seconds(0);
  std::variant<TradingPhase, TradingStatus> phase;
  bool allow_cancels_on_halt = false;

  friend auto operator==(const PhaseRecord& lhs, const PhaseRecord& rhs) noexcept -> bool = default;
};

}  // namespace simulator::trading_system::ies

template <>
struct fmt::formatter<
    std::variant<simulator::TradingPhase, simulator::TradingStatus>> {
  using formattable =
      std::variant<simulator::TradingPhase, simulator::TradingStatus>;

  constexpr static auto parse(auto& ctx) { return ctx.begin(); }

  auto format(const formattable& record, auto& ctx) const {
    return std::visit(
        [&ctx](const auto& value) {
          return fmt::format_to(ctx.out(), "\"{}\"", value);
        },
        record);
  }
};

template <>
struct fmt::formatter<simulator::trading_system::ies::PhaseRecord> {
  using formattable = simulator::trading_system::ies::PhaseRecord;

  constexpr static auto parse(auto& ctx) { return ctx.begin(); }

  auto format(const formattable& record, auto& ctx) const {
    return fmt::format_to(ctx.out(),
                          // clang-format off
                          "{{ "
                            "\"PhaseRecord\": {{ "
                              "\"begin\": \"{:%H:%M}\", "
                              "\"end\": \"{:%H:%M}\", "
                              "\"end_range\": \"{:%S}\", "
                              "\"phase\": \"{}\", "
                              "\"allow_cancels_on_halt\": \"{}\" "
                            "}} "
                          "}}",
                          // clang-format on
                          record.begin,
                          record.end,
                          record.end_range,
                          record.phase,
                          record.allow_cancels_on_halt);
  }
};

namespace simulator::trading_system::ies {

inline auto operator<<(std::ostream& stream,
                const PhaseRecord& record) -> std::ostream& {
  return stream << fmt::to_string(record);
}

} // namespace simulator::trading_system::ies

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_IES_PHASE_RECORD_HPP_
