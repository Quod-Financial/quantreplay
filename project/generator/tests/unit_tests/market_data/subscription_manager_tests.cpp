#include <fmt/format.h>
#include <gmock/gmock.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/market_data_book_cache.hpp"
#include "ih/market_data/order_book_prices.hpp"
#include "ih/market_data/price_cache.hpp"
#include "ih/market_data/subscription_manager.hpp"
#include "middleware/channels/market_data_request_channel.hpp"
#include "mocks/context/component_context.hpp"
#include "mocks/market_data_request_channel.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct GeneratorSubscriptionManager : public Test {
  static constexpr DatasourceId SourceDatasourceId = 42;
  static constexpr DatasourceId OtherDatasourceId = 43;

  StrictMock<mock::MarketDataRequestReceiver> receiver;
  std::shared_ptr<NiceMock<mock::ComponentContext>> context =
      std::make_shared<NiceMock<mock::ComponentContext>>();
  std::shared_ptr<PriceCache> price_cache = std::make_shared<PriceCache>();
  std::shared_ptr<MarketDataBookCache> book_cache =
      std::make_shared<MarketDataBookCache>();
  SubscriptionManager manager{price_cache, book_cache, context};

  GeneratorSubscriptionManager() {
    ON_CALL(*context, generate_identifier()).WillByDefault([this] {
      return fmt::format("MD-{}", ++generated_identifiers_);
    });
  }

  [[nodiscard]]
  static auto make_session(std::string sender_comp_id = "SENDER")
      -> protocol::Session {
    return protocol::Session{protocol::fix::Session{
        protocol::fix::BeginString{"FIXT.1.1"},
        protocol::fix::SenderCompId{std::move(sender_comp_id)},
        protocol::fix::TargetCompId{"TARGET"}}};
  }

  [[nodiscard]]
  static auto make_requirement(Symbol symbol = Symbol{"AAPL"},
                               MarketDepth depth = AllDepthLevels,
                               protocol::Session session = make_session(),
                               DatasourceId datasource_id = SourceDatasourceId)
      -> SubscriptionRequirement {
    return SubscriptionRequirement{.session = std::move(session),
                                   .symbol = std::move(symbol),
                                   .datasource_id = datasource_id,
                                   .depth = depth,
                                   .order_counterparty = std::nullopt};
  }

  [[nodiscard]]
  static auto make_incremental_requirement() -> SubscriptionRequirement {
    SubscriptionRequirement requirement = make_requirement();
    requirement.update_type = MarketDataUpdateType::Option::Incremental;
    return requirement;
  }

  [[nodiscard]]
  static auto make_reject(MdRequestId request_id,
                          protocol::Session session = make_session())
      -> protocol::MarketDataReject {
    protocol::MarketDataReject reject{std::move(session)};
    reject.request_id = std::move(request_id);
    return reject;
  }

  auto add_datasource_subscriptions(
      const std::vector<SubscriptionRequirement>& requirements) -> void {
    for (const SubscriptionRequirement& requirement : requirements) {
      manager.add_datasource_subscription(requirement);
    }
  }

  auto add_and_subscribe(
      const std::vector<SubscriptionRequirement>& requirements) -> void {
    EXPECT_CALL(receiver, process(_)).Times(AnyNumber());
    add_datasource_subscriptions(requirements);
    manager.subscribe();
    Mock::VerifyAndClearExpectations(&receiver);
  }

  auto publish_bid_price(const MdRequestId& request_id, double price) -> void {
    protocol::MarketDataSnapshot snapshot{make_session()};
    snapshot.request_id = request_id;

    MarketDataEntry entry;
    entry.type = MdEntryType::Option::Bid;
    entry.price = Price{price};
    entry.quantity = Quantity{100};
    snapshot.market_data_entries.push_back(entry);

    price_cache->process(snapshot);
  }

  [[nodiscard]]
  static auto make_full_refresh(const MdRequestId& request_id)
      -> protocol::MarketDataSnapshot {
    protocol::MarketDataSnapshot snapshot{make_session()};
    snapshot.request_id = request_id;
    return snapshot;
  }

  [[nodiscard]]
  static auto make_update(const MdRequestId& request_id,
                          MarketDataEntry entry) -> protocol::MarketDataUpdate {
    protocol::MarketDataUpdate update{make_session()};
    update.request_id = request_id;
    update.market_data_entries.push_back(std::move(entry));
    return update;
  }

  [[nodiscard]]
  auto updates_cached_book(const MdRequestId& request_id) -> bool {
    MarketDataEntry entry;
    entry.type = MdEntryType::Option::Bid;
    entry.id = MarketEntryId{"B1"};
    entry.price = Price{100.};
    entry.quantity = Quantity{100};
    entry.action = MarketEntryAction::Option::New;

    (void)book_cache->process(make_full_refresh(request_id));
    return book_cache->process(make_update(request_id, std::move(entry)))
        .snapshot.has_value();
  }

  auto diverge_cached_book(const MdRequestId& request_id) -> void {
    MarketDataEntry entry;
    entry.type = MdEntryType::Option::Bid;
    entry.id = MarketEntryId{"UNKNOWN"};
    entry.action = MarketEntryAction::Option::Change;

    (void)book_cache->process(make_full_refresh(request_id));
    (void)book_cache->process(make_update(request_id, std::move(entry)));
  }

  auto diverge(const MdRequestId& request_id) -> void {
    EXPECT_CALL(receiver, process(_)).Times(AnyNumber());
    manager.handle_divergence(request_id);
    Mock::VerifyAndClearExpectations(&receiver);
  }

 private:
  auto SetUp() -> void override {
    middleware::bind_market_data_request_channel(
        std::shared_ptr<mock::MarketDataRequestReceiver>{
            std::addressof(receiver), [](auto* /*pointer*/) {}});
  }

  auto TearDown() -> void override {
    middleware::release_market_data_request_channel();
  }

  std::size_t generated_identifiers_ = 0;
};

