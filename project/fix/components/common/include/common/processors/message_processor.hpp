#ifndef SIMULATOR_FIX_COMMON_PROCESSORS_MESSAGE_PROCESSOR_HPP_
#define SIMULATOR_FIX_COMMON_PROCESSORS_MESSAGE_PROCESSOR_HPP_

#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

namespace simulator::fix {

class MessageProcessor {
 public:
  MessageProcessor() = default;
  MessageProcessor(const MessageProcessor&) = default;
  MessageProcessor(MessageProcessor&&) noexcept = default;
  virtual ~MessageProcessor() = default;

  auto operator=(const MessageProcessor&) -> MessageProcessor& = default;
  auto operator=(MessageProcessor&&) noexcept -> MessageProcessor& = default;

  virtual auto process_message(const FIX::Message& fix_message,
                               const FIX::SessionID& fix_session) const
      -> void = 0;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_PROCESSORS_MESSAGE_PROCESSOR_HPP_
