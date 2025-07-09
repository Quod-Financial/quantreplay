#include <gtest/gtest.h>

#include <sstream>

#include "ih/state_persistence/serializer.hpp"

namespace simulator::trading_system::test {
namespace {

TEST(TradingSystemStatePersistenceJsonSerializer,
     SerializesEmptySnapshotToFormattedJson) {
  const market_state::Snapshot snapshot;
  std::stringstream ss;
  const JsonSerializer serializer;

  ASSERT_TRUE(serializer.serialize(snapshot, ss));

  // clang-format off
  const std::string expected =
R"({
    "venue_id": "",
    "instruments": []
})";
  // clang-format on
  ASSERT_EQ(ss.str(), expected);
}

TEST(TradingSystemStatePersistenceJsonSerializer, ReturnsErrorOnEmptyJson) {
  const JsonSerializer serializer;
  std::stringstream ss;

  auto result = serializer.deserialize(ss);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(result.error(), "Error parsing JSON: The document is empty.");
}

TEST(TradingSystemStatePersistenceJsonSerializer, ReturnsErrorOnMalformedJson) {
  const JsonSerializer serializer;
  std::stringstream ss{"{ not quoted key"};

  auto result = serializer.deserialize(ss);
  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(
      result.error(),
      "Error parsing JSON on offset 2: Missing a name for object member.");
}

TEST(TradingSystemStatePersistenceJsonSerializer,
     ReturnsErrorIncorrectVenueIdValueType) {
  const JsonSerializer serializer;
  // clang-format off
  std::stringstream ss{
R"({
    "venue_id": 123,
    "instruments": []
})"};
  // clang-format on

  auto result = serializer.deserialize(ss);

  ASSERT_FALSE(result.has_value());
  ASSERT_EQ(
      result.error(),
      "Error deserializing JSON: failed to parse field `venue_id`: unexpected "
      "data Type `rapidjson::Type::kNumberType`, `string` is expected");
}

TEST(TradingSystemStatePersistenceJsonSerializer, ReturnsParsedSnapshot) {
  const JsonSerializer serializer;
  // clang-format off
  std::stringstream ss{
R"({
    "venue_id": "Venue",
    "instruments": []
})"};
  // clang-format on

  auto result = serializer.deserialize(ss);

  ASSERT_TRUE(result.has_value());
  const auto& snapshot = result.value();
  ASSERT_EQ(snapshot.venue_id, "Venue");
  ASSERT_TRUE(snapshot.instruments.empty());
}

}  // namespace
}  // namespace simulator::trading_system::test
