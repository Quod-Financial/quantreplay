#ifndef SIMULATOR_CORE_COMMON_ATTRIBUTE_HPP_
#define SIMULATOR_CORE_COMMON_ATTRIBUTE_HPP_

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <chrono>
#include <cmath>
#include <string>

#include "core/common/enum_converter.hpp"
#include "core/common/json/type.hpp"
#include "core/common/meta.hpp"
#include "core/common/name.hpp"
#include "core/common/std_formatter.hpp"
#include "core/tools/time.hpp"

namespace simulator::core::attribute {

template <typename T>
concept TypedAttributeTag = requires { typename T::value_type; };

template <typename T>
concept NamedAttributeTag = requires {
  { T::name } -> std::same_as<const Name&>;
};

template <typename Tag>
  requires TypedAttributeTag<Tag>
struct Arithmetic;

template <typename Tag>
  requires TypedAttributeTag<Tag>
struct Enumerable;

template <typename Tag>
struct UtcTimestamp;

template <typename Tag>
struct LocalDate;

template <typename Tag>
struct Literal;

template <typename T>
struct is_arithmetic : std::false_type {};

template <typename Tag>
struct is_arithmetic<Arithmetic<Tag>> : std::true_type {};

template <typename T>
struct is_enumerable : std::false_type {};

template <typename Tag>
struct is_enumerable<Enumerable<Tag>> : std::true_type {};

template <typename T>
struct is_utc_timestamp : std::false_type {};

template <typename Tag>
struct is_utc_timestamp<UtcTimestamp<Tag>> : std::true_type {};

template <typename T>
struct is_local_date : std::false_type {};

template <typename Tag>
struct is_local_date<LocalDate<Tag>> : std::true_type {};

template <typename T>
struct is_literal : std::false_type {};

template <typename Tag>
struct is_literal<Literal<Tag>> : std::true_type {};

template <typename T>
concept DerivedAttributeTag =
    core::HasNameDescriptor<T> && requires { typename T::primary_type; } &&
    (attribute::is_arithmetic<typename T::primary_type>::value ||
     attribute::is_enumerable<typename T::primary_type>::value ||
     attribute::is_utc_timestamp<typename T::primary_type>::value ||
     attribute::is_local_date<typename T::primary_type>::value ||
     attribute::is_literal<typename T::primary_type>::value);

template <typename Tag, typename = void>
  requires DerivedAttributeTag<Tag>
struct Derived;

template <typename T>
struct is_derived : std::false_type {};

template <typename Tag>
struct is_derived<Derived<Tag>> : std::true_type {};

template <typename T>
concept RepresentsAttribute =
    is_arithmetic<T>::value || is_enumerable<T>::value ||
    is_utc_timestamp<T>::value || is_local_date<T>::value ||
    is_literal<T>::value || is_derived<T>::value;

template <typename Tag>
  requires TypedAttributeTag<Tag>
struct Arithmetic {
  using tag_type = Tag;
  using value_type = typename tag_type::value_type;

  constexpr explicit Arithmetic(value_type value) noexcept : value_(value) {}

  [[nodiscard]]
  constexpr auto value() const noexcept -> value_type {
    return value_;
  }

  [[nodiscard]]
  constexpr explicit operator value_type() const noexcept {
    return value();
  }

  constexpr auto operator<=>(const Arithmetic& other) const = default;

  [[nodiscard]]
  consteval static auto name() noexcept -> Name
    requires NamedAttributeTag<Tag>
  {
    return tag_type::name;
  }

 private:
  value_type value_;
};

template <typename Tag>
  requires TypedAttributeTag<Tag> &&
           std::floating_point<typename Tag::value_type>
struct Arithmetic<Tag> {
  using tag_type = Tag;
  using value_type = typename tag_type::value_type;

  static_assert(
      !std::same_as<value_type, float>,
      "32-bit floating point (float) arithmetic attributes are not supported");

  constexpr explicit Arithmetic(value_type value) noexcept
      : value_(normalize(value)) {}

  [[nodiscard]]
  constexpr auto value() const noexcept -> value_type {
    return value_;
  }

  [[nodiscard]]
  constexpr explicit operator value_type() const noexcept {
    return value();
  }

  [[nodiscard]]
  constexpr auto operator<=>(const Arithmetic& other) const = default;

  [[nodiscard]]
  consteval static auto name() noexcept -> Name
    requires NamedAttributeTag<Tag>
  {
    return tag_type::name;
  }

