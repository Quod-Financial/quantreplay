#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_VALIDATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_VALIDATOR_HPP_

#include "ih/common/validation/conclusion.hpp"
#include "ih/orders/validation/order_book_side.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"

namespace simulator::trading_system::matching_engine::order {

class Validator {
 public:
  Validator() = default;
  Validator(const Validator&) = default;
  Validator(Validator&&) = default;
  virtual ~Validator() = default;

  auto operator=(const Validator&) -> Validator& = default;
  auto operator=(Validator&&) -> Validator& = default;

  [[nodiscard]]
  virtual auto validate(const protocol::OrderPlacementRequest& request) const
      -> Conclusion = 0;

  [[nodiscard]]
  virtual auto validate(const protocol::OrderModificationRequest& request) const
      -> Conclusion = 0;

  [[nodiscard]]
  virtual auto validate(const protocol::OrderCancellationRequest& request) const
      -> Conclusion = 0;

  [[nodiscard]]
  virtual auto validate(const market_state::LimitOrder& order,
                        OrderBookSide order_book_side) const -> Conclusion = 0;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_VALIDATOR_HPP_
