#include <gmock/gmock.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "core/tools/time.hpp"
#include "ih/market_data/quote_streamer.hpp"
#include "ih/quotes/quote.hpp"
#include "middleware/channels/trading_reply_channel.hpp"
#include "middleware/routing/trading_reply_channel.hpp"
#include "mocks/trading_reply_receiver_mock.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::quoting_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct QuotingEngineQuoteStreamer : public Test {
  [[nodiscard]]
  static auto make_session(std::string sender) -> protocol::Session {
    return protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                               protocol::fix::SenderCompId{std::move(sender)},
                               protocol::fix::TargetCompId{"SIM"}}};
  }

  [[nodiscard]]
  static auto make_request(MdSubscriptionRequestType::Option type,
                           protocol::Session session)
      -> protocol::MarketDataRequest {
    protocol::MarketDataRequest request{std::move(session)};
    request.request_id = MdRequestId{"md-request"};
    request.request_type = type;
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
  static auto make_request(MdSubscriptionRequestType::Option type)
      -> protocol::MarketDataRequest {
    return make_request(type, make_session("CLIENT"));
  }

  [[nodiscard]]
  static auto make_subscription() -> protocol::MarketDataRequest {
    return make_request(MdSubscriptionRequestType::Option::Subscribe);
  }

  [[nodiscard]]
  static auto make_level(std::string entry_id, double price) -> PublishedLevel {
    return PublishedLevel{.entry_id = MarketEntryId{std::move(entry_id)},
                          .price = Price{price},
                          .quantity = Quantity{100}};
  }

  [[nodiscard]]
  static auto make_level(double price) -> PublishedLevel {
    return make_level("entry", price);
  }

  [[nodiscard]]
  static auto make_quote(std::vector<PublishedLevel> bids,
                         std::vector<PublishedLevel> asks) -> Quote {
    return Quote{.identifier = QuoteId{"quote"},
                 .publication_time = core::sys_us{},
                 .bids = std::move(bids),
                 .asks = std::move(asks)};
  }

  [[nodiscard]]
  static auto make_quote() -> Quote {
    return make_quote({make_level(1.25)}, {make_level(1.5)});
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

  [[nodiscard]]
  static auto entry_ids_of(const protocol::MarketDataSnapshot& snapshot)
      -> std::vector<std::optional<MarketEntryId>> {
    std::vector<std::optional<MarketEntryId>> identifiers;
    std::ranges::transform(snapshot.market_data_entries,
                           std::back_inserter(identifiers),
                           &MarketDataEntry::id);
    return identifiers;
  }

  [[nodiscard]]
  static auto HasRejectText(const std::string& text) {
    return Field(&protocol::MarketDataReject::reject_text,
                 Optional(Eq(RejectText{text})));
  }

  [[nodiscard]]
  static auto IsReject(MdRejectReason::Option reason, const std::string& text) {
    return AllOf(
        Field(&protocol::MarketDataReject::reject_reason, Optional(Eq(reason))),
        HasRejectText(text));
  }

  NiceMock<TradingReplyReceiverMock> reply_receiver;
  QuoteStreamer streamer;
  std::vector<protocol::MarketDataSnapshot> snapshots;
  std::vector<protocol::MarketDataReject> rejects;

 private:
  auto SetUp() -> void override {
    middleware::bind_trading_reply_channel(
        std::shared_ptr<middleware::TradingReplyReceiver>{
            std::addressof(reply_receiver), [](auto* /*pointer*/) {}});

    ON_CALL(reply_receiver, process(A<protocol::MarketDataSnapshot>()))
        .WillByDefault(Invoke([this](protocol::MarketDataSnapshot snapshot) {
          snapshots.push_back(std::move(snapshot));
        }));
    ON_CALL(reply_receiver, process(A<protocol::MarketDataReject>()))
        .WillByDefault(Invoke([this](protocol::MarketDataReject reject) {
          rejects.push_back(std::move(reject));
        }));
  }

  auto TearDown() -> void override {
    middleware::release_trading_reply_channel();
  }
};

TEST_F(QuotingEngineQuoteStreamer, RejectsRequestWithoutRequestId) {
  auto request = make_subscription();
  request.request_id.reset();

  streamer.process(request, nullptr);

  ASSERT_THAT(
      rejects,
      ElementsAre(HasRejectText("required market data request id missing")));
  ASSERT_THAT(snapshots, IsEmpty());
}

TEST_F(QuotingEngineQuoteStreamer, RejectsEntryTypeOtherThanBidAndOffer) {
  auto request = make_subscription();
  request.market_data_types.emplace_back(MdEntryType::Option::Trade);

  streamer.process(request, nullptr);

  ASSERT_THAT(
      rejects,
      ElementsAre(IsReject(MdRejectReason::Option::UnsupportedMdEntryType,
                           "market data entry type Trade is not "
                           "supported on a quoting listing")));
  ASSERT_THAT(snapshots, IsEmpty());
}

TEST_F(QuotingEngineQuoteStreamer, RejectsMarketDepthOtherThanFullBook) {
  auto request = make_subscription();
  request.market_depth = MarketDepth{1};

  streamer.process(request, nullptr);

  ASSERT_THAT(rejects,
              ElementsAre(IsReject(
                  MdRejectReason::Option::UnsupportedMarketDepth,
                  "market depth 1 is not supported on a quoting listing")));
  ASSERT_THAT(snapshots, IsEmpty());
}

TEST_F(QuotingEngineQuoteStreamer, RejectsIncrementalRefreshSubscription) {
  auto request = make_subscription();
  request.update_type = MarketDataUpdateType::Option::Incremental;

  streamer.process(request, nullptr);

  ASSERT_THAT(
      rejects,
      ElementsAre(IsReject(MdRejectReason::Option::UnsupportedMdUpdateType,
                           "market data update type Incremental is not "
                           "supported on a quoting listing")));
  ASSERT_THAT(snapshots, IsEmpty());
}

TEST_F(QuotingEngineQuoteStreamer, RejectsSubscriptionWithoutUpdateType) {
  auto request = make_subscription();
  request.update_type.reset();

  streamer.process(request, nullptr);

  ASSERT_THAT(
      rejects,
      ElementsAre(IsReject(MdRejectReason::Option::UnsupportedMdUpdateType,
                           "required market data update type missing")));
  ASSERT_THAT(snapshots, IsEmpty());
}

TEST_F(QuotingEngineQuoteStreamer, AnswersSnapshotRequestWithoutUpdateType) {
  auto request = make_request(MdSubscriptionRequestType::Option::Snapshot);
  request.update_type.reset();

  streamer.process(request, nullptr);

  ASSERT_THAT(rejects, IsEmpty());
  ASSERT_THAT(snapshots, SizeIs(1));
}

TEST_F(QuotingEngineQuoteStreamer, AcceptsUnsubscriptionWithoutUpdateType) {
  streamer.process(make_subscription(), nullptr);
  snapshots.clear();
  auto request = make_request(MdSubscriptionRequestType::Option::Unsubscribe);
  request.update_type.reset();

  streamer.process(request, nullptr);
  streamer.publish(make_quote());

  ASSERT_THAT(rejects, IsEmpty());
  ASSERT_THAT(snapshots, IsEmpty());
}

TEST_F(QuotingEngineQuoteStreamer, RejectsDuplicateSubscription) {
  streamer.process(make_subscription(), nullptr);

  streamer.process(make_subscription(), nullptr);

  ASSERT_THAT(rejects,
              ElementsAre(Field(
                  &protocol::MarketDataReject::reject_reason,
                  Optional(Eq(MdRejectReason::Option::DuplicateMdReqId)))));
}

TEST_F(QuotingEngineQuoteStreamer, RejectsUnsubscriptionOfUnknownSubscription) {
  streamer.process(make_request(MdSubscriptionRequestType::Option::Unsubscribe),
                   nullptr);

  ASSERT_THAT(
      rejects,
      ElementsAre(HasRejectText("no subscription found for the request id")));
}

TEST_F(QuotingEngineQuoteStreamer,
       SendsEmptySnapshotOnSubscriptionWithoutLatestQuote) {
  streamer.process(make_subscription(), nullptr);

  ASSERT_THAT(
      snapshots,
      ElementsAre(Field(&protocol::MarketDataSnapshot::market_data_entries,
                        IsEmpty())));
}

TEST_F(QuotingEngineQuoteStreamer, SendsLatestQuoteOnSubscription) {
  const Quote quote = make_quote({make_level(1.25)}, {make_level(1.5)});

  streamer.process(make_subscription(), &quote);

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(prices_of(snapshots.front()),
              UnorderedElementsAre(Price{1.25}, Price{1.5}));
}

TEST_F(QuotingEngineQuoteStreamer, EchoesRequestIdAndInstrumentInSnapshot) {
  const auto request = make_subscription();

  streamer.process(request, nullptr);

  ASSERT_THAT(snapshots,
              ElementsAre(AllOf(Field(&protocol::MarketDataSnapshot::request_id,
                                      Eq(request.request_id)),
                                Field(&protocol::MarketDataSnapshot::instrument,
                                      Eq(request.instruments.front())))));
}

TEST_F(QuotingEngineQuoteStreamer, StreamsPublishedQuoteToEverySubscription) {
  streamer.process(make_request(MdSubscriptionRequestType::Option::Subscribe,
                                make_session("FIRST")),
                   nullptr);
  streamer.process(make_request(MdSubscriptionRequestType::Option::Subscribe,
                                make_session("SECOND")),
                   nullptr);
  snapshots.clear();

  streamer.publish(make_quote());

  ASSERT_THAT(snapshots,
              UnorderedElementsAre(Field(&protocol::MarketDataSnapshot::session,
                                         Eq(make_session("FIRST"))),
                                   Field(&protocol::MarketDataSnapshot::session,
                                         Eq(make_session("SECOND")))));
}

TEST_F(QuotingEngineQuoteStreamer, StreamsPublishedQuoteToSubscription) {
  streamer.process(make_subscription(), nullptr);
  snapshots.clear();

  streamer.publish(make_quote({make_level(1.25)}, {make_level(1.5)}));

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(prices_of(snapshots.front()),
              UnorderedElementsAre(Price{1.25}, Price{1.5}));
}

TEST_F(QuotingEngineQuoteStreamer, AnswersSnapshotRequestWithLatestQuote) {
  const Quote quote = make_quote({make_level(1.25)}, {make_level(1.5)});

  streamer.process(make_request(MdSubscriptionRequestType::Option::Snapshot),
                   &quote);

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(prices_of(snapshots.front()),
              UnorderedElementsAre(Price{1.25}, Price{1.5}));
}

TEST_F(QuotingEngineQuoteStreamer, DoesNotStreamToSnapshotRequester) {
  streamer.process(make_request(MdSubscriptionRequestType::Option::Snapshot),
                   nullptr);
  snapshots.clear();

  streamer.publish(make_quote());

  ASSERT_THAT(snapshots, IsEmpty());
}

TEST_F(QuotingEngineQuoteStreamer, StopsStreamingAfterUnsubscription) {
  streamer.process(make_subscription(), nullptr);
  streamer.process(make_request(MdSubscriptionRequestType::Option::Unsubscribe),
                   nullptr);
  snapshots.clear();

  streamer.publish(make_quote());

  ASSERT_THAT(snapshots, IsEmpty());
  ASSERT_THAT(rejects, IsEmpty());
}

TEST_F(QuotingEngineQuoteStreamer, StopsStreamingToUnsubscribedSession) {
  streamer.process(make_subscription(), nullptr);
  streamer.unsubscribe(make_session("CLIENT"));
  snapshots.clear();

  streamer.publish(make_quote());

  ASSERT_THAT(snapshots, IsEmpty());
}

TEST_F(QuotingEngineQuoteStreamer, SendsBidsInQuoteOrder) {
  const Quote quote =
      make_quote({make_level(1.0), make_level(1.25), make_level(0.75)}, {});

  streamer.process(make_subscription(), &quote);

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(prices_of(snapshots.front()),
              ElementsAre(Price{1.0}, Price{1.25}, Price{0.75}));
}

TEST_F(QuotingEngineQuoteStreamer, SendsAsksInQuoteOrder) {
  const Quote quote =
      make_quote({}, {make_level(1.5), make_level(1.25), make_level(1.75)});

  streamer.process(make_subscription(), &quote);

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(prices_of(snapshots.front()),
              ElementsAre(Price{1.5}, Price{1.25}, Price{1.75}));
}

TEST_F(QuotingEngineQuoteStreamer, SendsBidsBeforeOffers) {
  const Quote quote = make_quote({make_level(1.25)}, {make_level(1.5)});

  streamer.process(make_subscription(), &quote);

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(snapshots.front().market_data_entries,
              ElementsAre(Field(&MarketDataEntry::type,
                                Optional(Eq(MdEntryType::Option::Bid))),
                          Field(&MarketDataEntry::type,
                                Optional(Eq(MdEntryType::Option::Offer)))));
}

TEST_F(QuotingEngineQuoteStreamer, SendsOnlyBidsWhenOnlyBidsRequested) {
  const Quote quote = make_quote({make_level(1.25)}, {make_level(1.5)});
  auto request = make_subscription();
  request.market_data_types = {MdEntryType{MdEntryType::Option::Bid}};

  streamer.process(request, &quote);

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(snapshots.front().market_data_entries,
              ElementsAre(Field(&MarketDataEntry::type,
                                Optional(Eq(MdEntryType::Option::Bid)))));
}

TEST_F(QuotingEngineQuoteStreamer, SendsOnlyOffersWhenOnlyOffersRequested) {
  const Quote quote = make_quote({make_level(1.25)}, {make_level(1.5)});
  auto request = make_subscription();
  request.market_data_types = {MdEntryType{MdEntryType::Option::Offer}};

  streamer.process(request, &quote);

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(snapshots.front().market_data_entries,
              ElementsAre(Field(&MarketDataEntry::type,
                                Optional(Eq(MdEntryType::Option::Offer)))));
}

TEST_F(QuotingEngineQuoteStreamer, MarksEntriesAsOpenTradeableQuotes) {
  const Quote quote = make_quote({make_level(1.25)}, {make_level(1.5)});

  streamer.process(make_subscription(), &quote);

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(snapshots.front().market_data_entries,
              Each(AllOf(Field(&MarketDataEntry::quote_condition,
                               Optional(Eq(QuoteCondition::Option::Open))),
                         Field(&MarketDataEntry::quote_type,
                               Optional(Eq(MdQuoteType::Option::Tradeable))))));
}

TEST_F(QuotingEngineQuoteStreamer, SendsEntryIdOfEveryQuoteLevel) {
  const Quote quote = make_quote({make_level("bid-entry", 1.25)},
                                 {make_level("offer-entry", 1.5)});

  streamer.process(make_subscription(), &quote);

  ASSERT_THAT(snapshots, SizeIs(1));
  ASSERT_THAT(entry_ids_of(snapshots.front()),
              ElementsAre(Optional(Eq(MarketEntryId{"bid-entry"})),
                          Optional(Eq(MarketEntryId{"offer-entry"}))));
}

TEST_F(QuotingEngineQuoteStreamer, KeepsEntryIdsOfQuoteInLaterSnapshots) {
  const Quote quote = make_quote({make_level("bid-entry", 1.25)},
                                 {make_level("offer-entry", 1.5)});
  streamer.process(make_request(MdSubscriptionRequestType::Option::Subscribe,
                                make_session("FIRST")),
                   nullptr);
  streamer.publish(quote);
  streamer.process(make_request(MdSubscriptionRequestType::Option::Subscribe,
                                make_session("SECOND")),
                   &quote);

  ASSERT_THAT(snapshots, SizeIs(3));
  ASSERT_THAT(entry_ids_of(snapshots[1]), Each(Ne(std::nullopt)));
  ASSERT_EQ(entry_ids_of(snapshots[2]), entry_ids_of(snapshots[1]));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::quoting_engine::test
