#include <gmock/gmock.h>

#include <variant>
#include <vector>

#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "core/tools/time.hpp"
#include "ih/common/data/market_data_updates.hpp"
#include "ih/common/events/event.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/market_data/market_data_facade.hpp"
#include "matching_engine/configuration.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/order_book_notification_builder.hpp"
#include "tools/protocol_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

struct MatchingEngineMarketDataFacade : Test {
  MatchingEngineMarketDataFacade() {
    ON_CALL(event_listener, on(_)).WillByDefault([this](const Event& event) {
      events.push_back(event);
    });
  }

  static auto make_configuration() -> Configuration {
    return {.clock = core::TzClock{"Europe/Kyiv"}};
  }

  auto subscribe(MarketDataUpdateType update_type,
                 std::vector<MdEntryType> types) -> void {
    auto request = make_message<protocol::MarketDataRequest>();
    request.request_id = MdRequestId{"request"};
    InstrumentDescriptor instrument;
    instrument.symbol = Symbol{"AAPL"};
    request.instruments = {instrument};
    request.market_data_types = std::move(types);
    request.request_type = MdSubscriptionRequestType::Option::Subscribe;
    request.update_type = update_type;
    facade.process(request);
  }

  auto seat_bid(Price price, Quantity quantity) -> void {
    facade.handle(OrderBookNotification{NewOrderAdded::init()
                                            .with_order_side(Side::Option::Buy)
                                            .with_order_price(price)
                                            .with_order_quantity(quantity)
                                            .create()});
    facade.publish();
  }

  auto push_cross(Price price) -> void {
    facade.handle(
        OrderBookNotification{NewTrade().with_trade_price(price).create()});
  }

  auto push_bid_consumed(Price price) -> void {
    facade.handle(OrderBookNotification{NewOrderReduced::init()
                                            .with_order_side(Side::Option::Buy)
                                            .with_order_price(price)
                                            .with_order_quantity(Quantity{0})
                                            .create()});
  }

  auto publish() -> void { facade.publish(); }

  auto publish_uncrossing() -> void { facade.publish_uncrossing(); }

  auto clear_events() -> void { events.clear(); }

  [[nodiscard]]
  auto snapshots() const -> std::vector<const protocol::MarketDataSnapshot*> {
    std::vector<const protocol::MarketDataSnapshot*> result;
    for (const Event& event : events) {
      if (const auto* notification =
              std::get_if<ClientNotification>(&event.value)) {
        if (const auto* snapshot = std::get_if<protocol::MarketDataSnapshot>(
                &notification->value)) {
          result.push_back(snapshot);
        }
      }
    }
    return result;
  }

  [[nodiscard]]
  auto updates() const -> std::vector<const protocol::MarketDataUpdate*> {
    std::vector<const protocol::MarketDataUpdate*> result;
    for (const Event& event : events) {
      if (const auto* notification =
              std::get_if<ClientNotification>(&event.value)) {
        if (const auto* update =
                std::get_if<protocol::MarketDataUpdate>(&notification->value)) {
          result.push_back(update);
        }
      }
    }
    return result;
  }

  static auto is_bid_at(Price price) {
    return AllOf(Field(&MarketDataEntry::type, Eq(MdEntryType::Option::Bid)),
                 Field(&MarketDataEntry::price, Optional(Eq(price))));
  }

  static auto is_trade_at(Price price) {
    return AllOf(Field(&MarketDataEntry::type, Eq(MdEntryType::Option::Trade)),
                 Field(&MarketDataEntry::price, Optional(Eq(price))));
  }

  NiceMock<EventListenerMock> event_listener;
  std::vector<Event> events;
  MarketDataFacade facade =
      MarketDataFacade::setup(make_configuration(), event_listener);
};

struct MatchingEngineMarketDataFacadeUncross : MatchingEngineMarketDataFacade {
};

