#include <gmock/gmock.h>

#include "common/trade.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace ::testing;  // NOLINT

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
