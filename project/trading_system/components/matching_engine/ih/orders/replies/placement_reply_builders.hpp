#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_PLACEMENT_REPLY_BUILDERS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_PLACEMENT_REPLY_BUILDERS_HPP_

#include "common/attributes.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

class PlacementConfirmationBuilder {
 public:
  explicit PlacementConfirmationBuilder(protocol::Session session);

  [[nodiscard]]
  auto build() const -> protocol::OrderPlacementConfirmation;

  auto for_order(const LimitOrder& order) -> PlacementConfirmationBuilder&;

  auto for_order(const MarketOrder& order) -> PlacementConfirmationBuilder&;

  auto with_execution_id(ExecutionId identifier)
      -> PlacementConfirmationBuilder&;

 private:
  protocol::OrderPlacementConfirmation message_;
};

[[nodiscard]]
auto prepare_placement_confirmation(const LimitOrder& order)
    -> PlacementConfirmationBuilder;

[[nodiscard]]
auto prepare_placement_confirmation(const MarketOrder& order)
    -> PlacementConfirmationBuilder;

class PlacementRejectBuilder {
 public:
  explicit PlacementRejectBuilder(protocol::Session session);

  [[nodiscard]]
  auto build() const -> protocol::OrderPlacementReject;

  auto for_request(const protocol::OrderPlacementRequest& request)
      -> PlacementRejectBuilder&;

  auto for_order(const LimitOrder& order) -> PlacementRejectBuilder&;

  auto for_order(const MarketOrder& order) -> PlacementRejectBuilder&;

  auto with_reason(RejectText reason) -> PlacementRejectBuilder&;

  auto with_order_id(OrderId identifier) -> PlacementRejectBuilder&;

  auto with_execution_id(ExecutionId identifier) -> PlacementRejectBuilder&;

 private:
  protocol::OrderPlacementReject message_;
};

[[nodiscard]]
auto prepare_placement_reject(const protocol::OrderPlacementRequest& request,
                              OrderId rejected_order_id)
    -> PlacementRejectBuilder;

[[nodiscard]]
auto prepare_placement_reject(const LimitOrder& order)
    -> PlacementRejectBuilder;

[[nodiscard]]
 auto prepare_placement_reject(const MarketOrder& order)
    -> PlacementRejectBuilder;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_PLACEMENT_REPLY_BUILDERS_HPP_