#include <gtest/gtest.h>

#include "core/common/enum_converter.hpp"
#include "core/common/json/type_enum.hpp"

namespace simulator::core::json::test {
namespace {

enum class JsonTestEnum : std::uint8_t { Option1, Option2 };

template <>
inline simulator::core::EnumConverter<simulator::core::json::test::JsonTestEnum>
    simulator::core::EnumConverter<
        simulator::core::json::test::JsonTestEnum>::instance_{{
        {simulator::core::json::test::JsonTestEnum::Option1, "Option1"},
        {simulator::core::json::test::JsonTestEnum::Option2, "Option2"},
    }};

TEST(CoreJsonEnumReader, ThrowsExceptionOnReadBoolAsEnum) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<JsonTestEnum>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonEnumReader, ThrowsExceptionOnReadInvalidEnum) {
  const rapidjson::Value value{"InvalidEnum"};

  ASSERT_THROW((void)Type<JsonTestEnum>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonEnumReader, ReadsEnum) {
  const rapidjson::Value value{"Option2"};

  ASSERT_EQ(Type<JsonTestEnum>::read_json_value(value), JsonTestEnum::Option2);
}

struct CoreJsonEnumWriter : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(CoreJsonEnumWriter, ThrowsExceptionOnWritingInvalidEnum) {
  ASSERT_THROW(Type<JsonTestEnum>::write_json_value(
                   value, doc.GetAllocator(), static_cast<JsonTestEnum>(-1)),
               std::runtime_error);
}

TEST_F(CoreJsonEnumWriter, WritesEnum) {
  Type<JsonTestEnum>::write_json_value(
      value, doc.GetAllocator(), JsonTestEnum::Option2);
  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "Option2");
}

}  // namespace
}  // namespace simulator::core::json::test