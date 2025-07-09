#include <arpa/inet.h>
#include <gtest/gtest.h>

#include "core/common/json/field.hpp"
#include "core/common/json/type_struct.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::core::json::test {

struct InnerJsonTestStruct {
  double double_field;
};

struct JsonTestStruct {
  InnerJsonTestStruct inner_struct_field;
  int int_field;
  std::string string_field;
  std::optional<int> optional_int_field;
};

}  // namespace simulator::core::json::test

template <>
struct simulator::core::json::Struct<
    simulator::core::json::test::InnerJsonTestStruct> {
  static constexpr auto fields = std::make_tuple(
      Field(&simulator::core::json::test::InnerJsonTestStruct::double_field,
            "double_field"));
};

template <>
struct simulator::core::json::Struct<
    simulator::core::json::test::JsonTestStruct> {
  static constexpr auto fields = std::make_tuple(
      Field(&simulator::core::json::test::JsonTestStruct::inner_struct_field,
            "inner_struct_field"),
      Field(&simulator::core::json::test::JsonTestStruct::int_field,
            "int_field"),
      Field(&simulator::core::json::test::JsonTestStruct::string_field,
            "string_field"),
      Field(&simulator::core::json::test::JsonTestStruct::optional_int_field,
            "optional_int_field"));
};

namespace simulator::core::json::test {
namespace {

struct CoreJsonStructReader : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value inner_value{rapidjson::Type::kObjectType};
  rapidjson::Value value{rapidjson::Type::kObjectType};
};

TEST_F(CoreJsonStructReader, ThrowsExceptionIfJsonDoesNotContainAllFields) {
  value.AddMember("int_field", 42, doc.GetAllocator());

  ASSERT_THROW((void)Type<JsonTestStruct>::read_json_value(value),
               std::runtime_error);
}

TEST_F(
    CoreJsonStructReader,
    ThrowsMeaningfulExceptionMessageIfJsonContainsTheWrongTypeForInnerStructField) {
  inner_value.AddMember(
      "double_field", "the value must be double", doc.GetAllocator());

  value.AddMember("inner_struct_field", inner_value.Move(), doc.GetAllocator());
  value.AddMember("int_field", 42, doc.GetAllocator());
  value.AddMember("string_field", "string", doc.GetAllocator());

  try {
    const auto object = Type<JsonTestStruct>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (const std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "failed to parse field `inner_struct_field`: failed to parse "
                 "field `double_field`: unexpected data Type "
                 "`rapidjson::Type::kStringType`, `double` is expected");
  }
}

TEST_F(CoreJsonStructReader, ReadsStruct) {
  inner_value.AddMember("double_field", 3.14, doc.GetAllocator());

  value.AddMember("inner_struct_field", inner_value.Move(), doc.GetAllocator());
  value.AddMember("int_field", 42, doc.GetAllocator());
  value.AddMember("string_field", "string", doc.GetAllocator());

  const auto object = Type<JsonTestStruct>::read_json_value(value);

  ASSERT_DOUBLE_EQ(object.inner_struct_field.double_field, 3.14);
  ASSERT_EQ(object.int_field, 42);
  ASSERT_EQ(object.string_field, "string");
}

TEST_F(CoreJsonStructReader, AllowsAbsenceOptionalStructField) {
  inner_value.AddMember("double_field", 3.14, doc.GetAllocator());

  value.AddMember("inner_struct_field", inner_value.Move(), doc.GetAllocator());
  value.AddMember("int_field", 42, doc.GetAllocator());
  value.AddMember("string_field", "string", doc.GetAllocator());

  const auto object = Type<JsonTestStruct>::read_json_value(value);

  ASSERT_EQ(object.optional_int_field, std::nullopt);
}

TEST_F(CoreJsonStructReader,
       ThrowsExceptionOnReadingBoolAsOptionalIntStructField) {
  inner_value.AddMember("double_field", 3.14, doc.GetAllocator());

  value.AddMember("inner_struct_field", inner_value.Move(), doc.GetAllocator());
  value.AddMember("int_field", 42, doc.GetAllocator());
  value.AddMember("string_field", "string", doc.GetAllocator());
  value.AddMember("optional_int_field", true, doc.GetAllocator());

  ASSERT_THROW((void)Type<JsonTestStruct>::read_json_value(value),
               std::runtime_error);
}

TEST_F(CoreJsonStructReader,
       ThrowsMeaningfulExceptionMessageOnReadingBoolAsOptionalIntStructField) {
  inner_value.AddMember("double_field", 3.14, doc.GetAllocator());

  value.AddMember("inner_struct_field", inner_value.Move(), doc.GetAllocator());
  value.AddMember("int_field", 42, doc.GetAllocator());
  value.AddMember("string_field", "string", doc.GetAllocator());
  value.AddMember("optional_int_field", true, doc.GetAllocator());

  try {
    const auto object = Type<JsonTestStruct>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (const std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "failed to parse field `optional_int_field`: unexpected data "
                 "Type `rapidjson::Type::kTrueType`, `int` is expected");
  }
}

TEST_F(CoreJsonStructReader, ReadsNullAsOptionalStructField) {
  inner_value.AddMember("double_field", 3.14, doc.GetAllocator());

  value.AddMember("inner_struct_field", inner_value.Move(), doc.GetAllocator());
  value.AddMember("int_field", 42, doc.GetAllocator());
  value.AddMember("string_field", "string", doc.GetAllocator());
  value.AddMember(
      "optional_int_field", rapidjson::Value{}.Move(), doc.GetAllocator());

  const auto object = Type<JsonTestStruct>::read_json_value(value);

  ASSERT_EQ(object.optional_int_field, std::nullopt);
}

TEST_F(CoreJsonStructReader, ReadsOptionalStructField) {
  inner_value.AddMember("double_field", 3.14, doc.GetAllocator());

  value.AddMember("inner_struct_field", inner_value.Move(), doc.GetAllocator());
  value.AddMember("int_field", 42, doc.GetAllocator());
  value.AddMember("string_field", "string", doc.GetAllocator());
  value.AddMember("optional_int_field", 3, doc.GetAllocator());

  const auto object = Type<JsonTestStruct>::read_json_value(value);
  ASSERT_EQ(object.optional_int_field, 3);
}

struct CoreJsonStructWriter : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(CoreJsonStructWriter, WritesStructIntField) {
  Type<JsonTestStruct>::write_json_value(
      value,
      doc.GetAllocator(),
      JsonTestStruct{3.14, 42, "string", std::nullopt});

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasInt("int_field", 42));
}

TEST_F(CoreJsonStructWriter, WritesStructStringField) {
  Type<JsonTestStruct>::write_json_value(
      value,
      doc.GetAllocator(),
      JsonTestStruct{3.14, 42, "string", std::nullopt});

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasString("string_field", "string"));
}

TEST_F(CoreJsonStructWriter, WritesStructNullOptionalField) {
  Type<JsonTestStruct>::write_json_value(
      value,
      doc.GetAllocator(),
      JsonTestStruct{3.14, 42, "string", std::nullopt});

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasNull("optional_int_field"));
}

TEST_F(CoreJsonStructWriter, WritesStructNotNullOptionalField) {
  Type<JsonTestStruct>::write_json_value(
      value,
      doc.GetAllocator(),
      JsonTestStruct{3.14, 42, "string", std::make_optional(3)});

  ASSERT_TRUE(value.IsObject());
  ASSERT_THAT(value, HasInt("optional_int_field", 3));
}

}  // namespace
}  // namespace simulator::core::json::test
