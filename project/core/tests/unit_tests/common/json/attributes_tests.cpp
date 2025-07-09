#include <gtest/gtest.h>

#include "core/common/json/type.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"

namespace simulator::core::json::test {
namespace {

TEST(CoreJsonAttributesReader, ReadsArithmeticAttribute) {
  const rapidjson::Value value{42};

  ASSERT_EQ(Type<simulator::SeqNum>::read_json_value(value),
            simulator::SeqNum{42});
}

TEST(CoreJsonAttributesReader, ReadsEnumerableAttribute) {
  const rapidjson::Value value{"OrderCancelled"};

  ASSERT_EQ(Type<simulator::ExecutionType>::read_json_value(value),
            simulator::ExecutionType::Option::OrderCancelled);
}

TEST(CoreJsonAttributesReader, ReadsLocalDateAttribute) {
  using namespace std::chrono_literals;
  const rapidjson::Value value{"2025-04-21"};

  ASSERT_EQ(Type<simulator::ExpireDate>::read_json_value(value).value(),
            core::local_days{2025y / 4 / 21});
}

TEST(CoreJsonAttributesReader, ReadsUtcTimestampAttribute) {
  using namespace std::chrono_literals;
  const rapidjson::Value value{"2025-04-21 14:47:59.270438"};

  ASSERT_EQ(Type<simulator::ExpireTime>::read_json_value(value).value(),
            core::sys_us{core::sys_days{2025y / 4 / 21} + 14h + 47min + 59s +
                         270438us});
}

TEST(CoreJsonAttributesReader, ReadsLiteralAttribute) {
  const rapidjson::Value value{"literal"};

  ASSERT_EQ(
      Type<simulator::BusinessRejectRefId>::read_json_value(value).value(),
      "literal");
}

TEST(CoreJsonAttributesReader, ReadsDerivedArithmeticAttribute) {
  const rapidjson::Value value{3.14};

  ASSERT_DOUBLE_EQ(Type<simulator::OrderPrice>::read_json_value(value).value(),
                   3.14);
}

TEST(CoreJsonAttributesReader, ReadsDerivedEnumerableAttribute) {
  const rapidjson::Value value{"Buy"};

  ASSERT_EQ(Type<simulator::AggressorSide>::read_json_value(value).value(),
            simulator::AggressorSide::Option::Buy);
}

TEST(CoreJsonAttributesReader, ReadsDerivedLiteralAttribute) {
  const rapidjson::Value value{"literal"};

  ASSERT_EQ(Type<simulator::BaseCurrency>::read_json_value(value).value(),
            "literal");
}

struct CoreJsonAttributesWriter : ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(CoreJsonAttributesWriter, WritesArithmeticAttribute) {
  Type<simulator::SeqNum>::write_json_value(
      value, doc.GetAllocator(), simulator::SeqNum{42});

  ASSERT_TRUE(value.IsInt());
  ASSERT_EQ(value.GetInt(), 42);
}

TEST_F(CoreJsonAttributesWriter, WritesEnumerableAttribute) {
  Type<simulator::ExecutionType>::write_json_value(
      value,
      doc.GetAllocator(),
      simulator::ExecutionType::Option::OrderCancelled);

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "OrderCancelled");
}

TEST_F(CoreJsonAttributesWriter, WritesLocalDateAttribute) {
  using namespace std::chrono_literals;
  Type<simulator::ExpireDate>::write_json_value(
      value,
      doc.GetAllocator(),
      simulator::ExpireDate{core::local_days{2025y / 4 / 21}});

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "2025-04-21");
}

TEST_F(CoreJsonAttributesWriter, WritesUtcTimestampAttribute) {
  using namespace std::chrono_literals;
  Type<simulator::ExpireTime>::write_json_value(
      value,
      doc.GetAllocator(),
      simulator::ExpireTime{core::sys_us{core::sys_days{2025y / 4 / 21} + 14h +
                                         47min + 59s + 270438us}});

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "2025-04-21 14:47:59.270438");
}

TEST_F(CoreJsonAttributesWriter, WritesLiteralAttribute) {
  Type<simulator::BusinessRejectRefId>::write_json_value(
      value, doc.GetAllocator(), simulator::BusinessRejectRefId{"literal"});

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "literal");
}

TEST_F(CoreJsonAttributesWriter, WritesDerivedArithmeticAttribute) {
  Type<simulator::OrderPrice>::write_json_value(
      value, doc.GetAllocator(), simulator::OrderPrice{3.14});

  ASSERT_TRUE(value.IsDouble());
  ASSERT_DOUBLE_EQ(value.GetDouble(), 3.14);
}

TEST_F(CoreJsonAttributesWriter, WritesDerivedEnumerableAttribute) {
  Type<simulator::AggressorSide>::write_json_value(
      value, doc.GetAllocator(), simulator::AggressorSide::Option::Buy);

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "Buy");
}

TEST_F(CoreJsonAttributesWriter, WritesDerivedLiteralAttribute) {
  Type<simulator::BaseCurrency>::write_json_value(
      value, doc.GetAllocator(), simulator::BaseCurrency{"literal"});

  ASSERT_TRUE(value.IsString());
  ASSERT_STREQ(value.GetString(), "literal");
}

}  // namespace
}  // namespace simulator::core::json::test
