#include "ih/registry/registry_updater.hpp"

#include <cassert>
#include <stdexcept>
#include <utility>

#include "ih/constants.hpp"
#include "ih/registry/generated_order_data.hpp"
#include "log/logging.hpp"

namespace simulator::generator {
namespace {

auto validate_client_order_id(const GeneratedMessage& message) -> void {
  if (!message.client_order_id.has_value() ||
      message.client_order_id->value().empty()) {
    throw std::invalid_argument{fmt::format(
        "registry updater expects a `{}' message to have non-empty ClOrdID",
        message.message_type)};
  }
}

auto validate_party_id(const GeneratedMessage& message) -> void {
  if (!message.party.has_value() || message.party->party_id().value().empty()) {
    throw std::invalid_argument{
        fmt::format("registry updater expects a `{}' message to have non-empty "
                    "1 counterparty (owner)",
                    message.message_type)};
  }
}

auto validate_side(const GeneratedMessage& message) -> void {
  if (!message.side.has_value()) {
    throw std::invalid_argument{
        fmt::format("registry updater expects a `{}' message to have Side",
                    message.message_type)};
  }
}

auto validate_order_status(const GeneratedMessage& message) -> void {
  if (!message.order_status.has_value()) {
    throw std::invalid_argument{fmt::format(
        "registry updater expects a `{}' message to have OrderStatus",
        message.message_type)};
  }
}

}  // namespace

OrderRegistryUpdater::OrderRegistryUpdater(
    GeneratedOrdersRegistry& registry) noexcept
    : registry_ref_{registry} {}

void OrderRegistryUpdater::update(GeneratedOrdersRegistry& registry,
                                  const GeneratedMessage& message) {
  OrderRegistryUpdater updater{registry};
  updater.update(message);
}

auto OrderRegistryUpdater::update(const GeneratedMessage& message) -> void {
  // Reject is not handled in this implementation:
  // not clear when a matching engine sends reject messages
  switch (message.message_type) {
    case MessageType::NewOrderSingle:
      handle_new_order(message);
      break;
    case MessageType::OrderCancelReplaceRequest:
      handle_modification(message);
      break;
    case MessageType::OrderCancelRequest:
      handle_cancellation(message);
      break;
    case MessageType::ExecutionReport:
      handle_execution(message);
      break;
    default: {
      log::debug(
          "generated orders registry updater received a `{}' "
          "order message, which will not be processed "
          "to update the registry",
          message.message_type);
      break;
    }
  }
}

auto OrderRegistryUpdater::handle_new_order(const GeneratedMessage& message)
    -> void {
  assert(message.message_type == MessageType::NewOrderSingle);

  if (!contains_resting_order(message)) {
    return;
  }

  validate_new_order(message);

  const auto& owner_id = message.party->party_id();
  const auto& order_id = message.client_order_id;
  const auto side = message.side;

  GeneratedOrderData::Builder ord_builder{owner_id, *order_id, *side};
  ord_builder.set_price(message.order_price.value_or(OrderPrice{0.}))
      .set_quantity(message.quantity.value_or(Quantity{0.}));

  GeneratedOrderData order{std::move(ord_builder)};
  const bool inserted = registry().add(std::move(order));
  if (!inserted) {
    log::warn(
        "generated orders registry updater failed to register a "
        "new generated order with `{0}' identifier for counterparty `{1}'",
        owner_id,
        owner_id);
  }
}

auto OrderRegistryUpdater::handle_modification(const GeneratedMessage& message)
    -> void {
  assert(message.message_type == MessageType::OrderCancelReplaceRequest);

  if (!contains_resting_order(message)) {
    return;
  }

  validate_modification(message);

  const auto& owner_id = message.party->party_id().value();
  const auto& order_id = *message.client_order_id;

  GeneratedOrderData::Patch update;
  update.set_updated_id(order_id);
  if (const auto price = message.order_price) {
    update.set_updated_price(*price);
  }
  if (const auto qty = message.quantity) {
    update.set_updated_quantity(*qty);
  }

  const bool updated = registry().update_by_owner(owner_id, std::move(update));
  if (!updated) {
    log::warn(
        "generated orders registry updater failed to update an order "
        "with new ID `{0}' for `{1}' counterparty as no active order "
        "was found for that counterparty",
        order_id,
        owner_id);
  }
}

auto OrderRegistryUpdater::handle_cancellation(const GeneratedMessage& message)
    -> void {
  assert(message.message_type == MessageType::OrderCancelRequest);

  if (!contains_resting_order(message)) {
    return;
  }

  validate_cancellation(message);

  const auto& owner_id = message.party->party_id().value();
  const bool removed = registry().remove_by_owner(owner_id);
  if (!removed) {
    log::warn(
        "generated orders registry updater failed to remove an order "
        "for `{0}' counterparty as no active order was found "
        "for that counterparty",
        owner_id);
  }
}

auto OrderRegistryUpdater::handle_execution(const GeneratedMessage& message)
    -> void {
  assert(message.message_type == MessageType::ExecutionReport);

  validate_execution(message);
  const auto& order_id = message.client_order_id->value();

  // We don't care about the registry response when handling execution
  // It's possible that we received a message for an order that is not stored
  // (f.e. aggressive order), no need to log unsuccessful operations
  switch (*message.order_status) {
    case OrderStatus::Option::PartiallyFilled: {
      // Only an order qty can be updated on partial execution
      if (const auto quantity = message.quantity) {
        GeneratedOrderData::Patch update;
        update.set_updated_quantity(*quantity);
        registry().update_by_identifier(order_id, std::move(update));
      }
      break;
    }
    case OrderStatus::Option::Filled:
    case OrderStatus::Option::Cancelled:
    case OrderStatus::Option::Rejected: {
      registry().remove_by_identifier(order_id);
      break;
    }
    case OrderStatus::Option::New:
    case OrderStatus::Option::Modified:
      break;
  }
}

auto OrderRegistryUpdater::registry() noexcept -> GeneratedOrdersRegistry& {
  return registry_ref_.get();
}

auto OrderRegistryUpdater::contains_resting_order(
    const GeneratedMessage& message) -> bool {
  const auto ord_type = message.order_type;
  const auto tif = message.time_in_force;

  assert(message.message_type == MessageType::NewOrderSingle ||
         message.message_type == MessageType::OrderCancelReplaceRequest ||
         message.message_type == MessageType::OrderCancelRequest);

  const bool is_resting_order = tif == constant::RestingTimeInForce &&
                                ord_type == constant::RestingOrderType;

  if (!is_resting_order) {
    log::warn(
        "Generator's order registry updater ignores a `{}' message, "
        "message contains a non-resting order (order_type={}, "
        "time_in_force={})",
        message.message_type,
        message.order_type,
        message.time_in_force);
  }

  return is_resting_order;
}

auto OrderRegistryUpdater::validate_new_order(const GeneratedMessage& message)
    -> void {
  validate_client_order_id(message);
  validate_party_id(message);
  validate_side(message);
}

auto OrderRegistryUpdater::validate_modification(
    const GeneratedMessage& message) -> void {
  validate_client_order_id(message);
  validate_party_id(message);
}

auto OrderRegistryUpdater::validate_cancellation(
    const GeneratedMessage& message) -> void {
  validate_party_id(message);
}

auto OrderRegistryUpdater::validate_execution(const GeneratedMessage& message)
    -> void {
  validate_client_order_id(message);
  validate_order_status(message);
}

}  // namespace simulator::generator