TEST_F(GeneratorSubscriptionManager,
       ReturnsNoRequestIdForRequirementWithEmptySymbol) {
  ASSERT_THAT(manager.add_datasource_subscription(make_requirement(Symbol{""})),
              Eq(std::nullopt));
}

TEST_F(GeneratorSubscriptionManager, ReturnsGeneratedIdentifierAsRequestId) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));

  ASSERT_THAT(manager.add_datasource_subscription(make_requirement()),
              Optional(Field(&MergedSubscription::request_id,
                             Eq(MdRequestId{"MD-42"}))));
}

TEST_F(GeneratorSubscriptionManager,
       ReturnsRequestIdOfExistingSubscriptionForMergedRequirement) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  manager.add_datasource_subscription(make_requirement());

  ASSERT_THAT(manager.add_datasource_subscription(make_requirement()),
              Optional(Field(&MergedSubscription::request_id,
                             Eq(MdRequestId{"MD-42"}))));
}

TEST_F(GeneratorSubscriptionManager, ReturnsDepthOfRequirement) {
  ASSERT_THAT(manager.add_datasource_subscription(
                  make_requirement(Symbol{"AAPL"}, MarketDepth{5})),
              Optional(Field(&MergedSubscription::depth, Eq(MarketDepth{5}))));
}

TEST_F(GeneratorSubscriptionManager, ReturnsMaximumDepthOfMergedRequirements) {
  manager.add_datasource_subscription(
      make_requirement(Symbol{"AAPL"}, MarketDepth{5}));

  ASSERT_THAT(manager.add_datasource_subscription(
                  make_requirement(Symbol{"AAPL"}, MarketDepth{1})),
              Optional(Field(&MergedSubscription::depth, Eq(MarketDepth{5}))));
}

TEST_F(GeneratorSubscriptionManager,
       ReturnsAllDepthLevelsWhenAnyMergedRequirementRequestsFullBook) {
  manager.add_datasource_subscription(
      make_requirement(Symbol{"AAPL"}, AllDepthLevels));

  ASSERT_THAT(manager.add_datasource_subscription(
                  make_requirement(Symbol{"AAPL"}, MarketDepth{5})),
              Optional(Field(&MergedSubscription::depth, Eq(AllDepthLevels))));
}

