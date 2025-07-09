#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_ORDER_REQUEST_PROCESSOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_ORDER_REQUEST_PROCESSOR_HPP_

#include "common/market_state/snapshot.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"

namespace simulator::trading_system::matching_engine {

class OrderRequestProcessor {
 public:
  OrderRequestProcessor() = default;
  OrderRequestProcessor(const OrderRequestProcessor&) = default;
  OrderRequestProcessor(OrderRequestProcessor&&) = default;
  virtual ~OrderRequestProcessor() = default;

  auto operator=(const OrderRequestProcessor&)
      -> OrderRequestProcessor& = default;
  auto operator=(OrderRequestProcessor&&) -> OrderRequestProcessor& = default;

  virtual auto process(const protocol::OrderPlacementRequest& request)
      -> void = 0;

  virtual auto process(const protocol::OrderModificationRequest& request)
      -> void = 0;

  virtual auto process(const protocol::OrderCancellationRequest& request)
      -> void = 0;

  virtual auto process(const protocol::SecurityStatusRequest& request)
      -> void = 0;

  virtual auto store_state(
      market_state::OrderBook& state) -> void = 0;

  virtual auto recover_state(market_state::OrderBook state) -> void = 0;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_ORDER_REQUEST_PROCESSOR_HPP_