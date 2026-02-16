#include <gmock/gmock.h>
#include <rapidjson/document.h>

#include "ih/state_persistence/json/detail/reader.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;                          // NOLINT
using namespace ::simulator::trading_system::test;  // NOLINT

TEST(TradingSystemJsonReader, ReturnsErrorOnReadIntegerAsBool) {
  const rapidjson::Value json_value{42};
  bool result;

  ASSERT_THAT(
      read(json_value, result),
      ReturnsError("unexpected data Type `rapidjson::Type::kNumberType', "
                   "`bool' is expected"));
}

TEST(TradingSystemJsonReader, ReadsBool) {
  const rapidjson::Value json_value{true};
  bool result = false;

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_TRUE(result);
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsUnsingedInt) {
  const rapidjson::Value json_value{true};
  unsigned int result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`unsigned int' is expected"));
}

TEST(TradingSystemJsonReader, ReadsUnsignedInt) {
  constexpr auto max = std::numeric_limits<unsigned int>::max();
  const rapidjson::Value json_value{max};
  unsigned int result{};

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_EQ(result, max);
}

TEST(TradingSystemJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsInt) {
  const rapidjson::Value json_value{true};
  int result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`int' is expected"));
}

TEST(TradingSystemJsonReader, ReadsInt) {
  constexpr auto max = std::numeric_limits<int>::max();
  const rapidjson::Value json_value{max};
  int result{};

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_EQ(result, max);
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsUInt64) {
  const rapidjson::Value json_value{true};
  std::uint64_t result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`uint64' is expected"));
}

TEST(TradingSystemJsonReader, ReadsUInt64) {
  constexpr auto max = std::numeric_limits<std::uint64_t>::max();
  const rapidjson::Value json_value{max};
  std::uint64_t result{};

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_EQ(result, max);
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsInt64) {
  const rapidjson::Value json_value{true};
  std::int64_t result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`int64' is expected"));
}

TEST(TradingSystemJsonReader, ReadsInt64) {
  constexpr auto min = std::numeric_limits<std::int64_t>::min();
  const rapidjson::Value json_value{min};
  std::int64_t result{};

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_EQ(result, min);
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsDouble) {
  const rapidjson::Value json_value{true};
  double result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`double' is expected"));
}

TEST(TradingSystemJsonReader, ReadsDouble) {
  const rapidjson::Value json_value{3.14};
  double result{};

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_DOUBLE_EQ(result, 3.14);
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsString) {
  const rapidjson::Value json_value{true};
  std::string result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`string' is expected"));
}

TEST(TradingSystemJsonReader, ReadsString) {
  const rapidjson::Value json_value{"string"};
  std::string result;

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_EQ(result, "string");
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsStringView) {
  const rapidjson::Value json_value{true};
  std::string_view result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`string_view' is expected"));
}

TEST(TradingSystemJsonReader, ReadsStringView) {
  const rapidjson::Value json_value{"string_view"};
  std::string_view result;

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_EQ(result, "string_view");
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsSysUs) {
  const rapidjson::Value json_value{true};
  core::sys_us result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`string_view' is expected"));
}

TEST(CoreJsonReader, ReturnsErrorOnReadInvalidSysUs) {
  const rapidjson::Value json_value{"2025-04-21 14:47:59.270438"};
  core::sys_us result;

  ASSERT_THAT(
      read(json_value, result),
      ReturnsError("failed to parse sys_us: `2025-04-21 14:47:59.270438'"));
}

TEST(TradingSystemJsonReader, ReadsSysUs) {
  using namespace std::chrono_literals;
  const rapidjson::Value json_value{"20250421-15:30:45.123456"};
  core::sys_us result;

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_EQ(result,
            core::sys_us{std::chrono::sys_days{2025y / 4 / 21} + 15h + 30min +
                         45s + 123456us});
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsLocalDays) {
  const rapidjson::Value json_value{true};
  core::local_days result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`string_view' is expected"));
}

TEST(CoreJsonReader, ReturnsErrorOnReadInvalidLocalDays) {
  const rapidjson::Value json_value{"2025-04-21"};
  core::local_days result;

  ASSERT_THAT(read(json_value, result),
              ReturnsError("failed to parse local_days: `2025-04-21'"));
}

TEST(TradingSystemJsonReader, ReadsLocalDays) {
  using namespace std::chrono_literals;
  const rapidjson::Value json_value{"20250421"};
  core::local_days result;

  ASSERT_TRUE(read(json_value, result).has_value());
  ASSERT_EQ(result, core::local_days{(2025y / 4 / 21)});
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsArray) {
  const rapidjson::Value json_value{true};

  std::vector<int> result;
  ASSERT_THAT(read(json_value, result),
              ReturnsError("unexpected data Type `rapidjson::Type::kTrueType', "
                           "`rapidjson::Type::kArrayType' is expected"));
}