TEST_F(GeneratorSubscriptionManager, ClearsCachedMarketDataOnUnsubscribe) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  EXPECT_CALL(receiver, process(_)).Times(AnyNumber());
  manager.add_datasource_subscription(make_requirement());
  price_cache->add(MdRequestId{"MD-42"});
  manager.subscribe();
  publish_bid_price(MdRequestId{"MD-42"}, 100.);
  ASSERT_THAT(
      price_cache
          ->market_state(MdRequestId{"MD-42"}, {}, OrderBookPrices::Aggregated)
          .best_bid_price,
      Optional(DoubleEq(100.)));

  manager.unsubscribe();

  ASSERT_THAT(
      price_cache
          ->market_state(MdRequestId{"MD-42"}, {}, OrderBookPrices::Aggregated)
          .best_bid_price,
      Eq(std::nullopt));
}

TEST_F(GeneratorSubscriptionManager, IgnoresRequirementWithEmptySymbol) {
  add_datasource_subscriptions({make_requirement(Symbol{""})});

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsSingleRequestPerDistinctInstrument) {
  add_datasource_subscriptions({make_requirement(), make_requirement()});

  EXPECT_CALL(receiver, process(_)).Times(1);

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsSeparateRequestsForDifferentSymbolsOnSameSession) {
  add_datasource_subscriptions(
      {make_requirement(Symbol{"AAPL"}), make_requirement(Symbol{"MSFT"})});

  EXPECT_CALL(receiver, process(_)).Times(2);

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsSeparateRequestsForSameSymbolOnDifferentSessions) {
  add_datasource_subscriptions(
      {make_requirement(Symbol{"AAPL"}, AllDepthLevels, make_session("A")),
       make_requirement(Symbol{"AAPL"}, AllDepthLevels, make_session("B"))});

  EXPECT_CALL(receiver, process(_)).Times(2);

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsMaximumDepthOfMergedRequirements) {
  add_datasource_subscriptions(
      {make_requirement(Symbol{"AAPL"}, MarketDepth{1}),
       make_requirement(Symbol{"AAPL"}, MarketDepth{5})});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::market_depth,
                            Optional(Eq(MarketDepth{5})))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsAllDepthLevelsWhenAnyRequirementRequestsFullBook) {
  add_datasource_subscriptions(
      {make_requirement(Symbol{"AAPL"}, MarketDepth{5}),
       make_requirement(Symbol{"AAPL"}, AllDepthLevels)});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::market_depth,
                            Optional(Eq(AllDepthLevels)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsAllDepthLevelsWhenFullBookRequirementPrecedesLimitedDepth) {
  add_datasource_subscriptions(
      {make_requirement(Symbol{"AAPL"}, AllDepthLevels),
       make_requirement(Symbol{"AAPL"}, MarketDepth{5})});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::market_depth,
                            Optional(Eq(AllDepthLevels)))));

  manager.subscribe();
}

