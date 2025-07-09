#include <gmock/gmock.h>

#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/cache/depth_cache.hpp"
#include "ih/market_data/streaming_settings.hpp"
#include "mocks/mock_market_entry_id_generator.hpp"
#include "tools/order_book_notification_builder.hpp"

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

namespace simulator::trading_system::matching_engine::mdata {
namespace {

struct DepthCacheTest : Test {
  static auto make_update(auto&&... changes) {
    return std::vector{
        OrderBookNotification{std::forward<decltype(changes)>(changes)}...};
  }

  static auto buy_order_added(OrderId id, Price px, Quantity vol) {
    return make_order_added(id, px, vol).with_order_side(Side::Option::Buy);
  }

  static auto buy_order_reduced(OrderId id, Price px, Quantity vol) {
    return make_order_reduced(id, px, vol).with_order_side(Side::Option::Buy);
  }

  static auto sell_order_added(OrderId id, Price px, Quantity vol) {
    return make_order_added(id, px, vol).with_order_side(Side::Option::Sell);
  }

  static auto sell_order_reduced(OrderId id, Price px, Quantity vol) {
    return make_order_reduced(id, px, vol).with_order_side(Side::Option::Sell);
  }

  static auto BidEntryWith(
      std::optional<Price> price,
      std::optional<Quantity> quantity,
      std::optional<MarketEntryAction> action = std::nullopt) {
    return AllOf(Field(&MarketDataEntry::price, Eq(price)),
                 Field(&MarketDataEntry::quantity, Eq(quantity)),
                 Field(&MarketDataEntry::type, Eq(MdEntryType::Option::Bid)),
                 Field(&MarketDataEntry::action, Eq(action)));
  }

  static auto OfferEntryWith(
      std::optional<Price> price,
      std::optional<Quantity> quantity,
      std::optional<MarketEntryAction> action = std::nullopt) {
    return AllOf(Field(&MarketDataEntry::price, Eq(price)),
                 Field(&MarketDataEntry::quantity, Eq(quantity)),
                 Field(&MarketDataEntry::type, Eq(MdEntryType::Option::Offer)),
                 Field(&MarketDataEntry::action, Eq(action)));
  }

  StreamingSettings settings;
  std::vector<MarketDataEntry> data;
  protocol::InstrumentState state;
  NiceMock<MockMarketEntryIdGenerator> id_generator;
  DepthCache cache{id_generator};

  constexpr static std::optional<Price> NoPrice = std::nullopt;
  constexpr static std::optional<Quantity> NoQuantity = std::nullopt;

 private:
  static auto make_order_added(OrderId id, Price px, Quantity vol)
      -> NewOrderAdded {
    return NewOrderAdded()
        .with_order_id(id)
        .with_order_price(px)
        .with_order_quantity(vol);
  }

  static auto make_order_reduced(OrderId id, Price px, Quantity vol)
      -> NewOrderReduced {
    return NewOrderReduced()
        .with_order_id(id)
        .with_order_price(px)
        .with_order_quantity(vol);
  }

  auto SetUp() -> void override {
    settings.enable_data_type_streaming(MdEntryType::Option::Bid)
        .enable_data_type_streaming(MdEntryType::Option::Offer);

    ON_CALL(id_generator, generate())
        .WillByDefault(Return(MarketEntryId{"entry-id"}));
  }
};

TEST_F(DepthCacheTest, ReportsEmptyInitialWhenNoUpdatesApplied) {
  cache.compose_initial(settings, data);

  ASSERT_THAT(data, IsEmpty());
}

TEST_F(DepthCacheTest, ReportsSingleBidLevelInFullUpdate) {
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(10), Quantity(100))));

  cache.compose_initial(settings, data);

  ASSERT_THAT(data, ElementsAre(BidEntryWith(Price(10), Quantity(100))));
}

TEST_F(DepthCacheTest, ReportsSingleOfferLevelInFullUpdate) {
  cache.update(
      make_update(sell_order_added(OrderId(1), Price(10), Quantity(100))));

  cache.compose_initial(settings, data);

  ASSERT_THAT(data, ElementsAre(OfferEntryWith(Price(10), Quantity(100))));
}

TEST_F(DepthCacheTest, ReportsMultipleLevelsInInitialUpdate) {
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(100), Quantity(100)),
                  buy_order_added(OrderId(2), Price(150), Quantity(150)),
                  sell_order_added(OrderId(3), Price(200), Quantity(200)),
                  sell_order_added(OrderId(4), Price(250), Quantity(250))));

  cache.compose_initial(settings, data);

  ASSERT_THAT(data,
              ElementsAre(BidEntryWith(Price(150), Quantity(150)),
                          BidEntryWith(Price(100), Quantity(100)),
                          OfferEntryWith(Price(200), Quantity(200)),
                          OfferEntryWith(Price(250), Quantity(250))));
}

