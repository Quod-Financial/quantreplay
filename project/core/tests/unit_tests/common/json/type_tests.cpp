#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

#include "core/common/json/type.hpp"
#include "core/tools/time.hpp"
#include "fixtures/given_expected_fixture.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::core::json::test {
namespace {

TEST(CoreJsonReader, ThrowsExceptionOnReadIntegerAsBool) {
  const rapidjson::Value value{42};

  ASSERT_THROW((void)Type<bool>::read_json_value(value), std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadIntegerAsBool) {
  const rapidjson::Value value{42};

  try {
    [[maybe_unused]] const auto result = Type<bool>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "unexpected data Type `rapidjson::Type::kNumberType`, `bool` "
                 "is expected");
  }
}

TEST(CoreJsonReader, ReadsBool) {
  const rapidjson::Value value{true};

  ASSERT_TRUE(Type<bool>::read_json_value(value));
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsUnsingedInt) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<unsigned int>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsUnsingedInt) {
  const rapidjson::Value value{true};

  try {
    [[maybe_unused]] const auto result =
        Type<unsigned int>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "unexpected data Type `rapidjson::Type::kTrueType`, `unsigned "
                 "int` is expected");
  }
}

TEST(CoreJsonReader, ReadsUnsignedInt) {
  constexpr auto max = std::numeric_limits<unsigned int>::max();
  const rapidjson::Value value{max};

  ASSERT_EQ(Type<unsigned int>::read_json_value(value), max);
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsInt) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<int>::read_json_value(value), std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsInt) {
  const rapidjson::Value value{true};

  try {
    [[maybe_unused]] const auto result = Type<int>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(
        e.what(),
        "unexpected data Type `rapidjson::Type::kTrueType`, `int` is expected");
  }
}

TEST(CoreJsonReader, ReadsInt) {
  constexpr auto min = std::numeric_limits<int>::min();
  const rapidjson::Value value{min};

  ASSERT_EQ(Type<int>::read_json_value(value), min);
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsUInt64) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<std::uint64_t>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsUInt64) {
  const rapidjson::Value value{true};

  try {
    [[maybe_unused]] const auto result =
        Type<std::uint64_t>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "unexpected data Type `rapidjson::Type::kTrueType`, `uint64` "
                 "is expected");
  }
}

TEST(CoreJsonReader, ReadsUInt64) {
  constexpr auto max = std::numeric_limits<std::uint64_t>::max();
  const rapidjson::Value value{max};

  ASSERT_EQ(Type<std::uint64_t>::read_json_value(value), max);
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsInt64) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<std::int64_t>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsInt64) {
  const rapidjson::Value value{true};

  try {
    [[maybe_unused]] const auto result =
        Type<std::int64_t>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "unexpected data Type `rapidjson::Type::kTrueType`, `int64` "
                 "is expected");
  }
}

TEST(CoreJsonReader, ReadsInt64) {
  constexpr auto min = std::numeric_limits<std::int64_t>::min();
  const rapidjson::Value value{min};

  ASSERT_EQ(Type<std::int64_t>::read_json_value(value), min);
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsDouble) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<double>::read_json_value(value), std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsDouble) {
  const rapidjson::Value value{true};

  try {
    [[maybe_unused]] const auto result = Type<double>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "unexpected data Type `rapidjson::Type::kTrueType`, `double` "
                 "is expected");
  }
}

TEST(CoreJsonReader, ReadsDouble) {
  const rapidjson::Value value{3.14};

  ASSERT_DOUBLE_EQ(Type<double>::read_json_value(value), 3.14);
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsString) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<std::string>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsString) {
  const rapidjson::Value value{true};

  try {
    [[maybe_unused]] const auto result =
        Type<std::string>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "unexpected data Type `rapidjson::Type::kTrueType`, `string` "
                 "is expected");
  }
}

TEST(CoreJsonReader, ReadsString) {
  const rapidjson::Value value{"string"};

  ASSERT_EQ(Type<std::string>::read_json_value(value), "string");
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsStringView) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<std::string_view>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsStringView) {
  const rapidjson::Value value{true};

  try {
    [[maybe_unused]] const auto result =
        Type<std::string_view>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "unexpected data Type `rapidjson::Type::kTrueType`, "
                 "`string_view` is expected");
  }
}

TEST(CoreJsonReader, ReadsStringView) {
  const rapidjson::Value value{"string_view"};

  ASSERT_EQ(Type<std::string_view>::read_json_value(value), "string_view");
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsSysUs) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<core::sys_us>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsSysUs) {
  const rapidjson::Value value{true};

  try {
    [[maybe_unused]] const auto result =
        Type<core::sys_us>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "unexpected data Type `rapidjson::Type::kTrueType`, "
                 "`string_view` is expected");
  }
}

