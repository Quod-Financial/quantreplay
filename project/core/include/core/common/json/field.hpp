#ifndef SIMULATOR_CORE_COMMON_JSON_FIELD_HPP_
#define SIMULATOR_CORE_COMMON_JSON_FIELD_HPP_

#include <string_view>

namespace simulator::core::json {

template <typename S, typename T>
struct Field {
  using value_type = T;

  constexpr Field(T S::* field_pointer, std::string_view field_name)
      : ptr{field_pointer}, name{field_name} {}

  T S::* ptr;
  std::string_view name;
};

template <typename T>
struct Struct {};

}  // namespace simulator::core::json

#endif  // SIMULATOR_CORE_COMMON_JSON_FIELD_HPP_
