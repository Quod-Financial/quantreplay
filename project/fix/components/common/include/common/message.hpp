#ifndef SIMULATOR_FIX_COMMON_MESSAGE_HPP_
#define SIMULATOR_FIX_COMMON_MESSAGE_HPP_

#include <quickfix/Fields.h>
#include <quickfix/Message.h>

namespace simulator::fix {

[[nodiscard]]
inline auto make_fix_message(const FIX::MsgType& fix_message_type)
    -> FIX::Message {
  FIX::Message fix_message;
  fix_message.getHeader().setField(fix_message_type);
  return fix_message;
}

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_MESSAGE_HPP_