TEST_F(MatchingEngineMarketDataFacadeUncross,
       SendsFrozenBookWithEachCrossThenFreshSnapshotToFullSubscriber) {
  subscribe(MarketDataUpdateType::Option::Snapshot,
            {MdEntryType::Option::Trade,
             MdEntryType::Option::Bid,
             MdEntryType::Option::Offer});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  push_cross(Price{10});
  push_cross(Price{20});
  publish_uncrossing();

  EXPECT_THAT(updates(), IsEmpty());
  ASSERT_THAT(snapshots(), SizeIs(3));
  EXPECT_THAT(snapshots().at(0)->market_data_entries,
              AllOf(Contains(is_bid_at(Price{100})),
                    Contains(is_trade_at(Price{10})),
                    Not(Contains(is_trade_at(Price{20})))));
  EXPECT_THAT(
      snapshots().at(1)->market_data_entries,
      AllOf(Contains(is_bid_at(Price{100})), Contains(is_trade_at(Price{20}))));
  EXPECT_THAT(snapshots().at(2)->market_data_entries,
              Contains(is_bid_at(Price{100})));
}

TEST_F(
    MatchingEngineMarketDataFacadeUncross,
    ReportsEachCrossAgainstFrozenBookThenRebasesFullSubscriberOntoReducedBook) {
  subscribe(MarketDataUpdateType::Option::Snapshot,
            {MdEntryType::Option::Trade,
             MdEntryType::Option::Bid,
             MdEntryType::Option::Offer});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  push_cross(Price{100});
  push_bid_consumed(Price{100});
  publish_uncrossing();

  ASSERT_THAT(snapshots(), SizeIs(2));
  EXPECT_THAT(snapshots().at(0)->market_data_entries,
              AllOf(Contains(is_bid_at(Price{100})),
                    Contains(is_trade_at(Price{100}))));
  EXPECT_THAT(snapshots().at(1)->market_data_entries,
              Not(Contains(Field(&MarketDataEntry::type,
                                 Eq(MdEntryType::Option::Bid)))));
}

TEST_F(MatchingEngineMarketDataFacadeUncross,
       SendsTradeOnlyWithEachCrossThenFreshSnapshotToIncrementalSubscriber) {
  subscribe(MarketDataUpdateType::Option::Incremental,
            {MdEntryType::Option::Trade,
             MdEntryType::Option::Bid,
             MdEntryType::Option::Offer});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  push_cross(Price{10});
  push_cross(Price{20});
  publish_uncrossing();

  ASSERT_THAT(updates(), SizeIs(2));
  EXPECT_THAT(updates().at(0)->market_data_entries,
              ElementsAre(is_trade_at(Price{10})));
  EXPECT_THAT(updates().at(1)->market_data_entries,
              ElementsAre(is_trade_at(Price{20})));
  ASSERT_THAT(snapshots(), SizeIs(1));
  EXPECT_THAT(snapshots().at(0)->market_data_entries,
              Contains(is_bid_at(Price{100})));
}

TEST_F(MatchingEngineMarketDataFacadeUncross,
       SendsOnlyFinalSnapshotToSubscriberNotStreamingTrades) {
  subscribe(MarketDataUpdateType::Option::Snapshot,
            {MdEntryType::Option::Bid, MdEntryType::Option::Offer});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  push_cross(Price{10});
  push_cross(Price{20});
  publish_uncrossing();

  EXPECT_THAT(updates(), IsEmpty());
  EXPECT_THAT(snapshots(), SizeIs(1));
}

TEST_F(MatchingEngineMarketDataFacadeUncross,
       SendsOnlyFinalSnapshotWhenNoOrdersCross) {
  subscribe(MarketDataUpdateType::Option::Snapshot,
            {MdEntryType::Option::Trade,
             MdEntryType::Option::Bid,
             MdEntryType::Option::Offer});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  publish_uncrossing();

  EXPECT_THAT(updates(), IsEmpty());
  EXPECT_THAT(snapshots(), SizeIs(1));
}

TEST_F(MatchingEngineMarketDataFacadeUncross,
       PublishesTradesAsOneUpdateOnRegularPublish) {
  subscribe(MarketDataUpdateType::Option::Incremental,
            {MdEntryType::Option::Trade});
  clear_events();

  push_cross(Price{10});
  push_cross(Price{20});
  publish();

  EXPECT_THAT(snapshots(), IsEmpty());
  ASSERT_THAT(updates(), SizeIs(1));
  EXPECT_THAT(updates().at(0)->market_data_entries,
              ElementsAre(is_trade_at(Price{10}), is_trade_at(Price{20})));
}

