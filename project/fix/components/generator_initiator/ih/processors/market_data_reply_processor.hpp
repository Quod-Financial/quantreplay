#ifndef SIMULATOR_GENERATOR_INITIATOR_IH_PROCESSORS_MARKET_DATA_REPLY_PROCESSOR_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_IH_PROCESSORS_MARKET_DATA_REPLY_PROCESSOR_HPP_

#include <quickfix/Exceptions.h>
#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <string>
#include <utility>

#include "common/processors/message_processor.hpp"
#include "common/session_conversion.hpp"
#include "ih/mapping/mapper_concepts.hpp"
#include "log/logging.hpp"
#include "middleware/routing/market_data_reply_channel.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"

namespace simulator::fix::generator_initiator {

template <typename MapperType>
  requires MarketDataReplyMapperConcept<MapperType>
class MarketDataReplyProcessor final : public MessageProcessor {
 public:
  auto process_message(const FIX::Message& fix_message,
                       const FIX::SessionID& fix_session) const
      -> void override;

 private:
  auto process_snapshot(const FIX::Message& fix_message,
                        const FIX::SessionID& fix_session) const -> void;

  auto process_update(const FIX::Message& fix_message,
                      const FIX::SessionID& fix_session) const -> void;

  auto process_reject(const FIX::Message& fix_message,
                      const FIX::SessionID& fix_session) const -> void;

  [[nodiscard]]
  static auto get_message_type(const FIX::Message& fix_message) -> std::string;

  MapperType message_mapper_;
};

template <typename MapperType>
  requires MarketDataReplyMapperConcept<MapperType>
auto MarketDataReplyProcessor<MapperType>::process_message(
    const FIX::Message& fix_message, const FIX::SessionID& fix_session) const
    -> void {
  const std::string message_type = get_message_type(fix_message);
  if (message_type == FIX::MsgType_MarketDataSnapshotFullRefresh) {
    process_snapshot(fix_message, fix_session);
  } else if (message_type == FIX::MsgType_MarketDataIncrementalRefresh) {
    process_update(fix_message, fix_session);
  } else if (message_type == FIX::MsgType_MarketDataRequestReject) {
    process_reject(fix_message, fix_session);
  } else {
    throw FIX::UnsupportedMessageType{message_type};
  }
}

template <typename MapperType>
  requires MarketDataReplyMapperConcept<MapperType>
auto MarketDataReplyProcessor<MapperType>::process_snapshot(
    const FIX::Message& fix_message, const FIX::SessionID& fix_session) const
    -> void {
  protocol::MarketDataSnapshot snapshot{decode_session(fix_session)};
  message_mapper_.map(fix_message, snapshot);

  middleware::send_market_data_reply(std::move(snapshot));
}

template <typename MapperType>
  requires MarketDataReplyMapperConcept<MapperType>
auto MarketDataReplyProcessor<MapperType>::process_update(
    const FIX::Message& fix_message, const FIX::SessionID& fix_session) const
    -> void {
  protocol::MarketDataUpdate update{decode_session(fix_session)};
  message_mapper_.map(fix_message, update);

  // An incremental refresh carries no instrument of its own, so a message
  // without a request id cannot be attributed to a subscription.
  if (!update.request_id.has_value()) {
    log::warn(
        "ignoring a market data incremental refresh without a request id, "
        "it can not be attributed to a subscription");
    return;
  }

  middleware::send_market_data_reply(std::move(update));
}

template <typename MapperType>
  requires MarketDataReplyMapperConcept<MapperType>
auto MarketDataReplyProcessor<MapperType>::process_reject(
    const FIX::Message& fix_message, const FIX::SessionID& fix_session) const
    -> void {
  protocol::MarketDataReject reject{decode_session(fix_session)};
  message_mapper_.map(fix_message, reject);

  middleware::send_market_data_reply(std::move(reject));
}

template <typename MapperType>
  requires MarketDataReplyMapperConcept<MapperType>
auto MarketDataReplyProcessor<MapperType>::get_message_type(
    const FIX::Message& fix_message) -> std::string {
  FIX::MsgType message_type;
  if (fix_message.getHeader().getFieldIfSet(message_type)) {
    return {message_type};
  }
  throw FIX::FieldNotFound{message_type.getTag()};
}

}  // namespace simulator::fix::generator_initiator

#endif  // SIMULATOR_GENERATOR_INITIATOR_IH_PROCESSORS_MARKET_DATA_REPLY_PROCESSOR_HPP_
