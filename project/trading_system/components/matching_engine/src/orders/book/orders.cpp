#include <fmt/format.h>

#include <cassert>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <utility>

#include "core/tools/time.hpp"
#include "idgen/execution_id.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_metadata.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

// region OrderAttributes

auto OrderAttributes::client_order_id() const
    -> const std::optional<ClientOrderId>& {
  return client_order_id_;
}

auto OrderAttributes::time_in_force() const -> TimeInForce {
  return time_in_force_;
}

auto OrderAttributes::expire_time() const -> std::optional<ExpireTime> {
  return expire_time_;
}

auto OrderAttributes::expire_date() const -> std::optional<ExpireDate> {
  return expire_date_;
}

auto OrderAttributes::short_sale_exemption_reason() const
    -> std::optional<ShortSaleExemptionReason> {
  return short_sale_exemption_reason_;
}

auto OrderAttributes::order_parties() const -> const std::vector<Party>& {
  return order_parties_;
}

auto OrderAttributes::order_owner() const -> std::optional<Party> {
  constexpr auto owner_pred = [](const Party& party) {
    return party.role() == PartyRole::Option::ExecutingFirm;
  };

  if (const auto owner_it = std::ranges::find_if(order_parties_, owner_pred);
      owner_it != std::end(order_parties_)) {
    return std::make_optional(*owner_it);
  }
  return std::nullopt;
}

auto OrderAttributes::set_client_order_id(ClientOrderId client_order_id)
    -> void {
  client_order_id_ = std::move(client_order_id);
}

auto OrderAttributes::set_time_in_force(TimeInForce time_in_force) -> void {
  time_in_force_ = time_in_force;
}

auto OrderAttributes::set_expire_time(ExpireTime expire_time) -> void {
  expire_time_ = std::move(expire_time);
}

auto OrderAttributes::set_expire_date(ExpireDate expire_date) -> void {
  expire_date_ = std::move(expire_date);
}

auto OrderAttributes::set_short_sale_exemption_reason(
    ShortSaleExemptionReason short_sale_exemption_reason) -> void {
  short_sale_exemption_reason_ = short_sale_exemption_reason;
}

auto OrderAttributes::set_order_parties(std::vector<Party> parties) -> void {
  std::swap(order_parties_, parties);
}

// endregion OrderAttributes

// region OrderRecord

OrderRecord::OrderRecord(OrderId order_id,
                         Side order_side,
                         protocol::Session client_session,
                         InstrumentDescriptor client_instrument_descriptor,
                         OrderAttributes order_attributes)
    : client_instrument_descriptor_(std::move(client_instrument_descriptor)),
      client_session_(std::move(client_session)),
      order_attributes_(std::move(order_attributes)),
      exec_id_generator_(order::ExecIdGenerator::create(order_id)),
      order_id_(order_id),
      order_time_(core::get_current_system_time()),
      order_side_(order_side),
      order_status_(OrderStatus::Option::New) {}

auto OrderRecord::attributes() const -> const OrderAttributes& {
  return order_attributes_;
}

auto OrderRecord::instrument() const -> const InstrumentDescriptor& {
  return client_instrument_descriptor_;
}

auto OrderRecord::client_session() const -> const protocol::Session& {
  return client_session_;
}

auto OrderRecord::order_id() const -> OrderId { return order_id_; }

auto OrderRecord::order_side() const -> Side { return order_side_; }

auto OrderRecord::order_time() const -> OrderTime { return order_time_; }

auto OrderRecord::order_status() const -> OrderStatus { return order_status_; }

auto OrderRecord::set_order_attributes(OrderAttributes attributes) -> void {
  std::swap(order_attributes_, attributes);
}

auto OrderRecord::set_order_time(OrderTime time) -> void { order_time_ = time; }

auto OrderRecord::set_order_status(OrderStatus status) -> void {
  order_status_ = status;
}

auto OrderRecord::make_execution_id() -> ExecutionId {
  return std::invoke(*exec_id_generator_);
}

// endregion OrderRecord

// region LimitOrder

LimitOrder::LimitOrder(OrderPrice price,
                       OrderQuantity quantity,
                       OrderRecord record)
    : record_(std::make_shared<OrderRecord>(std::move(record))),
      price_(price),
      total_quantity_(quantity),
      cum_executed_quantity_(0.0) {}

