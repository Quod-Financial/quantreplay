#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_MODIFICATION_REPLY_BUILDERS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_MODIFICATION_REPLY_BUILDERS_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

class ModificationConfirmationBuilder {
 public:
  explicit ModificationConfirmationBuilder(protocol::Session client_session);

  [[nodiscard]]
  auto build() const -> protocol::OrderModificationConfirmation;

  auto for_order(const LimitOrder& order) -> ModificationConfirmationBuilder&;

  auto with_execution_id(ExecutionId identifier)
      -> ModificationConfirmationBuilder&;

  auto with_orig_client_order_id(std::optional<OrigClientOrderId> identifier)
      -> ModificationConfirmationBuilder&;

 private:
  protocol::OrderModificationConfirmation message_;
};

[[nodiscard]]
auto prepare_modification_confirmation(const LimitOrder& order)
    -> ModificationConfirmationBuilder;

class ModificationRejectBuilder {
 public:
  explicit ModificationRejectBuilder(protocol::Session session);

  [[nodiscard]]
  auto build() const -> protocol::OrderModificationReject;

  auto for_request(const protocol::OrderModificationRequest& request)
      -> ModificationRejectBuilder&;

  auto with_reason(RejectText reject_reason) -> ModificationRejectBuilder&;

  auto with_order_status(OrderStatus status) -> ModificationRejectBuilder&;

  auto with_order_id(std::optional<OrderId> identifier)
      -> ModificationRejectBuilder&;

  auto with_client_order_id(std::optional<ClientOrderId> identifier)
      -> ModificationRejectBuilder&;

  auto with_orig_client_order_id(std::optional<OrigClientOrderId> identifier)
      -> ModificationRejectBuilder&;

 private:
  protocol::OrderModificationReject message_;
};

[[nodiscard]]
auto prepare_modification_reject(
    const protocol::OrderModificationRequest& request)
    -> ModificationRejectBuilder;

[[nodiscard]]
auto prepare_modification_reject(const LimitUpdate& update)
    -> ModificationRejectBuilder;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_MODIFICATION_REPLY_BUILDERS_HPP_