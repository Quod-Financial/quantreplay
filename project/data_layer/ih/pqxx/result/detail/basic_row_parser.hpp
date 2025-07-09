#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_DETAIL_BASIC_ROW_PARSER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_DETAIL_BASIC_ROW_PARSER_HPP_

#include <functional>
#include <pqxx/row>
#include <string>

#include "ih/pqxx/common/column_resolver.hpp"
#include "ih/pqxx/common/enumeration_resolver.hpp"

namespace simulator::data_layer::internal_pqxx::detail {

class BasicRowParser {
  template <typename T>
  constexpr inline static bool is_character_v =
      std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, char>;

  template <typename T>
  constexpr inline static bool is_enumerable_v = std::is_enum_v<T>;

  template <typename T>
  constexpr inline static bool is_default_decodable_v =
      !is_character_v<T> && !is_enumerable_v<T>;

  template <typename T>
  constexpr inline static bool is_resolvable_column_v =
      std::is_invocable_r_v<std::string, internal_pqxx::ColumnResolver, T>;

  template <typename T>
  constexpr inline static bool is_resolvable_enum_v =
      std::is_invocable_r_v<std::string, internal_pqxx::EnumerationResolver, T>;

 public:
  explicit BasicRowParser(const pqxx::row& db_row) noexcept : row_(db_row) {}

  template <typename ColumnType, typename Value>
  auto operator()(ColumnType column, Value& value)
      -> std::enable_if_t<is_default_decodable_v<Value>, bool> {
    static_assert(
        is_resolvable_column_v<ColumnType>,
        "Given ColumnType can not be resolved by pqxx::ColumnResolver");

    const auto idx = get_column_idx(column);
    if (!idx.has_value()) {
      return false;
    }

    const pqxx::field& field = row().at(*idx);
    std::optional<Value> parsed_value = field.get<Value>();
    if (!parsed_value.has_value()) {
      return false;
    }

    value = std::move(*parsed_value);
    return true;
  }

  template <typename ColumnType, typename Value>
  auto operator()(ColumnType column, Value& value)
      -> std::enable_if_t<is_character_v<Value>, bool> {
    static_assert(
        is_resolvable_column_v<ColumnType>,
        "Given ColumnType can not be resolved by pqxx::ColumnResolver");

    const auto idx = get_column_idx(column);
    if (!idx.has_value()) {
      return false;
    }

    const pqxx::field& field = row().at(*idx);
    std::optional<std::string> parsed_value = field.get<std::string>();
    if (!parsed_value.has_value() || parsed_value->empty()) {
      return false;
    }

    value = parsed_value->front();
    return true;
  }

  template <typename ColumnType, typename Value>
  auto operator()(ColumnType column, Value& enum_value)
      -> std::enable_if_t<is_enumerable_v<Value>, bool> {
    static_assert(
        is_resolvable_column_v<ColumnType>,
        "Given ColumnType can not be resolved by pqxx::ColumnResolver");

    static_assert(is_resolvable_enum_v<Value>,
                  "Given enumerable Value type can not be resolved by "
                  "pqxx::EnumerationResolver");

    std::string encoded_enum{};
    if ((*this)(column, encoded_enum)) {
      enum_resolver_(encoded_enum, enum_value);
      return true;
    }
    return false;
  }

 private:
  template <typename ColumnType>
  [[nodiscard]]
  auto get_column_idx(ColumnType column) const noexcept
      -> std::optional<pqxx::row::size_type> {
    try {
      const std::string column_name = column_resolver_(column);
      return row().column_number(column_name);
    } catch (...) {
      return std::nullopt;
    }
  }

  [[nodiscard]]
  auto row() const noexcept -> const pqxx::row& {
    return row_;
  }

  std::reference_wrapper<const pqxx::row> row_;

  internal_pqxx::ColumnResolver column_resolver_;
  internal_pqxx::EnumerationResolver enum_resolver_;
};

}  // namespace simulator::data_layer::internal_pqxx::detail

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_DETAIL_BASIC_ROW_PARSER_HPP_