TEST(CoreJsonReader, ThrowsExceptionOnReadInvalidSysUs) {
  const rapidjson::Value value{"14:47:59.270438 2025-04-21"};

  ASSERT_THROW((void)Type<core::sys_us>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadInvalidSysUs) {
  const rapidjson::Value value{"14:47:59.270438 2025-04-21"};

  try {
    [[maybe_unused]] const auto result =
        Type<core::sys_us>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "failed to parse sys_us: 14:47:59.270438 2025-04-21");
  }
}

TEST(CoreJsonReader, ReadsSysUs) {
  using namespace std::chrono_literals;
  const rapidjson::Value value{"2025-04-21 14:47:59.270438"};

  ASSERT_EQ(Type<core::sys_us>::read_json_value(value),
            core::sys_us{core::sys_days{2025y / 4 / 21} + 14h + 47min + 59s +
                         270438us});
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsLocalDays) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<core::local_days>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsLocalDays) {
  const rapidjson::Value value{true};

  try {
    [[maybe_unused]] const auto result =
        Type<core::local_days>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "unexpected data Type `rapidjson::Type::kTrueType`, "
                 "`string_view` is expected");
  }
}

TEST(CoreJsonReader, ThrowsExceptionOnReadInvalidLocalDays) {
  const rapidjson::Value value{"21-Apr-2025"};

  ASSERT_THROW((void)Type<core::local_days>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadInvalidLocalDays) {
  const rapidjson::Value value{"21-Apr-2025"};

  try {
    [[maybe_unused]] const auto result =
        Type<core::local_days>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(), "failed to parse sys_days: 21-Apr-2025");
  }
}

TEST(CoreJsonReader, ReadsLocalDays) {
  using namespace std::chrono_literals;
  const rapidjson::Value value{"2025-04-21"};

  ASSERT_EQ(Type<core::local_days>::read_json_value(value),
            core::local_days{2025y / 4 / 21});
}

TEST(CoreJsonReader, ThrowsExceptionOnReadFromNotFoundKey) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kObjectType};
  value.AddMember("key", 3.14, doc.GetAllocator());

  ASSERT_THROW((void)json::read_json_value<double>(value, "invalid_key"),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadFromNotFoundKey) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kObjectType};
  value.AddMember("key", 3.14, doc.GetAllocator());

  try {
    [[maybe_unused]] const auto result =
        json::read_json_value<double>(value, "invalid_key");
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(), "missing field `invalid_key` in JSON object");
  }
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsIntByKey) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kObjectType};
  value.AddMember("key", true, doc.GetAllocator());

  ASSERT_THROW((void)json::read_json_value<int>(value, "key"),
               std::runtime_error);
}

TEST(CoreJsonReader, ThrowsMeaningfulExceptionMessageOnReadBoolAsIntByKey) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kObjectType};
  value.AddMember("key", true, doc.GetAllocator());

  try {
    [[maybe_unused]] const auto result =
        json::read_json_value<int>(value, "key");
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "failed to parse field `key`: unexpected data Type "
                 "`rapidjson::Type::kTrueType`, "
                 "`int` is expected");
  }
}

TEST(CoreJsonReader, ReadsValueFromJsonObjectByKey) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kObjectType};
  value.AddMember("key", 3.14, doc.GetAllocator());

  const auto result = json::read_json_value<double>(value, "key");

  ASSERT_DOUBLE_EQ(result, 3.14);
}

TEST(CoreJsonReader, ThrowsExceptionOnReadBoolAsOptionalIntByKey) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kObjectType};
  value.AddMember("key", true, doc.GetAllocator());

  ASSERT_THROW((void)json::read_json_value<std::optional<int>>(value, "key"),
               std::runtime_error);
}

TEST(CoreJsonReader,
     ThrowsMeaningfulExceptionMessageOnReadBoolAsOptionalIntByKey) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kObjectType};
  value.AddMember("key", true, doc.GetAllocator());

  try {
    [[maybe_unused]] const auto result =
        json::read_json_value<std::optional<int>>(value, "key");
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "failed to parse field `key`: unexpected data Type "
                 "`rapidjson::Type::kTrueType`, `int` is expected");
  }
}

TEST(CoreJsonReader, ReadsNullOptionalValueFromJsonObjectByKey) {
  rapidjson::Document doc;
  const rapidjson::Value value{rapidjson::Type::kObjectType};

  const auto result =
      json::read_json_value<std::optional<double>>(value, "key");

  ASSERT_FALSE(result.has_value());
}

TEST(CoreJsonReader, ReadsOptionalValueFromJsonObjectByKey) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kObjectType};
  value.AddMember("key", 3.14, doc.GetAllocator());

  const auto result =
      json::read_json_value<std::optional<double>>(value, "key");

  ASSERT_TRUE(result.has_value());
  ASSERT_DOUBLE_EQ(result.value(), 3.14);
}

struct CoreJsonWriter : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(CoreJsonWriter, WritesBool) {
  Type<bool>::write_json_value(value, doc.GetAllocator(), true);

