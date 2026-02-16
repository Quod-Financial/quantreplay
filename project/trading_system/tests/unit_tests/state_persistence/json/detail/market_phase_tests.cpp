#include <gmock/gmock.h>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;

struct TradingSystemJsonMarketPhase : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value json_value;
};

TEST_F(TradingSystemJsonMarketPhase, ReadsFromJson) {
  json_value.SetObject();
  json_value.AddMember(
      "TradingSessionSubID", "OutOfMainSessionTrading", doc.GetAllocator());
  json_value.AddMember("SecurityTradingStatus", "Resume", doc.GetAllocator());

  MarketPhase phase = MarketPhase::open();
  ASSERT_TRUE(json::read(json_value, phase).has_value());

  ASSERT_EQ(phase.trading_phase(), TradingPhase::Option::Closed);
  ASSERT_EQ(phase.trading_status(), TradingStatus::Option::Resume);
}

TEST_F(TradingSystemJsonMarketPhase, WritingSetsJsonValueTypeToObject) {
  ASSERT_TRUE(json::write(json_value, doc.GetAllocator(), MarketPhase::open())
                  .has_value());
  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemJsonMarketPhase, WritesToJson) {
  using namespace simulator::trading_system::test;

  constexpr MarketPhase phase{TradingPhase::Option::Closed,
                              TradingStatus::Option::Halt};

  ASSERT_TRUE(json::write(json_value, doc.GetAllocator(), phase).has_value());

  ASSERT_THAT(json_value,
              HasString("TradingSessionSubID", "OutOfMainSessionTrading"));
  ASSERT_THAT(json_value, HasString("SecurityTradingStatus", "TradingHalt"));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
