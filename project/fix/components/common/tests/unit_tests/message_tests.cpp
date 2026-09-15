#include <gmock/gmock.h>
#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>

#include "common/message.hpp"

namespace simulator::fix::test {
namespace {

using namespace testing;  // NOLINT

TEST(FixMessage, CreatesMessageWithMessageType) {
  const FIX::Message fix_message =
      make_fix_message(FIX::MsgType{FIX::MsgType_MarketDataRequest});

  ASSERT_THAT(fix_message.getHeader().getField(FIX::FIELD::MsgType), Eq("V"));
}

}  // namespace
}  // namespace simulator::fix::test
