#include <gmock/gmock.h>

#include "common/market_state/json/order_book.hpp"
#include "common/market_state/snapshot.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::market_state::test {
namespace {

using namespace ::testing;

struct TradingSystemCommonOrderBook : public ::testing::Test {
  [[nodiscard]]
  auto json(const LimitOrder& order) -> rapidjson::Value {
    rapidjson::Value json_value;
    core::json::Type<LimitOrder>::write_json_value(
        json_value, doc.GetAllocator(), order);
    return json_value;
  }

  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(TradingSystemCommonOrderBook, ReadsEmptyOrdersFromJson) {
  value.SetObject();
  value.AddMember("buy_orders",
                  rapidjson::Value(rapidjson::Type::kArrayType),
                  doc.GetAllocator());
  value.AddMember("sell_orders",
                  rapidjson::Value(rapidjson::Type::kArrayType),
                  doc.GetAllocator());

  const auto order_book = core::json::Type<OrderBook>::read_json_value(value);

  ASSERT_TRUE(order_book.buy_orders.empty());
  ASSERT_TRUE(order_book.sell_orders.empty());
}

TEST_F(TradingSystemCommonOrderBook, ReadsFromJson) {
  value.SetObject();

  LimitOrder order;
  order.client_order_id = ClientOrderId{"buy"};
  rapidjson::Value json_buy_orders{rapidjson::Type::kArrayType};
  json_buy_orders.PushBack(json(order).Move(), doc.GetAllocator());
  value.AddMember("buy_orders", json_buy_orders.Move(), doc.GetAllocator());

  order.client_order_id = ClientOrderId{"sell"};
  rapidjson::Value json_sell_orders{rapidjson::Type::kArrayType};
  json_sell_orders.PushBack(json(order).Move(), doc.GetAllocator());
  value.AddMember("sell_orders", json_sell_orders.Move(), doc.GetAllocator());

  const auto order_book = core::json::Type<OrderBook>::read_json_value(value);

  ASSERT_EQ(order_book.buy_orders.size(), 1);
  ASSERT_EQ(order_book.buy_orders[0].client_order_id, ClientOrderId{"buy"});
  ASSERT_EQ(order_book.sell_orders.size(), 1);
  ASSERT_EQ(order_book.sell_orders[0].client_order_id, ClientOrderId{"sell"});
}

TEST_F(TradingSystemCommonOrderBook, WritesEmptyContainersToJson) {
  using namespace simulator::trading_system::test;

  const OrderBook order_book;

  core::json::Type<OrderBook>::write_json_value(
      value, doc.GetAllocator(), order_book);

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasArraySize("buy_orders", 0));
  ASSERT_THAT(value, HasArraySize("sell_orders", 0));
}

TEST_F(TradingSystemCommonOrderBook, WritesToJson) {
  using namespace simulator::trading_system::test;

  OrderBook order_book;
  LimitOrder order;
  order.client_order_id = ClientOrderId{"buy"};
  order_book.buy_orders.push_back(order);

  order.client_order_id = ClientOrderId{"sell"};
  order_book.sell_orders.push_back(order);

  core::json::Type<OrderBook>::write_json_value(
      value, doc.GetAllocator(), order_book);

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasArraySize("buy_orders", 1));
  ASSERT_THAT(value["buy_orders"][0], HasString("client_order_id", "buy"));

  ASSERT_THAT(value, HasArraySize("sell_orders", 1));
  ASSERT_THAT(value["sell_orders"][0], HasString("client_order_id", "sell"));
}

}  // namespace
}  // namespace simulator::trading_system::market_state::test
