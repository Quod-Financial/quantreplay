#include <gtest/gtest.h>

#include "ih/common/data/market_data_updates.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

TEST(MatchingEngineOrderAddedFormatting, FmtFormatting) {
  const OrderAdded event{.order_owner = std::make_optional(PartyId{"party_id"}),
                         .order_price = std::make_optional(Price{100.0}),
                         .order_quantity = Quantity{10},
                         .order_id = OrderId{1},
                         .order_side = Side::Option::Buy,
                         .order_type = OrderType::Option::Limit};
  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "OrderAdded": { "order_id": 1, "order_price": 100, "order_quantity": 10, "order_side": "Buy", "order_type": "Limit", "order_owner": "party_id" } })");
}

TEST(MatchingEngineOrderAddedFormatting, FmtFormattingWithoutOptionalFields) {
  const OrderAdded event{.order_owner = std::nullopt,
                         .order_price = std::nullopt,
                         .order_quantity = Quantity{10},
                         .order_id = OrderId{1},
                         .order_side = Side::Option::Sell,
                         .order_type = OrderType::Option::Market};
  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "OrderAdded": { "order_id": 1, "order_price": none, "order_quantity": 10, "order_side": "Sell", "order_type": "Market", "order_owner": none } })");
}

TEST(MatchingEngineOrderReducedFormatting, FmtFormatting) {
  const OrderReduced event{.order_price = std::make_optional(Price{100.0}),
                           .order_quantity = Quantity{10},
                           .order_id = OrderId{1},
                           .order_side = Side::Option::Buy};
  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "OrderReduced": { "order_id": 1, "order_price": 100, "order_quantity": 10, "order_side": "Buy" } })");
}

TEST(MatchingEngineOrderReducedFormatting, FmtFormattingWithoutPrice) {
  const OrderReduced event{.order_price = std::nullopt,
                           .order_quantity = Quantity{10},
                           .order_id = OrderId{1},
                           .order_side = Side::Option::Sell};
  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "OrderReduced": { "order_id": 1, "order_price": none, "order_quantity": 10, "order_side": "Sell" } })");
}

TEST(MatchingEngineOrderRemovedFormatting, FmtFormatting) {
  const OrderRemoved event{.order_price = std::make_optional(Price{100.0}),
                           .order_id = OrderId{1},
                           .order_side = Side::Option::Buy};
  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "OrderRemoved": { "order_id": 1, "order_price": 100, "order_side": "Buy" } })");
}

TEST(MatchingEngineOrderRemovedFormatting, FmtFormattingWithoutPrice) {
  const OrderRemoved event{.order_price = std::nullopt,
                           .order_id = OrderId{1},
                           .order_side = Side::Option::Sell};
  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "OrderRemoved": { "order_id": 1, "order_price": none, "order_side": "Sell" } })");
}

TEST(MatchingEngineLastTradeRecoverFormatting, FmtFormatting) {
  using namespace std::chrono_literals;

  const LastTradeRecover event{
      std::make_optional<Trade>(BuyerId{"buyer_id"},
                                SellerId{"seller_id"},
                                Price{3.14},
                                Quantity{42.3},
                                AggressorSide{Side::Option::Buy},
                                core::sys_us{core::sys_days{2025y / 12 / 31} +
                                             13h + 30min + 59s + 123456us},
                                MarketPhase::open())};

  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "LastTradeRecover": { "trade": { "buyer": "buyer_id", )"
      R"("seller": "seller_id", "trade_price": 3.14, "traded_quantity": 42.3, )"
      R"("aggressor_side": "Buy", "trade_time": "2025-12-31 13:30:59.123456", )"
      R"("market_phase": { TradingPhase=Open, TradingStatus=Resume } } } })");
}

TEST(MatchingEngineLastTradeRecoverFormatting, FmtFormattingWithoutTrade) {
  const LastTradeRecover event{std::nullopt};

  ASSERT_EQ(fmt::to_string(event),
            R"({ "LastTradeRecover": { "trade": none } })");
}

TEST(MatchingEngineInstrumentInfoRecoverFormatting, FmtFormatting) {
  using namespace std::chrono_literals;

  const InstrumentInfoRecover event{
      std::make_optional<market_state::InstrumentInfo>(Price{100.1},
                                                       Price{300.3})};

  ASSERT_EQ(fmt::to_string(event),
            "{ \"InstrumentInfoRecover\": { \"info\": { "
            "\"low_price\": 100.1, \"high_price\": 300.3, "
            "\"opening_price\": none, \"opening_price_time\": none, "
            "\"closing_price\": none, \"closing_price_time\": none, "
            "\"auction_clearing_price\": none, "
            "\"auction_clearing_quantity\": none, "
            "\"previous_closing_price\": none, "
            "\"trade_volume\": none, "
            "\"last_open_phase_traded_price\": none } } }");
}

TEST(MatchingEngineInstrumentInfoRecoverFormatting, FmtFormattingWithoutInfo) {
  const InstrumentInfoRecover event{std::nullopt};

  ASSERT_EQ(fmt::to_string(event),
            R"({ "InstrumentInfoRecover": { "info": none } })");
}

TEST(MatchingEngineTradeResultFormatting, FmtFormatting) {
  const TradeResult event{.price = Price{130}, .quantity = Quantity{500}};

  ASSERT_EQ(fmt::to_string(event),
            R"({ "TradeResult": { "price": 130, "quantity": 500 } })");
}