 private:
  constexpr auto normalize(value_type value) -> value_type {
    constexpr double tolerance = 1e-10;

    const auto integer = static_cast<double>(static_cast<std::int64_t>(value));
    const auto frac = std::round((value - integer) / tolerance) * tolerance;
    return integer + frac;
  }

  value_type value_;
};

template <typename Tag>
  requires TypedAttributeTag<Tag>
struct Enumerable {
  using tag_type = Tag;
  using value_type = typename tag_type::value_type;

  static_assert(
      core::Enumerable<value_type>,
      "attribute::Enumerable template can not be instantiated with a tag type, "
      "that defines a value_type, which does not confirm core::Enumerable");

  using Option = value_type;

  // Allow implicit Enumerable creation from an underlying enum type
  // NOLINTNEXTLINE(*explicit*)
  constexpr Enumerable(value_type value) noexcept : value_(value) {}

  [[nodiscard]]
  constexpr auto value() const noexcept -> value_type {
    return value_;
  }

  // Allow implicit Enumerable conversion to an underlying enum type
  [[nodiscard]]
  constexpr operator value_type() const noexcept {  // NOLINT(*explicit*)
    return value_;
  }

  [[nodiscard]]
  consteval static auto name() noexcept -> Name
    requires NamedAttributeTag<Tag>
  {
    return tag_type::name;
  }

 private:
  value_type value_;
};

template <typename Tag>
struct UtcTimestamp {
  using tag_type = Tag;
  using value_type = std::chrono::sys_time<std::chrono::microseconds>;

  template <typename Duration>
  constexpr explicit UtcTimestamp(std::chrono::sys_time<Duration> time_point)
      : value_(std::chrono::floor<std::chrono::microseconds>(time_point)) {}

  [[nodiscard]]
  constexpr auto value() const noexcept -> value_type {
    return value_;
  }

  [[nodiscard]]
  constexpr explicit operator value_type() const noexcept {
    return value();
  }

  [[nodiscard]]
  constexpr auto operator<=>(const UtcTimestamp& other) const = default;

  [[nodiscard]]
  consteval static auto name() noexcept -> Name
    requires NamedAttributeTag<Tag>
  {
    return tag_type::name;
  }

 private:
  value_type value_;
};

template <typename Tag>
struct LocalDate {
  using tag_type = Tag;
  using value_type = std::chrono::local_days;

  constexpr explicit LocalDate(std::chrono::local_days value) noexcept
      : value_(value) {}

  constexpr explicit LocalDate(std::chrono::year_month_day ymd) noexcept
      : value_(ymd) {}

  [[nodiscard]]
  constexpr auto value() const noexcept -> value_type {
    return value_;
  }

  [[nodiscard]]
  constexpr explicit operator value_type() const noexcept {
    return value();
  }

  [[nodiscard]]
  constexpr auto operator<=>(const LocalDate& other) const = default;

  [[nodiscard]]
  constexpr static auto name() noexcept -> Name
    requires NamedAttributeTag<Tag>
  {
    return tag_type::name;
  }

 private:
  value_type value_;
};

template <typename Tag>
struct Literal {
  using tag_type = Tag;
  using value_type = std::string;

  explicit Literal(value_type value) noexcept : value_(std::move(value)) {}

  [[nodiscard]]
  auto value() const& noexcept -> value_type const& {
    return value_;
  }

  [[nodiscard]]
  auto value() && noexcept -> value_type {
    return std::move(value_);
  }

  [[nodiscard]]
  explicit operator value_type const&() const& noexcept {
    return value();
  }

  [[nodiscard]]
  auto operator<=>(const Literal& other) const = default;

  [[nodiscard]]
  constexpr static auto name() noexcept -> Name
    requires NamedAttributeTag<Tag>
  {
    return tag_type::name;
  }

