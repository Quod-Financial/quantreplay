#include <gtest/gtest.h>

#include "ih/common/data/market_data_updates.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

TEST(MatchingEngineOrderAddedFormatting, FmtFormatting) {
  const OrderAdded event{.order_owner = std::make_optional(PartyId{"party_id"}),
                         .order_price = std::make_optional(Price{100.0}),
                         .order_quantity = Quantity{10},
                         .order_id = OrderId{1},
                         .order_side = Side::Option::Buy};
  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "OrderAdded": { "order_id": 1, "order_price": 100, "order_quantity": 10, "order_side": "Buy", "order_owner": "party_id" } })");
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

TEST(MatchingEngineOrderRemovedFormatting, FmtFormatting) {
  const OrderRemoved event{.order_price = std::make_optional(Price{100.0}),
                           .order_id = OrderId{1},
                           .order_side = Side::Option::Buy};
  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "OrderRemoved": { "order_id": 1, "order_price": 100, "order_side": "Buy" } })");
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

TEST(MatchingEngineInstrumentInfoRecoverFormatting, FmtFormatting) {
  using namespace std::chrono_literals;

  const InstrumentInfoRecover event{
      std::make_optional<market_state::InstrumentInfo>(Price{100.1},
                                                       Price{300.3})};

  ASSERT_EQ(
      fmt::to_string(event),
      R"({ "InstrumentInfoRecover": { "info": { "low_price": 100.1, "high_price": 300.3 } } })");
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
