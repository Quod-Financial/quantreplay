#ifndef SIMULATOR_CORE_TESTS_TEST_DEFINITION_TEST_ATTRIBUTES_HPP_
#define SIMULATOR_CORE_TESTS_TEST_DEFINITION_TEST_ATTRIBUTES_HPP_

#include <fmt/format.h>

#include <cstdint>
#include <string_view>

#include "core/common/attribute.hpp"
#include "core/common/enum_converter.hpp"
#include "core/common/name.hpp"

namespace simulator::core::test {

enum class AttributeEnumerator : std::uint8_t { Option1, Option2, Option3 };

template <>
inline simulator::core::EnumConverter<
    simulator::core::test::AttributeEnumerator>
    simulator::core::EnumConverter<
        simulator::core::test::AttributeEnumerator>::instance_{{
        {simulator::core::test::AttributeEnumerator::Option1, "Option1"},
        {simulator::core::test::AttributeEnumerator::Option2, "Option2"},
        {simulator::core::test::AttributeEnumerator::Option3, "Option3"},
    }};

namespace tag {

struct IntTag {
  using value_type = std::int32_t;
  constexpr static Name name{.singular = "TestIntAttribute",
                             .plural = "TestIntAttributes"};
};

struct DecimalTag {
  using value_type = double;
  constexpr static Name name{.singular = "TestDecimalAttribute",
                             .plural = "TestDecimalAttributes"};
};

struct EnumTag {
  using value_type = AttributeEnumerator;
  constexpr static Name name{.singular = "TestEnumAttribute",
                             .plural = "TestEnumAttributes"};
};

struct UtcTimestampTag {
  constexpr static Name name{.singular = "TestTimestampAttribute",
                             .plural = "TestTimestampAttributes"};
};

struct LocalDateTag {
  constexpr static Name name{.singular = "TestDateAttribute",
                             .plural = "TestDateAttributes"};
};

struct LiteralTag {
  constexpr static Name name{.singular = "TestLiteralAttribute",
                             .plural = "TestLiteralAttributes"};
};

}  // namespace tag

// clang-format off
using IntAttribute = core::attribute::Arithmetic<test::tag::IntTag>;
using DecimalAttribute = core::attribute::Arithmetic<test::tag::DecimalTag>;
using EnumAttribute = core::attribute::Enumerable<test::tag::EnumTag>;
using UtcTimestampAttribute = core::attribute::UtcTimestamp<test::tag::UtcTimestampTag>;
using LocalDateAttribute = core::attribute::LocalDate<test::tag::LocalDateTag>;
using LiteralAttribute = core::attribute::Literal<test::tag::LiteralTag>;
// clang-format on

namespace tag {

struct DerivedIntTag {
  using primary_type = test::IntAttribute;
  constexpr static Name name{.singular = "TestDerivedIntAttribute",
                             .plural = "TestDerivedIntAttributes"};
};

struct DerivedEnumTag {
  using primary_type = test::EnumAttribute;
  constexpr static Name name{.singular = "TestDerivedEnumAttribute",
                             .plural = "TestDerivedEnumAttributes"};
};

struct DerivedLiteralTag {
  using primary_type = test::LiteralAttribute;
  constexpr static Name name{.singular = "TestDerivedLiteralAttribute",
                             .plural = "TestDerivedLiteralAttributes"};
};

}  // namespace tag

using DerivedIntAttribute = core::attribute::Derived<test::tag::DerivedIntTag>;

using DerivedEnumAttribute =
    core::attribute::Derived<test::tag::DerivedEnumTag>;

using DerivedLiteralAttribute =
    core::attribute::Derived<test::tag::DerivedLiteralTag>;

}  // namespace simulator::core::test

template <>
struct fmt::formatter<simulator::core::test::AttributeEnumerator>
    : formatter<std::string_view> {
  using formattable = simulator::core::test::AttributeEnumerator;

  auto format(formattable enumerator, format_context& ctx) const {
    using base_formatter = formatter<std::string_view>;
    try {
      return base_formatter::format(
          simulator::core::EnumConverter<formattable>::str(enumerator), ctx);
    } catch (const std::runtime_error& ex) {
      return base_formatter::format("undefined", ctx);
    }
  }
};

#endif  // SIMULATOR_CORE_TESTS_TEST_DEFINITION_TEST_ATTRIBUTES_HPP_
