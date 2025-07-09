#ifndef SIMULATOR_CORE_COMMON_JSON_TYPE_CONTAINER_HPP_
#define SIMULATOR_CORE_COMMON_JSON_TYPE_CONTAINER_HPP_

#include <vector>

#include "core/common/json/type.hpp"
#include "core/common/meta.hpp"

namespace simulator::core::json {

template <Vector T>
struct Type<T> {
  [[nodiscard]]
  static auto read_json_value(const rapidjson::Value& json_value) -> T {
    if (!json_value.IsArray()) {
      throw std::runtime_error{
          fmt::format("expected JSON array, got `{}`", json_value.GetType())};
    }

    T result;
    for (rapidjson::SizeType i = 0; i < json_value.Size(); ++i) {
      try {
        result.emplace_back(
            Type<typename T::value_type>::read_json_value(json_value[i]));
      } catch (const std::runtime_error& e) {
        throw std::runtime_error{fmt::format(
            "failed to parse JSON array item #{}: {}", i, e.what())};
      }
    }

    return result;
  }

  static auto write_json_value(rapidjson::Value& json_value,
                               rapidjson::Document::AllocatorType& allocator,
                               const T& value) -> void {
    json_value = rapidjson::Value(rapidjson::Type::kArrayType);
    json_value.Reserve(static_cast<rapidjson::SizeType>(value.size()),
                       allocator);

    for (const auto& item : value) {
      rapidjson::Value element;
      Type<typename T::value_type>::write_json_value(element, allocator, item);
      json_value.PushBack(element.Move(), allocator);
    }
  }
};

}  // namespace simulator::core::json

#endif  // SIMULATOR_CORE_COMMON_JSON_TYPE_CONTAINER_HPP_