TEST(MatchingEngineAuctionFinalPriceUpdateFormatting, FmtFormatting) {
  const AuctionFinalPriceUpdate event{
      .auction_phase = TradingPhase{TradingPhase::Option::OpeningAuction},
      .clearing_value =
          TradeResult{.price = Price{130}, .quantity = Quantity{500}}};

  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "AuctionFinalPriceUpdate": { "auction_phase": "OpeningAuction", )"
      R"("clearing_value": { "TradeResult": { "price": 130, )"
      R"("quantity": 500 } } } })");
}

TEST(MatchingEngineAuctionFinalPriceUpdateFormatting,
     FmtFormattingWhenNotCrossed) {
  constexpr AuctionFinalPriceUpdate event{
      .auction_phase = TradingPhase{TradingPhase::Option::OpeningAuction},
      .clearing_value = std::nullopt};

  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "AuctionFinalPriceUpdate": { "auction_phase": "OpeningAuction", )"
      R"("clearing_value": none } })");
}

TEST(MatchingEngineAuctionFinalPriceUpdateFormatting,
     FmtFormattingWhenDefaultConstructed) {
  constexpr AuctionFinalPriceUpdate event{};

  ASSERT_EQ(fmt::to_string(event),
            R"({ "AuctionFinalPriceUpdate": { "auction_phase": "Open", )"
            R"("clearing_value": none } })");
}

TEST(MatchingEngineEarlyPriceUpdateFormatting, FmtFormatting) {
  const EarlyPriceUpdate event{
      .early_value =
          TradeResult{.price = Price{105}, .quantity = Quantity{100}}};

  ASSERT_EQ(fmt::to_string(event),
            R"({ "EarlyPriceUpdate": { "early_value": { "TradeResult": )"
            R"({ "price": 105, "quantity": 100 } } } })");
}

TEST(MatchingEngineEarlyPriceUpdateFormatting, FmtFormattingWhenCleared) {
  const EarlyPriceUpdate event{};

  ASSERT_EQ(fmt::to_string(event),
            R"({ "EarlyPriceUpdate": { "early_value": none } })");
}

TEST(MatchingEngineIndicativePriceQuantityFormatting, FmtFormatting) {
  const AuctionIndicativeUpdate::IndicativePriceQuantity value{
      .price = Price{130}, .quantity = Quantity{500}};

  ASSERT_EQ(
      fmt::to_string(value),
      R"({ "IndicativePriceQuantity": { "price": 130, "quantity": 500 } })");
}

TEST(MatchingEngineIndicativePriceQuantityFormatting,
     FmtFormattingWhenNotCrossed) {
  const AuctionIndicativeUpdate::IndicativePriceQuantity value{
      .price = std::nullopt, .quantity = Quantity{0}};

  ASSERT_EQ(
      fmt::to_string(value),
      R"({ "IndicativePriceQuantity": { "price": none, "quantity": 0 } })");
}

TEST(MatchingEngineMarketFormatting, FmtFormatting) {
  constexpr AuctionIndicativeUpdate::Imbalance value{
      .size = Quantity{300},
      .side = TradeCondition::Option::ImbalanceMoreBuyers};

  ASSERT_EQ(
      fmt::to_string(value),
      R"({ "Imbalance": { "size": 300, "side": "ImbalanceMoreBuyers" } })");
}

TEST(MatchingEngineAuctionIndicativeUpdateFormatting, FmtFormatting) {
  const AuctionIndicativeUpdate event{
      .auction_phase = TradingPhase{TradingPhase::Option::OpeningAuction},
      .price_qty =
          AuctionIndicativeUpdate::IndicativePriceQuantity{
              .price = Price{130}, .quantity = Quantity{500}},
      .imbalance = AuctionIndicativeUpdate::Imbalance{
          .size = Quantity{300},
          .side = TradeCondition::Option::ImbalanceMoreSellers}};

  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "AuctionIndicativeUpdate": { "auction_phase": "OpeningAuction", )"
      R"("price_qty": { "IndicativePriceQuantity": { "price": 130, )"
      R"("quantity": 500 } }, "imbalance": { "Imbalance": )"
      R"({ "size": 300, "side": "ImbalanceMoreSellers" } } } })");
}

TEST(MatchingEngineAuctionIndicativeUpdateFormatting,
     FmtFormattingWhenCleared) {
  const AuctionIndicativeUpdate event{
      .auction_phase = TradingPhase{TradingPhase::Option::ClosingAuction},
      .price_qty = std::nullopt,
      .imbalance = std::nullopt};

  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "AuctionIndicativeUpdate": { "auction_phase": "ClosingAuction", )"
      R"("price_qty": none, "imbalance": none } })");
}

TEST(MatchingEngineAuctionIndicativeUpdateFormatting,
     FmtFormattingWhenDefaultConstructed) {
  const AuctionIndicativeUpdate event{};

  ASSERT_EQ(fmt::to_string(event),
            R"({ "AuctionIndicativeUpdate": { "auction_phase": "Open", )"
            R"("price_qty": none, "imbalance": none } })");
}

TEST(MatchingEngineTzDayPassedFormatting, FmtFormatting) {
  using namespace std::chrono_literals;

  constexpr TzDayPassed event{.sys_tick_time =
                                  core::sys_us{core::sys_days{2025y / 12 / 31} +
                                               13h + 30min + 59s + 123456us}};

  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "TzDayPassed": { "sys_tick_time": "2025-12-31 13:30:59.123456" } })");
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