 private:
  value_type value_;
};

// Derived attribute template for Arithmetic/Timestamp/Date primary attributes
template <typename Tag>
  requires DerivedAttributeTag<Tag>
struct Derived<
    Tag,
    std::enable_if_t<is_arithmetic<typename Tag::primary_type>::value ||
                     is_utc_timestamp<typename Tag::primary_type>::value ||
                     is_local_date<typename Tag::primary_type>::value>> {
  using tag_type = Tag;
  using primary_type = typename Tag::primary_type;
  using value_type = typename primary_type::value_type;

  constexpr explicit Derived(primary_type primary_attribute) noexcept
      : primary_attribute_(primary_attribute) {}

  constexpr explicit Derived(value_type value) noexcept
      : Derived(primary_type{value}) {}

  [[nodiscard]] constexpr auto value() const noexcept -> value_type {
    return primary_attribute_.value();
  }

  [[nodiscard]] constexpr auto primary_attribute() const noexcept
      -> primary_type {
    return primary_attribute_;
  }

  // Explicit conversion operators to a primary attribute type
  constexpr explicit operator primary_type() const noexcept {
    return primary_attribute_;
  }

  // Explicit conversion operators to an underlying value type
  constexpr explicit operator value_type() const noexcept {
    return static_cast<value_type>(primary_attribute_);
  }

  // Comparison with other primary attribute type & derivative attribute
  [[nodiscard]] constexpr auto operator<=>(
      const primary_type& primary_attribute) const noexcept {
    return this->primary_attribute_ <=> primary_attribute;
  }

  [[nodiscard]] constexpr auto operator==(
      const primary_type& primary_attribute) const noexcept {
    return this->primary_attribute_ == primary_attribute;
  }

  [[nodiscard]] constexpr auto operator<=>(
      const Derived& derived_attribute) const noexcept {
    return this->primary_attribute_ <=> derived_attribute.primary_attribute_;
  }

  [[nodiscard]] constexpr auto operator==(
      const Derived& derived_attribute) const noexcept {
    return this->primary_attribute_ == derived_attribute.primary_attribute_;
  }

  [[nodiscard]] constexpr static auto name() noexcept -> Name {
    return tag_type::name;
  }

 private:
  primary_type primary_attribute_;
};

// Derived attribute template for Enumerable primary attributes
template <typename Tag>
  requires DerivedAttributeTag<Tag>
struct Derived<
    Tag,
    std::enable_if_t<is_enumerable<typename Tag::primary_type>::value>> {
  using tag_type = Tag;
  using primary_type = typename tag_type::primary_type;
  using value_type = typename primary_type::value_type;

  using Option = value_type;

  constexpr explicit Derived(primary_type primary_attribute) noexcept
      : primary_attribute_(primary_attribute) {}

  // Allow implicit Derived attribute construction from an underlying enum value
  // NOLINTNEXTLINE(*explicit*)
  constexpr Derived(value_type enum_value) noexcept
      : Derived(primary_type{enum_value}) {}

  [[nodiscard]] constexpr auto value() const noexcept -> value_type {
    return primary_attribute_.value();
  }

  [[nodiscard]] constexpr auto primary_attribute() const noexcept
      -> primary_type {
    return primary_attribute_;
  }

  // Allow implicit enumerable DerivedAttribute conversion
  // to an underlying enum type
  // NOLINTNEXTLINE(*explicit*)
  constexpr operator value_type() const noexcept {
    return static_cast<value_type>(primary_attribute_);
  }

  constexpr explicit operator primary_type() const noexcept {
    return primary_attribute_;
  }

  constexpr auto operator==(const primary_type& other) const noexcept -> bool {
    return this->primary_attribute_ == other;
  }

  constexpr auto operator==(const value_type& other) const noexcept -> bool {
    return this->primary_attribute_ == other;
  }

  [[nodiscard]] constexpr static auto name() noexcept -> Name {
    return tag_type::name;
  }

 private:
  primary_type primary_attribute_;
};

// Partial specialization for derived literal attribute template
template <typename Tag>
  requires DerivedAttributeTag<Tag>
struct Derived<
    Tag,
    std::enable_if_t<is_literal<typename Tag::primary_type>::value>> {
  using tag_type = Tag;
  using primary_type = typename Tag::primary_type;
  using value_type = typename primary_type::value_type;

  explicit Derived(primary_type primary_attribute) noexcept
      : primary_attribute_(std::move(primary_attribute)) {}

  explicit Derived(value_type value) noexcept
      : Derived(primary_type{std::move(value)}) {}

  [[nodiscard]] auto value() const& noexcept -> value_type const& {
    return primary_attribute_.value();
  }

  [[nodiscard]] auto value() && noexcept -> value_type {
    return std::move(primary_attribute_).value();
  }

  [[nodiscard]] auto primary_attribute() const& noexcept
      -> const primary_type& {
    return primary_attribute_;
  }

  [[nodiscard]] auto primary_attribute() && noexcept -> primary_type {
    return std::move(primary_attribute_);
  }

  // Explicit conversion operators to a primary attribute type
  explicit operator const primary_type&() const noexcept {
    return primary_attribute_;
  }

  // Explicit conversion operators to an underlying value type
  explicit operator const value_type&() const& noexcept {
    return static_cast<const value_type&>(primary_attribute_);
  }

  // Comparison with other primary attribute type & derivative attribute
  [[nodiscard]] auto operator<=>(
      const primary_type& primary_attribute) const noexcept {
    return this->primary_attribute_ <=> primary_attribute;
  }

  [[nodiscard]] auto operator==(
      const primary_type& primary_attribute) const noexcept {
    return this->primary_attribute_ == primary_attribute;
  }

  [[nodiscard]] auto operator<=>(
      const Derived& derived_attribute) const noexcept {
    return this->primary_attribute_ <=> derived_attribute.primary_attribute_;
  }

  [[nodiscard]] auto operator==(
      const Derived& derived_attribute) const noexcept {
    return this->primary_attribute_ == derived_attribute.primary_attribute_;
  }

  [[nodiscard]] constexpr static auto name() noexcept -> Name {
    return tag_type::name;
  }

 private:
  primary_type primary_attribute_;
};

}  // namespace simulator::core::attribute

