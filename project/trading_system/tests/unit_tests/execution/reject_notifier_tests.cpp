#include <gmock/gmock.h>

#include "core/domain/attributes.hpp"
#include "ih/execution/reject_notifier.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "mocks/trading_reply_receiver_mock.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace ::testing;  // NOLINT

struct TradingSystemRejectNotifier : public Test {
  template <typename RequestType>
  static auto make_request() -> RequestType {
    return RequestType{protocol::Session{protocol::generator::Session{}}};
  }

  NiceMock<TradingReplyReceiverMock> middleware_receiver;
  RejectNotifier notifier;

 private:
  auto SetUp() -> void override {
    middleware::bind_trading_reply_channel(
        std::shared_ptr<TradingReplyReceiverMock>{
            std::addressof(middleware_receiver), [](auto* /*pointer*/) {}});
  }

  auto TearDown() -> void override {
    middleware::release_trading_reply_channel();
  }
};

TEST_F(TradingSystemRejectNotifier, RejectsOrderPlacementRequest) {
  auto const request = make_request<protocol::OrderPlacementRequest>();
  auto reject = make_request<protocol::OrderPlacementReject>();

  EXPECT_CALL(middleware_receiver, process(A<protocol::OrderPlacementReject>()))
      .WillOnce(SaveArg<0>(&reject));
  notifier.reject(request, "reason");

  EXPECT_THAT(reject.reject_text, Optional(Eq(RejectText{"reason"})));
  EXPECT_THAT(reject.venue_order_id, Optional(MatchesRegex("[0-9]{18}")));
  EXPECT_THAT(reject.execution_id, Optional(MatchesRegex("[0-9]{18}-1")));
}

TEST_F(TradingSystemRejectNotifier, RejectsOrderModificationRequest) {
  auto const request = make_request<protocol::OrderModificationRequest>();
  auto reject = make_request<protocol::OrderModificationReject>();

  EXPECT_CALL(middleware_receiver,
              process(A<protocol::OrderModificationReject>()))
      .WillOnce(SaveArg<0>(&reject));
  notifier.reject(request, "reason");

  EXPECT_THAT(reject.reject_text, Optional(Eq(RejectText{"reason"})));
  EXPECT_THAT(reject.order_status, Optional(Eq(OrderStatus::Option::Rejected)));
}

TEST_F(TradingSystemRejectNotifier, RejectsOrderCancellationRequest) {
  auto const request = make_request<protocol::OrderCancellationRequest>();
  auto reject = make_request<protocol::OrderCancellationReject>();

  EXPECT_CALL(middleware_receiver,
              process(A<protocol::OrderCancellationReject>()))
      .WillOnce(SaveArg<0>(&reject));
  notifier.reject(request, "reason");

  EXPECT_THAT(reject.reject_text, Optional(Eq(RejectText{"reason"})));
  EXPECT_THAT(reject.order_status, Optional(Eq(OrderStatus::Option::Rejected)));
}

TEST_F(TradingSystemRejectNotifier, RejectsMarketDataRequest) {
  auto const request = make_request<protocol::MarketDataRequest>();
  auto reject = make_request<protocol::MarketDataReject>();

  EXPECT_CALL(middleware_receiver, process(A<protocol::MarketDataReject>()))
      .WillOnce(SaveArg<0>(&reject));
  notifier.reject(request, "reason");

  EXPECT_THAT(reject.reject_text, Optional(Eq(RejectText{"reason"})));
  EXPECT_THAT(reject.reject_reason,
              Optional(Eq(MdRejectReason::Option::UnknownSymbol)));
}

TEST_F(TradingSystemRejectNotifier,
       NotifiesNoInstrumentsSetInMarketDataRequest) {
  auto const request = make_request<protocol::MarketDataRequest>();
  auto reject = make_request<protocol::MarketDataReject>();

  EXPECT_CALL(middleware_receiver, process(A<protocol::MarketDataReject>()))
      .WillOnce(SaveArg<0>(&reject));
  notifier.notify_no_instruments_requested(request);

  EXPECT_THAT(reject.reject_reason, Eq(std::nullopt));
  EXPECT_THAT(
      reject.reject_text,
      Optional(Eq(RejectText{"no securities requested in the request"})));
}

TEST_F(TradingSystemRejectNotifier,
       NotifiesMultipleInstrumentsSetInMarketDataRequest) {
  auto const request = make_request<protocol::MarketDataRequest>();
  auto reject = make_request<protocol::MarketDataReject>();

  EXPECT_CALL(middleware_receiver, process(A<protocol::MarketDataReject>()))
      .WillOnce(SaveArg<0>(&reject));
  notifier.notify_multiple_instruments_requested(request);

  EXPECT_THAT(reject.reject_reason, Eq(std::nullopt));
  EXPECT_THAT(
      reject.reject_text,
      Optional(Eq(RejectText{
          "market data requests on multiple securities are not allowed"})));
}

}  // namespace
}  // namespace simulator::trading_system::test