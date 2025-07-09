#include <gmock/gmock.h>

#include "common/market_state/json/snapshot.hpp"
#include "common/market_state/snapshot.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::market_state::test {
namespace {

using namespace ::testing;

struct TradingSystemCommonSnapshot : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(TradingSystemCommonSnapshot, ReadsFromJson) {
  value.SetObject();

  rapidjson::Value json_instruments;
  core::json::Type<std::vector<InstrumentState>>::write_json_value(
      json_instruments, doc.GetAllocator(), std::vector<InstrumentState>{});

  value.AddMember("venue_id", "venue_id", doc.GetAllocator());
  value.AddMember("instruments", json_instruments.Move(), doc.GetAllocator());

  const auto snapshot = core::json::Type<Snapshot>::read_json_value(value);

  ASSERT_EQ(snapshot.venue_id, "venue_id");
  ASSERT_TRUE(snapshot.instruments.empty());
}

TEST_F(TradingSystemCommonSnapshot, WritesToJson) {
  using namespace simulator::trading_system::test;

  const Snapshot snapshot{.venue_id = "venue_id",
                          .instruments = std::vector<InstrumentState>{}};

  core::json::Type<Snapshot>::write_json_value(
      value, doc.GetAllocator(), snapshot);

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasString("venue_id", "venue_id"));
  ASSERT_THAT(value, HasArraySize("instruments", 0));
}

}  // namespace
}  // namespace simulator::trading_system::market_state::test
