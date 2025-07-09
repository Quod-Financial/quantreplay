#include <gmock/gmock.h>

#include "common/market_state/json/instrument_state.hpp"
#include "common/market_state/snapshot.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::market_state::test {
namespace {

using namespace ::testing;

struct TradingSystemCommonInstrumentState : public ::testing::Test {
  template <typename T>
  [[nodiscard]]
  auto json(const T& instrument) -> rapidjson::Value {
    rapidjson::Value json_value;
    core::json::Type<T>::write_json_value(
        json_value, doc.GetAllocator(), instrument);
    return json_value;
  }

  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(TradingSystemCommonInstrumentState, ReadsFromJson) {
  value.SetObject();

  Instrument instrument;
  instrument.symbol = Symbol{"Symbol"};
  value.AddMember("instrument", json(instrument).Move(), doc.GetAllocator());

  const Trade trade{BuyerId{"BuyerId"},
                    std::nullopt,
                    Price{0.},
                    Quantity{0.},
                    std::nullopt,
                    core::get_current_system_time(),
                    MarketPhase::open()};
  value.AddMember("last_trade", json(trade).Move(), doc.GetAllocator());

  InstrumentInfo info;
  info.low_price = Price{3.14};
  value.AddMember("info", json(info).Move(), doc.GetAllocator());
  value.AddMember("order_book", json(OrderBook{}).Move(), doc.GetAllocator());

  const auto instrument_state =
      core::json::Type<InstrumentState>::read_json_value(value);

  ASSERT_EQ(instrument_state.instrument.symbol, Symbol{"Symbol"});
  ASSERT_TRUE(instrument_state.last_trade.has_value());
  ASSERT_EQ(instrument_state.last_trade->buyer, BuyerId{"BuyerId"});
  ASSERT_THAT(instrument_state.info,
              Optional(Field(&InstrumentInfo::low_price, Eq(Price{3.14}))));
  ASSERT_TRUE(instrument_state.order_book.buy_orders.empty());
  ASSERT_TRUE(instrument_state.order_book.sell_orders.empty());
}

TEST_F(TradingSystemCommonInstrumentState, WritesToJson) {
  using namespace simulator::trading_system::test;

  Instrument instrument;
  instrument.symbol = Symbol{"Symbol"};

  const Trade trade{BuyerId{"BuyerId"},
                    std::nullopt,
                    Price{0.},
                    Quantity{0.},
                    std::nullopt,
                    core::get_current_system_time(),
                    MarketPhase::open()};

  InstrumentInfo info;
  info.low_price = Price{3.14};

  const InstrumentState instrument_state{.instrument = instrument,
                                         .last_trade = trade,
                                         .info = info,
                                         .order_book = OrderBook{}};

  core::json::Type<InstrumentState>::write_json_value(
      value, doc.GetAllocator(), instrument_state);

  ASSERT_THAT(value, HasInner("instrument", HasString("symbol", "Symbol")));
  ASSERT_THAT(value, HasInner("last_trade", HasString("buyer", "BuyerId")));
  ASSERT_THAT(value, HasInner("info", HasDouble("low_price", 3.14)));
  ASSERT_THAT(value, HasInner("order_book", HasArraySize("buy_orders", 0)));
  ASSERT_THAT(value, HasInner("order_book", HasArraySize("sell_orders", 0)));
}

}  // namespace
}  // namespace simulator::trading_system::market_state::test
