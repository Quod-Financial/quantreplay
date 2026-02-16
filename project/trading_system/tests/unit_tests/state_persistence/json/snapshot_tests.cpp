#include <gmock/gmock.h>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "ih/state_persistence/json/snapshot.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;  // NOLINT

struct TradingSystemJsonSnapshot : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value json_value;
};

TEST_F(TradingSystemJsonSnapshot, ReadsFromJson) {
  json_value.SetObject();

  rapidjson::Value json_instruments;
  json::write(json_instruments,
              doc.GetAllocator(),
              std::vector<market_state::InstrumentData>{});

  json_value.AddMember("VenueID", "venue_id", doc.GetAllocator());
  json_value.AddMember(
      "Instruments", json_instruments.Move(), doc.GetAllocator());

  market_state::Snapshot snapshot;
  ASSERT_TRUE(json::read(json_value, snapshot).has_value());

  ASSERT_EQ(snapshot.venue_id, "venue_id");
  ASSERT_TRUE(snapshot.instruments.empty());
}

TEST_F(TradingSystemJsonSnapshot, WritingSetsJsonValueTypeToObject) {
  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), market_state::Snapshot{})
          .has_value());

  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemJsonSnapshot, WritesToJson) {
  using namespace simulator::trading_system::test;

  const market_state::Snapshot snapshot{
      .venue_id = "venue_id",
      .instruments = std::vector<market_state::InstrumentData>{}};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), snapshot).has_value());

  ASSERT_THAT(json_value, HasString("VenueID", "venue_id"));
  ASSERT_THAT(json_value, HasArraySize("Instruments", 0));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
