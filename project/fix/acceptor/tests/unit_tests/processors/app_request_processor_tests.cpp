#include <gmock/gmock.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <memory>
#include <string>

#include "ih/processors/app_request_processor.hpp"
#include "mocks/from_fix_mapper_stub.hpp"
#include "mocks/trading_request_receiver_mock.hpp"

namespace simulator::fix::acceptor::test {
namespace {

using namespace testing;  // NOLINT

struct AcceptorAppRequestProcessor : public Test {
  static auto make_fix_message(const std::string& message_type)
      -> FIX::Message {
    FIX::Message fix_message;
    fix_message.getHeader().setField(FIX::MsgType{message_type});
    return fix_message;
  }

  StrictMock<TradingRequestReceiverMock> request_receiver;
  AppRequestProcessor<FromFixMapperStub> processor;
  FIX::SessionID fix_session{"FIXT.1.1", "SenderCompID", "TargetCompID"};

 private:
  auto SetUp() -> void override {
    std::shared_ptr<middleware::TradingRequestReceiver> receiver{
        std::addressof(request_receiver), [](auto* /*pointer*/) {}};

    middleware::bind_trading_request_channel(receiver);
  }

  auto TearDown() -> void override {
    middleware::release_trading_request_channel();
  }
};

TEST_F(AcceptorAppRequestProcessor, DispatchesNewOrderSingle) {
  const auto fix_message = make_fix_message(FIX::MsgType_NewOrderSingle);

  EXPECT_CALL(request_receiver, process(A<protocol::OrderPlacementRequest>()));

  ASSERT_NO_THROW(processor.process_fix_request(fix_message, fix_session));
}

TEST_F(AcceptorAppRequestProcessor, DispatchesOrderCancelReplaceRequest) {
  const auto fix_message =
      make_fix_message(FIX::MsgType_OrderCancelReplaceRequest);

  EXPECT_CALL(request_receiver,
              process(A<protocol::OrderModificationRequest>()));

  ASSERT_NO_THROW(processor.process_fix_request(fix_message, fix_session));
}

TEST_F(AcceptorAppRequestProcessor, DispatchesOrderCancelRequest) {
  const auto fix_message = make_fix_message(FIX::MsgType_OrderCancelRequest);

  EXPECT_CALL(request_receiver,
              process(A<protocol::OrderCancellationRequest>()));

  ASSERT_NO_THROW(processor.process_fix_request(fix_message, fix_session));
}

TEST_F(AcceptorAppRequestProcessor, DispatchersMarketDataRequest) {
  const auto fix_message = make_fix_message(FIX::MsgType_MarketDataRequest);

  EXPECT_CALL(request_receiver, process(A<protocol::MarketDataRequest>()));

  ASSERT_NO_THROW(processor.process_fix_request(fix_message, fix_session));
}

TEST_F(AcceptorAppRequestProcessor, ReportsUnknownMessageType) {
  const auto fix_message = make_fix_message("UnknownMessageType");

  ASSERT_THROW(processor.process_fix_request(fix_message, fix_session),
               FIX::UnsupportedMessageType);
}

TEST_F(AcceptorAppRequestProcessor, ReportsMessageWithoutMessageType) {
  auto fix_message = make_fix_message("");
  fix_message.getHeader().removeField(FIX::FIELD::MsgType);

  ASSERT_THROW(processor.process_fix_request(fix_message, fix_session),
               FIX::FieldNotFound);
}

}  // namespace
}  // namespace simulator::fix::acceptor::test