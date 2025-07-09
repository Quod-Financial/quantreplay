#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_PHASE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_PHASE_HPP_

#include <fmt/base.h>

#include <optional>

#include "core/common/std_formatter.hpp"
#include "core/domain/attributes.hpp"

namespace simulator::trading_system {

class Phase {
 public:
  struct Settings {
    auto operator==(const Settings& other) const -> bool = default;

    bool allow_cancels = false;
  };

  Phase(TradingPhase phase, TradingStatus status, Settings setting) noexcept;

  auto operator==(const Phase& other) const -> bool = default;

  [[nodiscard]]
  auto phase() const -> TradingPhase;

  [[nodiscard]]
  auto status() const -> TradingStatus;

  [[nodiscard]]
  auto settings() const -> std::optional<Settings>;

 private:
  TradingPhase phase_;
  TradingStatus status_;
  std::optional<Settings> settings_;
};

}  // namespace simulator::trading_system

template <>
struct fmt::formatter<simulator::trading_system::Phase::Settings> {
  using formattable = simulator::trading_system::Phase::Settings;

  constexpr static auto parse(auto& ctx) { return ctx.begin(); }

  auto format(const formattable& setting, auto& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(),
                          // clang-format off
                        "{{ "
                          "\"Settings\": {{ "
                            "\"allow_cancels\": {} "
                          "}} "
                        "}}",
                          // clang-format on
                          setting.allow_cancels);
  }
};

template <>
struct fmt::formatter<simulator::trading_system::Phase> {
  using formattable = simulator::trading_system::Phase;

  constexpr static auto parse(auto& ctx) { return ctx.begin(); }

  auto format(const formattable& phase, auto& ctx) const
      -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(),
                          // clang-format off
                        "{{ "
                          "\"Phase\": {{ "
                            "\"phase\": \"{}\", "
                            "\"status\": \"{}\", "
                            "\"settings\": {} "
                          "}} "
                        "}}",
                          // clang-format on
                          phase.phase(),
                          phase.status(),
                          phase.settings());
  }
};

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_PHASE_HPP_
