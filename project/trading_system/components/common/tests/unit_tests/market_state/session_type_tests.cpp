#include <gtest/gtest.h>
#include <rapidjson/document.h>

#include "common/market_state/snapshot.hpp"
#include "core/common/json/type_enum.hpp"

namespace simulator::trading_system::market_state::test {
namespace {

TEST(TradingSystemStatePersistenceSessionTypeFmt, FormatsFixToString) {
  ASSERT_EQ(fmt::to_string(SessionType::Fix), "\"Fix\"");
}

TEST(TradingSystemStatePersistenceSessionTypeFmt, FormatsGeneratorToString) {
  ASSERT_EQ(fmt::to_string(SessionType::Generator), "\"Generator\"");
}

struct TradingSystemStatePersistenceSessionType : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(TradingSystemStatePersistenceSessionType, ReadsFixFromJson) {
  value.SetString("Fix", doc.GetAllocator());
  ASSERT_EQ(simulator::core::json::Type<SessionType>::read_json_value(value),
            SessionType::Fix);
}

TEST_F(TradingSystemStatePersistenceSessionType, ReadsGeneratorFromJson) {
  value.SetString("Generator", doc.GetAllocator());
  ASSERT_EQ(simulator::core::json::Type<SessionType>::read_json_value(value),
            SessionType::Generator);
}

TEST_F(TradingSystemStatePersistenceSessionType, WritesFixToJson) {
  simulator::core::json::Type<SessionType>::write_json_value(
      value, doc.GetAllocator(), SessionType::Fix);
  ASSERT_TRUE(value.IsString());
  ASSERT_EQ(value.GetString(), "Fix");
}

TEST_F(TradingSystemStatePersistenceSessionType, WritesGeneratorToJson) {
  simulator::core::json::Type<SessionType>::write_json_value(
      value, doc.GetAllocator(), SessionType::Generator);
  ASSERT_TRUE(value.IsString());
  ASSERT_EQ(value.GetString(), "Generator");
}

}  // namespace
}  // namespace simulator::trading_system::market_state::test
