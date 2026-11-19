#include <gmock/gmock.h>

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/orders/order_rejecter.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "mocks/trading_reply_receiver_mock.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::quoting_engine::test {
namespace {

using namespace ::testing;  // NOLINT

struct QuotingEngineOrderRejecter : public Test {
  [[nodiscard]]
  static auto make_session() -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"CLIENT"},
                               protocol::fix::TargetCompId{"SIM"}}};
  }

  [[nodiscard]]
  static auto make_request() -> protocol::OrderPlacementRequest {
    return protocol::OrderPlacementRequest{make_session()};
  }

  NiceMock<TradingReplyReceiverMock> reply_receiver;
  OrderRejecter rejecter;
  std::vector<protocol::OrderPlacementReject> rejects;

 private:
  auto SetUp() -> void override {
    middleware::bind_trading_reply_channel(
        std::shared_ptr<middleware::TradingReplyReceiver>{
            std::addressof(reply_receiver), [](auto* /*pointer*/) {}});

    ON_CALL(reply_receiver, process(A<protocol::OrderPlacementReject>()))
        .WillByDefault(Invoke([this](protocol::OrderPlacementReject reject) {
          rejects.push_back(std::move(reject));
        }));
  }

  auto TearDown() -> void override {
    middleware::release_trading_reply_channel();
  }
};

TEST_F(QuotingEngineOrderRejecter, ExplainsThatOrdersAreNotSupported) {
  rejecter.reject(make_request());

  ASSERT_THAT(rejects,
              ElementsAre(Field(
                  &protocol::OrderPlacementReject::reject_text,
                  Optional(Eq(RejectText{
                      "orders are not supported on a quoting listing"})))));
}

TEST_F(QuotingEngineOrderRejecter, RejectsGivenPlacementRequest) {
  auto request = make_request();
  request.client_order_id = ClientOrderId{"client-order"};

  rejecter.reject(request);

  ASSERT_THAT(
      rejects,
      ElementsAre(AllOf(
          Field(&protocol::OrderPlacementReject::session, Eq(make_session())),
          Field(&protocol::OrderPlacementReject::client_order_id,
                Optional(Eq(ClientOrderId{"client-order"}))))));
}

TEST_F(QuotingEngineOrderRejecter,
       AssignsDistinctVenueOrderIdsToSuccessiveRejects) {
  rejecter.reject(make_request());
  rejecter.reject(make_request());

  ASSERT_THAT(rejects, SizeIs(2));
  ASSERT_THAT(rejects[0].venue_order_id,
              AllOf(Ne(std::nullopt), Ne(rejects[1].venue_order_id)));
}

TEST_F(QuotingEngineOrderRejecter,
       AssignsDistinctExecutionIdsToSuccessiveRejects) {
  rejecter.reject(make_request());
  rejecter.reject(make_request());

  ASSERT_THAT(rejects, SizeIs(2));
  ASSERT_THAT(rejects[0].execution_id,
              AllOf(Ne(std::nullopt), Ne(rejects[1].execution_id)));
}

}  // namespace
}  // namespace simulator::trading_system::quoting_engine::test
