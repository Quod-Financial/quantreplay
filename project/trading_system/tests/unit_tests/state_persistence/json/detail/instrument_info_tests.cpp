#include <gmock/gmock.h>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;

struct TradingSystemJsonInstrumentInfo : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value json_value;
};

TEST_F(TradingSystemJsonInstrumentInfo, ReadsFromJson) {
  json_value.SetObject();
  json_value.AddMember("TradingSessionLowPrice", 100.1, doc.GetAllocator());
  json_value.AddMember("TradingSessionHighPrice", 300.3, doc.GetAllocator());

  market_state::InstrumentInfo info;
  ASSERT_TRUE(json::read(json_value, info).has_value());

  ASSERT_EQ(info.low_price, Price{100.1});
  ASSERT_EQ(info.high_price, Price{300.3});
}

TEST_F(TradingSystemJsonInstrumentInfo, WritingSetsJsonValueTypeToObject) {
  using namespace simulator::trading_system::test;

  const market_state::InstrumentInfo instrument_info{
      .low_price = Price{100.1}, .high_price = Price{300.3}};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_info).has_value());
  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemJsonInstrumentInfo, WritesToJson) {
  using namespace simulator::trading_system::test;

  const market_state::InstrumentInfo instrument_info{
      .low_price = Price{100.1}, .high_price = Price{300.3}};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_info).has_value());

  ASSERT_THAT(json_value, HasDouble("TradingSessionLowPrice", 100.1));
  ASSERT_THAT(json_value, HasDouble("TradingSessionHighPrice", 300.3));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
