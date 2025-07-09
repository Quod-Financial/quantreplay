#ifndef SIMULATOR_GENERATOR_IH_UTILS_REQUEST_BUILDER_HPP_
#define SIMULATOR_GENERATOR_IH_UTILS_REQUEST_BUILDER_HPP_

#include <optional>

#include "ih/adaptation/generated_message.hpp"

namespace simulator::generator {

class RequestBuilder {
 public:
  auto make_new_order_request() noexcept -> RequestBuilder&;

  auto make_modification_request() noexcept -> RequestBuilder&;

  auto make_cancel_request() noexcept -> RequestBuilder&;

  auto with_clordid(ClientOrderId clordid) noexcept -> RequestBuilder&;

  auto with_orig_clordid(OrigClientOrderId orig_clordid) noexcept
      -> RequestBuilder&;

  auto with_counterparty(PartyId counterparty) noexcept -> RequestBuilder&;

  auto with_aggressive_attributes() noexcept -> RequestBuilder&;

  auto with_resting_attributes() noexcept -> RequestBuilder&;

  auto with_price(OrderPrice price) noexcept -> RequestBuilder&;

  auto with_quantity(Quantity quantity) noexcept -> RequestBuilder&;

  auto with_side(Side side) noexcept -> RequestBuilder&;

  static auto construct(RequestBuilder&& builder) -> GeneratedMessage;

 private:
  static auto validate(const RequestBuilder& builder) -> void;

  std::optional<ClientOrderId> target_clordid_;
  std::optional<OrigClientOrderId> target_orig_clordid_;
  std::optional<PartyId> target_cpid_;

  std::optional<OrderPrice> target_price_;
  std::optional<Quantity> target_qty_;

  std::optional<MessageType> target_msg_type_;
  std::optional<TimeInForce> target_tif_;
  std::optional<OrderType> target_ord_type_;
  std::optional<Side> target_side_;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_UTILS_REQUEST_BUILDER_HPP_