TEST_F(DepthCacheTest, ReportsFirstLevelOnlyInInitialUpdate) {
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(100), Quantity(100)),
                  buy_order_added(OrderId(2), Price(150), Quantity(150)),
                  sell_order_added(OrderId(3), Price(200), Quantity(200)),
                  sell_order_added(OrderId(4), Price(250), Quantity(250))));

  settings.enable_top_of_book_only_streaming();
  cache.compose_initial(settings, data);

  ASSERT_THAT(data,
              ElementsAre(BidEntryWith(Price(150), Quantity(150)),
                          OfferEntryWith(Price(200), Quantity(200))));
}

TEST_F(DepthCacheTest, ReportsInitialWhenFullUpdateRequested) {
  cache.update(
      make_update(buy_order_added(OrderId(2), Price(150), Quantity(150)),
                  sell_order_added(OrderId(3), Price(200), Quantity(200))));
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(100), Quantity(100)),
                  sell_order_added(OrderId(4), Price(250), Quantity(250))));

  settings.enable_full_update_streaming();
  cache.compose_initial(settings, data);

  ASSERT_THAT(data,
              ElementsAre(BidEntryWith(Price(150), Quantity(150)),
                          BidEntryWith(Price(100), Quantity(100)),
                          OfferEntryWith(Price(200), Quantity(200)),
                          OfferEntryWith(Price(250), Quantity(250))));
}

TEST_F(DepthCacheTest, ReportsInitialWithTopLevelOnlyWhenFullUpdateRequested) {
  cache.update(
      make_update(buy_order_added(OrderId(2), Price(150), Quantity(150)),
                  sell_order_added(OrderId(3), Price(200), Quantity(200))));
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(100), Quantity(100)),
                  sell_order_added(OrderId(4), Price(250), Quantity(250))));

  settings.enable_full_update_streaming();
  settings.enable_top_of_book_only_streaming();
  cache.compose_initial(settings, data);

  ASSERT_THAT(data,
              ElementsAre(BidEntryWith(Price(150), Quantity(150)),
                          OfferEntryWith(Price(200), Quantity(200))));
}

TEST_F(DepthCacheTest, ReportsInitialWithExcludedOrders) {
  const auto update = make_update(NewOrderAdded()
                                      .with_order_id(OrderId(1))
                                      .with_order_owner(PartyId("owner"))
                                      .with_order_side(Side::Option::Buy)
                                      .with_order_price(Price(100))
                                      .with_order_quantity(Quantity(200)),
                                  NewOrderAdded()
                                      .with_order_id(OrderId(2))
                                      .with_order_side(Side::Option::Buy)
                                      .with_order_price(Price(100))
                                      .with_order_quantity(Quantity(500)));
  cache.configure(DepthCache::Config{.allow_orders_exclusion = true});
  cache.update(update);

  settings.filter_orders_by_owner(PartyId("owner"));
  cache.compose_initial(settings, data);

  ASSERT_THAT(data, ElementsAre(BidEntryWith(Price(100), Quantity(500))));
}

TEST_F(DepthCacheTest, ReportsEmptyIncrementalUpdateWhenNoUpdatesApplied) {
  cache.compose_update(settings, data);

  ASSERT_THAT(data, IsEmpty());
}

TEST_F(DepthCacheTest, ReportsNewBidLevelInIncrementalUpdate) {
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(10), Quantity(100))));

  cache.compose_update(settings, data);

  ASSERT_THAT(data,
              ElementsAre(BidEntryWith(
                  Price(10), Quantity(100), MarketEntryAction::Option::New)));
}

TEST_F(DepthCacheTest, ReportsNewOfferLevelInIncrementalUpdate) {
  cache.update(
      make_update(sell_order_added(OrderId(1), Price(10), Quantity(100))));

  cache.compose_update(settings, data);

  ASSERT_THAT(data,
              ElementsAre(OfferEntryWith(
                  Price(10), Quantity(100), MarketEntryAction::Option::New)));
}

TEST_F(DepthCacheTest, ReportsChangedBidLevelInIncrementalUpdate) {
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(10), Quantity(100))));
  cache.update(
      make_update(buy_order_reduced(OrderId(1), Price(10), Quantity(50))));

  cache.compose_update(settings, data);

  ASSERT_THAT(data,
              ElementsAre(BidEntryWith(
                  Price(10), Quantity(50), MarketEntryAction::Option::Change)));
}

TEST_F(DepthCacheTest, ReportsChangedOfferLevelInIncrementalUpdate) {
  cache.update(
      make_update(sell_order_added(OrderId(1), Price(10), Quantity(100))));
  cache.update(
      make_update(sell_order_reduced(OrderId(1), Price(10), Quantity(50))));

  cache.compose_update(settings, data);

  ASSERT_THAT(data,
              ElementsAre(OfferEntryWith(
                  Price(10), Quantity(50), MarketEntryAction::Option::Change)));
}

