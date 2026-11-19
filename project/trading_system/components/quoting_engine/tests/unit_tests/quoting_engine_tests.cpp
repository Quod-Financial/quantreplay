#include <gmock/gmock.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "common/attributes.hpp"
#include "common/instrument.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "core/tools/price_tick_ranges.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "middleware/routing/trading_reply_channel.hpp"
#include "mocks/trading_reply_receiver_mock.hpp"
#include "protocol/admin/quote.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/app/session_terminated_event.hpp"
#include "protocol/types/session.hpp"
#include "quoting_engine/quote_id_generator.hpp"
#include "quoting_engine/quoting_engine.hpp"
#include "test_utils/inline_service.hpp"

namespace simulator::trading_system::quoting_engine::test {
namespace {

using namespace ::testing;  // NOLINT

[[nodiscard]]
auto make_quoting_instrument() -> Instrument {
  Instrument instrument;
  instrument.engine_type = EngineType::Quoting;
  return instrument;
}

struct QuotingEngineRequests : public Test {
  [[nodiscard]]
  static auto make_session() -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"CLIENT"},
                               protocol::fix::TargetCompId{"SIM"}}};
  }

  template <typename RequestType>
  [[nodiscard]]
  static auto make_request() -> RequestType {
    return RequestType{make_session()};
  }

  [[nodiscard]]
  static auto make_security_status_request(MdSubscriptionRequestType type)
      -> protocol::SecurityStatusRequest {
    auto request = make_request<protocol::SecurityStatusRequest>();
    request.request_id = SecurityStatusReqId{"status-request"};
    request.request_type = type;
    return request;
  }

  [[nodiscard]]
  static auto IsOpenStatus() {
    return AllOf(Field(&protocol::SecurityStatus::trading_phase,
                       Optional(Eq(TradingPhase::Option::Open))),
                 Field(&protocol::SecurityStatus::trading_status,
                       Optional(Eq(TradingStatus::Option::Resume))));
  }

  NiceMock<TradingReplyReceiverMock> reply_receiver;
  InlineService executor;
  QuotingEngine engine{make_quoting_instrument(),
                       std::make_shared<QuoteIdGenerator>(),
                       executor};

 private:
  auto SetUp() -> void override {
    middleware::bind_trading_reply_channel(
        std::shared_ptr<middleware::TradingReplyReceiver>{
            std::addressof(reply_receiver), [](auto* /*pointer*/) {}});
  }

  auto TearDown() -> void override {
    middleware::release_trading_reply_channel();
  }
};

TEST_F(QuotingEngineRequests, RejectsOrderPlacement) {
  EXPECT_CALL(reply_receiver, process(A<protocol::OrderPlacementReject>()));

  engine.execute(make_request<protocol::OrderPlacementRequest>());
}

TEST_F(QuotingEngineRequests, ReportsOpenSecurityStatusOnSnapshotRequest) {
  EXPECT_CALL(reply_receiver,
              process(Matcher<protocol::SecurityStatus>(IsOpenStatus())));

  engine.execute(make_security_status_request(
      MdSubscriptionRequestType::Option::Snapshot));
}

TEST_F(QuotingEngineRequests, ReportsOpenSecurityStatusOnSubscriptionRequest) {
  EXPECT_CALL(reply_receiver,
              process(Matcher<protocol::SecurityStatus>(IsOpenStatus())));

  engine.execute(make_security_status_request(
      MdSubscriptionRequestType::Option::Subscribe));
}

TEST_F(QuotingEngineRequests,
       DropsSecurityStatusSubscriptionsOfTerminatedSession) {
  engine.execute(make_security_status_request(
      MdSubscriptionRequestType::Option::Subscribe));
  engine.handle(protocol::SessionTerminatedEvent{make_session()});

  EXPECT_CALL(reply_receiver, process(A<protocol::BusinessMessageReject>()))
      .Times(0);
  EXPECT_CALL(reply_receiver, process(A<protocol::SecurityStatus>()));

  engine.execute(make_security_status_request(
      MdSubscriptionRequestType::Option::Subscribe));
}

// NOLINTBEGIN(*magic-numbers*)

struct QuotingEngineQuotePublication : public Test {
  [[nodiscard]]
  static auto make_entry(double price, double quantity)
      -> protocol::QuoteEntry {
    return protocol::QuoteEntry{.price = Price{price},
                                .quantity = Quantity{quantity}};
  }

  [[nodiscard]]
  static auto make_request(std::vector<protocol::QuoteEntry> bids,
                           std::vector<protocol::QuoteEntry> asks)
      -> protocol::QuotePublicationRequest {
    protocol::QuotePublicationRequest request;
    request.bids = std::move(bids);
    request.asks = std::move(asks);
    return request;
  }

  [[nodiscard]]
  static auto make_valid_request() -> protocol::QuotePublicationRequest {
    return make_request({make_entry(1.25, 100)}, {make_entry(1.5, 200)});
  }

  [[nodiscard]]
  auto publish(const protocol::QuotePublicationRequest& request)
      -> protocol::QuotePublicationReply {
    protocol::QuotePublicationReply reply;
    engine.execute(request, reply);
    return reply;
  }

  [[nodiscard]]
  auto publish_valid_quote() -> std::optional<QuoteId> {
    return publish(make_valid_request()).quote_id;
  }

  [[nodiscard]]
  static auto make_tick_constrained_instrument() -> Instrument {
    Instrument instrument;
    instrument.price_tick_ranges = core::PriceTickRanges{{.tick_size = 0.25}};
    instrument.engine_type = EngineType::Quoting;
    return instrument;
  }

  InlineService executor;
  QuotingEngine engine{make_tick_constrained_instrument(),
                       std::make_shared<QuoteIdGenerator>(),
                       executor};
};