// The market simulator, as an acceptor, does not support subscriptions
// with depth other than 0 and 1.
// But as an initiator, it must support subscriptions at any depth.
TEST_F(GeneratorSubscriptionManager, SendsUnsupportedDepthAsConfigured) {
  add_datasource_subscriptions(
      {make_requirement(Symbol{"AAPL"}, MarketDepth{5})});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::market_depth,
                            Optional(Eq(MarketDepth{5})))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsRequestWithSingleInstrument) {
  add_datasource_subscriptions({make_requirement()});

  EXPECT_CALL(
      receiver,
      process(Field(&protocol::MarketDataRequest::instruments, SizeIs(1))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsRequestWithSymbol) {
  add_datasource_subscriptions({make_requirement()});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::instruments,
                            ElementsAre(Field(&InstrumentDescriptor::symbol,
                                              Optional(Eq(Symbol{"AAPL"})))))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsRequestWithBidAndOfferEntryTypes) {
  add_datasource_subscriptions({make_requirement()});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::market_data_types,
                            ElementsAre(Eq(MdEntryType::Option::Bid),
                                        Eq(MdEntryType::Option::Offer)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsRequestWithFullRefreshUpdateType) {
  add_datasource_subscriptions({make_requirement()});

  EXPECT_CALL(
      receiver,
      process(Field(&protocol::MarketDataRequest::update_type,
                    Optional(Eq(MarketDataUpdateType::Option::Snapshot)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsRequestWithIncrementalUpdateTypeForIncrementalRequirement) {
  add_datasource_subscriptions({make_incremental_requirement()});

  EXPECT_CALL(
      receiver,
      process(Field(&protocol::MarketDataRequest::update_type,
                    Optional(Eq(MarketDataUpdateType::Option::Incremental)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsFullRefreshUpdateTypeWhenAnyMergedRequirementIsFullRefresh) {
  add_datasource_subscriptions({make_incremental_requirement(),
                                make_requirement(),
                                make_incremental_requirement()});

  EXPECT_CALL(
      receiver,
      process(Field(&protocol::MarketDataRequest::update_type,
                    Optional(Eq(MarketDataUpdateType::Option::Snapshot)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsIncrementalUpdateTypeWhenAllMergedRequirementsAreIncremental) {
  add_datasource_subscriptions(
      {make_incremental_requirement(), make_incremental_requirement()});

  EXPECT_CALL(
      receiver,
      process(Field(&protocol::MarketDataRequest::update_type,
                    Optional(Eq(MarketDataUpdateType::Option::Incremental)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsIncrementalUnsubscribeRequestForIncrementalSubscription) {
  add_and_subscribe({make_incremental_requirement()});

  EXPECT_CALL(
      receiver,
      process(Field(&protocol::MarketDataRequest::update_type,
                    Optional(Eq(MarketDataUpdateType::Option::Incremental)))));

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsRequestWithSubscribeRequestType) {
  add_datasource_subscriptions({make_requirement()});

  EXPECT_CALL(receiver,
              process(Field(
                  &protocol::MarketDataRequest::request_type,
                  Optional(Eq(MdSubscriptionRequestType::Option::Subscribe)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsRequestWithRequestedSession) {
  add_datasource_subscriptions(
      {make_requirement(Symbol{"AAPL"}, AllDepthLevels, make_session("A"))});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::session,
                            Eq(make_session("A")))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsEngagedZeroMarketDepthForFullBookSubscription) {
  add_datasource_subscriptions(
      {make_requirement(Symbol{"AAPL"}, AllDepthLevels)});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::market_depth,
                            Optional(Eq(MarketDepth{0})))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsRequestWithGeneratedRequestId) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_datasource_subscriptions({make_requirement()});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::request_id,
                            Optional(Eq(MdRequestId{"MD-42"})))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsNothingWhenNoRequirementsConfigured) {
  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.subscribe();
  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsNothingOnRepeatedSubscribe) {
  add_and_subscribe({make_requirement()});

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsRemainingRequestsWhenSendingOneRequestThrows) {
  add_datasource_subscriptions(
      {make_requirement(Symbol{"AAPL"}), make_requirement(Symbol{"MSFT"})});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::instruments,
                            ElementsAre(Field(&InstrumentDescriptor::symbol,
                                              Optional(Eq(Symbol{"AAPL"})))))))
      .WillOnce(Throw(std::runtime_error{"channel failure"}));

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::instruments,
                            ElementsAre(Field(&InstrumentDescriptor::symbol,
                                              Optional(Eq(Symbol{"MSFT"})))))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsNoUnsubscribeRequestWhenNotSubscribed) {
  add_datasource_subscriptions({make_requirement()});

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsNoUnsubscribeRequestForSubscriptionThatFailedToSubscribe) {
  add_datasource_subscriptions({make_requirement()});
  EXPECT_CALL(receiver, process(_))
      .WillOnce(Throw(std::runtime_error{"channel failure"}));
  manager.subscribe();
  Mock::VerifyAndClearExpectations(&receiver);

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager, SendsNothingOnRepeatedUnsubscribe) {
  add_and_subscribe({make_requirement()});
  EXPECT_CALL(receiver, process(_)).Times(1);
  manager.unsubscribe();
  Mock::VerifyAndClearExpectations(&receiver);

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsUnsubscribeRequestWithRequestIdOfSubscription) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_requirement()});

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::request_id,
                            Optional(Eq(MdRequestId{"MD-42"})))));

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsUnsubscribeRequestWithUnsubscribeRequestType) {
  add_and_subscribe({make_requirement()});

  EXPECT_CALL(
      receiver,
      process(
          Field(&protocol::MarketDataRequest::request_type,
                Optional(Eq(MdSubscriptionRequestType::Option::Unsubscribe)))));

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager, ResendsSubscribeRequestAfterUnsubscribe) {
  add_and_subscribe({make_requirement()});
  EXPECT_CALL(receiver, process(_)).Times(1);
  manager.unsubscribe();
  Mock::VerifyAndClearExpectations(&receiver);

  EXPECT_CALL(receiver,
              process(Field(
                  &protocol::MarketDataRequest::request_type,
                  Optional(Eq(MdSubscriptionRequestType::Option::Subscribe)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager, ReturnsNoRequestIdForUnknownSymbol) {
  add_datasource_subscriptions({make_requirement()});

  ASSERT_THAT(manager.find_request_id(SourceDatasourceId, Symbol{"MSFT"}),
              Eq(std::nullopt));
}

TEST_F(GeneratorSubscriptionManager, ReturnsNoRequestIdForUnknownDatasource) {
  add_datasource_subscriptions({make_requirement()});

  ASSERT_THAT(manager.find_request_id(OtherDatasourceId, Symbol{"AAPL"}),
              Eq(std::nullopt));
}

TEST_F(GeneratorSubscriptionManager, ReturnsRequestIdOfConfiguredInstrument) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_datasource_subscriptions({make_requirement()});

  ASSERT_THAT(manager.find_request_id(SourceDatasourceId, Symbol{"AAPL"}),
              Optional(Eq(MdRequestId{"MD-42"})));
}

TEST_F(GeneratorSubscriptionManager,
       ReturnsRequestIdOfMergedSubscriptionForEveryDatasource) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_datasource_subscriptions(
      {make_requirement(
           Symbol{"AAPL"}, AllDepthLevels, make_session(), SourceDatasourceId),
       make_requirement(
           Symbol{"AAPL"}, AllDepthLevels, make_session(), OtherDatasourceId)});

  EXPECT_THAT(manager.find_request_id(SourceDatasourceId, Symbol{"AAPL"}),
              Optional(Eq(MdRequestId{"MD-42"})));
  EXPECT_THAT(manager.find_request_id(OtherDatasourceId, Symbol{"AAPL"}),
              Optional(Eq(MdRequestId{"MD-42"})));
}

TEST_F(GeneratorSubscriptionManager, SendsNothingOnSessionDisconnection) {
  add_and_subscribe({make_requirement()});

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.handle_session_disconnected(make_session());
}

TEST_F(GeneratorSubscriptionManager,
       ClearsCachedMarketDataOnSessionDisconnection) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  price_cache->add(MdRequestId{"MD-42"});
  add_and_subscribe({make_requirement()});
  publish_bid_price(MdRequestId{"MD-42"}, 100.);
  ASSERT_THAT(
      price_cache
          ->market_state(MdRequestId{"MD-42"}, {}, OrderBookPrices::Aggregated)
          .best_bid_price,
      Optional(DoubleEq(100.)));

  manager.handle_session_disconnected(make_session());

  ASSERT_THAT(
      price_cache
          ->market_state(MdRequestId{"MD-42"}, {}, OrderBookPrices::Aggregated)
          .best_bid_price,
      Eq(std::nullopt));
}

TEST_F(GeneratorSubscriptionManager,
       SendsNoUnsubscribeRequestAfterSessionDisconnection) {
  add_and_subscribe({make_requirement()});
  manager.handle_session_disconnected(make_session());

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager,
       ResendsSubscribeRequestOnSessionReestablishment) {
  add_and_subscribe({make_requirement()});
  manager.handle_session_disconnected(make_session());

  EXPECT_CALL(receiver,
              process(Field(
                  &protocol::MarketDataRequest::request_type,
                  Optional(Eq(MdSubscriptionRequestType::Option::Subscribe)))));

  manager.handle_session_connected(make_session());
}

TEST_F(GeneratorSubscriptionManager,
       ResendsSubscribeRequestWithRequestIdOfSubscription) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_requirement()});
  manager.handle_session_disconnected(make_session());

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::request_id,
                            Optional(Eq(MdRequestId{"MD-42"})))));

  manager.handle_session_connected(make_session());
}

TEST_F(GeneratorSubscriptionManager,
       ResendsSubscribeRequestsOfReestablishedSessionOnly) {
  add_and_subscribe(
      {make_requirement(Symbol{"AAPL"}, AllDepthLevels, make_session("A")),
       make_requirement(Symbol{"MSFT"}, AllDepthLevels, make_session("B"))});
  manager.handle_session_disconnected(make_session("A"));
  manager.handle_session_disconnected(make_session("B"));

  EXPECT_CALL(receiver,
              process(Field(&protocol::MarketDataRequest::session,
                            Eq(make_session("A")))));

  manager.handle_session_connected(make_session("A"));
}

// A subscribe request sent while the session was down never reached the
// counterparty, so the logon that follows has to request the subscription.
TEST_F(GeneratorSubscriptionManager,
       ResendsSubscribeRequestOnSessionConnectionWithoutDisconnection) {
  add_and_subscribe({make_requirement()});

  EXPECT_CALL(receiver,
              process(Field(
                  &protocol::MarketDataRequest::request_type,
                  Optional(Eq(MdSubscriptionRequestType::Option::Subscribe)))));

  manager.handle_session_connected(make_session());
}

TEST_F(GeneratorSubscriptionManager,
       SendsNothingOnSessionConnectionWhenNeverSubscribed) {
  add_datasource_subscriptions({make_requirement()});

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.handle_session_connected(make_session());
}

TEST_F(GeneratorSubscriptionManager,
       SendsNothingOnSessionConnectionAfterUnsubscribe) {
  add_and_subscribe({make_requirement()});
  EXPECT_CALL(receiver, process(_)).Times(1);
  manager.unsubscribe();
  Mock::VerifyAndClearExpectations(&receiver);

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.handle_session_connected(make_session());
}

TEST_F(GeneratorSubscriptionManager,
       SendsNothingOnSessionConnectionForUnknownSession) {
  add_and_subscribe({make_requirement()});

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.handle_session_connected(make_session("OTHER"));
}

TEST_F(GeneratorSubscriptionManager,
       SendsUnsubscribeRequestAfterSessionReestablishment) {
  add_and_subscribe({make_requirement()});
  manager.handle_session_disconnected(make_session());
  EXPECT_CALL(receiver, process(_)).Times(1);
  manager.handle_session_connected(make_session());
  Mock::VerifyAndClearExpectations(&receiver);

  EXPECT_CALL(
      receiver,
      process(
          Field(&protocol::MarketDataRequest::request_type,
                Optional(Eq(MdSubscriptionRequestType::Option::Unsubscribe)))));

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager, IgnoresRejectOfUnknownSubscription) {
  add_and_subscribe({make_requirement()});
  manager.handle_reject(make_reject(MdRequestId{"MD-UNKNOWN"}));

  EXPECT_CALL(
      receiver,
      process(
          Field(&protocol::MarketDataRequest::request_type,
                Optional(Eq(MdSubscriptionRequestType::Option::Unsubscribe)))));

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager, IgnoresRejectReceivedOnForeignSession) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_requirement()});
  manager.handle_reject(
      make_reject(MdRequestId{"MD-42"}, make_session("OTHER")));

  EXPECT_CALL(
      receiver,
      process(
          Field(&protocol::MarketDataRequest::request_type,
                Optional(Eq(MdSubscriptionRequestType::Option::Unsubscribe)))));

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsNoUnsubscribeRequestForRejectedSubscription) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_requirement()});
  manager.handle_reject(make_reject(MdRequestId{"MD-42"}));

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager,
       ResendsSubscribeRequestForRejectedSubscriptionOnSubscribe) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_requirement()});
  manager.handle_reject(make_reject(MdRequestId{"MD-42"}));

  EXPECT_CALL(receiver,
              process(Field(
                  &protocol::MarketDataRequest::request_type,
                  Optional(Eq(MdSubscriptionRequestType::Option::Subscribe)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       ClearsCachedMarketDataOfRejectedSubscription) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  price_cache->add(MdRequestId{"MD-42"});
  add_and_subscribe({make_requirement()});
  publish_bid_price(MdRequestId{"MD-42"}, 100.);
  ASSERT_THAT(
      price_cache
          ->market_state(MdRequestId{"MD-42"}, {}, OrderBookPrices::Aggregated)
          .best_bid_price,
      Optional(DoubleEq(100.)));

  manager.handle_reject(make_reject(MdRequestId{"MD-42"}));

  ASSERT_THAT(
      price_cache
          ->market_state(MdRequestId{"MD-42"}, {}, OrderBookPrices::Aggregated)
          .best_bid_price,
      Eq(std::nullopt));
}

TEST_F(GeneratorSubscriptionManager,
       CachesBookOfIncrementalSubscriptionOnSubscribe) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));

  add_and_subscribe({make_incremental_requirement()});

  ASSERT_TRUE(updates_cached_book(MdRequestId{"MD-42"}));
}

