#ifndef SIMULATOR_FIX_COMMON_COMMUNICATORS_FIX_MESSAGE_SENDER_HPP_
#define SIMULATOR_FIX_COMMON_COMMUNICATORS_FIX_MESSAGE_SENDER_HPP_

#include "common/communicators/message_sender.hpp"

namespace simulator::fix {

class FixMessageSender : public MessageSender {
 public:
  auto send_message(FIX::Message& fix_message,
                    const FIX::SessionID& fix_session) const noexcept
      -> void override;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_COMMUNICATORS_FIX_MESSAGE_SENDER_HPP_
