#ifndef SIMULATOR_ACCEPTOR_IH_PROCESSORS_APP_REPLY_PROCESSOR_HPP_
#define SIMULATOR_ACCEPTOR_IH_PROCESSORS_APP_REPLY_PROCESSOR_HPP_

#include <quickfix/FixValues.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <concepts>
#include <functional>

#include "common/mapping/setting/mapping_settings.hpp"
#include "common/meta.hpp"
#include "common/session_conversion.hpp"
#include "ih/communicators/reply_sender.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/security_status.hpp"

namespace simulator::fix::acceptor {

// clang-format off
template <typename Mapper>
concept ReplyMapperConcept =
    std::default_initializable<Mapper> &&
    ReplyMessageMapper<Mapper, protocol::BusinessMessageReject, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::OrderPlacementConfirmation, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::OrderPlacementReject, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::OrderModificationConfirmation, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::OrderModificationReject, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::OrderCancellationConfirmation, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::OrderCancellationReject, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::MarketDataReject, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::MarketDataSnapshot, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::MarketDataUpdate, FIX::Message, MappingSettings::Setting> &&
    ReplyMessageMapper<Mapper, protocol::SecurityStatus, FIX::Message, MappingSettings::Setting>;
// clang-format on

template <typename Mapper>
  requires ReplyMapperConcept<Mapper>
class AppReplyProcessor {
 public:
  AppReplyProcessor(const ReplySender& reply_sender,
                    MappingSettings mapping_settings) noexcept
      : mapping_settings_{std::move(mapping_settings)},
        reply_sender_{reply_sender} {}

  auto process_reply(const protocol::BusinessMessageReject& reply) const
      -> void {
    send_as(FIX::MsgType{FIX::MsgType_BusinessMessageReject}, reply);
  }

  auto process_reply(const protocol::ExecutionReport& reply) const -> void {
    send_as(FIX::MsgType{FIX::MsgType_ExecutionReport}, reply);
  }

  auto process_reply(const protocol::OrderPlacementConfirmation& reply) const
      -> void {
    send_as(FIX::MsgType{FIX::MsgType_ExecutionReport}, reply);
  }

  auto process_reply(const protocol::OrderPlacementReject& reply) const
      -> void {
    send_as(FIX::MsgType{FIX::MsgType_ExecutionReport}, reply);
  }

  auto process_reply(const protocol::OrderModificationConfirmation& reply) const
      -> void {
    send_as(FIX::MsgType{FIX::MsgType_ExecutionReport}, reply);
  }

  auto process_reply(const protocol::OrderModificationReject& reply) const
      -> void {
    send_as(FIX::MsgType{FIX::MsgType_OrderCancelReject}, reply);
  }

  auto process_reply(const protocol::OrderCancellationConfirmation& reply) const
      -> void {
    send_as(FIX::MsgType{FIX::MsgType_ExecutionReport}, reply);
  }

  auto process_reply(const protocol::OrderCancellationReject& reply) const
      -> void {
    send_as(FIX::MsgType{FIX::MsgType_OrderCancelReject}, reply);
  }

  auto process_reply(const protocol::MarketDataReject& reply) const -> void {
    send_as(FIX::MsgType{FIX::MsgType_MarketDataRequestReject}, reply);
  }

  auto process_reply(const protocol::MarketDataSnapshot& reply) const -> void {
    send_as(FIX::MsgType{FIX::MsgType_MarketDataSnapshotFullRefresh}, reply);
  }

  auto process_reply(const protocol::MarketDataUpdate& reply) const -> void {
    send_as(FIX::MsgType{FIX::MsgType_MarketDataIncrementalRefresh}, reply);
  }

  auto process_reply(const protocol::SecurityStatus& reply) const -> void {
    send_as(FIX::MsgType{FIX::MsgType_SecurityStatus}, reply);
  }

 private:
  template <typename InternalMessageType>
  auto send_as(const FIX::MsgType& fix_message_type,
               const InternalMessageType& internal_message) const -> void;

  [[nodiscard]]
  static auto make_fix_message(const FIX::MsgType& fix_message_type)
      -> FIX::Message;

  Mapper mapper_;
  MappingSettings mapping_settings_;
  std::reference_wrapper<const ReplySender> reply_sender_;
};

template <typename Mapper>
  requires ReplyMapperConcept<Mapper>
template <typename InternalMessageType>
auto AppReplyProcessor<Mapper>::send_as(
    const FIX::MsgType& fix_message_type,
    const InternalMessageType& internal_message) const -> void {
  FIX::Message fix_message = make_fix_message(fix_message_type);
  const FIX::SessionID fix_session = encode_session(internal_message.session);

  map_target_sub_id(internal_message.session, fix_message);
  mapper_.map(internal_message,
              fix_message,
              mapping_settings_.get_setting(fix_session));

  reply_sender_.get().send_reply_message(fix_message, fix_session);
}

template <typename Mapper>
  requires ReplyMapperConcept<Mapper>
auto AppReplyProcessor<Mapper>::make_fix_message(
    const FIX::MsgType& fix_message_type) -> FIX::Message {
  FIX::Message fix_message;
  fix_message.getHeader().setField(fix_message_type);
  return fix_message;
}

}  // namespace simulator::fix::acceptor

#endif  // SIMULATOR_ACCEPTOR_IH_PROCESSORS_APP_REPLY_PROCESSOR_HPP_