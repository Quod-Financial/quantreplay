#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_ORDER_EVENT_HANDLER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_ORDER_EVENT_HANDLER_HPP_

#include "common/events.hpp"
#include "core/domain/attributes.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

class OrderEventHandler {
 public:
  OrderEventHandler() = default;
  OrderEventHandler(const OrderEventHandler&) = default;
  OrderEventHandler(OrderEventHandler&&) = default;
  virtual ~OrderEventHandler() = default;

  auto operator=(const OrderEventHandler&) -> OrderEventHandler& = default;
  auto operator=(OrderEventHandler&&) -> OrderEventHandler& = default;

  virtual auto handle(const event::Tick& tick) -> void = 0;

  virtual auto handle(const event::PhaseTransition& phase_transition)
      -> void = 0;

  virtual auto handle_disconnection(const protocol::Session& session)
      -> void = 0;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_ORDER_EVENT_HANDLER_HPP_
