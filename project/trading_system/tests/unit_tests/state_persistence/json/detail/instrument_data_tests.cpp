#include <gmock/gmock.h>

#include "common/instrument_state.hpp"
#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "ih/state_persistence/json/snapshot.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;

struct TradingSystemJsonInstrumentData : public ::testing::Test {
  template <typename T>
  [[nodiscard]]
  auto json(const T& object) -> rapidjson::Value {
    rapidjson::Value value;
    json::write(value, doc.GetAllocator(), object);
    return value;
  }

  rapidjson::Document doc;
  rapidjson::Value json_value;
};

TEST_F(TradingSystemJsonInstrumentData, ReadsFromJson) {
  json_value.SetObject();

  instrument::Cache::InstrumentSpecification specification;
  specification.symbol = Symbol{"symbol"};
  json_value.AddMember(
      "Instrument", json(specification).Move(), doc.GetAllocator());

  const json::Trade trade{"BuyerId",
                          std::nullopt,
                          0.,
                          0.,
                          std::nullopt,
                          core::get_current_system_time(),
                          MarketPhase::open()};
  json_value.AddMember("LastTrade", json(trade).Move(), doc.GetAllocator());

  market_state::InstrumentInfo info;
  info.low_price = Price{3.14};
  json_value.AddMember("Info", json(info).Move(), doc.GetAllocator());
  json_value.AddMember(
      "OrderBook", json(market_state::OrderBook{}).Move(), doc.GetAllocator());

  market_state::InstrumentData instrument_data;
  ASSERT_TRUE(json::read(json_value, instrument_data));

  ASSERT_EQ(instrument_data.specification.symbol, Symbol{"symbol"});
  ASSERT_TRUE(instrument_data.state.last_trade.has_value());
  ASSERT_EQ(instrument_data.state.last_trade->buyer, BuyerId{"BuyerId"});
  ASSERT_THAT(instrument_data.state.info,
              Optional(Field(&market_state::InstrumentInfo::low_price,
                             Eq(Price{3.14}))));
  ASSERT_TRUE(instrument_data.state.order_book.buy_orders.empty());
  ASSERT_TRUE(instrument_data.state.order_book.sell_orders.empty());
}

TEST_F(TradingSystemJsonInstrumentData, WritingSetsJsonValueTypeToObject) {
  const market_state::InstrumentData instrument_data;

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_data).has_value());

  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemJsonInstrumentData, WritesToJson) {
  using namespace simulator::trading_system::test;

  instrument::Cache::InstrumentSpecification specification;
  specification.symbol = Symbol{"symbol"};

  const trading_system::Trade trade{BuyerId{"BuyerId"},
                                    std::nullopt,
                                    Price{0.},
                                    Quantity{0.},
                                    std::nullopt,
                                    core::get_current_system_time(),
                                    MarketPhase::open()};

  market_state::InstrumentInfo info;
  info.low_price = Price{3.14};

  const market_state::InstrumentData instrument_data{
      .specification = specification,
      .state = {.last_trade = trade,
                .info = info,
                .order_book = market_state::OrderBook{}}};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_data).has_value());

  ASSERT_THAT(json_value,
              HasInner("Instrument", HasString("Symbol", "symbol")));
  ASSERT_THAT(json_value, HasInner("LastTrade", HasString("Buyer", "BuyerId")));
  ASSERT_THAT(json_value,
              HasInner("Info", HasDouble("TradingSessionLowPrice", 3.14)));
  ASSERT_THAT(json_value, HasInner("OrderBook", HasArraySize("BuyOrders", 0)));
  ASSERT_THAT(json_value, HasInner("OrderBook", HasArraySize("SellOrders", 0)));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
