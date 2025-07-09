#include "ih/utils/request_builder.hpp"

#include <cassert>
#include <stdexcept>

#include "ih/constants.hpp"

namespace simulator::generator {

auto RequestBuilder::make_new_order_request() noexcept -> RequestBuilder& {
  target_msg_type_ = MessageType::NewOrderSingle;
  return *this;
}

auto RequestBuilder::make_modification_request() noexcept -> RequestBuilder& {
  target_msg_type_ = MessageType::OrderCancelReplaceRequest;
  return *this;
}

auto RequestBuilder::make_cancel_request() noexcept -> RequestBuilder& {
  target_msg_type_ = MessageType::OrderCancelRequest;
  return *this;
}

auto RequestBuilder::with_clordid(ClientOrderId clordid) noexcept
    -> RequestBuilder& {
  target_clordid_ = std::make_optional(std::move(clordid));
  return *this;
}

auto RequestBuilder::with_orig_clordid(OrigClientOrderId orig_clordid) noexcept
    -> RequestBuilder& {
  target_orig_clordid_ = std::make_optional(std::move(orig_clordid));
  return *this;
}

auto RequestBuilder::with_counterparty(PartyId counterparty) noexcept
    -> RequestBuilder& {
  target_cpid_ = std::make_optional(std::move(counterparty));
  return *this;
}

auto RequestBuilder::with_aggressive_attributes() noexcept -> RequestBuilder& {
  target_ord_type_ = generator::constant::AggressiveOrderType;
  target_tif_ = generator::constant::AggressiveTimeInForce;
  return *this;
}

auto RequestBuilder::with_resting_attributes() noexcept -> RequestBuilder& {
  target_ord_type_ = generator::constant::RestingOrderType;
  target_tif_ = generator::constant::RestingTimeInForce;
  return *this;
}

auto RequestBuilder::with_price(OrderPrice price) noexcept -> RequestBuilder& {
  target_price_ = std::make_optional(price);
  return *this;
}

auto RequestBuilder::with_quantity(Quantity quantity) noexcept
    -> RequestBuilder& {
  target_qty_ = std::make_optional(quantity);
  return *this;
}

auto RequestBuilder::with_side(Side side) noexcept -> RequestBuilder& {
  target_side_ = std::make_optional(side);
  return *this;
}

auto RequestBuilder::construct(RequestBuilder&& builder) -> GeneratedMessage {
  validate(builder);

  GeneratedMessage message;

  assert(builder.target_msg_type_.has_value());
  message.message_type = *builder.target_msg_type_;

  assert(builder.target_ord_type_);
  message.order_type = builder.target_ord_type_;

  assert(builder.target_tif_);
  message.time_in_force = builder.target_tif_;

  assert(builder.target_side_);
  message.side = builder.target_side_;

  assert(builder.target_clordid_);
  message.client_order_id = std::move(builder.target_clordid_);

  message.orig_client_order_id = std::move(builder.target_orig_clordid_);
  message.party_id = std::move(builder.target_cpid_);
  message.order_price = builder.target_price_;
  message.quantity = builder.target_qty_;

  return message;
}

auto RequestBuilder::validate(const RequestBuilder& builder) -> void {
  if (!builder.target_msg_type_.has_value()) {
    throw std::invalid_argument{
        "can not construct order request without a MessageType value"};
  }

  if (!builder.target_ord_type_.has_value()) {
    throw std::invalid_argument{
        "can not construct order request without a OrderType value"};
  }

  if (!builder.target_tif_.has_value()) {
    throw std::invalid_argument{
        "can not construct order request without a TimeInForce value"};
  }

  if (!builder.target_side_.has_value()) {
    throw std::invalid_argument{
        "can not construct order request without a Side value"};
  }

  if (!builder.target_clordid_.has_value()) {
    throw std::invalid_argument{
        "can not construct order request without a ClOrdID value"};
  }
}

}  // namespace simulator::generator