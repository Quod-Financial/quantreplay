#include <gtest/gtest.h>

#include "core/common/json/enum_converter.hpp"
#include "test_utils/utils.hpp"

namespace simulator::core::json::test {
namespace {

enum class TestEnum : std::uint8_t { Option1, Option2, Option3 };

template <>
inline EnumConverter<simulator::core::json::test::TestEnum>
    EnumConverter<simulator::core::json::test::TestEnum>::instance_ = {
        {{simulator::core::json::test::TestEnum::Option1, "Option1"},
         {simulator::core::json::test::TestEnum::Option2, "Option2"},
         {simulator::core::json::test::TestEnum::Option3, "Option3"}}};

TEST(CoreJsonEnumConverter, ThrowsExceptionOnInvalidEnumValue) {
  constexpr auto value = core::test::invalid_enum_value<TestEnum>();
  ASSERT_THROW((void)json::EnumConverter<TestEnum>::str(value),
               std::runtime_error);
}

TEST(CoreJsonEnumConverter,
     ThrowsMeaningfulExceptionMessageOnInvalidEnumValue) {
  constexpr auto value = core::test::invalid_enum_value<TestEnum>();
  try {
    [[maybe_unused]] const auto result = EnumConverter<TestEnum>::str(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(), "unknown enumeration value `255'");
  }
}

TEST(CoreJsonEnumConverter, ConvertsEnumToString) {
  ASSERT_EQ(EnumConverter<TestEnum>::str(TestEnum::Option2), "Option2");
}

TEST(CoreJsonEnumConverter, ThrowsExceptionOnInvalidEnumString) {
  constexpr auto str = "InvalidEnumString";
  ASSERT_THROW((void)EnumConverter<TestEnum>::value(str), std::runtime_error);
}

TEST(CoreJsonEnumConverter,
     ThrowsMeaningfulExceptionMessageOnInvalidEnumString) {
  constexpr auto str = "InvalidEnumString";
  try {
    [[maybe_unused]] const auto result = EnumConverter<TestEnum>::value(str);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(), "unknown enumeration string `InvalidEnumString'");
  }
}

TEST(CoreJsonEnumConverter, ConvertsStringToEnum) {
  ASSERT_EQ(EnumConverter<TestEnum>::value("Option3"), TestEnum::Option3);
}

}  // namespace
}  // namespace simulator::core::json::test