auto LimitOrder::id() const -> OrderId {
  assert(record_);
  return record_->order_id();
}

auto LimitOrder::attributes() const -> const OrderAttributes& {
  assert(record_);
  return record_->attributes();
}

auto LimitOrder::owner() const -> std::optional<Party> {
  assert(record_);
  return record_->attributes().order_owner();
}

auto LimitOrder::side() const -> Side {
  assert(record_);
  return record_->order_side();
}

auto LimitOrder::status() const -> OrderStatus {
  assert(record_);
  return record_->order_status();
}

auto LimitOrder::client_session() const -> const protocol::Session& {
  assert(record_);
  return record_->client_session();
}

auto LimitOrder::client_order_id() const
    -> const std::optional<ClientOrderId>& {
  return attributes().client_order_id();
}

auto LimitOrder::instrument() const -> const InstrumentDescriptor& {
  assert(record_);
  return record_->instrument();
}

auto LimitOrder::time_in_force() const -> TimeInForce {
  assert(record_);
  return record_->attributes().time_in_force();
}

auto LimitOrder::expire_time() const -> std::optional<ExpireTime> {
  assert(record_);
  return record_->attributes().expire_time();
}

auto LimitOrder::expire_date() const -> std::optional<ExpireDate> {
  assert(record_);
  return record_->attributes().expire_date();
}

auto LimitOrder::short_sale_exemption_reason() const
    -> std::optional<ShortSaleExemptionReason> {
  assert(record_);
  return record_->attributes().short_sale_exemption_reason();
}

auto LimitOrder::price() const -> OrderPrice { return price_; }

auto LimitOrder::total_quantity() const -> OrderQuantity {
  return total_quantity_;
}

auto LimitOrder::cum_executed_quantity() const -> CumExecutedQuantity {
  return cum_executed_quantity_;
}

auto LimitOrder::leaves_quantity() const -> LeavesQuantity {
  const auto total = static_cast<double>(total_quantity_);
  const auto executed = static_cast<double>(cum_executed_quantity_);
  return LeavesQuantity{std::max(total - executed, 0.0)};
}

auto LimitOrder::time() const -> OrderTime {
  assert(record_);
  return record_->order_time();
}

auto LimitOrder::executed() const -> bool {
  return static_cast<double>(cum_executed_quantity_) >=
         static_cast<double>(total_quantity_);
}

auto LimitOrder::make_execution_id() -> ExecutionId {
  assert(record_);
  return record_->make_execution_id();
}

auto LimitOrder::execute(ExecutedQuantity quantity) -> void {
  const auto prev_executed = static_cast<double>(cum_executed_quantity_);
  const auto curr_executed = static_cast<double>(quantity);
  cum_executed_quantity_ = CumExecutedQuantity{prev_executed + curr_executed};

  assert(record_);
  record_->set_order_status(executed() ? OrderStatus::Option::Filled
                                       : OrderStatus::Option::PartiallyFilled);
}

auto LimitOrder::amend(Update update) -> void {
  if (static_cast<double>(update.quantity) <=
      static_cast<double>(cum_executed_quantity_)) [[unlikely]] {
    throw std::logic_error(fmt::format(
        "cannot amend limit order - invalid quantity '{}'", update.quantity));
  }

  assert(record_);
  record_->set_order_status(OrderStatus::Option::Modified);
  record_->set_order_attributes(std::move(update.attributes));
  if (update.price != price_ || update.quantity > total_quantity_) {
    record_->set_order_time(OrderTime(core::get_current_system_time()));
  }

  price_ = update.price;
  total_quantity_ = update.quantity;
}

auto LimitOrder::cancel() -> void {
  assert(record_);
  record_->set_order_status(OrderStatus::Option::Cancelled);
}

// endregion LimitOrder

LimitUpdate::LimitUpdate(protocol::Session session,
                         Side side,
                         LimitOrder::Update update)
    : client_session(std::move(session)),
      order_diff(std::move(update)),
      order_side(side) {}

OrderCancel::OrderCancel(protocol::Session session, Side side)
    : client_session(std::move(session)), order_side(side) {}

// region MarketOrder