TEST_F(GeneratorSubscriptionManager, DropsCachedBookWhenSubscribeRequestFails) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_datasource_subscriptions({make_incremental_requirement()});
  EXPECT_CALL(receiver, process(_))
      .WillOnce(Throw(std::runtime_error{"send failed"}));

  manager.subscribe();

  ASSERT_FALSE(updates_cached_book(MdRequestId{"MD-42"}));
}

TEST_F(GeneratorSubscriptionManager, DropsCachedBookOnUnsubscribe) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_incremental_requirement()});
  EXPECT_CALL(receiver, process(_)).Times(AnyNumber());

  manager.unsubscribe();

  ASSERT_FALSE(updates_cached_book(MdRequestId{"MD-42"}));
}

TEST_F(GeneratorSubscriptionManager, DropsCachedBookOnSessionDisconnection) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_incremental_requirement()});

  manager.handle_session_disconnected(make_session());

  ASSERT_FALSE(updates_cached_book(MdRequestId{"MD-42"}));
}

TEST_F(GeneratorSubscriptionManager, DropsCachedBookOfRejectedSubscription) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_incremental_requirement()});

  manager.handle_reject(make_reject(MdRequestId{"MD-42"}));

  ASSERT_FALSE(updates_cached_book(MdRequestId{"MD-42"}));
}

