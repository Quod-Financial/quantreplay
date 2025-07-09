#include <gtest/gtest.h>

#include "core/domain/json/market_phase.hpp"
#include "core/domain/market_phase.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::core::test {
namespace {

using namespace ::testing;  // NOLINT

struct CoreMarketPhaseJson : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
};

TEST_F(CoreMarketPhaseJson, ReadsFromJson) {
  json_value.AddMember("trading_phase", "Open", doc.GetAllocator());
  json_value.AddMember("trading_status", "Resume", doc.GetAllocator());

  const auto market_phase =
      simulator::core::json::Type<MarketPhase>::read_json_value(json_value);

  ASSERT_EQ(market_phase.trading_phase(), TradingPhase::Option::Open);
  ASSERT_EQ(market_phase.trading_status(), TradingStatus::Option::Resume);
}

TEST_F(CoreMarketPhaseJson, WritesToJson) {
  using namespace json::test;

  constexpr auto market_phase =
      MarketPhase{TradingPhase::Option::Closed, TradingStatus::Option::Halt};

  json::Type<MarketPhase>::write_json_value(
      json_value, doc.GetAllocator(), market_phase);

  ASSERT_TRUE(json_value.IsObject());
  ASSERT_THAT(json_value, HasString("trading_phase", "Closed"));
  ASSERT_THAT(json_value, HasString("trading_status", "Halt"));
}

}  // namespace
}  // namespace simulator::core::test
