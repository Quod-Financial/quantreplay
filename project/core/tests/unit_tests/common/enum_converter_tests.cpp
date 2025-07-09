#include <gtest/gtest.h>

#include "core/common/enum_converter.hpp"

namespace simulator::core::test {
namespace {

enum class TestEnum : std::uint8_t { Option1, Option2, Option3 };

template <>
inline EnumConverter<simulator::core::test::TestEnum>
    EnumConverter<simulator::core::test::TestEnum>::instance_ = {
        {{simulator::core::test::TestEnum::Option1, "Option1"},
         {simulator::core::test::TestEnum::Option2, "Option2"},
         {simulator::core::test::TestEnum::Option3, "Option3"}}};

TEST(CoreEnumConverter, ThrowsExceptionOnInvalidEnumValue) {
  constexpr auto value = static_cast<TestEnum>(-1);
  ASSERT_THROW((void)EnumConverter<TestEnum>::str(value), std::runtime_error);
}

TEST(CoreEnumConverter, ThrowsMeaningfulExceptionMessageOnInvalidEnumValue) {
  constexpr auto value = static_cast<TestEnum>(-1);
  try {
    [[maybe_unused]] const auto result = EnumConverter<TestEnum>::str(value);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(), "unknown enumeration value `255`");
  }
}

TEST(CoreEnumConverter, ConvertsEnumToString) {
  ASSERT_EQ(EnumConverter<TestEnum>::str(TestEnum::Option2), "Option2");
}

TEST(CoreEnumConverter, ThrowsExceptionOnInvalidEnumString) {
  constexpr auto str = "InvalidEnumString";
  ASSERT_THROW((void)EnumConverter<TestEnum>::value(str), std::runtime_error);
}

TEST(CoreEnumConverter, ThrowsMeaningfulExceptionMessaggeOnInvalidEnumString) {
  constexpr auto str = "InvalidEnumString";
  try {
    [[maybe_unused]] const auto result = EnumConverter<TestEnum>::value(str);
    FAIL() << "std::runtime_error exception must be thrown";
  } catch (std::runtime_error& e) {
    ASSERT_STREQ(e.what(), "unknown enumeration string `InvalidEnumString`");
  }
}

TEST(CoreEnumConverter, ConvertsStringToEnum) {
  ASSERT_EQ(EnumConverter<TestEnum>::value("Option3"), TestEnum::Option3);
}

}  // namespace
}  // namespace simulator::core::test
