#include <gmock/gmock.h>

#include "common/instrument_state.hpp"
#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;

struct TradingSystemJsonOrderBook : public ::testing::Test {
  [[nodiscard]]
  auto json(const market_state::LimitOrder& order) -> rapidjson::Value {
    rapidjson::Value value;
    json::write(value, doc.GetAllocator(), order);
    return value;
  }

  rapidjson::Document doc;
  rapidjson::Value json_value;
};

TEST_F(TradingSystemJsonOrderBook, ReadsEmptyOrdersFromJson) {
  json_value.SetObject();
  json_value.AddMember("BuyOrders",
                       rapidjson::Value{rapidjson::Type::kArrayType},
                       doc.GetAllocator());
  json_value.AddMember("SellOrders",
                       rapidjson::Value{rapidjson::Type::kArrayType},
                       doc.GetAllocator());

  market_state::OrderBook order_book;
  ASSERT_TRUE(json::read(json_value, order_book).has_value());

  ASSERT_TRUE(order_book.buy_orders.empty());
  ASSERT_TRUE(order_book.sell_orders.empty());
}

TEST_F(TradingSystemJsonOrderBook, ReadsFromJson) {
  json_value.SetObject();

  market_state::LimitOrder order;
  order.client_order_id = ClientOrderId{"buy"};
  rapidjson::Value json_buy_orders{rapidjson::Type::kArrayType};
  json_buy_orders.PushBack(json(order).Move(), doc.GetAllocator());
  json_value.AddMember("BuyOrders", json_buy_orders.Move(), doc.GetAllocator());

  order.client_order_id = ClientOrderId{"sell"};
  rapidjson::Value json_sell_orders{rapidjson::Type::kArrayType};
  json_sell_orders.PushBack(json(order).Move(), doc.GetAllocator());
  json_value.AddMember(
      "SellOrders", json_sell_orders.Move(), doc.GetAllocator());

  market_state::OrderBook order_book;
  ASSERT_TRUE(json::read(json_value, order_book).has_value());

  ASSERT_EQ(order_book.buy_orders.size(), 1);
  ASSERT_EQ(order_book.buy_orders[0].client_order_id, ClientOrderId{"buy"});
  ASSERT_EQ(order_book.sell_orders.size(), 1);
  ASSERT_EQ(order_book.sell_orders[0].client_order_id, ClientOrderId{"sell"});
}

TEST_F(TradingSystemJsonOrderBook, WritingSetsJsonValueTypeToObject) {
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), market_state::OrderBook{})
          .has_value());

  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemJsonOrderBook, WritesEmptyContainersToJson) {
  using namespace simulator::trading_system::test;

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), market_state::OrderBook{})
          .has_value());

  ASSERT_THAT(json_value, HasArraySize("BuyOrders", 0));
  ASSERT_THAT(json_value, HasArraySize("SellOrders", 0));
}

TEST_F(TradingSystemJsonOrderBook, WritesToJson) {
  using namespace simulator::trading_system::test;

  market_state::OrderBook order_book;
  market_state::LimitOrder order;
  order.client_order_id = ClientOrderId{"buy"};
  order_book.buy_orders.push_back(order);

  order.client_order_id = ClientOrderId{"sell"};
  order_book.sell_orders.push_back(order);

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), order_book).has_value());

  ASSERT_THAT(json_value, HasArraySize("BuyOrders", 1));
  ASSERT_THAT(json_value["BuyOrders"][0], HasString("ClientOrderID", "buy"));

  ASSERT_THAT(json_value, HasArraySize("SellOrders", 1));
  ASSERT_THAT(json_value["SellOrders"][0], HasString("ClientOrderID", "sell"));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
