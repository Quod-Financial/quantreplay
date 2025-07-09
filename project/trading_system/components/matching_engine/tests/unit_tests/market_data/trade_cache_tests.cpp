#include <gmock/gmock.h>

#include <optional>
#include <vector>

#include "ih/market_data/cache/trade_cache.hpp"
#include "ih/market_data/streaming_settings.hpp"
#include "mocks/mock_market_entry_id_generator.hpp"
#include "tools/order_book_notification_builder.hpp"

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

namespace simulator::trading_system::matching_engine::mdata::tests {
namespace {

struct TradeCache : Test {
  static auto make_test_trade(const Price price, const Quantity quantity)
      -> Trade {
    return NewTrade()
        .with_trade_price(price)
        .with_traded_quantity(quantity)
        .create();
  }

  static auto make_update(auto&&... trades) {
    return std::vector{
        OrderBookNotification{std::forward<decltype(trades)>(trades)}...};
  }

  NiceMock<MockMarketEntryIdGenerator> id_generator;
  StreamingSettings settings;
  std::vector<MarketDataEntry> entries;
  mdata::TradeCache cache{id_generator};

 private:
  auto SetUp() -> void override {
    settings.enable_data_type_streaming(MdEntryType::Option::Trade);
    ON_CALL(id_generator, generate())
        .WillByDefault(Return(MarketEntryId{"entry-id"}));
  }
};

MATCHER_P2(EntryHas, price, quantity, "") {
  return ExplainMatchResult(Field(&MarketDataEntry::price, Optional(Eq(price))),
                            arg,
                            result_listener) &&
         ExplainMatchResult(
             Field(&MarketDataEntry::quantity, Optional(Eq(quantity))),
             arg,
             result_listener);
}

TEST_F(TradeCache, ReportsEmptyIncrementalUpdate) {
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(TradeCache, ReportsSingleTradeInIncrementalUpdate) {
  cache.update(make_update(NewTrade()));

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries, SizeIs(1));
}

TEST_F(TradeCache, ReportsMultipleTradesInIncrementalUpdate) {
  cache.update(make_update(NewTrade(), NewTrade(), NewTrade()));

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries, SizeIs(3));
}

TEST_F(TradeCache, SpecifiesTradeEntryAttributes) {
  using namespace std::chrono_literals;
  cache.update(make_update(
      NewTrade()
          .with_buyer(BuyerId("B"))
          .with_seller(SellerId("S"))
          .with_trade_price(Price(100.))
          .with_traded_quantity(Quantity(200.))
          .with_aggressor_side(AggressorSide::Option::Buy)
          .with_trade_time(core::sys_us{core::sys_days{2025y / 12 / 31} + 13h +
                                        30min + 59s + 123456us})));

  cache.compose_update(settings, entries);

  EXPECT_THAT(entries.at(0).type, Eq(MdEntryType::Option::Trade));
  EXPECT_THAT(entries.at(0).action, Eq(MarketEntryAction::Option::New));
  EXPECT_THAT(entries.at(0).id, Ne(std::nullopt));
  EXPECT_THAT(entries.at(0).price, Eq(Price{100.}));
  EXPECT_THAT(entries.at(0).quantity, Eq(Quantity{200.}));
  EXPECT_THAT(entries.at(0).buyer_id, Eq(BuyerId{"B"}));
  EXPECT_THAT(entries.at(0).seller_id, Eq(SellerId{"S"}));
  EXPECT_THAT(entries.at(0).aggressor_side, Eq(AggressorSide::Option::Buy));
  EXPECT_THAT(
      entries.at(0).time,
      Optional(Eq(MarketEntryTime{core::sys_us{
          core::sys_days{2025y / 12 / 31} + 13h + 30min + 59s + 123456us}})));
}

TEST_F(TradeCache, SpecifiesMarketPhase) {
  auto trade = NewTrade{}
                   .with_market_phase({TradingPhase::Option::Closed,
                                       TradingStatus::Option::Halt})
                   .create();
  cache.update(make_update(std::move(trade)));

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries.at(0).phase,
              Eq(MarketPhase{TradingPhase::Option::Closed,
                             TradingStatus::Option::Halt}));
}

TEST_F(TradeCache, ReportsEmptyInitial) {
  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(TradeCache, ReportsOnlyLastTradeInInitial) {
  cache.update(make_update(make_test_trade(Price{50.}, Quantity{150.}),
                           make_test_trade(Price{100.}, Quantity{200.}),
                           make_test_trade(Price{400.}, Quantity{1000.})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(EntryHas(Price{400.}, Quantity{1000.})));
}

TEST_F(TradeCache, ReportsAllTradesInFullUpdate) {
  cache.update(make_update(make_test_trade(Price{50.}, Quantity{150.}),
                           make_test_trade(Price{100.}, Quantity{200.}),
                           make_test_trade(Price{400.}, Quantity{1000.})));
  settings.enable_full_update_streaming();

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries, SizeIs(3));
}

TEST_F(TradeCache, DoesNotReportTradeVolumeInUpdateWhenDisabled) {
  cache.update(make_update(NewTrade()));
  cache.configure({.report_trade_volume = false,
                   .report_trade_parties = true,
                   .report_trade_aggressor_side = true});

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries.at(0).quantity, Eq(std::nullopt));
}

