#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_EARLY_PRICE_REPORTER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_EARLY_PRICE_REPORTER_HPP_

#include <gsl/pointers>
#include <optional>

#include "common/events.hpp"
#include "core/tools/time.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/matchers/auction_price_calculator.hpp"
#include "ih/orders/phase_handler.hpp"

namespace simulator::trading_system::matching_engine::order {

class EarlyPriceReporter : private EventReporter {
 public:
  EarlyPriceReporter(EventListener& event_listener,
                     const PhaseHandler& phase_handler);

  auto operator()(const event::Tick& tick,
                  const std::optional<AuctionResult>& auction_result) -> void;

  auto report_cleared() -> void;

 private:
  gsl::not_null<const PhaseHandler*> phase_handler_;
  std::optional<core::sys_us> baseline_;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_EARLY_PRICE_REPORTER_HPP_
