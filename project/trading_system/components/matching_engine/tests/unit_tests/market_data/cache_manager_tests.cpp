#include <gmock/gmock.h>

#include <optional>

#include "core/domain/market_data_entry.hpp"
#include "core/tools/time.hpp"
#include "ih/common/data/market_data_updates.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/market_data/cache/cache_manager.hpp"
#include "ih/market_data/streaming_settings.hpp"
#include "matching_engine/configuration.hpp"
#include "tools/order_book_notification_builder.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

struct MatchingEngineCacheManager : Test {
  static auto make_configuration() -> Configuration {
    return {.clock = core::TzClock{"Europe/Kyiv"}};
  }

  auto push(auto&& notification) -> void {
    cache.push(OrderBookNotification{
        std::forward<decltype(notification)>(notification)});
  }

  static auto resting_bid(Price price, Quantity quantity) -> OrderAdded {
    return NewOrderAdded::init()
        .with_order_side(Side::Option::Buy)
        .with_order_price(price)
        .with_order_quantity(quantity)
        .create();
  }

  static auto remove_bid(Price price) -> OrderReduced {
    return NewOrderReduced::init()
        .with_order_side(Side::Option::Buy)
        .with_order_price(price)
        .with_order_quantity(Quantity{0})
        .create();
  }

  static auto trade_at(Price price) -> Trade {
    return NewTrade().with_trade_price(price).create();
  }

  static auto closing_auction_cross(Price price, Quantity quantity)
      -> AuctionFinalPriceUpdate {
    return AuctionFinalPriceUpdate{
        .auction_phase = TradingPhase{TradingPhase::Option::ClosingAuction},
        .clearing_value = TradeResult{.price = price, .quantity = quantity}};
  }

  static auto is_bid_at(Price price) {
    return AllOf(Field(&MarketDataEntry::type, Eq(MdEntryType::Option::Bid)),
                 Field(&MarketDataEntry::price, Optional(Eq(price))));
  }

  static auto is_trade() {
    return Field(&MarketDataEntry::type, Eq(MdEntryType::Option::Trade));
  }

  static auto is_trade_at(Price price) {
    return AllOf(Field(&MarketDataEntry::type, Eq(MdEntryType::Option::Trade)),
                 Field(&MarketDataEntry::price, Optional(Eq(price))));
  }

  StreamingSettings settings;
  CacheManager cache{make_configuration()};
};

TEST_F(MatchingEngineCacheManager,
       ReportsPendingTradesInCrossOrderIgnoringRest) {
  push(trade_at(Price{10}));
  push(remove_bid(Price{9}));
  push(trade_at(Price{20}));

  EXPECT_THAT(cache.pending_trades(),
              ElementsAre(Field(&Trade::trade_price, Eq(Price{10})),
                          Field(&Trade::trade_price, Eq(Price{20}))));
}

TEST_F(MatchingEngineCacheManager, ReportsNoPendingTradesWhenNoOrdersCross) {
  push(resting_bid(Price{9}, Quantity{5}));

  EXPECT_THAT(cache.pending_trades(), IsEmpty());
}

TEST_F(MatchingEngineCacheManager, ComposesTradeEntryWhenTradesAreRequested) {
  settings.enable_data_type_streaming(MdEntryType::Option::Trade);

  const auto entry = cache.compose_trade(settings, trade_at(Price{42}));

  ASSERT_THAT(entry, Ne(std::nullopt));
  EXPECT_THAT(entry->type, Eq(MdEntryType::Option::Trade));
  EXPECT_THAT(entry->price, Optional(Eq(Price{42})));
}

TEST_F(MatchingEngineCacheManager,
       ComposesNoTradeEntryWhenTradesAreNotRequested) {
  EXPECT_THAT(cache.compose_trade(settings, trade_at(Price{42})),
              Eq(std::nullopt));
}

