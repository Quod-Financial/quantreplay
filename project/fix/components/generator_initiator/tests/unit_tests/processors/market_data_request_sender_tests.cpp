#include <gmock/gmock.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <vector>

#include "common/mapping/setting/mapping_settings.hpp"
#include "common/session_conversion.hpp"
#include "ih/processors/market_data_request_sender.hpp"
#include "mocks/message_sender_mock.hpp"
#include "mocks/to_fix_mapper_stub.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix::generator_initiator::test {
namespace {

using namespace testing;  // NOLINT

MATCHER_P(MsgTypeIs, type, "") {
  return arg.getHeader().getField(FIX::FIELD::MsgType) == type;
}

struct InitiatorMarketDataRequestSender : public Test {
  static auto make_request() -> protocol::MarketDataRequest {
    return protocol::MarketDataRequest{protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"SENDER"},
                               protocol::fix::TargetCompId{"TARGET"}}}};
  }

  static auto make_request_with_non_fix_session()
      -> protocol::MarketDataRequest {
    return protocol::MarketDataRequest{
        protocol::Session{protocol::generator::Session{}}};
  }

  StrictMock<MessageSenderMock> message_sender;
  MarketDataRequestSender<ToFixMapperStub> sender{
      message_sender, MappingSettings{std::vector<FIX::Session*>{}}};
};

TEST_F(InitiatorMarketDataRequestSender, ReportsSessionEncodingError) {
  const auto request = make_request_with_non_fix_session();

  ASSERT_THROW(sender.process_request(request), InvalidSessionCategoryError);
}

TEST_F(InitiatorMarketDataRequestSender, SendsRequestAsMarketDataRequest) {
  const auto request = make_request();

  EXPECT_CALL(message_sender,
              send_message(MsgTypeIs(FIX::MsgType_MarketDataRequest), _));

  sender.process_request(request);
}

TEST_F(InitiatorMarketDataRequestSender, SendsRequestToSessionItCarries) {
  const auto request = make_request();

  EXPECT_CALL(
      message_sender,
      send_message(_, Eq(FIX::SessionID{"FIXT.1.1", "SENDER", "TARGET"})));

  sender.process_request(request);
}

}  // namespace
}  // namespace simulator::fix::generator_initiator::test