MarketOrder::MarketOrder(OrderQuantity quantity, OrderRecord record)
    : record_(std::make_shared<OrderRecord>(std::move(record))),
      total_quantity_(quantity),
      cum_executed_quantity_(0.0) {}

auto MarketOrder::id() const -> OrderId {
  assert(record_);
  return record_->order_id();
}

auto MarketOrder::attributes() const -> const OrderAttributes& {
  assert(record_);
  return record_->attributes();
}

auto MarketOrder::owner() const -> std::optional<Party> {
  assert(record_);
  return record_->attributes().order_owner();
}

auto MarketOrder::side() const -> Side {
  assert(record_);
  return record_->order_side();
}

auto MarketOrder::status() const -> OrderStatus {
  assert(record_);
  return record_->order_status();
}

auto MarketOrder::client_session() const -> const protocol::Session& {
  assert(record_);
  return record_->client_session();
}

auto MarketOrder::client_order_id() const
    -> const std::optional<ClientOrderId>& {
  return attributes().client_order_id();
}

auto MarketOrder::instrument() const -> const InstrumentDescriptor& {
  assert(record_);
  return record_->instrument();
}

auto MarketOrder::time_in_force() const -> TimeInForce {
  assert(record_);
  return record_->attributes().time_in_force();
}

auto MarketOrder::expire_time() const -> std::optional<ExpireTime> {
  assert(record_);
  return record_->attributes().expire_time();
}

auto MarketOrder::expire_date() const -> std::optional<ExpireDate> {
  assert(record_);
  return record_->attributes().expire_date();
}

auto MarketOrder::short_sale_exemption_reason() const
    -> std::optional<ShortSaleExemptionReason> {
  assert(record_);
  return record_->attributes().short_sale_exemption_reason();
}

auto MarketOrder::total_quantity() const -> OrderQuantity {
  return total_quantity_;
}

auto MarketOrder::cum_executed_quantity() const -> CumExecutedQuantity {
  return cum_executed_quantity_;
}

auto MarketOrder::leaves_quantity() const -> LeavesQuantity {
  const auto total = static_cast<double>(total_quantity_);
  const auto executed = static_cast<double>(cum_executed_quantity_);
  return LeavesQuantity{std::max(total - executed, 0.0)};
}

auto MarketOrder::executed() const -> bool {
  return static_cast<double>(cum_executed_quantity_) >=
         static_cast<double>(total_quantity_);
}

auto MarketOrder::execute(ExecutedQuantity quantity) -> void {
  const auto prev_executed = static_cast<double>(cum_executed_quantity_);
  const auto curr_executed = static_cast<double>(quantity);
  cum_executed_quantity_ = CumExecutedQuantity{prev_executed + curr_executed};

  assert(record_);
  record_->set_order_status(executed() ? OrderStatus::Option::Filled
                                       : OrderStatus::Option::PartiallyFilled);
}

auto MarketOrder::cancel() -> void {
  assert(record_);
  record_->set_order_status(OrderStatus::Option::Cancelled);
}

auto MarketOrder::make_execution_id() -> ExecutionId {
  assert(record_);
  return record_->make_execution_id();
}

// endregion MarketOrder

}  // namespace simulator::trading_system::matching_engine

namespace fmt {

auto formatter<simulator::trading_system::matching_engine::LimitOrder>::format(
    const formattable& order, format_context& context) const
    -> decltype(context.out()) {
  return format_to(
      context.out(),
      R"({{ "LimitOrder": {{ "id":{}, "status":"{}", "side":"{}", "price":{}, "cum_executed_qty":{}, "leaves_qty":{} }} }})",
      order.id(),
      order.status(),
      order.side(),
      order.price(),
      order.cum_executed_quantity(),
      order.leaves_quantity());
}

auto formatter<simulator::trading_system::matching_engine::MarketOrder>::format(
    const formattable& order, format_context& context) const
    -> decltype(context.out()) {
  return format_to(
      context.out(),
      R"({{ "MarketOrder": {{ "id":{}, "status":"{}", "side":"{}", "cum_executed_qty":{}, "leaves_qty":{} }} }})",
      order.id(),
      order.status(),
      order.side(),
      order.cum_executed_quantity(),
      order.leaves_quantity());
}

}  // namespace fmt
