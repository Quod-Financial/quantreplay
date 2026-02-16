#include <gmock/gmock.h>

#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;  // NOLINT

struct TradingSystemJsonWriter : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(TradingSystemJsonWriter, WritesNulloptAsNull) {
  ASSERT_TRUE(write(value, doc.GetAllocator(), std::nullopt).has_value());
  ASSERT_TRUE(value.IsNull());
}

TEST_F(TradingSystemJsonWriter, WritesBool) {
  ASSERT_TRUE(write(value, doc.GetAllocator(), true).has_value());
  ASSERT_TRUE(value.IsBool());
  ASSERT_TRUE(value.GetBool());
}

TEST_F(TradingSystemJsonWriter, WritesUnsignedInt) {
  constexpr auto max = std::numeric_limits<unsigned int>::max();

  ASSERT_TRUE(write(value, doc.GetAllocator(), max).has_value());
  ASSERT_TRUE(value.IsUint());
  ASSERT_EQ(value.GetUint(), max);
}

TEST_F(TradingSystemJsonWriter, WritesInt) {
  constexpr auto min = std::numeric_limits<int>::min();

  ASSERT_TRUE(write(value, doc.GetAllocator(), min).has_value());
  ASSERT_TRUE(value.IsInt());
  ASSERT_EQ(value.GetInt(), min);
}

TEST_F(TradingSystemJsonWriter, WritesUInt64) {
  constexpr auto max = std::numeric_limits<std::uint64_t>::max();

  ASSERT_TRUE(write(value, doc.GetAllocator(), max).has_value());
  ASSERT_TRUE(value.IsUint64());
  ASSERT_EQ(value.GetUint64(), max);
}

TEST_F(TradingSystemJsonWriter, WritesInt64) {
  constexpr auto min = std::numeric_limits<std::int64_t>::min();

  ASSERT_TRUE(write(value, doc.GetAllocator(), min).has_value());
  ASSERT_TRUE(value.IsInt64());
  ASSERT_EQ(value.GetInt64(), min);
}

TEST_F(TradingSystemJsonWriter, WritesDouble) {
  ASSERT_TRUE(write(value, doc.GetAllocator(), 3.14).has_value());
  ASSERT_TRUE(value.IsDouble());
  ASSERT_DOUBLE_EQ(value.GetDouble(), 3.14);
}

TEST_F(TradingSystemJsonWriter, WritesString) {
  const std::string str{"string"};

  ASSERT_TRUE(write(value, doc.GetAllocator(), str).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "string");
}

TEST_F(TradingSystemJsonWriter, WritesStringView) {
  constexpr std::string_view str{"string_view"};

  ASSERT_TRUE(write(value, doc.GetAllocator(), str).has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_EQ(value.GetString(), str.data());
  ASSERT_STREQ(value.GetString(), "string_view");
}

TEST_F(TradingSystemJsonWriter, WritesSysUs) {
  using namespace std::chrono_literals;

  ASSERT_TRUE(write(value,
                    doc.GetAllocator(),
                    core::sys_us{core::sys_days{2025y / 4 / 21} + 15h + 30min +
                                 45s + 123456us})
                  .has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "20250421-15:30:45.123456");
}

TEST_F(TradingSystemJsonWriter, WritesLocalDays) {
  using namespace std::chrono_literals;
  ASSERT_TRUE(write(value, doc.GetAllocator(), core::local_days{2025y / 4 / 21})
                  .has_value());
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "20250421");
}

TEST_F(TradingSystemJsonWriter, WritesVectorToJsonArray) {
  using namespace simulator::trading_system::test;
  const std::vector<double> vec{2.74, 3.14};

  ASSERT_TRUE(write(value, doc.GetAllocator(), vec).has_value());
  ASSERT_TRUE(value.IsArray());
  ASSERT_TRUE(value[0].IsDouble());
  ASSERT_DOUBLE_EQ(value[0].GetDouble(), 2.74);
  ASSERT_TRUE(value[1].IsDouble());
  ASSERT_DOUBLE_EQ(value[1].GetDouble(), 3.14);
}

TEST_F(TradingSystemJsonWriter, WritesValueToJsonObjectWithKey) {
  using namespace simulator::trading_system::test;
  value.SetObject();

  ASSERT_TRUE(write(value, doc.GetAllocator(), "key", 3.14).has_value());
  ASSERT_THAT(value, HasDouble("key", 3.14));
}

TEST_F(TradingSystemJsonWriter,
       WritesNullOptionalValueAsNullToJsonObjectWithKey) {
  using namespace simulator::trading_system::test;
  value.SetObject();
  constexpr std::optional<double> opt_value{std::nullopt};

  ASSERT_TRUE(write(value, doc.GetAllocator(), "key", opt_value).has_value());
  ASSERT_THAT(value, HasNull("key"));
}

TEST_F(TradingSystemJsonWriter, WritesOptionalValueToJsonObjectWithKey) {
  using namespace simulator::trading_system::test;
  value.SetObject();
  constexpr std::optional<double> opt_value{3.14};

  ASSERT_TRUE(write(value, doc.GetAllocator(), "key", opt_value).has_value());
  ASSERT_THAT(value, HasDouble("key", 3.14));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
