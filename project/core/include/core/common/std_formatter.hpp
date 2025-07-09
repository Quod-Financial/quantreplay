#ifndef SIMULATOR_CORE_COMMON_STD_FORMATTER_HPP_
#define SIMULATOR_CORE_COMMON_STD_FORMATTER_HPP_

#include <fmt/base.h>

#include <optional>
#include <string_view>

template <typename T>
  requires fmt::is_formattable<T>::value
struct fmt::formatter<std::optional<T>> {
 private:
  static constexpr std::string_view none{"none"};

 public:
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); };

  auto format(const std::optional<T>& value, format_context& context) const
      -> decltype(context.out()) {
    return value.has_value() ? format_to(context.out(), "{}", *value)
                             : format_to(context.out(), none);
  }
};

#endif  // SIMULATOR_CORE_COMMON_STD_FORMATTER_HPP_
