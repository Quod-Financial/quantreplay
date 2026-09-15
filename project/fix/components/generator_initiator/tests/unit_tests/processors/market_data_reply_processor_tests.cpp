#include <gmock/gmock.h>
#include <quickfix/Exceptions.h>
#include <quickfix/Fields.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <memory>
#include <string>

#include "ih/processors/market_data_reply_processor.hpp"
#include "mocks/from_fix_mapper_stub.hpp"
#include "mocks/market_data_reply_receiver_mock.hpp"

namespace simulator::fix::generator_initiator::test {
namespace {

using namespace testing;  // NOLINT

struct InitiatorMarketDataReplyProcessor : public Test {
  static auto make_fix_message(const std::string& message_type)
      -> FIX::Message {
    FIX::Message fix_message;
    fix_message.getHeader().setField(FIX::MsgType{message_type});
    fix_message.setField(FIX::MDReqID{"request-id"});
    return fix_message;
  }

  StrictMock<MarketDataReplyReceiverMock> reply_receiver;
  MarketDataReplyProcessor<FromFixMapperStub> processor;
  FIX::SessionID fix_session{"FIXT.1.1", "SenderCompID", "TargetCompID"};

 private:
  auto SetUp() -> void override {
    middleware::bind_market_data_reply_channel(
        std::shared_ptr<middleware::MarketDataReplyReceiver>{
            std::addressof(reply_receiver), [](auto* /*pointer*/) {}});
  }

  auto TearDown() -> void override {
    middleware::release_market_data_reply_channel();
  }
};

TEST_F(InitiatorMarketDataReplyProcessor, ReportsUnknownMessageType) {
  const auto fix_message = make_fix_message("UnknownMessageType");

  ASSERT_THROW(processor.process_message(fix_message, fix_session),
               FIX::UnsupportedMessageType);
}

TEST_F(InitiatorMarketDataReplyProcessor, ReportsMessageWithoutMessageType) {
  auto fix_message = make_fix_message("");
  fix_message.getHeader().removeField(FIX::FIELD::MsgType);

  ASSERT_THROW(processor.process_message(fix_message, fix_session),
               FIX::FieldNotFound);
}

TEST_F(InitiatorMarketDataReplyProcessor,
       IgnoresIncrementalRefreshWithoutRequestId) {
  auto fix_message =
      make_fix_message(FIX::MsgType_MarketDataIncrementalRefresh);
  fix_message.removeField(FIX::FIELD::MDReqID);

  EXPECT_CALL(reply_receiver, process(A<protocol::MarketDataUpdate>()))
      .Times(0);

  ASSERT_NO_THROW(processor.process_message(fix_message, fix_session));
}

TEST_F(InitiatorMarketDataReplyProcessor, DispatchesSnapshotFullRefresh) {
  const auto fix_message =
      make_fix_message(FIX::MsgType_MarketDataSnapshotFullRefresh);

  EXPECT_CALL(reply_receiver, process(A<protocol::MarketDataSnapshot>()));

  ASSERT_NO_THROW(processor.process_message(fix_message, fix_session));
}

TEST_F(InitiatorMarketDataReplyProcessor, DispatchesIncrementalRefresh) {
  const auto fix_message =
      make_fix_message(FIX::MsgType_MarketDataIncrementalRefresh);

  EXPECT_CALL(reply_receiver, process(A<protocol::MarketDataUpdate>()));

  ASSERT_NO_THROW(processor.process_message(fix_message, fix_session));
}

TEST_F(InitiatorMarketDataReplyProcessor, DispatchesMarketDataRequestReject) {
  const auto fix_message =
      make_fix_message(FIX::MsgType_MarketDataRequestReject);

  EXPECT_CALL(reply_receiver, process(A<protocol::MarketDataReject>()));

  ASSERT_NO_THROW(processor.process_message(fix_message, fix_session));
}

TEST_F(InitiatorMarketDataReplyProcessor, DispatchesSnapshotWithoutRequestId) {
  auto fix_message =
      make_fix_message(FIX::MsgType_MarketDataSnapshotFullRefresh);
  fix_message.removeField(FIX::FIELD::MDReqID);

  EXPECT_CALL(reply_receiver, process(A<protocol::MarketDataSnapshot>()));

  ASSERT_NO_THROW(processor.process_message(fix_message, fix_session));
}

}  // namespace
}  // namespace simulator::fix::generator_initiator::test
