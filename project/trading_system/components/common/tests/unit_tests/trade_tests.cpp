#include <gmock/gmock.h>
#include <rapidjson/document.h>

#include "common/json/trade.hpp"
#include "common/trade.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace ::testing;  // NOLINT

TEST(TradingSystemTrade, ReadsNullOptionalFieldsFromJson) {
  using namespace std::chrono_literals;

  rapidjson::Document doc;
  doc.Parse(R"({
    "buyer": null,
    "seller": null,
    "trade_price": 3.14,
    "traded_quantity": 42.3,
    "aggressor_side": null,
    "trade_time": "2025-12-31 13:30:59.123456",
    "market_phase": {
        "trading_phase": "Open",
        "trading_status": "Resume"
    }
  })");

  const auto trade = core::json::Type<Trade>::read_json_value(doc);

  ASSERT_EQ(trade.buyer, std::nullopt);
  ASSERT_EQ(trade.seller, std::nullopt);
  ASSERT_DOUBLE_EQ(trade.trade_price.value(), 3.14);
  ASSERT_EQ(trade.traded_quantity.value(), 42.3);
  ASSERT_EQ(trade.aggressor_side, std::nullopt);
  ASSERT_EQ(trade.trade_time,
            core::sys_us{core::sys_days{2025y / 12 / 31} + 13h + 30min + 59s +
                         123456us});
  ASSERT_EQ(
      trade.market_phase,
      (MarketPhase{TradingPhase::Option::Open, TradingStatus::Option::Resume}));
}

TEST(TradingSystemTrade, ReadsFromJson) {
  using namespace std::chrono_literals;

  rapidjson::Document doc;
  doc.Parse(R"({
    "buyer": "buyer",
    "seller": "seller",
    "trade_price": 3.14,
    "traded_quantity": 42.3,
    "aggressor_side": "Buy",
    "trade_time": "2025-12-31 13:30:59.123456",
    "market_phase": {
        "trading_phase": "Open",
        "trading_status": "Resume"
    }
  })");

  const auto trade = core::json::Type<Trade>::read_json_value(doc);

  ASSERT_EQ(trade.buyer.value(), BuyerId{"buyer"});
  ASSERT_EQ(trade.seller.value(), SellerId{"seller"});
  ASSERT_DOUBLE_EQ(trade.trade_price.value(), 3.14);
  ASSERT_EQ(trade.traded_quantity.value(), 42.3);
  ASSERT_EQ(trade.aggressor_side.value(), AggressorSide{Side::Option::Buy});
  ASSERT_EQ(trade.trade_time,
            core::sys_us{core::sys_days{2025y / 12 / 31} + 13h + 30min + 59s +
                         123456us});
  ASSERT_EQ(
      trade.market_phase,
      (MarketPhase{TradingPhase::Option::Open, TradingStatus::Option::Resume}));
}

TEST(TradingSystemTrade, WritesNullOptionalFieldsToJson) {
  constexpr Trade trade{std::nullopt,
                        std::nullopt,
                        Price{0.0},
                        Quantity{0.0},
                        std::nullopt,
                        core::sys_us{},
                        MarketPhase{TradingPhase::Option::Open,
                                    core::enumerators::TradingStatus::Resume}};

  rapidjson::Document doc;
  rapidjson::Value value;

  core::json::Type<Trade>::write_json_value(value, doc.GetAllocator(), trade);

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasNull("buyer"));
  ASSERT_THAT(value, HasNull("seller"));
  ASSERT_THAT(value, HasDouble("trade_price", 0.0));
  ASSERT_THAT(value, HasDouble("traded_quantity", 0.0));
  ASSERT_THAT(value, HasNull("aggressor_side"));
  ASSERT_THAT(value, HasString("trade_time", "1970-01-01 00:00:00.000000"));
  ASSERT_THAT(value,
              HasInner("market_phase", HasString("trading_phase", "Open")));
  ASSERT_THAT(value,
              HasInner("market_phase", HasString("trading_status", "Resume")));
}

TEST(TradingSystemTrade, WritesToJson) {
  using namespace std::chrono_literals;

  const Trade trade{BuyerId{"buyer"},
                    SellerId{"seller"},
                    Price{3.14},
                    Quantity{42.3},
                    AggressorSide{Side::Option::Buy},
                    core::sys_us{core::sys_days{2025y / 12 / 31} + 13h + 30min +
                                 59s + 123456us},
                    MarketPhase{TradingPhase::Option::Open,
                                core::enumerators::TradingStatus::Resume}};

  rapidjson::Document doc;
  rapidjson::Value value;
  core::json::Type<Trade>::write_json_value(value, doc.GetAllocator(), trade);

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasString("buyer", "buyer"));
  ASSERT_THAT(value, HasString("seller", "seller"));
  ASSERT_THAT(value, HasDouble("trade_price", 3.14));
  ASSERT_THAT(value, HasDouble("traded_quantity", 42.3));
  ASSERT_THAT(value, HasString("aggressor_side", "Buy"));
  ASSERT_THAT(value, HasString("trade_time", "2025-12-31 13:30:59.123456"));
  ASSERT_THAT(value,
              HasInner("market_phase", HasString("trading_phase", "Open")));
  ASSERT_THAT(value,
              HasInner("market_phase", HasString("trading_status", "Resume")));
}

TEST(TradingSystemTrade, FormatsToString) {
  using namespace std::chrono_literals;

  const Trade trade{BuyerId{"buyer_id"},
                    SellerId{"seller_id"},
                    Price{3.14},
                    Quantity{42.3},
                    AggressorSide{Side::Option::Buy},
                    core::sys_us{core::sys_days{2025y / 12 / 31} + 13h + 30min +
                                 59s + 123456us},
                    MarketPhase{TradingPhase::Option::Open,
                                core::enumerators::TradingStatus::Resume}};

  ASSERT_EQ(
      fmt::to_string(trade),
      R"({ "buyer": "buyer_id", "seller": "seller_id", "trade_price": 3.14, )"
      R"("traded_quantity": 42.3, "aggressor_side": "Buy", )"
      R"("trade_time": "2025-12-31 13:30:59.123456", )"
      R"("market_phase": { TradingPhase=Open, TradingStatus=Resume } })");
}

}  // namespace
}  // namespace simulator::trading_system::test
