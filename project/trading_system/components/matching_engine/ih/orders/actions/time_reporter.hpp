#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_TIME_REPORTER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_TIME_REPORTER_HPP_

#include "common/events.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"

namespace simulator::trading_system::matching_engine::order {

class TimeReporter : private EventReporter {
 public:
  explicit TimeReporter(EventListener& event_listener);

  auto operator()(const event::Tick& tick) const -> void;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_TIME_REPORTER_HPP_
