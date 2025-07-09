#include <gtest/gtest.h>

#include "core/common/json/type.hpp"
#include "core/tools/time.hpp"
#include "test_definitions/test_attributes.hpp"

namespace simulator::core::json::test {
namespace {

using IntAttribute = core::test::IntAttribute;
using DecimalAttribute = core::test::DecimalAttribute;
using EnumAttribute = core::test::EnumAttribute;
using UtcTimestampAttribute = core::test::UtcTimestampAttribute;
using LocalDateAttribute = core::test::LocalDateAttribute;
using LiteralAttribute = core::test::LiteralAttribute;
using DerivedIntAttribute = core::test::DerivedIntAttribute;
using DerivedEnumAttribute = core::test::DerivedEnumAttribute;
using DerivedLiteralAttribute = core::test::DerivedLiteralAttribute;

TEST(CoreJsonAttributeReader, ThrowsExceptionOnReadBoolAsIntAttribute) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<IntAttribute>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonAttributeReader, ReadsIntAttribute) {
  const rapidjson::Value value{42};

  ASSERT_EQ(Type<IntAttribute>::read_json_value(value), IntAttribute{42});
}

TEST(CoreJsonAttributeReader, ThrowsExceptionOnReadBoolAsDecimalAttribute) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<DecimalAttribute>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonAttributeReader, ReadsDecimalAttribute) {
  const rapidjson::Value value{42.2};

  ASSERT_EQ(Type<DecimalAttribute>::read_json_value(value),
            DecimalAttribute{42.2});
}

TEST(CoreJsonAttributeReader, ThrowsExceptionOnReadBoolAsEnumAttribute) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<EnumAttribute>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonAttributeReader, ReadsEnumAttribute) {
  const rapidjson::Value value{"Option2"};

  ASSERT_EQ(Type<EnumAttribute>::read_json_value(value),
            EnumAttribute::Option::Option2);
}

TEST(CoreJsonAttributeReader,
     ThrowsExceptionOnReadBoolAsUtcTimestampAttribute) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<UtcTimestampAttribute>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonAttributeReader, ReadsUtcTimestampAttribute) {
  using namespace std::chrono_literals;
  const rapidjson::Value value{"2025-04-21 14:47:59.270438"};

  ASSERT_EQ(Type<UtcTimestampAttribute>::read_json_value(value),
            UtcTimestampAttribute{core::sys_us{core::sys_days{2025y / 4 / 21} +
                                               14h + 47min + 59s + 270438us}});
}

TEST(CoreJsonAttributeReader, ThrowsExceptionOnReadBoolAsLocalDateAttribute) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<LocalDateAttribute>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonAttributeReader, ReadsLocalDateAttribute) {
  using namespace std::chrono_literals;
  const rapidjson::Value value{"2025-04-21"};

  ASSERT_EQ(Type<LocalDateAttribute>::read_json_value(value),
            LocalDateAttribute{core::local_days{2025y / 4 / 21}});
}

TEST(CoreJsonAttributeReader, ThrowsExceptionOnReadBoolAsLiteralAttribute) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<LiteralAttribute>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonAttributeReader, ReadsLiteralAttribute) {
  const rapidjson::Value value{"literal"};

  ASSERT_EQ(Type<LiteralAttribute>::read_json_value(value),
            LiteralAttribute{"literal"});
}

TEST(CoreJsonAttributeReader, ThrowsExceptionOnReadBoolAsDerivedIntAttribute) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<DerivedIntAttribute>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonAttributeReader, ReadsDerivedIntAttribute) {
  const rapidjson::Value value{42};

  ASSERT_EQ(Type<DerivedIntAttribute>::read_json_value(value),
            DerivedIntAttribute{IntAttribute{42}});
}

TEST(CoreJsonAttributeReader, ThrowsExceptionOnReadBoolAsDerivedEnumAttribute) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<DerivedEnumAttribute>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonAttributeReader, ReadsDerivedEnumAttribute) {
  const rapidjson::Value value{"Option2"};

  ASSERT_EQ(Type<DerivedEnumAttribute>::read_json_value(value),
            DerivedEnumAttribute{EnumAttribute::Option::Option2});
}

TEST(CoreJsonAttributeReader,
     ThrowsExceptionOnReadBoolAsDerivedLiteralAttribute) {
  const rapidjson::Value value{true};

  ASSERT_THROW((void)Type<DerivedLiteralAttribute>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonAttributeReader, ReadsDerivedLiteralAttribute) {
  const rapidjson::Value value{"literal"};

  ASSERT_EQ(Type<DerivedLiteralAttribute>::read_json_value(value),
            DerivedLiteralAttribute{LiteralAttribute{"literal"}});
}

struct CoreJsonAttributeWriter : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(CoreJsonAttributeWriter, WritesIntAttribute) {
  Type<IntAttribute>::write_json_value(
      value, doc.GetAllocator(), IntAttribute{42});

  ASSERT_TRUE(value.IsInt());
  ASSERT_EQ(value.GetInt(), 42);
}

TEST_F(CoreJsonAttributeWriter, WritesDecimalAttribute) {
  Type<DecimalAttribute>::write_json_value(
      value, doc.GetAllocator(), DecimalAttribute{42.2});

  ASSERT_TRUE(value.IsDouble());
  ASSERT_DOUBLE_EQ(value.GetDouble(), 42.2);
}

TEST_F(CoreJsonAttributeWriter, WritesEnumAttribute) {
  Type<EnumAttribute>::write_json_value(
      value, doc.GetAllocator(), EnumAttribute::Option::Option3);

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "Option3");
}

TEST_F(CoreJsonAttributeWriter, WritesUtcTimestampAttribute) {
  using namespace std::chrono_literals;
  Type<UtcTimestampAttribute>::write_json_value(
      value,
      doc.GetAllocator(),
      UtcTimestampAttribute{core::sys_us{core::sys_days{2025y / 4 / 21} + 14h +
                                         47min + 59s + 270438us}});

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "2025-04-21 14:47:59.270438");
}

TEST_F(CoreJsonAttributeWriter, WritesLocalDateAttribute) {
  using namespace std::chrono_literals;
  Type<LocalDateAttribute>::write_json_value(
      value,
      doc.GetAllocator(),
      LocalDateAttribute{core::local_days{2025y / 4 / 21}});

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "2025-04-21");
}

TEST_F(CoreJsonAttributeWriter, WritesLiteralAttribute) {
  Type<LiteralAttribute>::write_json_value(
      value, doc.GetAllocator(), LiteralAttribute{"literal"});

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "literal");
}

TEST_F(CoreJsonAttributeWriter, WritesDerivedIntAttribute) {
  Type<DerivedIntAttribute>::write_json_value(
      value, doc.GetAllocator(), DerivedIntAttribute{IntAttribute{42}});

  ASSERT_TRUE(value.IsInt());
  ASSERT_EQ(value.GetInt(), 42);
}

TEST_F(CoreJsonAttributeWriter, WritesDerivedEnumAttribute) {
  Type<DerivedEnumAttribute>::write_json_value(
      value,
      doc.GetAllocator(),
      DerivedEnumAttribute{EnumAttribute::Option::Option2});

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "Option2");
}

TEST_F(CoreJsonAttributeWriter, WritesDerivedLiteralAttribute) {
  Type<DerivedLiteralAttribute>::write_json_value(
      value, doc.GetAllocator(), DerivedLiteralAttribute{"literal"});

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "literal");
}

}  // namespace
}  // namespace simulator::core::json::test