namespace simulator::core::json {

template <attribute::TypedAttributeTag Tag>
struct Type<attribute::Arithmetic<Tag>> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> attribute::Arithmetic<Tag> {
    return attribute::Arithmetic<Tag>{
        Type<typename Tag::value_type>::read_json_value(json_value)};
  }

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const attribute::Arithmetic<Tag>& value)
      -> void {
    Type<typename Tag::value_type>::write_json_value(
        json_value, allocator, value.value());
  }
};

template <attribute::TypedAttributeTag Tag>
struct Type<attribute::Enumerable<Tag>> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> attribute::Enumerable<Tag> {
    return attribute::Enumerable<Tag>{
        EnumConverter<typename Tag::value_type>::value(
            Type<std::string_view>::read_json_value(json_value))};
  }

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const attribute::Enumerable<Tag>& value)
      -> void {
    Type<std::string_view>::write_json_value(
        json_value,
        allocator,
        EnumConverter<typename Tag::value_type>::str(value.value()));
  }
};

template <typename Tag>
struct Type<attribute::UtcTimestamp<Tag>> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> attribute::UtcTimestamp<Tag> {
    return attribute::UtcTimestamp<Tag>{
        Type<typename attribute::UtcTimestamp<Tag>::value_type>::
            read_json_value(json_value)};
  }

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const attribute::UtcTimestamp<Tag>& value)
      -> void {
    Type<typename attribute::UtcTimestamp<Tag>::value_type>::write_json_value(
        json_value, allocator, value.value());
  }
};

template <typename Tag>
struct Type<attribute::LocalDate<Tag>> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> attribute::LocalDate<Tag> {
    return attribute::LocalDate<Tag>{
        Type<typename attribute::LocalDate<Tag>::value_type>::read_json_value(
            json_value)};
  }

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const attribute::LocalDate<Tag>& value) -> void {
    Type<typename attribute::LocalDate<Tag>::value_type>::write_json_value(
        json_value, allocator, value.value());
  }
};

template <typename Tag>
struct Type<attribute::Literal<Tag>> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> attribute::Literal<Tag> {
    return attribute::Literal<Tag>{
        Type<typename attribute::Literal<Tag>::value_type>::read_json_value(
            json_value)};
  }

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const attribute::Literal<Tag>& value) -> void {
    Type<typename attribute::Literal<Tag>::value_type>::write_json_value(
        json_value, allocator, value.value());
  }
};

template <typename Tag>
struct Type<attribute::Derived<Tag>> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value)
      -> attribute::Derived<Tag> {
    return attribute::Derived<Tag>{
        Type<typename Tag::primary_type>::read_json_value(json_value)};
  }

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const attribute::Derived<Tag>& value) -> void {
    Type<typename Tag::primary_type>::write_json_value(
        json_value, allocator, value.primary_attribute());
  }
};

}  // namespace simulator::core::json

// Formatter template for Arithmetic attributes
template <typename Tag>
struct fmt::formatter<simulator::core::attribute::Arithmetic<Tag>>
    : formatter<simulator::core::value_type_t<Tag>> {
  using formattable = simulator::core::attribute::Arithmetic<Tag>;

  auto format(formattable arithmetic, format_context& ctx) const {
    using simulator::core::value_type_t;
    using base_formatter = formatter<value_type_t<formattable>>;

    return base_formatter::format(arithmetic.value(), ctx);
  }
};

