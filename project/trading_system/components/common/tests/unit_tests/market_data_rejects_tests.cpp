#include <gmock/gmock.h>

#include <optional>
#include <string_view>

#include "common/market_data_rejects.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace ::testing;  // NOLINT

[[nodiscard]]
auto make_session() -> protocol::Session {
  return protocol::Session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                             protocol::fix::SenderCompId{"CLIENT"},
                             protocol::fix::TargetCompId{"SIM"}}};
}

struct TradingSystemMarketDataRequestViolation : public Test {
  [[nodiscard]]
  static auto make_complete_request() -> protocol::MarketDataRequest {
    protocol::MarketDataRequest request{make_session()};
    request.request_id = MdRequestId{"md-request"};
    request.request_type = MdSubscriptionRequestType::Option::Subscribe;
    request.instruments.push_back(InstrumentDescriptor{});
    request.market_data_types.emplace_back(MdEntryType::Option::Bid);
    return request;
  }

  protocol::MarketDataRequest request = make_complete_request();
};

TEST_F(TradingSystemMarketDataRequestViolation, ReportsMissingRequestId) {
  request.request_id.reset();

  ASSERT_THAT(find_market_data_request_violation(request),
              Optional(Eq("required market data request id missing")));
}

TEST_F(TradingSystemMarketDataRequestViolation, ReportsMissingRequestType) {
  request.request_type.reset();

  ASSERT_THAT(find_market_data_request_violation(request),
              Optional(Eq("required market data request type missing")));
}

TEST_F(TradingSystemMarketDataRequestViolation,
       ReportsRequestOnSeveralInstruments) {
  request.instruments.push_back(InstrumentDescriptor{});

  ASSERT_THAT(find_market_data_request_violation(request),
              Optional(Eq("invalid number of instruments in the request")));
}

TEST_F(TradingSystemMarketDataRequestViolation, ReportsMissingEntryTypes) {
  request.market_data_types.clear();

  ASSERT_THAT(
      find_market_data_request_violation(request),
      Optional(Eq("no supported market data types specified in the request")));
}

TEST_F(TradingSystemMarketDataRequestViolation, AcceptsCompleteRequest) {
  ASSERT_EQ(find_market_data_request_violation(request), std::nullopt);
}

struct TradingSystemMarketDataReject : public Test {
  [[nodiscard]]
  static auto make_request() -> protocol::MarketDataRequest {
    protocol::MarketDataRequest request{make_session()};
    request.request_id = MdRequestId{"md-request"};
    return request;
  }

  protocol::MarketDataRequest request = make_request();
};

TEST_F(TradingSystemMarketDataReject, RepliesToRequestingSession) {
  const auto reject = make_reject(request, "reason");

  ASSERT_EQ(reject.session, make_session());
}

TEST_F(TradingSystemMarketDataReject, EchoesRequestId) {
  const auto reject = make_reject(request, "reason");

  ASSERT_THAT(reject.request_id, Optional(Eq(MdRequestId{"md-request"})));
}

TEST_F(TradingSystemMarketDataReject, ExplainsRejectWithGivenReason) {
  const auto reject = make_reject(request, "reason");

  ASSERT_THAT(reject.reject_text, Optional(Eq(RejectText{"reason"})));
}

TEST_F(TradingSystemMarketDataReject, SetsRejectReasonOfDuplicateRequestId) {
  const auto reject =
      make_reject(request, MdRejectReason::Option::DuplicateMdReqId);

  ASSERT_THAT(reject.reject_reason,
              Optional(Eq(MdRejectReason::Option::DuplicateMdReqId)));
  ASSERT_THAT(reject.reject_text,
              Optional(Eq(RejectText{"request identifier is used already"})));
}

TEST_F(TradingSystemMarketDataReject, SetsGivenRejectReasonWithGivenText) {
  const auto reject = make_reject(
      request, MdRejectReason::Option::UnsupportedMarketDepth, "depth 5");

  ASSERT_THAT(reject.reject_reason,
              Optional(Eq(MdRejectReason::Option::UnsupportedMarketDepth)));
  ASSERT_THAT(reject.reject_text, Optional(Eq(RejectText{"depth 5"})));
}

TEST_F(TradingSystemMarketDataReject, ExplainsUnknownSubscription) {
  const auto reject = make_unknown_subscription_reject(request);

  ASSERT_THAT(
      reject.reject_text,
      Optional(Eq(RejectText{"no subscription found for the request id"})));
}

}  // namespace
}  // namespace simulator::trading_system::test
