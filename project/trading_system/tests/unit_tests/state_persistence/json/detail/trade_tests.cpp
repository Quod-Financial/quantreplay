#include <gmock/gmock.h>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;

struct TradingSystemJsonTrade : public ::testing::Test {
  json::Trade trade;
};

TEST_F(TradingSystemJsonTrade, ReadsNullOptionalFieldsFromJson) {
  using namespace std::chrono_literals;

  rapidjson::Document doc;
  doc.Parse(R"({
    "Buyer": null,
    "Seller": null,
    "TradePrice": 3.14,
    "TradedQuantity": 42.3,
    "AggressorSide": null,
    "TradeTime": "20251231-13:30:59.123456",
    "MarketPhase": {
        "TradingSessionSubID": "OutOfMainSessionTrading",
        "SecurityTradingStatus": "TradingHalt"
    }
  })");

  ASSERT_TRUE(json::read(doc, trade).has_value());

  ASSERT_EQ(trade.buyer, std::nullopt);
  ASSERT_EQ(trade.seller, std::nullopt);
  ASSERT_DOUBLE_EQ(trade.trade_price, 3.14);
  ASSERT_EQ(trade.traded_quantity, 42.3);
  ASSERT_EQ(trade.aggressor_side, std::nullopt);
  ASSERT_EQ(trade.trade_time,
            core::sys_us{core::sys_days{2025y / 12 / 31} + 13h + 30min + 59s +
                         123456us});
  ASSERT_EQ(
      trade.market_phase,
      (MarketPhase{TradingPhase::Option::Closed, TradingStatus::Option::Halt}));
}

TEST_F(TradingSystemJsonTrade, ReadsFromJson) {
  using namespace std::chrono_literals;

  rapidjson::Document doc;
  doc.Parse(R"({
    "Buyer": "buyer",
    "Seller": "seller",
    "TradePrice": 3.14,
    "TradedQuantity": 42.3,
    "AggressorSide": "Buy",
    "TradeTime": "20251231-13:30:59.123456",
    "MarketPhase": {
        "TradingSessionSubID": "OutOfMainSessionTrading",
        "SecurityTradingStatus": "TradingHalt"
    }
  })");

  ASSERT_TRUE(json::read(doc, trade).has_value());

  ASSERT_EQ(trade.buyer, "buyer");
  ASSERT_EQ(trade.seller, "seller");
  ASSERT_DOUBLE_EQ(trade.trade_price, 3.14);
  ASSERT_EQ(trade.traded_quantity, 42.3);
  ASSERT_EQ(trade.aggressor_side.value(), AggressorSide{Side::Option::Buy});
  ASSERT_EQ(trade.trade_time,
            core::sys_us{core::sys_days{2025y / 12 / 31} + 13h + 30min + 59s +
                         123456us});
  ASSERT_EQ(
      trade.market_phase,
      (MarketPhase{TradingPhase::Option::Closed, TradingStatus::Option::Halt}));
}

TEST_F(TradingSystemJsonTrade, WritingSetsJsonValueTypeToObject) {
  rapidjson::Document doc;
  rapidjson::Value value;

  ASSERT_TRUE(json::write(value, doc.GetAllocator(), trade).has_value());

  ASSERT_TRUE(value.IsObject());
}

TEST_F(TradingSystemJsonTrade, WritesNullOptionalFieldsToJson) {
  using namespace simulator::trading_system::test;

  rapidjson::Document doc;
  rapidjson::Value value;

  ASSERT_TRUE(json::write(value, doc.GetAllocator(), trade).has_value());

  ASSERT_THAT(value, HasNull("Buyer"));
  ASSERT_THAT(value, HasNull("Seller"));
  ASSERT_THAT(value, HasDouble("TradePrice", 0.0));
  ASSERT_THAT(value, HasDouble("TradedQuantity", 0.0));
  ASSERT_THAT(value, HasNull("AggressorSide"));
  ASSERT_THAT(value, HasString("TradeTime", "19700101-00:00:00.000000"));
  ASSERT_THAT(
      value,
      HasInner("MarketPhase", HasString("TradingSessionSubID", "Continuous")));
  ASSERT_THAT(
      value,
      HasInner("MarketPhase", HasString("SecurityTradingStatus", "Resume")));
}

TEST_F(TradingSystemJsonTrade, WritesToJson) {
  using namespace std::chrono_literals;
  using namespace simulator::trading_system::test;

  trade = json::Trade{"buyer",
                      "seller",
                      3.14,
                      42.3,
                      Side::Option::Buy,
                      core::sys_us{core::sys_days{2025y / 12 / 31} + 13h +
                                   30min + 59s + 123456us},
                      MarketPhase{TradingPhase::Option::Closed,
                                  core::enumerators::TradingStatus::Halt}};

  rapidjson::Document doc;
  rapidjson::Value value;
  ASSERT_TRUE(json::write(value, doc.GetAllocator(), trade).has_value());

  ASSERT_THAT(value, HasString("Buyer", "buyer"));
  ASSERT_THAT(value, HasString("Seller", "seller"));
  ASSERT_THAT(value, HasDouble("TradePrice", 3.14));
  ASSERT_THAT(value, HasDouble("TradedQuantity", 42.3));
  ASSERT_THAT(value, HasString("AggressorSide", "Buy"));
  ASSERT_THAT(value, HasString("TradeTime", "20251231-13:30:59.123456"));
  ASSERT_THAT(
      value,
      HasInner("MarketPhase",
               HasString("TradingSessionSubID", "OutOfMainSessionTrading")));
  ASSERT_THAT(value,
              HasInner("MarketPhase",
                       HasString("SecurityTradingStatus", "TradingHalt")));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