  ASSERT_TRUE(value.IsBool());
  ASSERT_TRUE(value.GetBool());
}

TEST_F(CoreJsonWriter, WritesUnsignedInt) {
  constexpr auto max = std::numeric_limits<unsigned int>::max();

  Type<unsigned int>::write_json_value(value, doc.GetAllocator(), max);

  ASSERT_TRUE(value.IsUint());
  ASSERT_EQ(value.GetUint(), max);
}

TEST_F(CoreJsonWriter, WritesInt) {
  constexpr auto min = std::numeric_limits<int>::min();

  Type<int>::write_json_value(value, doc.GetAllocator(), min);

  ASSERT_TRUE(value.IsInt());
  ASSERT_EQ(value.GetInt(), min);
}

TEST_F(CoreJsonWriter, WritesUInt64) {
  constexpr auto max = std::numeric_limits<std::uint64_t>::max();

  Type<std::uint64_t>::write_json_value(value, doc.GetAllocator(), max);

  ASSERT_TRUE(value.IsUint64());
  ASSERT_EQ(value.GetUint64(), max);
}

TEST_F(CoreJsonWriter, WritesInt64) {
  constexpr auto min = std::numeric_limits<std::int64_t>::min();

  Type<std::int64_t>::write_json_value(value, doc.GetAllocator(), min);

  ASSERT_TRUE(value.IsInt64());
  ASSERT_EQ(value.GetInt64(), min);
}

TEST_F(CoreJsonWriter, WritesDouble) {
  Type<double>::write_json_value(value, doc.GetAllocator(), 3.14);

  ASSERT_TRUE(value.IsDouble());
  ASSERT_DOUBLE_EQ(value.GetDouble(), 3.14);
}

TEST_F(CoreJsonWriter, WritesString) {
  Type<std::string>::write_json_value(value, doc.GetAllocator(), "string");

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "string");
}

TEST_F(CoreJsonWriter, WritesStringView) {
  constexpr std::string_view str{"string_view"};

  Type<std::string_view>::write_json_value(value, doc.GetAllocator(), str);

  ASSERT_TRUE(value.IsString());
  ASSERT_EQ(value.GetString(), str.data());
  ASSERT_STREQ(value.GetString(), "string_view");
}

TEST_F(CoreJsonWriter, WritesSysUs) {
  using namespace std::chrono_literals;
  Type<core::sys_us>::write_json_value(
      value,
      doc.GetAllocator(),
      core::sys_us{core::sys_days{2025y / 4 / 21} + 14h + 47min + 59s +
                   270438us});
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "2025-04-21 14:47:59.270438");
}

TEST_F(CoreJsonWriter, WritesLocalDays) {
  using namespace std::chrono_literals;
  Type<core::local_days>::write_json_value(
      value, doc.GetAllocator(), core::local_days{2025y / 4 / 21});
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "2025-04-21");
}

TEST_F(CoreJsonWriter, WritesValueToJsonObjectWithKey) {
  value.SetObject();

  json::write_json_value(value, doc.GetAllocator(), 3.14, "key");

  ASSERT_THAT(value, HasDouble("key", 3.14));
}

TEST_F(CoreJsonWriter, WritesNullOptionalValueAsNullToJsonObjectWithKey) {
  value.SetObject();
  constexpr std::optional<double> opt_value{std::nullopt};

  json::write_json_value(value, doc.GetAllocator(), opt_value, "key");

  ASSERT_THAT(value, HasNull("key"));
}

TEST_F(CoreJsonWriter, WritesOptionalValueToJsonObjectWithKey) {
  value.SetObject();
  constexpr std::optional<double> opt_value{3.14};

  json::write_json_value(value, doc.GetAllocator(), opt_value, "key");

  ASSERT_THAT(value, HasDouble("key", 3.14));
}

struct CoreRapidJsonTypeConversion
    : public simulator::core::test::GivenExpectedFixture<rapidjson::Type,
                                                         std::string> {};

TEST_P(CoreRapidJsonTypeConversion, FmtFormatting) {
  ASSERT_EQ(fmt::to_string(given()), expected());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Conversion,
    CoreRapidJsonTypeConversion,
    Values(std::make_pair(rapidjson::Type::kNullType, "rapidjson::Type::kNullType"),
           std::make_pair(rapidjson::Type::kFalseType, "rapidjson::Type::kFalseType"),
           std::make_pair(rapidjson::Type::kTrueType, "rapidjson::Type::kTrueType"),
           std::make_pair(rapidjson::Type::kObjectType, "rapidjson::Type::kObjectType"),
           std::make_pair(rapidjson::Type::kArrayType, "rapidjson::Type::kArrayType"),
           std::make_pair(rapidjson::Type::kStringType, "rapidjson::Type::kStringType"),
           std::make_pair(rapidjson::Type::kNumberType, "rapidjson::Type::kNumberType")));
// clang-format on

}  // namespace
}  // namespace simulator::core::json::test