TEST_F(GeneratorSubscriptionManager, IgnoresDivergenceOfUnknownSubscription) {
  add_and_subscribe({make_incremental_requirement()});
  manager.handle_divergence(MdRequestId{"MD-UNKNOWN"});

  EXPECT_CALL(
      receiver,
      process(
          Field(&protocol::MarketDataRequest::request_type,
                Optional(Eq(MdSubscriptionRequestType::Option::Unsubscribe)))));

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager,
       SendsNothingOnDivergenceOfUnsubscribedSubscription) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_datasource_subscriptions({make_incremental_requirement()});

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.handle_divergence(MdRequestId{"MD-42"});
}

TEST_F(GeneratorSubscriptionManager, SendsUnsubscribeRequestOnDivergence) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_incremental_requirement()});

  EXPECT_CALL(
      receiver,
      process(AllOf(
          Field(&protocol::MarketDataRequest::request_id,
                Optional(Eq(MdRequestId{"MD-42"}))),
          Field(
              &protocol::MarketDataRequest::request_type,
              Optional(Eq(MdSubscriptionRequestType::Option::Unsubscribe))))));

  manager.handle_divergence(MdRequestId{"MD-42"});
}

TEST_F(GeneratorSubscriptionManager,
       SendsNoUnsubscribeRequestForDivergedSubscription) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_incremental_requirement()});
  diverge(MdRequestId{"MD-42"});

  EXPECT_CALL(receiver, process(_)).Times(0);

  manager.unsubscribe();
}

