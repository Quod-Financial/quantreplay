#ifndef SIMULATOR_ACCEPTOR_IH_COMMUNICATORS_FIX_REPLY_SENDER_HPP_
#define SIMULATOR_ACCEPTOR_IH_COMMUNICATORS_FIX_REPLY_SENDER_HPP_

#include "ih/communicators/reply_sender.hpp"

namespace simulator::fix::acceptor {

class FixReplySender : public ReplySender {
 public:
  auto send_reply_message(FIX::Message& fix_message,
                          const FIX::SessionID& fix_session) const noexcept
      -> void override;
};

}  // namespace simulator::fix::acceptor

#endif  // SIMULATOR_ACCEPTOR_IH_COMMUNICATORS_FIX_REPLY_SENDER_HPP_