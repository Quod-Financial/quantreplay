#include <gmock/gmock.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>

#include "common/mapping/setting/mapping_settings.hpp"
#include "common/session_conversion.hpp"
#include "ih/processors/app_reply_processor.hpp"
#include "mocks/reply_sender_mock.hpp"
#include "mocks/to_fix_mapper_stub.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix::acceptor::test {
namespace {

using namespace testing;  // NOLINT

MATCHER_P(MsgTypeIs, type, "") {
  return arg.getHeader().getField(FIX::FIELD::MsgType) == type;
}

struct AcceptorAppReplySender : Test {
  template <typename MessageType>
  static auto make_message() -> MessageType {
    return MessageType{protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"SENDER"},
                               protocol::fix::TargetCompId{"TARGET"}}}};
  }

  static auto make_message_with_non_fix_session()
      -> protocol::OrderPlacementConfirmation {
    return protocol::OrderPlacementConfirmation{
        protocol::Session{protocol::generator::Session{}}};
  }

  StrictMock<ReplySenderMock> reply_sender;
  AppReplyProcessor<ToFixMapperStub> processor{
      reply_sender, MappingSettings{std::vector<FIX::Session*>{}}};
};

TEST_F(AcceptorAppReplySender, SendsExecutionReportAsIs) {
  const auto reply = make_message<protocol::ExecutionReport>();

  EXPECT_CALL(reply_sender, send_reply_message(_, _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender,
       SendsOrderPlacementConfirmationAsExecutionReport) {
  const auto reply = make_message<protocol::OrderPlacementConfirmation>();

  EXPECT_CALL(reply_sender,
              send_reply_message(MsgTypeIs(FIX::MsgType_ExecutionReport), _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender, SendsOrderPlacementRejectAsExecutionReport) {
  const auto reply = make_message<protocol::OrderPlacementReject>();

  EXPECT_CALL(reply_sender,
              send_reply_message(MsgTypeIs(FIX::MsgType_ExecutionReport), _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender,
       SendsOrderModificationConfirmationAsExecutionReport) {
  const auto reply = make_message<protocol::OrderModificationConfirmation>();

  EXPECT_CALL(reply_sender,
              send_reply_message(MsgTypeIs(FIX::MsgType_ExecutionReport), _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender,
       SendsOrderModificationRejectAsOrderCancelReject) {
  const auto reply = make_message<protocol::OrderModificationReject>();

  EXPECT_CALL(reply_sender,
              send_reply_message(MsgTypeIs(FIX::MsgType_OrderCancelReject), _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender,
       SendsOrderCancellationConfirmationAsExecutionReport) {
  const auto reply = make_message<protocol::OrderCancellationConfirmation>();

  EXPECT_CALL(reply_sender,
              send_reply_message(MsgTypeIs(FIX::MsgType_ExecutionReport), _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender,
       SendsOrderCancellationRejectAsOrderCancelReject) {
  const auto reply = make_message<protocol::OrderCancellationReject>();

  EXPECT_CALL(reply_sender,
              send_reply_message(MsgTypeIs(FIX::MsgType_OrderCancelReject), _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender, SendsMarketDataRejectAsMarketDataRequestReject) {
  const auto reply = make_message<protocol::MarketDataReject>();

  EXPECT_CALL(
      reply_sender,
      send_reply_message(MsgTypeIs(FIX::MsgType_MarketDataRequestReject), _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender,
       SendsMarketDataSnapshotAsMarketDataSnapshotFullRefresh) {
  const auto reply = make_message<protocol::MarketDataSnapshot>();

  EXPECT_CALL(reply_sender,
              send_reply_message(
                  MsgTypeIs(FIX::MsgType_MarketDataSnapshotFullRefresh), _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender,
       SendsMarketDataUpdateAsMarketDataIncrementalRefresh) {
  const auto reply = make_message<protocol::MarketDataUpdate>();

  EXPECT_CALL(reply_sender,
              send_reply_message(
                  MsgTypeIs(FIX::MsgType_MarketDataIncrementalRefresh), _));

  processor.process_reply(reply);
}

TEST_F(AcceptorAppReplySender, ReportsSessionEncodingError) {
  const auto reply = make_message_with_non_fix_session();

  ASSERT_THROW(processor.process_reply(reply), InvalidSessionCategoryError);
}

}  // namespace
}  // namespace simulator::fix::acceptor::test