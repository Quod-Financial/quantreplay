#ifndef SIMULATOR_PROJECT_DATA_LAYER_IH_COMMON_QUERIES_DATA_EXTRACTOR_HPP_
#define SIMULATOR_PROJECT_DATA_LAYER_IH_COMMON_QUERIES_DATA_EXTRACTOR_HPP_

#include <fmt/format.h>

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>

namespace simulator::data_layer {

template <typename ColumnResolver,
          typename EnumerationResolver,
          typename ValueSanitizer>
class DataExtractor {
  template <typename T>
  constexpr inline static bool can_represent_column_v =
      std::is_invocable_r_v<std::string, ColumnResolver, T>;

  template <typename T>
  constexpr inline static bool is_resolvable_enum_v =
      std::is_invocable_r_v<std::string, EnumerationResolver, T>;

  template <typename T>
  constexpr inline static bool is_boolean_v =
      std::is_same_v<bool, std::remove_cv_t<std::remove_reference_t<T>>>;

 public:
  using ColumnValuePair = std::pair<std::string, std::string>;
  using ExtractedData = std::vector<ColumnValuePair>;

  DataExtractor(ColumnResolver& column_resolver,
                EnumerationResolver& enum_values_resolver,
                ValueSanitizer& sanitizer) noexcept
      : column_resolver_(column_resolver),
        enum_resolver_(enum_values_resolver),
        sanitizer_(sanitizer) {}

  [[nodiscard]]
  auto extracted_data() const noexcept -> const ExtractedData& {
    return extracted_data_;
  }

  template <typename Column, typename T>
  auto operator()(Column column, T value) -> std::enable_if_t<is_boolean_v<T>> {
    // It's critical to enable this function by `is_boolean` trait
    // to prevent implicit conversions (f.e. const char[]) to boolean
    const std::string str = value ? "true" : "false";
    (*this)(column, str);
  }

  template <typename Column, typename T>
  auto operator()(Column column, T value)
      -> std::enable_if_t<std::is_arithmetic_v<T> && !is_boolean_v<T>> {
    static_assert(
        can_represent_column_v<Column>,
        "Given Column type can not represent a column (attribute) or can "
        "not be resolved by given EnumerationResolver");

    // fmt::to_string is used to drop trailing zeros for fp types,
    // std::to_string returns a str with 6-digit precision (filled with 0)
    std::string string_value = sanitizer_(fmt::to_string(value));
    std::string column_name = column_resolver_(column);
    extracted_data_.emplace_back(
        std::make_pair(std::move(column_name), std::move(string_value)));
  }

  template <typename Column>
  auto operator()(Column column, const std::string& value) -> void {
    static_assert(
        can_represent_column_v<Column>,
        "Given Column type can not represent a column (attribute) or can "
        "not be resolved by given EnumerationResolver");

    std::string column_name = column_resolver_(column);
    std::string string_value = sanitizer_(value);
    extracted_data_.emplace_back(
        std::make_pair(std::move(column_name), std::move(string_value)));
  }

  template <typename Column, typename T>
  auto operator()(Column column, T value)
      -> std::enable_if_t<std::is_enum_v<T>> {
    static_assert(is_resolvable_enum_v<T>,
                  "Passed enumeration value can not be resolved by given "
                  "EnumerationResolver");

    const std::string resolved_value = enum_resolver_(value);
    (*this)(column, resolved_value);
  }

 private:
  ExtractedData extracted_data_;

  std::reference_wrapper<ColumnResolver> column_resolver_;
  std::reference_wrapper<EnumerationResolver> enum_resolver_;
  std::reference_wrapper<ValueSanitizer> sanitizer_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_PROJECT_DATA_LAYER_IH_COMMON_QUERIES_DATA_EXTRACTOR_HPP_