TEST_F(TradeCache, DoesNotReportTradeVolumeInInitialWhenDisabled) {
  cache.update(make_update(NewTrade()));
  cache.configure({.report_trade_volume = false,
                   .report_trade_parties = true,
                   .report_trade_aggressor_side = true});

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries.at(0).quantity, Eq(std::nullopt));
}

TEST_F(TradeCache, DoesNotReportTradePartiesInUpdateWhenDisabled) {
  cache.update(make_update(NewTrade()));
  cache.configure({.report_trade_volume = true,
                   .report_trade_parties = false,
                   .report_trade_aggressor_side = true});

  cache.compose_update(settings, entries);

  EXPECT_THAT(entries.at(0).buyer_id, Eq(std::nullopt));
  EXPECT_THAT(entries.at(0).seller_id, Eq(std::nullopt));
}

TEST_F(TradeCache, DoesNotReportTradePartiesInInitialWhenDisabled) {
  cache.update(make_update(NewTrade()));
  cache.configure({.report_trade_volume = true,
                   .report_trade_parties = false,
                   .report_trade_aggressor_side = true});

  cache.compose_initial(settings, entries);

  EXPECT_THAT(entries.at(0).buyer_id, Eq(std::nullopt));
  EXPECT_THAT(entries.at(0).seller_id, Eq(std::nullopt));
}

TEST_F(TradeCache, DoesNotReportTradeAggressorSideInUpdateWhenDisabled) {
  cache.update(make_update(NewTrade()));
  cache.configure({.report_trade_volume = true,
                   .report_trade_parties = true,
                   .report_trade_aggressor_side = false});

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries.at(0).aggressor_side, Eq(std::nullopt));
}

TEST_F(TradeCache, DoesNotReportTradeAggressorSideInInitialWhenDisabled) {
  cache.update(make_update(NewTrade()));
  cache.configure({.report_trade_volume = true,
                   .report_trade_parties = true,
                   .report_trade_aggressor_side = false});

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries.at(0).aggressor_side, Eq(std::nullopt));
}

TEST_F(TradeCache, StoresNullLastTradeIfNotUpdated) {
  std::optional<Trade> last_trade;
  cache.store_state(last_trade);

  ASSERT_EQ(last_trade, std::nullopt);
}

TEST_F(TradeCache, StoresLastTrade) {
  cache.update(make_update(make_test_trade(Price{50.}, Quantity{150.}),
                           make_test_trade(Price{100.}, Quantity{200.}),
                           make_test_trade(Price{400.}, Quantity{1000.})));

  std::optional<Trade> last_trade;
  cache.store_state(last_trade);

  ASSERT_NE(last_trade, std::nullopt);
  ASSERT_EQ(last_trade->trade_price, Price{400.});
}

TEST_F(TradeCache, RemovesPreviousTradesOnLastTradeRecoverInInitial) {
  cache.update(make_update(make_test_trade(Price{50.}, Quantity{150.}),
                           LastTradeRecover{}));

  cache.compose_initial(settings, entries);

  ASSERT_TRUE(entries.empty());
}

TEST_F(TradeCache, RemovesPreviousTradesOnLastTradeRecover) {
  cache.update(make_update(make_test_trade(Price{50.}, Quantity{150.}),
                           make_test_trade(Price{100.}, Quantity{200.}),
                           make_test_trade(Price{400.}, Quantity{1000.}),
                           LastTradeRecover{}));

  cache.compose_update(settings, entries);

  ASSERT_TRUE(entries.empty());
}

TEST_F(TradeCache, RecoversInInitialTradeFromLastTradeRecoverIfItIsPresent) {
  cache.update(make_update(
      make_test_trade(Price{50.}, Quantity{150.}),
      LastTradeRecover{.trade = make_test_trade(Price{100.}, Quantity{200.})}));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(EntryHas(Price{100.}, Quantity{200.})));
}

TEST_F(TradeCache, RecoversTradeFromLastTradeRecoverIfItIsPresent) {
  cache.update(make_update(
      make_test_trade(Price{50.}, Quantity{150.}),
      LastTradeRecover{.trade = make_test_trade(Price{100.}, Quantity{200.})}));

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries, ElementsAre(EntryHas(Price{100.}, Quantity{200.})));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::tests

// NOLINTEND(*magic-numbers*,*non-private-member*)
