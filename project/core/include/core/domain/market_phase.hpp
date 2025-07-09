#ifndef SIMULATOR_CORE_DOMAIN_MARKET_PHASE_HPP_
#define SIMULATOR_CORE_DOMAIN_MARKET_PHASE_HPP_

#include <fmt/base.h>

#include <iosfwd>

#include "core/common/name.hpp"
#include "core/common/std_formatter.hpp"
#include "core/domain/attributes.hpp"

namespace simulator {

struct MarketPhase {
  constexpr MarketPhase(TradingPhase phase, TradingStatus status)
      : phase_(phase), status_(status) {}

  [[nodiscard]]
  constexpr auto trading_phase() const -> TradingPhase {
    return phase_;
  }

  [[nodiscard]]
  constexpr auto trading_status() const -> TradingStatus {
    return status_;
  }

  [[nodiscard]]
  constexpr auto operator==(const MarketPhase& other) const -> bool = default;

  [[nodiscard]]
  constexpr static auto open() {
    return MarketPhase{TradingPhase::Option::Open,
                       TradingStatus::Option::Resume};
  }

  [[nodiscard]]
  consteval static auto name() {
    return core::Name{.singular = "MarketPhase", .plural = "MarketPhases"};
  }

 private:
  TradingPhase phase_;
  TradingStatus status_;
};

auto operator<<(std::ostream& ostream, const MarketPhase& phase)
    -> std::ostream&;

}  // namespace simulator

template <>
struct fmt::formatter<simulator::MarketPhase> {
  using formattable = simulator::MarketPhase;

  constexpr auto parse(format_parse_context& context) {
    return context.begin();
  }

  auto format(const formattable& phase, format_context& context) const
      -> decltype(context.out());
};

#endif  // SIMULATOR_CORE_DOMAIN_MARKET_PHASE_HPP_
