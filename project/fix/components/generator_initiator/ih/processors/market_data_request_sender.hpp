#ifndef SIMULATOR_GENERATOR_INITIATOR_IH_PROCESSORS_MARKET_DATA_REQUEST_SENDER_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_IH_PROCESSORS_MARKET_DATA_REQUEST_SENDER_HPP_

#include <quickfix/FixValues.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <utility>

#include "common/communicators/message_sender.hpp"
#include "common/mapping/setting/mapping_settings.hpp"
#include "common/message.hpp"
#include "common/session_conversion.hpp"
#include "ih/mapping/mapper_concepts.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::fix::generator_initiator {

template <typename Mapper>
  requires MarketDataRequestMapperConcept<Mapper>
class MarketDataRequestSender {
 public:
  MarketDataRequestSender(const MessageSender& message_sender,
                          MappingSettings mapping_settings) noexcept
      : mapping_settings_{std::move(mapping_settings)},
        message_sender_{message_sender} {}

  auto process_request(const protocol::MarketDataRequest& request) const
      -> void {
    FIX::Message fix_message =
        make_fix_message(FIX::MsgType{FIX::MsgType_MarketDataRequest});
    const FIX::SessionID fix_session = encode_session(request.session);

    map_target_sub_id(request.session, fix_message);
    mapper_.map(
        request, fix_message, mapping_settings_.get_setting(fix_session));

    message_sender_.send_message(fix_message, fix_session);
  }

 private:
  Mapper mapper_;
  MappingSettings mapping_settings_;
  const MessageSender& message_sender_;
};

}  // namespace simulator::fix::generator_initiator

#endif  // SIMULATOR_GENERATOR_INITIATOR_IH_PROCESSORS_MARKET_DATA_REQUEST_SENDER_HPP_