struct MatchingEngineMarketDataFacadePublish : MatchingEngineMarketDataFacade {
  auto push_early_price(Price price, Quantity quantity) -> void {
    facade.handle(OrderBookNotification{
        EarlyPriceUpdate{.early_price = price, .early_quantity = quantity}});
  }

  auto push_market_bid(Quantity quantity) -> void {
    facade.handle(OrderBookNotification{
        NewOrderAdded::init().with_order_quantity(quantity).create()});
  }

  static auto is_early_price_at(Price price) {
    return AllOf(
        Field(&MarketDataEntry::type, Eq(MdEntryType::Option::EarlyPrice)),
        Field(&MarketDataEntry::price, Optional(Eq(price))));
  }

  static auto is_market_bid_of(Quantity quantity) {
    return AllOf(
        Field(&MarketDataEntry::type, Eq(MdEntryType::Option::MarketBid)),
        Field(&MarketDataEntry::quantity, Optional(Eq(quantity))));
  }
};

TEST_F(MatchingEngineMarketDataFacadePublish,
       SendsFullSnapshotWhenRequestedBookChanges) {
  subscribe(MarketDataUpdateType::Option::Snapshot,
            {MdEntryType::Option::Bid, MdEntryType::Option::Offer});
  clear_events();

  seat_bid(Price{100}, Quantity{5});

  ASSERT_THAT(snapshots(), SizeIs(1));
  EXPECT_THAT(snapshots().at(0)->market_data_entries,
              Contains(is_bid_at(Price{100})));
}

TEST_F(MatchingEngineMarketDataFacadePublish,
       SkipsFullSnapshotWhenOnlyUnrequestedEarlyPriceChanges) {
  subscribe(MarketDataUpdateType::Option::Snapshot,
            {MdEntryType::Option::Bid, MdEntryType::Option::Offer});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  push_early_price(Price{10}, Quantity{3});
  publish();

  EXPECT_THAT(snapshots(), IsEmpty());
  EXPECT_THAT(updates(), IsEmpty());
}

TEST_F(MatchingEngineMarketDataFacadePublish,
       SendsFullSnapshotOnEachEarlyPriceTickToSubscriberStreamingIt) {
  subscribe(MarketDataUpdateType::Option::Snapshot,
            {MdEntryType::Option::Bid, MdEntryType::Option::EarlyPrice});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  push_early_price(Price{10}, Quantity{3});
  publish();
  push_early_price(Price{10}, Quantity{3});
  publish();

  ASSERT_THAT(snapshots(), SizeIs(2));
  EXPECT_THAT(snapshots().at(1)->market_data_entries,
              AllOf(Contains(is_bid_at(Price{100})),
                    Contains(is_early_price_at(Price{10}))));
}

TEST_F(MatchingEngineMarketDataFacadePublish,
       SkipsFullSnapshotWhenOnlyUnrequestedMarketOrderRests) {
  subscribe(MarketDataUpdateType::Option::Snapshot,
            {MdEntryType::Option::Bid, MdEntryType::Option::Offer});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  push_market_bid(Quantity{7});
  publish();

  EXPECT_THAT(snapshots(), IsEmpty());
  EXPECT_THAT(updates(), IsEmpty());
}

TEST_F(MatchingEngineMarketDataFacadePublish,
       SendsFullSnapshotWithRestingMarketOrderToSubscriberStreamingIt) {
  subscribe(MarketDataUpdateType::Option::Snapshot,
            {MdEntryType::Option::Bid, MdEntryType::Option::MarketBid});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  push_market_bid(Quantity{7});
  publish();

  ASSERT_THAT(snapshots(), SizeIs(1));
  EXPECT_THAT(snapshots().at(0)->market_data_entries,
              AllOf(Contains(is_bid_at(Price{100})),
                    Contains(is_market_bid_of(Quantity{7}))));
}

TEST_F(MatchingEngineMarketDataFacadePublish,
       SendsNothingToIncrementalSubscriberWhenOnlyUnrequestedMarketOrderRests) {
  subscribe(MarketDataUpdateType::Option::Incremental,
            {MdEntryType::Option::Bid, MdEntryType::Option::Offer});
  seat_bid(Price{100}, Quantity{5});
  clear_events();

  push_market_bid(Quantity{7});
  publish();

  EXPECT_THAT(snapshots(), IsEmpty());
  EXPECT_THAT(updates(), IsEmpty());
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