TEST(TradingSystemJsonReader,
     ReturnsErrorOnReadUnexpectedIntFromArrayWithDoubles) {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kArrayType};
  json_value.PushBack(3.14, doc.GetAllocator());
  json_value.PushBack(42, doc.GetAllocator());

  std::vector<double> result;
  ASSERT_THAT(
      read(json_value, result),
      ReturnsError("failed to parse JSON array item #1: unexpected data Type "
                   "`rapidjson::Type::kNumberType', "
                   "`double' is expected"));
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadUnexpectedEnumValueFromArray) {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kArrayType};
  json_value.PushBack("FXSpot", doc.GetAllocator());
  json_value.PushBack("InvalidValue", doc.GetAllocator());

  std::vector<SecurityType> result;
  ASSERT_THAT(read(json_value, result),
              ReturnsError("failed to parse JSON array item #1: Unknown "
                           "SecurityType string `InvalidValue'"));
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadFromNotFoundKey) {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
  json_value.AddMember("key", 3.14, doc.GetAllocator());
  int result;

  ASSERT_THAT(read(json_value, "invalid_key", result),
              ReturnsError("missing field `invalid_key' in JSON object"));
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsIntByKey) {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
  json_value.AddMember("key", true, doc.GetAllocator());
  int result;

  ASSERT_THAT(read(json_value, "key", result),
              ReturnsError("failed to parse field `key': unexpected data Type "
                           "`rapidjson::Type::kTrueType', `int' is expected"));
}

TEST(TradingSystemJsonReader, ReadsValueFromJsonObjectByKey) {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
  json_value.AddMember("key", 3.14, doc.GetAllocator());
  double result;

  ASSERT_TRUE(read(json_value, "key", result).has_value());
  ASSERT_DOUBLE_EQ(result, 3.14);
}

TEST(TradingSystemJsonReader, ReturnsErrorOnReadBoolAsOptionalIntByKey) {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
  json_value.AddMember("key", true, doc.GetAllocator());

  std::optional<int> result;
  ASSERT_THAT(read(json_value, "key", result),
              ReturnsError("failed to parse field `key': unexpected data Type "
                           "`rapidjson::Type::kTrueType', `int' is expected"));
}

TEST(TradingSystemJsonReader,
     DoesNotChangeOptionalDestinationIfJsonObjectDoesNotHaveKey) {
  rapidjson::Document doc;
  const rapidjson::Value json_value{rapidjson::Type::kObjectType};
  std::optional<int> result = 42;

  ASSERT_TRUE(read(json_value, "key", result).has_value());
  ASSERT_THAT(result, Optional(Eq(42)));
}

TEST(TradingSystemJsonReader, ReadsOptionalValueFromJsonObjectByKey) {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
  json_value.AddMember("key", 3.14, doc.GetAllocator());
  std::optional<double> result = 2.71;

  ASSERT_TRUE(read(json_value, "key", result).has_value());
  ASSERT_DOUBLE_EQ(result.value(), 3.14);
}

TEST(TradingSystemJsonReader, ReadsOptionalEnumerableFromJsonObjectByKey) {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
  json_value.AddMember("key", "FXSpot", doc.GetAllocator());

  std::optional<SecurityType> result;
  ASSERT_TRUE(read(json_value, "key", result).has_value());
  ASSERT_EQ(result.value(), SecurityType::Option::FxSpot);
}

TEST(TradingSystemJsonReader, ReturnsErrorOnNotFoundArrayByKey) {
  rapidjson::Document doc;
  rapidjson::Value json_value{rapidjson::Type::kObjectType};
  json_value.AddMember("key", "json_value", doc.GetAllocator());

  std::vector<int> result;
  ASSERT_THAT(read(json_value, "invalid_key", result),
              ReturnsError("missing field `invalid_key' in JSON object"));
}

TEST(TradingSystemJsonReader, ReadsArrayOfIntegersAsVectorByKey) {
  rapidjson::Document doc;
  rapidjson::Value array{rapidjson::Type::kArrayType};
  array.PushBack(42, doc.GetAllocator());
  array.PushBack(3, doc.GetAllocator());

  rapidjson::Value json_value{rapidjson::Type::kObjectType};
  json_value.AddMember("key", array, doc.GetAllocator());

  std::vector<int> result;
  ASSERT_TRUE(read(json_value, "key", result).has_value());
  ASSERT_THAT(result, ElementsAre(42, 3));
}

TEST(TradingSystemJsonReader, ReadsArrayOfEnumeratorsAsVectorByKey) {
  rapidjson::Document doc;
  rapidjson::Value array{rapidjson::Type::kArrayType};
  array.PushBack("FXSpot", doc.GetAllocator());
  array.PushBack("Index", doc.GetAllocator());

  rapidjson::Value json_value{rapidjson::Type::kObjectType};
  json_value.AddMember("key", array, doc.GetAllocator());

  std::vector<SecurityType> result;
  ASSERT_TRUE(read(json_value, "key", result).has_value());
  ASSERT_THAT(
      result,
      ElementsAre(SecurityType::Option::FxSpot, SecurityType::Option::Index));
}

}  // namespace
}  // namespace simulator::trading_system::json::test