TEST_F(MatchingEngineCacheManager, ComposesBookWithoutTheLastTrade) {
  settings.enable_data_type_streaming(MdEntryType::Option::Bid)
      .enable_data_type_streaming(MdEntryType::Option::Trade);
  push(resting_bid(Price{100}, Quantity{5}));
  push(trade_at(Price{100}));
  cache.apply_pending_changes();

  ASSERT_THAT(cache.compose_initial(settings), Contains(is_trade()));

  const auto book = cache.compose_book(settings);
  EXPECT_THAT(book, Contains(is_bid_at(Price{100})));
  EXPECT_THAT(book, Not(Contains(is_trade())));
}

TEST_F(MatchingEngineCacheManager,
       ComposesBookIgnoringUnappliedPendingChanges) {
  settings.enable_data_type_streaming(MdEntryType::Option::Bid);
  push(resting_bid(Price{100}, Quantity{5}));
  cache.apply_pending_changes();

  push(remove_bid(Price{100}));
  EXPECT_THAT(cache.compose_book(settings), Contains(is_bid_at(Price{100})));

  cache.apply_pending_changes();
  EXPECT_THAT(cache.compose_book(settings),
              Not(Contains(Field(&MarketDataEntry::type,
                                 Eq(MdEntryType::Option::Bid)))));
}

TEST_F(MatchingEngineCacheManager,
       ComposesFullUpdateWithBatchTradesAndFullBook) {
  settings.enable_data_type_streaming(MdEntryType::Option::Bid)
      .enable_data_type_streaming(MdEntryType::Option::Trade);
  push(resting_bid(Price{100}, Quantity{5}));
  cache.apply_pending_changes();

  push(trade_at(Price{10}));
  push(trade_at(Price{20}));
  cache.apply_pending_changes();

  EXPECT_THAT(cache.compose_full_update(settings),
              ElementsAre(is_trade_at(Price{10}),
                          is_trade_at(Price{20}),
                          is_bid_at(Price{100})));
}

TEST_F(MatchingEngineCacheManager, ComposesUpdateAsDeltaOfTheLastBatchOnly) {
  settings.enable_data_type_streaming(MdEntryType::Option::Bid)
      .enable_data_type_streaming(MdEntryType::Option::Trade);
  push(resting_bid(Price{100}, Quantity{5}));
  cache.apply_pending_changes();

  push(trade_at(Price{10}));
  push(trade_at(Price{20}));
  cache.apply_pending_changes();

  EXPECT_THAT(cache.compose_update(settings),
              ElementsAre(is_trade_at(Price{10}), is_trade_at(Price{20})));
}

TEST_F(MatchingEngineCacheManager, HasNoUpdateWhenOnlyUnrequestedTypesChanged) {
  settings.enable_data_type_streaming(MdEntryType::Option::Bid)
      .enable_data_type_streaming(MdEntryType::Option::Offer);

  push(trade_at(Price{10}));
  cache.apply_pending_changes();

  EXPECT_FALSE(cache.has_update(settings));
}

TEST_F(MatchingEngineCacheManager, HasUpdateWhenRequestedTradeOccurs) {
  settings.enable_data_type_streaming(MdEntryType::Option::Trade);

  push(trade_at(Price{10}));
  cache.apply_pending_changes();

  EXPECT_TRUE(cache.has_update(settings));
}

TEST_F(MatchingEngineCacheManager, HasUpdateWhenRequestedDepthChanges) {
  settings.enable_data_type_streaming(MdEntryType::Option::Bid);

  push(resting_bid(Price{100}, Quantity{5}));
  cache.apply_pending_changes();

  EXPECT_TRUE(cache.has_update(settings));
}

TEST_F(MatchingEngineCacheManager, ExposesLastOpenPhaseTradedPrice) {
  push(trade_at(Price{100}));
  cache.apply_pending_changes();

  EXPECT_EQ(cache.last_open_phase_traded_price(), Price{100});
}

TEST_F(MatchingEngineCacheManager, ExposesClosingPrice) {
  push(closing_auction_cross(Price{120}, Quantity{5}));
  cache.apply_pending_changes();

  EXPECT_EQ(cache.closing_price(), Price{120});
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test