TEST_F(DepthCacheTest, ReportsDeletedBidLevelInIncrementalUpdate) {
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(10), Quantity(100))));
  cache.update(
      make_update(buy_order_reduced(OrderId(1), Price(10), Quantity(0))));

  cache.compose_update(settings, data);

  ASSERT_THAT(data,
              ElementsAre(BidEntryWith(
                  Price(10), NoQuantity, MarketEntryAction::Option::Delete)));
}

TEST_F(DepthCacheTest, ReportsDeletedOfferLevelInIncrementalUpdate) {
  cache.update(
      make_update(sell_order_added(OrderId(1), Price(10), Quantity(100))));
  cache.update(
      make_update(sell_order_reduced(OrderId(1), Price(10), Quantity(0))));

  cache.compose_update(settings, data);

  ASSERT_THAT(data,
              ElementsAre(OfferEntryWith(
                  Price(10), NoQuantity, MarketEntryAction::Option::Delete)));
}

TEST_F(DepthCacheTest, ReportsFirstBidLevelOnlyInIncrementalUpdate) {
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(10), Quantity(100))));
  cache.update(
      make_update(buy_order_added(OrderId(2), Price(20), Quantity(200))));

  settings.enable_top_of_book_only_streaming();
  cache.compose_update(settings, data);

  ASSERT_THAT(
      data,
      ElementsAre(
          BidEntryWith(
              Price(20), Quantity(200), MarketEntryAction::Option::New),
          BidEntryWith(
              Price(10), NoQuantity, MarketEntryAction::Option::Delete)));
}

TEST_F(DepthCacheTest, ReportsFirstOfferLevelOnlyInIncrementalUpdate) {
  cache.update(
      make_update(sell_order_added(OrderId(1), Price(20), Quantity(200))));
  cache.update(
      make_update(sell_order_added(OrderId(2), Price(10), Quantity(100))));

  settings.enable_top_of_book_only_streaming();
  cache.compose_update(settings, data);

  ASSERT_THAT(
      data,
      ElementsAre(
          OfferEntryWith(
              Price(10), Quantity(100), MarketEntryAction::Option::New),
          OfferEntryWith(
              Price(20), NoQuantity, MarketEntryAction::Option::Delete)));
}

TEST_F(DepthCacheTest, ReportsUpdateWithExcludedOrders) {
  const auto update = make_update(NewOrderAdded()
                                      .with_order_id(OrderId(1))
                                      .with_order_owner(PartyId("owner"))
                                      .with_order_side(Side::Option::Buy)
                                      .with_order_price(Price(100))
                                      .with_order_quantity(Quantity(200)),
                                  NewOrderAdded()
                                      .with_order_id(OrderId(2))
                                      .with_order_side(Side::Option::Buy)
                                      .with_order_price(Price(100))
                                      .with_order_quantity(Quantity(500)));
  cache.configure(DepthCache::Config{.allow_orders_exclusion = true});
  cache.update(update);

  settings.filter_orders_by_owner(PartyId("owner"));
  cache.compose_update(settings, data);

  ASSERT_THAT(data,
              ElementsAre(BidEntryWith(
                  Price(100), Quantity(500), MarketEntryAction::Option::New)));
}

TEST_F(DepthCacheTest, CapturesEmptyStateWhenNoUpdatesApplied) {
  cache.capture(state);

  EXPECT_THAT(state.best_bid_price, Eq(std::nullopt));
  EXPECT_THAT(state.best_offer_price, Eq(std::nullopt));
  EXPECT_THAT(state.current_bid_depth, Eq(CurrentBidDepth(0)));
  EXPECT_THAT(state.current_offer_depth, Eq(CurrentOfferDepth(0)));
}

TEST_F(DepthCacheTest, CapturesBidState) {
  cache.update(
      make_update(buy_order_added(OrderId(1), Price(20), Quantity(100)),
                  buy_order_added(OrderId(2), Price(20), Quantity(200)),
                  buy_order_added(OrderId(3), Price(10), Quantity(500))));

  cache.capture(state);

  EXPECT_THAT(state.best_bid_price, Eq(Price(20)));
  EXPECT_THAT(state.current_bid_depth, Eq(CurrentBidDepth(2)));
}

TEST_F(DepthCacheTest, CapturesOfferState) {
  cache.update(
      make_update(sell_order_added(OrderId(1), Price(10), Quantity(100)),
                  sell_order_added(OrderId(2), Price(10), Quantity(200)),
                  sell_order_added(OrderId(3), Price(20), Quantity(500))));

  cache.capture(state);

  EXPECT_THAT(state.best_offer_price, Eq(Price(10)));
  EXPECT_THAT(state.current_offer_depth, Eq(CurrentOfferDepth(2)));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata

// NOLINTEND(*magic-numbers*,*non-private-member*)