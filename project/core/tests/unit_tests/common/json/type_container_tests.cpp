#include <gmock/gmock.h>

#include "core/common/json/type_container.hpp"

namespace simulator::core::json::test {
namespace {

using namespace ::testing;  // NOLINT

TEST(CoreJsonContainerReader, ThrowsExceptionOnReadJsonObjectAsVector) {
  const rapidjson::Value value{rapidjson::Type::kObjectType};

  ASSERT_THROW((void)Type<std::vector<double>>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonContainerReader, ReadsEmptyJsonArray) {
  const rapidjson::Value value{rapidjson::Type::kArrayType};

  ASSERT_TRUE(Type<std::vector<double>>::read_json_value(value).empty());
}

TEST(CoreJsonContainerReader, ThrowsExceptionOnReadIntAsDoubleVector) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kArrayType};
  value.PushBack(3.14, doc.GetAllocator());
  value.PushBack(42, doc.GetAllocator());

  ASSERT_THROW((void)Type<std::vector<double>>::read_json_value(value),
               std::runtime_error);
}

TEST(CoreJsonContainerReader,
     ThrowsMeaningfulExceptionMessageOnReadIntAsDoubleVector) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kArrayType};
  value.PushBack(3.14, doc.GetAllocator());
  value.PushBack(42, doc.GetAllocator());

  try {
    [[maybe_unused]] const auto result =
        Type<std::vector<double>>::read_json_value(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(),
                 "failed to parse JSON array item #1: unexpected data Type "
                 "`rapidjson::Type::kNumberType`, `double` is expected");
  }
}

TEST(CoreJsonContainerReader, ReadsJsonArrayAsVector) {
  rapidjson::Document doc;
  rapidjson::Value value{rapidjson::Type::kArrayType};
  value.PushBack(3.14, doc.GetAllocator());
  value.PushBack(2.74, doc.GetAllocator());

  const auto container = Type<std::vector<double>>::read_json_value(value);
  ASSERT_THAT(container, ElementsAre(3.14, 2.74));
}

TEST(CoreJsonContainerWriter, WritesEmptyJsonArray) {
  rapidjson::Document doc;
  rapidjson::Value value;

  Type<std::vector<double>>::write_json_value(value, doc.GetAllocator(), {});

  ASSERT_TRUE(value.IsArray());
  ASSERT_TRUE(value.Empty());
}

TEST(CoreJsonContainerWriter, WritesJsonArrayAsVector) {
  rapidjson::Document doc;
  rapidjson::Value value;

  Type<std::vector<double>>::write_json_value(
      value, doc.GetAllocator(), std::vector<double>{2.74, 3.14});

  ASSERT_TRUE(value.IsArray());
  ASSERT_TRUE(value[0].IsDouble());
  ASSERT_DOUBLE_EQ(value[0].GetDouble(), 2.74);
  ASSERT_TRUE(value[1].IsDouble());
  ASSERT_DOUBLE_EQ(value[1].GetDouble(), 3.14);
}

}  // namespace
}  // namespace simulator::core::json::test
