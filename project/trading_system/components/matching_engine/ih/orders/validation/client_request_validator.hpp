#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_REQUEST_VALIDATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_REQUEST_VALIDATOR_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "common/market_state/snapshot.hpp"
#include "core/tools/time.hpp"
#include "ih/orders/validation/validator.hpp"
#include "ih/orders/validation/errors.hpp"
#include "ih/common/validation/validation.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"

namespace simulator::trading_system::matching_engine::order {

class ClientRequestValidator : public Validator {
  template <typename RequestType>
  using Validation = matching_engine::Validation<RequestType, ValidationError>;

 public:
  struct Config {
    core::TzClock clock;
    std::optional<MinQuantity> min_quantity = std::nullopt;
    std::optional<MaxQuantity> max_quantity = std::nullopt;
    std::optional<QuantityTick> quantity_tick = std::nullopt;
    std::optional<PriceTick> price_tick = std::nullopt;
  };

  auto configure(Config config) -> void;

  [[nodiscard]]
  auto validate(const protocol::OrderPlacementRequest& request) const
      -> Conclusion override;

  [[nodiscard]]
  auto validate(const protocol::OrderModificationRequest& request) const
      -> Conclusion override;

  [[nodiscard]]
  auto validate(const protocol::OrderCancellationRequest& request) const
      -> Conclusion override;

  [[nodiscard]]
  auto validate(const market_state::LimitOrder& order,
                OrderBookSide order_book_side) const -> Conclusion override;

 private:
  auto run(Validation<protocol::OrderPlacementRequest>& validation) const
      -> void;

  auto run(Validation<protocol::OrderModificationRequest>& validation) const
      -> void;

  auto run(Validation<protocol::OrderCancellationRequest>& validation) const
      -> void;

  auto run(Validation<market_state::LimitOrder>& validation,
           OrderBookSide order_book_side) const -> void;

  Config config_{};
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_VALIDATION_REQUEST_VALIDATOR_HPP_
