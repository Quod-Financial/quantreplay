#include <gmock/gmock.h>

#include <memory>
#include <string>
#include <string_view>

#include "ih/dispatchers/market_data_request_dispatcher.hpp"
#include "mocks/market_data_request_receiver_mock.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::test {
namespace {

using namespace ::testing;  // NOLINT

struct AppMarketDataRequestDispatcher : public Test {
  [[nodiscard]]
  static auto session(const std::string_view target_comp_id)
      -> protocol::fix::Session {
    return protocol::fix::Session{
        protocol::fix::BeginString{"FIXT.1.1"},
        protocol::fix::SenderCompId{"MktSimulator"},
        protocol::fix::TargetCompId{std::string{target_comp_id}}};
  }

  [[nodiscard]]
  static auto request_for(const std::string_view target_comp_id)
      -> protocol::MarketDataRequest {
    return protocol::MarketDataRequest{
        protocol::Session{session(target_comp_id)}};
  }

  [[nodiscard]]
  static auto route(const std::string_view target_comp_id,
                    MarketDataRequestReceiverMock& receiver)
      -> MarketDataRequestDispatcher::Route {
    return {.session = session(target_comp_id),
            .receiver = std::shared_ptr<middleware::MarketDataRequestReceiver>{
                std::addressof(receiver), [](auto* /*pointer*/) {}}};
  }

  StrictMock<MarketDataRequestReceiverMock> first_receiver;
  StrictMock<MarketDataRequestReceiverMock> second_receiver;
};

TEST_F(AppMarketDataRequestDispatcher,
       DropsRequestForSessionWithoutReceiver) {
  MarketDataRequestDispatcher dispatcher{{route("First", first_receiver)}};
  EXPECT_CALL(first_receiver, process(_)).Times(0);

  dispatcher.process(request_for("Unknown"));
}

TEST_F(AppMarketDataRequestDispatcher, ForwardsRequestToReceiverOfItsSession) {
  MarketDataRequestDispatcher dispatcher{
      {route("First", first_receiver), route("Second", second_receiver)}};
  EXPECT_CALL(second_receiver,
              process(Field(&protocol::MarketDataRequest::session,
                            Eq(protocol::Session{session("Second")}))));

  dispatcher.process(request_for("Second"));
}

}  // namespace
}  // namespace simulator::test