TEST_F(QuotingEngineQuotePublication, RefusesInvalidQuoteAsInvalidRequest) {
  const auto reply = publish(make_request({}, {}));

  ASSERT_THAT(
      reply.result,
      Optional(protocol::QuotePublicationReply::Result::InvalidRequest));
  ASSERT_EQ(reply.error_message, "quote has neither bids nor asks");
}

TEST_F(QuotingEngineQuotePublication, ValidatesQuoteAgainstListingConstraints) {
  const auto reply = publish(make_request({make_entry(1.3, 100)}, {}));

  ASSERT_THAT(
      reply.result,
      Optional(protocol::QuotePublicationReply::Result::InvalidRequest));
  ASSERT_EQ(reply.error_message, "bid price tick constraint violated");
}

TEST_F(QuotingEngineQuotePublication, PublishesValidQuote) {
  const auto reply = publish(make_valid_request());

  ASSERT_THAT(reply.result,
              Optional(protocol::QuotePublicationReply::Result::Published));
}

TEST_F(QuotingEngineQuotePublication, RepliesWithQuoteIdOfPublishedQuote) {
  const auto reply = publish(make_valid_request());

  ASSERT_TRUE(reply.quote_id.has_value());
}

TEST_F(QuotingEngineQuotePublication, RepliesWithoutQuoteIdToRefusedQuote) {
  const auto reply = publish(make_request({}, {}));

  ASSERT_EQ(reply.quote_id, std::nullopt);
}

TEST_F(QuotingEngineQuotePublication,
       AssignsDistinctQuoteIdsToSuccessiveQuotes) {
  const auto first_quote_id = publish_valid_quote();
  const auto second_quote_id = publish_valid_quote();

  ASSERT_THAT(second_quote_id, Ne(std::nullopt));
  ASSERT_THAT(first_quote_id, AllOf(Ne(std::nullopt), Ne(second_quote_id)));
}

struct QuotingEngineMarketData : public Test {
  [[nodiscard]]
  static auto make_session() -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{"CLIENT"},
                               protocol::fix::TargetCompId{"SIM"}}};
  }

  [[nodiscard]]
  static auto make_subscription() -> protocol::MarketDataRequest {
    protocol::MarketDataRequest request{make_session()};
    request.request_id = MdRequestId{"md-request"};
    request.request_type = MdSubscriptionRequestType::Option::Subscribe;
    InstrumentDescriptor instrument;
    instrument.symbol = Symbol{"EUR/USD"};
    request.instruments.push_back(std::move(instrument));
    request.market_data_types = {MdEntryType{MdEntryType::Option::Bid},
                                 MdEntryType{MdEntryType::Option::Offer}};
    request.market_depth = MarketDepth{0};
    request.update_type = MarketDataUpdateType::Option::Snapshot;
    return request;
  }

  [[nodiscard]]
  static auto make_entry(double price) -> protocol::QuoteEntry {
    return protocol::QuoteEntry{.price = Price{price},
                                .quantity = Quantity{100}};
  }

  [[nodiscard]]
  static auto prices_of(const protocol::MarketDataSnapshot& snapshot)
      -> std::vector<std::optional<Price>> {
    std::vector<std::optional<Price>> prices;
    std::ranges::transform(snapshot.market_data_entries,
                           std::back_inserter(prices),
                           &MarketDataEntry::price);
    return prices;
  }

  auto publish(std::vector<protocol::QuoteEntry> bids,
               std::vector<protocol::QuoteEntry> asks) -> void {
    protocol::QuotePublicationRequest request;
    request.bids = std::move(bids);
    request.asks = std::move(asks);
    protocol::QuotePublicationReply reply;
    engine.execute(request, reply);
  }

  auto publish_quote() -> void {
    publish({make_entry(1.25)}, {make_entry(1.5)});
  }

  NiceMock<TradingReplyReceiverMock> reply_receiver;
  InlineService executor;
  QuotingEngine engine{make_quoting_instrument(),
                       std::make_shared<QuoteIdGenerator>(),
                       executor};
  std::vector<protocol::MarketDataSnapshot> snapshots;

 private:
  auto SetUp() -> void override {
    middleware::bind_trading_reply_channel(
        std::shared_ptr<middleware::TradingReplyReceiver>{
            std::addressof(reply_receiver), [](auto* /*pointer*/) {}});

    ON_CALL(reply_receiver, process(A<protocol::MarketDataSnapshot>()))
        .WillByDefault(Invoke([this](protocol::MarketDataSnapshot snapshot) {
          snapshots.push_back(std::move(snapshot));
        }));
  }

  auto TearDown() -> void override {
    middleware::release_trading_reply_channel();
  }
};

TEST_F(QuotingEngineMarketData, SendsLatestQuoteOnSubscription) {
  publish({make_entry(1.0)}, {make_entry(2.0)});
  publish({make_entry(1.25)}, {make_entry(1.5)});

  engine.execute(make_subscription());

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(prices_of(snapshots.front()),
              UnorderedElementsAre(Price{1.25}, Price{1.5}));
}

TEST_F(QuotingEngineMarketData, StreamsPublishedQuoteToSubscription) {
  engine.execute(make_subscription());
  snapshots.clear();

  publish({make_entry(1.25)}, {make_entry(1.5)});

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(prices_of(snapshots.front()),
              UnorderedElementsAre(Price{1.25}, Price{1.5}));
}

TEST_F(QuotingEngineMarketData, StopsStreamingToTerminatedSession) {
  engine.execute(make_subscription());
  engine.handle(protocol::SessionTerminatedEvent{make_session()});
  snapshots.clear();

  publish_quote();

  ASSERT_THAT(snapshots, IsEmpty());
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::quoting_engine::test
