#include <fmt/format.h>
#include <gtest/gtest.h>

#include <chrono>
#include <compare>
#include <optional>
#include <string>

#include "core/common/attribute.hpp"
#include "test_definitions/test_attributes.hpp"

namespace simulator::core::test {
namespace {

// NOLINTBEGIN(*-magic-numbers)

TEST(CoreIntAttibute, CreateAttribute) {
  constexpr IntAttribute attribute{42};

  EXPECT_EQ(attribute.value(), 42);
}

TEST(CoreIntAttibute, CastAttribute) {
  constexpr IntAttribute attribute{42};

  EXPECT_EQ(static_cast<std::int32_t>(attribute), 42);
}

TEST(CoreIntAttibute, CompareThreeWay) {
  constexpr IntAttribute less{41};
  constexpr IntAttribute base{42};
  constexpr IntAttribute greater{43};

  EXPECT_EQ(base <=> base, std::strong_ordering::equal);
  EXPECT_EQ(less <=> base, std::strong_ordering::less);
  EXPECT_EQ(greater <=> base, std::strong_ordering::greater);
}

TEST(CoreIntAttibute, FormatAttribute) {
  constexpr IntAttribute attribute{42};

  EXPECT_EQ(fmt::format("{}", attribute), "42");
}

TEST(CoreIntAttibute, FormatOptional) {
  constexpr std::optional<IntAttribute> attribute{42};

  EXPECT_EQ(fmt::format("{}", attribute), "42");
}

TEST(CoreIntAttibute, FormatNullopt) {
  constexpr std::optional<IntAttribute> attribute;

  EXPECT_EQ(fmt::format("{}", attribute), "none");
}

TEST(CoreIntAttibute, GetName) {
  constexpr std::optional<IntAttribute> attribute;

  EXPECT_EQ(name_of(attribute).singular, "TestIntAttribute");
  EXPECT_EQ(name_of(attribute).plural, "TestIntAttributes");
}

TEST(CoreDecimalAttibute, CreateAttribute) {
  constexpr DecimalAttribute attribute{42.2};

  EXPECT_DOUBLE_EQ(attribute.value(), 42.2);
}

TEST(CoreDecimalAttibute, CastAttribute) {
  constexpr DecimalAttribute attribute{42.2};

  EXPECT_DOUBLE_EQ(static_cast<double>(attribute), 42.2);
}

TEST(CoreDecimalAttibute, CompareThreeWay) {
  constexpr DecimalAttribute less{42.1};
  constexpr DecimalAttribute base{42.2};
  constexpr DecimalAttribute greater{42.3};

  EXPECT_EQ(base <=> base, std::partial_ordering::equivalent);
  EXPECT_EQ(base <=> less, std::partial_ordering::greater);
  EXPECT_EQ(base <=> greater, std::partial_ordering::less);
}

TEST(CoreDecimalAttibute, Format) {
  constexpr DecimalAttribute attribute{42.42};

  EXPECT_EQ(fmt::format("{}", attribute), "42.42");
}

TEST(CoreDecimalAttibute, FormatOptional) {
  constexpr std::optional<DecimalAttribute> attribute{42.42};

  EXPECT_EQ(fmt::format("{}", attribute), "42.42");
}

TEST(CoreDecimalAttibute, FormatNullopt) {
  constexpr std::optional<DecimalAttribute> attribute;

  EXPECT_EQ(fmt::format("{}", attribute), "none");
}

TEST(CoreDecimalAttibute, GetName) {
  constexpr std::optional<DecimalAttribute> attribute;

  EXPECT_EQ(name_of(attribute).singular, "TestDecimalAttribute");
  EXPECT_EQ(name_of(attribute).plural, "TestDecimalAttributes");
}

TEST(CoreEnumAttibute, CreateAttribute) {
  constexpr EnumAttribute attribute = EnumAttribute::Option::Option1;

  EXPECT_EQ(attribute.value(), EnumAttribute::Option::Option1);
}

TEST(CoreEnumAttibute, CastAttribute) {
  constexpr EnumAttribute attribute{EnumAttribute::Option::Option2};
  constexpr EnumAttribute::Option option = attribute;

  // Enum attribute could be implicitly converted to enumerator type
  EXPECT_EQ(option, EnumAttribute::Option::Option2);
}

TEST(CoreEnumAttibute, CompareEq) {
  constexpr EnumAttribute option1{EnumAttribute::Option::Option1};
  constexpr EnumAttribute option2{EnumAttribute::Option::Option2};
  constexpr EnumAttribute option3{EnumAttribute::Option::Option3};

  EXPECT_TRUE(option1 == option1);
  EXPECT_TRUE(option1 != option2);
  EXPECT_TRUE(option1 != option3);
}

TEST(CoreEnumAttibute, Format) {
  constexpr EnumAttribute attribute{EnumAttribute::Option::Option3};

  EXPECT_EQ(fmt::format("{}", attribute), "Option3");
}

TEST(CoreEnumAttibute, FormatOptional) {
  constexpr std::optional<EnumAttribute> attribute =
      EnumAttribute::Option::Option2;

  EXPECT_EQ(fmt::format("{}", attribute), "Option2");
}

TEST(CoreEnumAttibute, FormatNullopt) {
  constexpr std::optional<EnumAttribute> attribute;

  EXPECT_EQ(fmt::format("{}", attribute), "none");
}

TEST(CoreEnumAttibute, GetName) {
  constexpr std::optional<EnumAttribute> attribute;

  EXPECT_EQ(name_of(attribute).singular, "TestEnumAttribute");
  EXPECT_EQ(name_of(attribute).plural, "TestEnumAttributes");
}

TEST(CoreUtcTimestampAttibute, CreateAttribute) {
  using namespace std::chrono_literals;
  using sys_us = std::chrono::sys_time<std::chrono::microseconds>;

  constexpr UtcTimestampAttribute attribute{sys_us(100s)};

  EXPECT_EQ(attribute.value(), sys_us(100s));
}

TEST(CoreUtcTimestampAttibute, CastAttribute) {
  using namespace std::chrono_literals;
  using std::chrono::microseconds;
  using std::chrono::sys_time;
  using sys_us = std::chrono::sys_time<std::chrono::microseconds>;

  constexpr UtcTimestampAttribute attribute{sys_us(100s)};

  EXPECT_EQ(static_cast<sys_time<microseconds>>(attribute), sys_us(100s));
}

TEST(CoreUtcTimestampAttibute, CompareThreeWay) {
  using namespace std::chrono_literals;
  using sys_us = std::chrono::sys_time<std::chrono::microseconds>;

  constexpr UtcTimestampAttribute less{sys_us(99s)};
  constexpr UtcTimestampAttribute base{sys_us(100s)};
  constexpr UtcTimestampAttribute greater{sys_us(101s)};

  EXPECT_EQ(base <=> base, std::strong_ordering::equal);
  EXPECT_EQ(less <=> base, std::strong_ordering::less);
  EXPECT_EQ(greater <=> base, std::strong_ordering::greater);
}

TEST(CoreUtcTimestampAttibute, Format) {
  using namespace std::chrono_literals;
  using sys_us = std::chrono::sys_time<std::chrono::microseconds>;

  // UNIX epoch + 1 second + 1 millisecond + 1 microsecond
  constexpr UtcTimestampAttribute attribute{sys_us() + 1s + 1ms + 1us};

  EXPECT_EQ(fmt::format("{}", attribute), "1970-Jan-01 00:00:01.001001");
}

TEST(CoreUtcTimestampAttibute, FormatOptional) {
  using namespace std::chrono_literals;
  using sys_us = std::chrono::sys_time<std::chrono::microseconds>;

  // UNIX epoch + 1 second + 1 millisecond + 1 microsecond
  constexpr std::optional<UtcTimestampAttribute> attribute{sys_us() + 1s + 1ms +
                                                           1us};

  EXPECT_EQ(fmt::format("{}", attribute), "1970-Jan-01 00:00:01.001001");
}

TEST(CoreUtcTimestampAttibute, FormatNullopt) {
  constexpr std::optional<UtcTimestampAttribute> attribute;

  EXPECT_EQ(fmt::format("{}", attribute), "none");
}

TEST(CoreUtcTimestampAttibute, GetName) {
  constexpr std::optional<UtcTimestampAttribute> attribute;

  EXPECT_EQ(name_of(attribute).singular, "TestTimestampAttribute");
  EXPECT_EQ(name_of(attribute).plural, "TestTimestampAttributes");
}

TEST(CoreDateAttibute, CreateAttribute) {
  using std::chrono::days;
  using std::chrono::local_days;

  constexpr LocalDateAttribute attribute{local_days(days(42))};

  EXPECT_EQ(attribute.value(), local_days(days(42)));
}

TEST(CoreDateAttibute, CastAttribute) {
  using std::chrono::days;
  using std::chrono::local_days;

  constexpr LocalDateAttribute attribute{local_days(days(42))};

  EXPECT_EQ(static_cast<local_days>(attribute), local_days(days(42)));
}

TEST(CoreDateAttibute, CompareThreeWay) {
  using std::chrono::days;
  using std::chrono::local_days;

  constexpr LocalDateAttribute less{local_days(days(99))};
  constexpr LocalDateAttribute base{local_days(days(100))};
  constexpr LocalDateAttribute greater{local_days(days(101))};

  EXPECT_EQ(base <=> base, std::strong_ordering::equal);
  EXPECT_EQ(less <=> base, std::strong_ordering::less);
  EXPECT_EQ(greater <=> base, std::strong_ordering::greater);
}

TEST(CoreDateAttibute, Format) {
  using std::chrono::days;
  using std::chrono::January;
  using std::chrono::local_days;

  // UNIX epoch + 1 day
  constexpr LocalDateAttribute attribute{local_days{January / 2 / 1970}};

  EXPECT_EQ(fmt::format("{}", attribute), "1970-Jan-02");
}

TEST(CoreDateAttibute, FormatOptional) {
  using std::chrono::days;
  using std::chrono::January;
  using std::chrono::local_days;

  // UNIX epoch + 1 day
  constexpr std::optional<LocalDateAttribute> attribute{
      local_days{January / 2 / 1970}};

  EXPECT_EQ(fmt::format("{}", attribute), "1970-Jan-02");
}

TEST(CoreDateAttibute, FormatNullopt) {
  constexpr std::optional<LocalDateAttribute> attribute;

  EXPECT_EQ(fmt::format("{}", attribute), "none");
}

TEST(CoreDateAttibute, GetName) {
  constexpr std::optional<LocalDateAttribute> attribute;

  EXPECT_EQ(name_of(attribute).singular, "TestDateAttribute");
  EXPECT_EQ(name_of(attribute).plural, "TestDateAttributes");
}

TEST(CoreLiteralAttibute, CreateAttribute) {
  LiteralAttribute attribute{"ABC"};

  EXPECT_EQ(attribute.value(), "ABC");
  EXPECT_EQ(std::move(attribute).value(), "ABC");
}

TEST(CoreLiteralAttibute, CastAttribute) {
  const LiteralAttribute attribute{"ABC"};

  EXPECT_EQ(static_cast<std::string const&>(attribute), "ABC");
  EXPECT_EQ(static_cast<std::string>(attribute), "ABC");
}

TEST(CoreLiteralAttibute, CompareThreeWay) {
  const LiteralAttribute less{"AB"};
  const LiteralAttribute base{"ABC"};
  const LiteralAttribute greater{"ABCD"};

  EXPECT_EQ(base <=> base, std::strong_ordering::equal);
  EXPECT_EQ(base <=> less, std::strong_ordering::greater);
  EXPECT_EQ(base <=> greater, std::strong_ordering::less);
}

TEST(CoreLiteralAttibute, Format) {
  const LiteralAttribute attribute{"AttributeValue"};

  EXPECT_EQ(fmt::format("{}", attribute), R"("AttributeValue")");
}

TEST(CoreLiteralAttibute, FormatOptional) {
  const std::optional<LiteralAttribute> attribute{"AttributeValue"};

  EXPECT_EQ(fmt::format("{}", attribute), R"("AttributeValue")");
}

TEST(CoreLiteralAttibute, FormatNullopt) {
  constexpr std::optional<LiteralAttribute> attribute;

  EXPECT_EQ(fmt::format("{}", attribute), "none");
}

TEST(CoreLiteralAttibute, GetName) {
  constexpr std::optional<LiteralAttribute> attribute;

  EXPECT_EQ(name_of(attribute).singular, "TestLiteralAttribute");
  EXPECT_EQ(name_of(attribute).plural, "TestLiteralAttributes");
}

TEST(CoreDerivedIntAttribute, CreateFromValue) {
  constexpr DerivedIntAttribute attribute{42};

  EXPECT_EQ(attribute.value(), 42);
}

TEST(CoreDerivedIntAttribute, CreateFromPrimaryAttribute) {
  constexpr IntAttribute primary{42};
  constexpr DerivedIntAttribute derived{primary};

  EXPECT_EQ(derived.primary_attribute(), primary);
}

TEST(CoreDerivedIntAttribute, CastToValue) {
  constexpr DerivedIntAttribute attribute{42};

  EXPECT_EQ(static_cast<int>(attribute), 42);
}

TEST(CoreDerivedIntAttribute, CastToPrimaryAttribute) {
  constexpr IntAttribute primary{42};
  constexpr DerivedIntAttribute derived{primary};

  EXPECT_EQ(static_cast<IntAttribute>(derived), primary);
}

TEST(CoreDerivedIntAttribute, CompareEqWithPrimaryAttribute) {
  constexpr IntAttribute base{42};
  constexpr IntAttribute other{43};
  constexpr DerivedIntAttribute derived{base};

  EXPECT_EQ(derived, base);
  EXPECT_NE(derived, other);
}

TEST(CoreDerivedIntAttribute, CompareEq) {
  constexpr DerivedIntAttribute base{42};
  constexpr DerivedIntAttribute other{43};

  EXPECT_EQ(base, base);
  EXPECT_NE(base, other);
}

TEST(CoreDerivedIntAttribute, CompareThreeWayWithPrimaryAttribute) {
  constexpr IntAttribute less{41};
  constexpr IntAttribute base{42};
  constexpr IntAttribute greater{43};
  constexpr DerivedIntAttribute derived{base};

  EXPECT_EQ(derived <=> base, std::strong_ordering::equal);
  EXPECT_EQ(derived <=> less, std::strong_ordering::greater);
  EXPECT_EQ(derived <=> greater, std::strong_ordering::less);
}

TEST(CoreDerivedIntAttribute, CompareThreeWay) {
  constexpr DerivedIntAttribute less{41};
  constexpr DerivedIntAttribute base{42};
  constexpr DerivedIntAttribute greater{43};

  EXPECT_EQ(base <=> base, std::strong_ordering::equal);
  EXPECT_EQ(base <=> less, std::strong_ordering::greater);
  EXPECT_EQ(base <=> greater, std::strong_ordering::less);
}

TEST(CoreDerivedIntAttribute, Format) {
  constexpr DerivedIntAttribute attribute{42};

  EXPECT_EQ(fmt::format("{}", attribute), "42");
}

TEST(CoreDerivedIntAttribute, FormatOptional) {
  constexpr std::optional<DerivedIntAttribute> attribute{42};

  EXPECT_EQ(fmt::format("{}", attribute), "42");
}

TEST(CoreDerivedIntAttribute, FormatNullopt) {
  constexpr std::optional<DerivedIntAttribute> attribute;

  EXPECT_EQ(fmt::format("{}", attribute), "none");
}

TEST(CoreDerivedIntAttribute, GetName) {
  const std::optional<DerivedIntAttribute> attribute;

  EXPECT_EQ(name_of(attribute).singular, "TestDerivedIntAttribute");
  EXPECT_EQ(name_of(attribute).plural, "TestDerivedIntAttributes");
}

TEST(CoreDerivedEnumAttribute, CreateFromValue) {
  constexpr DerivedEnumAttribute attribute{
      DerivedEnumAttribute::Option::Option2};

  EXPECT_EQ(attribute.value(), AttributeEnumerator::Option2);
}

TEST(CoreDerivedEnumAttribute, CreateFromPrimaryAttribute) {
  constexpr EnumAttribute primary{EnumAttribute::Option::Option1};
  constexpr DerivedEnumAttribute derived{primary};

  EXPECT_EQ(derived.primary_attribute(), primary);
}

TEST(CoreDerivedEnumAttribute, CastToValue) {
  constexpr DerivedEnumAttribute attribute{
      DerivedEnumAttribute::Option::Option3};

  EXPECT_EQ(static_cast<AttributeEnumerator>(attribute),
            AttributeEnumerator::Option3);
}

TEST(CoreDerivedEnumAttribute, CastToPrimaryAttribute) {
  constexpr EnumAttribute primary{EnumAttribute::Option::Option1};
  constexpr DerivedEnumAttribute derived{primary};

  EXPECT_EQ(static_cast<EnumAttribute>(derived), primary);
}

TEST(CoreDerivedEnumAttribute, CompareEqWithPrimaryAttribute) {
  constexpr EnumAttribute base{EnumAttribute::Option::Option1};
  constexpr EnumAttribute other{EnumAttribute::Option::Option2};
  constexpr DerivedEnumAttribute derived{base};

  EXPECT_EQ(derived, base);
  EXPECT_NE(derived, other);
}

TEST(CoreDerivedEnumAttribute, CompareEq) {
  constexpr DerivedEnumAttribute base{DerivedEnumAttribute::Option::Option1};
  constexpr DerivedEnumAttribute other{DerivedEnumAttribute::Option::Option2};

  EXPECT_EQ(base, base);
  EXPECT_NE(base, other);
}

TEST(CoreDerivedEnumAttribute, Format) {
  constexpr DerivedEnumAttribute attribute{
      DerivedEnumAttribute::Option::Option1};

  EXPECT_EQ(fmt::format("{}", attribute), "Option1");
}

TEST(CoreDerivedEnumAttribute, FormatOptional) {
  constexpr std::optional<DerivedEnumAttribute> attribute{
      DerivedEnumAttribute::Option::Option1};

  EXPECT_EQ(fmt::format("{}", attribute), "Option1");
}

TEST(CoreDerivedEnumAttribute, FormatNullopt) {
  constexpr std::optional<DerivedEnumAttribute> attribute;

  EXPECT_EQ(fmt::format("{}", attribute), "none");
}

TEST(CoreDerivedEnumAttribute, GetName) {
  constexpr std::optional<DerivedEnumAttribute> attribute;

  EXPECT_EQ(name_of(attribute).singular, "TestDerivedEnumAttribute");
  EXPECT_EQ(name_of(attribute).plural, "TestDerivedEnumAttributes");
}

TEST(CoreDerivedLiteralAttribute, CreateFromValue) {
  DerivedLiteralAttribute attribute{"ABC"};

  EXPECT_EQ(attribute.value(), "ABC");
  EXPECT_EQ(std::move(attribute).value(), "ABC");
}

TEST(CoreDerivedLiteralAttribute, CreateFromPrimaryAttribute) {
  const LiteralAttribute primary{"ABC"};
  DerivedLiteralAttribute derived{primary};

  EXPECT_EQ(derived.primary_attribute(), primary);
  EXPECT_EQ(std::move(derived).primary_attribute(), primary);
}

TEST(CoreDerivedLiteralAttribute, CastToValue) {
  const DerivedLiteralAttribute attribute{"ABC"};

  EXPECT_EQ(static_cast<const std::string&>(attribute), "ABC");
}

TEST(CoreDerivedLiteralAttribute, CastToPrimaryAttribute) {
  const LiteralAttribute primary{"ABC"};
  const DerivedLiteralAttribute derived{primary};

  EXPECT_EQ(static_cast<const LiteralAttribute&>(derived), primary);
}

TEST(CoreDerivedLiteralAttribute, CompareEqWithPrimaryAttribute) {
  const LiteralAttribute base{"ABC"};
  const LiteralAttribute other{"ABCD"};
  const DerivedLiteralAttribute derived{base};

  EXPECT_EQ(derived, base);
  EXPECT_NE(derived, other);
}

TEST(CoreDerivedLiteralAttribute, CompareEq) {
  const DerivedLiteralAttribute base{"ABC"};
  const DerivedLiteralAttribute other{"ABCD"};

  EXPECT_EQ(base, base);
  EXPECT_NE(base, other);
}

TEST(CoreDerivedLiteralAttribute, CompareThreeWayWithPrimaryAttribute) {
  const LiteralAttribute less{"AB"};
  const LiteralAttribute base{"ABC"};
  const LiteralAttribute greater{"ABCD"};
  const DerivedLiteralAttribute derived{base};

  EXPECT_EQ(derived <=> base, std::strong_ordering::equal);
  EXPECT_EQ(derived <=> less, std::strong_ordering::greater);
  EXPECT_EQ(derived <=> greater, std::strong_ordering::less);
}

TEST(CoreDerivedLiteralAttribute, CompareThreeWay) {
  const DerivedLiteralAttribute less{"AB"};
  const DerivedLiteralAttribute base{"ABC"};
  const DerivedLiteralAttribute greater{"ABCD"};

  EXPECT_EQ(base <=> base, std::strong_ordering::equal);
  EXPECT_EQ(less <=> base, std::strong_ordering::less);
  EXPECT_EQ(greater <=> base, std::strong_ordering::greater);
}

TEST(CoreDerivedLiteralAttribute, Format) {
  const DerivedLiteralAttribute attribute{"ABC"};

  EXPECT_EQ(fmt::format("{}", attribute), R"("ABC")");
}

TEST(CoreDerivedLiteralAttribute, FormatOptional) {
  const std::optional<DerivedLiteralAttribute> attribute{"ABC"};

  EXPECT_EQ(fmt::format("{}", attribute), R"("ABC")");
}

TEST(CoreDerivedLiteralAttribute, FormatNullopt) {
  constexpr std::optional<DerivedLiteralAttribute> attribute;

  EXPECT_EQ(fmt::format("{}", attribute), "none");
}

TEST(CoreDerivedLiteralAttribute, GetName) {
  constexpr std::optional<DerivedLiteralAttribute> attribute;

  EXPECT_EQ(name_of(attribute).singular, "TestDerivedLiteralAttribute");
  EXPECT_EQ(name_of(attribute).plural, "TestDerivedLiteralAttributes");
}

// NOLINTEND(*-magic-numbers)

}  // namespace
}  // namespace simulator::core::test