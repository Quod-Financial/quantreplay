#ifndef SIMULATOR_FIX_COMMON_COMMUNICATORS_MESSAGE_SENDER_HPP_
#define SIMULATOR_FIX_COMMON_COMMUNICATORS_MESSAGE_SENDER_HPP_

#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

namespace simulator::fix {

class MessageSender {
 public:
  MessageSender() = default;
  MessageSender(const MessageSender&) = default;
  MessageSender(MessageSender&&) noexcept = default;
  virtual ~MessageSender() = default;

  auto operator=(const MessageSender&) -> MessageSender& = default;
  auto operator=(MessageSender&&) noexcept -> MessageSender& = default;

  virtual auto send_message(FIX::Message& fix_message,
                            const FIX::SessionID& fix_session) const noexcept
      -> void = 0;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_COMMUNICATORS_MESSAGE_SENDER_HPP_