TEST_F(GeneratorSubscriptionManager,
       ResendsSubscribeRequestForDivergedSubscriptionOnSubscribe) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_incremental_requirement()});
  diverge(MdRequestId{"MD-42"});

  EXPECT_CALL(receiver,
              process(Field(
                  &protocol::MarketDataRequest::request_type,
                  Optional(Eq(MdSubscriptionRequestType::Option::Subscribe)))));

  manager.subscribe();
}

TEST_F(GeneratorSubscriptionManager,
       ClearsCachedMarketDataOfDivergedSubscription) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  price_cache->add(MdRequestId{"MD-42"});
  add_and_subscribe({make_incremental_requirement()});
  publish_bid_price(MdRequestId{"MD-42"}, 100.);

  diverge(MdRequestId{"MD-42"});

  ASSERT_THAT(
      price_cache
          ->market_state(MdRequestId{"MD-42"}, {}, OrderBookPrices::Aggregated)
          .best_bid_price,
      Eq(std::nullopt));
}

TEST_F(GeneratorSubscriptionManager,
       KeepsFullRefreshesOfDivergedBookUnpublished) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));
  add_and_subscribe({make_incremental_requirement()});
  diverge_cached_book(MdRequestId{"MD-42"});

  diverge(MdRequestId{"MD-42"});

  ASSERT_FALSE(book_cache->process(make_full_refresh(MdRequestId{"MD-42"})));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
