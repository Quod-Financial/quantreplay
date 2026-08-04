#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_INDICATIVE_REPORTER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_INDICATIVE_REPORTER_HPP_

#include <gsl/pointers>
#include <optional>

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/matchers/auction_price_calculator.hpp"
#include "ih/orders/phase_handler.hpp"

namespace simulator::trading_system::matching_engine::order {

class AuctionIndicativeReporter : private EventReporter {
 public:
  AuctionIndicativeReporter(EventListener& event_listener,
                            const PhaseHandler& phase_handler);

  auto operator()(const std::optional<AuctionResult>& auction_result) const
      -> void;

  auto report_cleared() const -> void;

 private:
  gsl::not_null<const PhaseHandler*> phase_handler_;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_AUCTION_INDICATIVE_REPORTER_HPP_
