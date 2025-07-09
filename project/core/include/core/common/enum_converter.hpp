#ifndef SIMULATOR_CORE_COMMON_ENUM_CONVERTER_HPP_
#define SIMULATOR_CORE_COMMON_ENUM_CONVERTER_HPP_

#include <fmt/format.h>

#include <type_traits>
#include <unordered_map>

#include "core/common/meta.hpp"

namespace simulator::core {

template <Enumerable E>
class EnumConverter {
 public:
  EnumConverter() = delete;
  EnumConverter(const EnumConverter&) = delete;
  EnumConverter(EnumConverter&&) = delete;
  auto operator=(const EnumConverter&) -> EnumConverter& = delete;
  auto operator=(EnumConverter&&) -> EnumConverter& = delete;

  constexpr EnumConverter(
      const std::initializer_list<std::pair<E, std::string_view>>& data) {
    for (auto& pair : data) {
      value_to_str_[pair.first] = pair.second;
      str_to_value_[pair.second] = pair.first;
    }
  }

  [[nodiscard]]
  static constexpr auto str(E value) -> std::string_view {
    auto it = instance_.value_to_str_.find(value);
    if (it != instance_.value_to_str_.end()) {
      return it->second;
    }
    throw std::runtime_error{
        fmt::format("unknown enumeration value `{}`",
                    static_cast<std::underlying_type_t<E>>(value))};
  }

  [[nodiscard]]
  static constexpr auto value(std::string_view str) -> E {
    auto it = instance_.str_to_value_.find(str);
    if (it != instance_.str_to_value_.end()) {
      return it->second;
    }
    throw std::runtime_error{
        fmt::format("unknown enumeration string `{}`", str)};
  }

 private:
  std::unordered_map<E, std::string_view> value_to_str_;
  std::unordered_map<std::string_view, E> str_to_value_;

  static EnumConverter<E> instance_;
};

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_COMMON_ENUM_CONVERTER_HPP_
