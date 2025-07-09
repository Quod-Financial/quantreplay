#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_CANCELLATION_REPLY_BUILDERS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_CANCELLATION_REPLY_BUILDERS_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

class CancellationConfirmationBuilder {
 public:
  explicit CancellationConfirmationBuilder(protocol::Session session);

  [[nodiscard]]
  auto build() const -> protocol::OrderCancellationConfirmation;

  auto for_order(const LimitOrder& order) -> CancellationConfirmationBuilder&;

  auto with_execution_id(ExecutionId identifier)
      -> CancellationConfirmationBuilder&;

  auto with_client_order_id(std::optional<ClientOrderId> identifier)
      -> CancellationConfirmationBuilder&;

  auto with_orig_client_order_id(std::optional<OrigClientOrderId> identifier)
      -> CancellationConfirmationBuilder&;

 private:
  protocol::OrderCancellationConfirmation message_;
};

[[nodiscard]]
auto prepare_cancellation_confirmation(const LimitOrder& order)
    -> CancellationConfirmationBuilder;

class CancellationRejectBuilder {
 public:
  explicit CancellationRejectBuilder(protocol::Session session);

  [[nodiscard]]
  auto build() const -> protocol::OrderCancellationReject;

  auto for_request(const protocol::OrderCancellationRequest& request)
      -> CancellationRejectBuilder&;

  auto with_reason(RejectText reason) -> CancellationRejectBuilder&;

  auto with_order_status(OrderStatus status) -> CancellationRejectBuilder&;

  auto with_order_id(std::optional<OrderId> identifier)
      -> CancellationRejectBuilder&;

  auto with_client_order_id(std::optional<ClientOrderId> identifier)
      -> CancellationRejectBuilder&;

  auto with_orig_client_order_id(std::optional<OrigClientOrderId> identifier)
      -> CancellationRejectBuilder&;

 private:
  protocol::OrderCancellationReject message_;
};

[[nodiscard]]
auto prepare_cancellation_reject(
    const protocol::OrderCancellationRequest& request)
    -> CancellationRejectBuilder;

[[nodiscard]]
auto prepare_cancellation_reject(const OrderCancel& cancel)
    -> CancellationRejectBuilder;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_CANCELLATION_REPLY_BUILDERS_HPP_