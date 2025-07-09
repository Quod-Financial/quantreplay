#ifndef SIMULATOR_CORE_COMMON_NAME_HPP_
#define SIMULATOR_CORE_COMMON_NAME_HPP_

#include <fmt/format.h>

#include <concepts>
#include <string_view>
#include <type_traits>

#include "core/common/meta.hpp"

namespace simulator::core {

struct Name {
  std::string_view singular;
  std::string_view plural;
};

template <typename T>
concept Named = requires {
  { T::name() } -> std::same_as<Name>;
};

template <typename T>
concept HasNameDescriptor = requires {
  { T::name } -> std::same_as<const Name&>;
};

template <typename T>
[[nodiscard]] constexpr auto name_of() -> Name {
  using requested_type = std::remove_cv_t<std::remove_reference_t<T>>;

  if constexpr (Named<requested_type>) {
    using named_type = requested_type;
    return named_type::name();
  } else if constexpr (Optional<requested_type>) {
    using named_type = typename requested_type::value_type;
    static_assert(
        Named<named_type>,
        "std::optional<T>::value_type does not meet Named requirements");
    return named_type::name();
  } else if constexpr (Vector<requested_type>) {
    using named_type = typename requested_type::value_type;
    static_assert(
        Named<named_type>,
        "std::vector<T, Alloc>::value_type does not meet Named requirements");
    return named_type::name();
  }
}

template <typename T>
[[nodiscard]] constexpr auto name_of(const T& /*object*/) noexcept -> Name {
  return name_of<T>();
}

}  // namespace simulator::core

template <>
struct fmt::formatter<simulator::core::Name> {
 private:
  using formattable = simulator::core::Name;
  enum class Presentation : char {
    Singular = 's',
    Plural = 'p'
  } presentation_{Presentation::Singular};

 public:
  constexpr auto parse(format_parse_context& ctx) {
    // NOLINTBEGIN(*-pointer-arithmetic,*-qualified-auto)
    auto iter = ctx.begin();
    const auto end = ctx.end();

    if (iter != end && (*iter == 's' || *iter == 'p')) {
      presentation_ = static_cast<Presentation>(*iter);
      ++iter;
    }
    if (iter != end && *iter != '}') {
      fmt::report_error(
          "bad format for simulator::core::Name, only ':s' (for singular name) "
          "and ':p' (for plural name) are allowed");
    }
    // NOLINTEND(*-pointer-arithmetic,*-qualified-auto)

    return iter;
  }

  auto format(const formattable& name, format_context& ctx) const {
    return presentation_ == Presentation::Plural
               ? format_to(ctx.out(), "{}", name.plural)
               : format_to(ctx.out(), "{}", name.singular);
  }
};

#endif  // SIMULATOR_CORE_COMMON_NAME_HPP_
