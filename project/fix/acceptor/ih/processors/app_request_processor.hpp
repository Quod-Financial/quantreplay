#ifndef SIMULATOR_ACCEPTOR_IH_PROCESSORS_APP_REQUEST_PROCESSOR_HPP_
#define SIMULATOR_ACCEPTOR_IH_PROCESSORS_APP_REQUEST_PROCESSOR_HPP_

#include <quickfix/Exceptions.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <concepts>
#include <string>

#include "common/meta.hpp"
#include "common/session_conversion.hpp"
#include "ih/processors/request_processor.hpp"
#include "middleware/routing/trading_request_channel.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix::acceptor {

// clang-format off
template <typename Mapper>
concept RequestMapperConcept =
    std::default_initializable<Mapper> &&
    RequestMessageMapper<Mapper, FIX::Message, protocol::OrderPlacementRequest> &&
    RequestMessageMapper<Mapper, FIX::Message, protocol::OrderModificationRequest> &&
    RequestMessageMapper<Mapper, FIX::Message, protocol::OrderCancellationRequest> &&
    RequestMessageMapper<Mapper, FIX::Message, protocol::MarketDataRequest> &&
    RequestMessageMapper<Mapper, FIX::Message, protocol::SecurityStatusRequest>;
// clang-format on

template <typename MapperType>
  requires RequestMapperConcept<MapperType>
class AppRequestProcessor final : public RequestProcessor {
 public:
  auto process_fix_request(const FIX::Message& fix_message,
                           const FIX::SessionID& fix_session) const
      -> void override;

 private:
  template <typename InternalMessageType>
  auto dispatch_as(const FIX::Message& fix_message,
                   const FIX::SessionID& fix_session) const -> void;

  [[nodiscard]]
  static auto get_message_type(const FIX::Message& fix_message) -> std::string;

  MapperType message_mapper_;
};

template <typename MapperType>
  requires RequestMapperConcept<MapperType>
auto AppRequestProcessor<MapperType>::process_fix_request(
    const FIX::Message& fix_message, const FIX::SessionID& fix_session) const
    -> void {
  const std::string message_type = get_message_type(fix_message);
  if (message_type == FIX::MsgType_NewOrderSingle) {
    dispatch_as<protocol::OrderPlacementRequest>(fix_message, fix_session);
  } else if (message_type == FIX::MsgType_OrderCancelReplaceRequest) {
    dispatch_as<protocol::OrderModificationRequest>(fix_message, fix_session);
  } else if (message_type == FIX::MsgType_OrderCancelRequest) {
    dispatch_as<protocol::OrderCancellationRequest>(fix_message, fix_session);
  } else if (message_type == FIX::MsgType_MarketDataRequest) {
    dispatch_as<protocol::MarketDataRequest>(fix_message, fix_session);
  } else if (message_type == FIX::MsgType_SecurityStatusRequest) {
    dispatch_as<protocol::SecurityStatusRequest>(fix_message, fix_session);
  } else {
    throw FIX::UnsupportedMessageType(message_type);
  }
}

template <typename MapperType>
  requires RequestMapperConcept<MapperType>
template <typename InternalMessageType>
auto AppRequestProcessor<MapperType>::dispatch_as(
    const FIX::Message& fix_message, const FIX::SessionID& fix_session) const
    -> void {
  InternalMessageType internal_message{decode_session(fix_session)};

  map_sender_sub_id(fix_message, internal_message.session);
  message_mapper_.map(fix_message, internal_message);

  middleware::send_trading_request(std::move(internal_message));
}

template <typename MapperType>
  requires RequestMapperConcept<MapperType>
auto AppRequestProcessor<MapperType>::get_message_type(
    const FIX::Message& fix_message) -> std::string {
  FIX::MsgType message_type;
  if (fix_message.getHeader().getFieldIfSet(message_type)) {
    return {message_type};
  }
  throw FIX::FieldNotFound(message_type.getTag());
}

}  // namespace simulator::fix::acceptor

#endif  // SIMULATOR_ACCEPTOR_IH_PROCESSORS_APP_REQUEST_PROCESSOR_HPP_