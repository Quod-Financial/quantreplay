#include <gmock/gmock.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include "common/communicators/fix_message_sender.hpp"
#include "common/message.hpp"

namespace simulator::fix::test {
namespace {

using namespace testing;  // NOLINT

TEST(FixMessageSending, DoesNotThrowWhenTargetSessionIsUnknown) {
  const FixMessageSender sender;
  FIX::Message fix_message =
      make_fix_message(FIX::MsgType{FIX::MsgType_MarketDataRequest});
  const FIX::SessionID unknown_session{"FIXT.1.1", "SENDER", "TARGET"};

  ASSERT_NO_THROW(sender.send_message(fix_message, unknown_session));
}

}  // namespace
}  // namespace simulator::fix::test
