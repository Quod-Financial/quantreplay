#ifndef SIMULATOR_ACCEPTOR_IH_COMMUNICATORS_REPLY_SENDER_HPP_
#define SIMULATOR_ACCEPTOR_IH_COMMUNICATORS_REPLY_SENDER_HPP_

#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

namespace simulator::fix::acceptor {

class ReplySender {
 public:
  ReplySender() = default;
  ReplySender(const ReplySender&) = default;
  ReplySender(ReplySender&&) noexcept = default;
  virtual ~ReplySender() = default;

  auto operator=(const ReplySender&) -> ReplySender& = default;
  auto operator=(ReplySender&&) noexcept -> ReplySender& = default;

  virtual auto send_reply_message(
      FIX::Message& fix_message,
      const FIX::SessionID& fix_session) const noexcept -> void = 0;
};

}  // namespace simulator::fix::acceptor

#endif  // SIMULATOR_ACCEPTOR_IH_COMMUNICATORS_REPLY_SENDER_HPP_