// Formatter template for Enumerable attributes
template <typename Tag>
struct fmt::formatter<simulator::core::attribute::Enumerable<Tag>>
    : formatter<simulator::core::value_type_t<Tag>> {
  using formattable = simulator::core::attribute::Enumerable<Tag>;

  auto format(formattable enumerable, format_context& ctx) const {
    using simulator::core::value_type_t;
    using base_formatter = formatter<value_type_t<formattable>>;

    return base_formatter::format(enumerable.value(), ctx);
  }
};

// Formatter template for Timestamp attributes
template <typename Tag>
struct fmt::formatter<simulator::core::attribute::UtcTimestamp<Tag>> {
  using formattable = simulator::core::attribute::UtcTimestamp<Tag>;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(formattable timestamp, format_context& ctx) const {
    return format_to(ctx.out(), "{:%Y-%b-%d %T}", timestamp.value());
  }
};

// Formatter template for Date attributes
template <typename Tag>
struct fmt::formatter<simulator::core::attribute::LocalDate<Tag>> {
  using formattable = simulator::core::attribute::LocalDate<Tag>;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(formattable date, format_context& ctx) const {
    return format_to(ctx.out(), "{:%Y-%b-%d}", date.value());
  }
};

// Formatter template for Literal attributes
template <typename Tag>
struct fmt::formatter<simulator::core::attribute::Literal<Tag>> {
  using formattable = simulator::core::attribute::Literal<Tag>;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(const formattable& literal, format_context& ctx) const {
    return format_to(ctx.out(), R"("{}")", literal.value());
  }
};

// Formatter template for derived attributes
template <typename Tag>
struct fmt::formatter<simulator::core::attribute::Derived<Tag>>
    : formatter<
          typename simulator::core::attribute::Derived<Tag>::primary_type> {
  using formattable = simulator::core::attribute::Derived<Tag>;

  auto format(const formattable& derived_attribute, format_context& ctx) const {
    using base_formatter = formatter<typename formattable::primary_type>;
    return base_formatter::format(derived_attribute.primary_attribute(), ctx);
  }
};

// NOLINTBEGIN

#define SIMULATOR_DECLARE_ATTRIBUTE(NS, NAME, TYPE)                         \
  namespace NS {                                                            \
  using NAME = ::simulator::core::attribute::TYPE<NS::tag::NAME>;           \
  }                                                                         \
  namespace simulator::core::attribute {                                    \
  extern template struct TYPE<NS::tag::NAME>;                               \
  auto operator<<(std::ostream& stream, const ::NS::NAME& value)            \
      -> std::ostream&;                                                     \
  }                                                                         \
  namespace simulator::core::json {                                         \
  extern template struct Type<NS::NAME>;                                    \
  }                                                                         \
  extern template struct ::simulator::core::attribute::TYPE<NS::tag::NAME>; \
  extern template struct ::fmt::formatter<NS::NAME>;

#define SIMULATOR_DECLARE_ENUMERABLE_ATTRIBUTE(NS, NAME) \
  SIMULATOR_DECLARE_ATTRIBUTE(NS, NAME, Enumerable)      \
  extern template struct ::simulator::core::EnumConverter<NS::NAME::value_type>;

#define SIMULATOR_DEFINE_ATTRIBUTE(NS, NAME, TYPE)                   \
  template struct ::simulator::core::attribute::TYPE<NS::tag::NAME>; \
  template struct ::simulator::core::json::Type<NS::NAME>;           \
  template struct ::fmt::formatter<NS::NAME>;                        \
  namespace simulator::core::attribute {                             \
  auto operator<<(std::ostream& stream, const ::NS::NAME& value)     \
      -> std::ostream& {                                             \
    return stream << fmt::to_string(value);                          \
  }                                                                  \
  }

#define SIMULATOR_DEFINE_ENUMERABLE_ATTRIBUTE(NS, NAME) \
  SIMULATOR_DEFINE_ATTRIBUTE(NS, NAME, Enumerable);     \
  template struct ::simulator::core::EnumConverter<NS::NAME::value_type>;

// NOLINTEND

#endif  // SIMULATOR_CORE_COMMON_ATTRIBUTE_HPP_