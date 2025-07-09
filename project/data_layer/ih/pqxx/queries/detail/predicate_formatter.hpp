#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_PREDICATE_FORMATTER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_PREDICATE_FORMATTER_HPP_

#include <fmt/ranges.h>

#include <functional>

#include "api/predicate/predicate.hpp"

namespace simulator::data_layer::internal_pqxx::detail {

template <typename ColumnResolver,
          typename EnumerationResolver,
          typename ValueSanitizer>
class PredicateFormatter {
  template <typename T>
  constexpr inline static bool is_enum_value_v =
      std::is_enum_v<std::remove_cv_t<std::remove_reference_t<T>>>;

 public:
  PredicateFormatter(ColumnResolver& column_resolver,
                     EnumerationResolver& enum_values_resolver,
                     ValueSanitizer& sanitizer) noexcept
      : column_resolver_(column_resolver),
        enum_resolver_(enum_values_resolver),
        sanitizer_(sanitizer) {}

  template <typename Model>
  auto accept(const predicate::Expression<Model>& expression) {
    lexemes_.clear();
    expression.accept(*this);
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    constexpr std::string_view separator{" "};
    return fmt::format("{}", fmt::join(lexemes_, separator));
  }

  template <typename Column, typename Value>
  auto operator()(Column column,
                  predicate::BasicOperation basic_operation,
                  Value&& value) -> std::enable_if_t<!is_enum_value_v<Value>> {
    const std::string col = column_resolver_(column);
    const std::string_view operation = format(basic_operation);
    const std::string val = sanitizer_(std::forward<Value>(value));
    lexemes_.emplace_back(fmt::format("{} {} {}", col, operation, val));
  }

  template <typename Column, typename Value>
  auto operator()(Column column,
                  predicate::BasicOperation basic_operation,
                  Value value) -> std::enable_if_t<is_enum_value_v<Value>> {
    const std::string string_value = enum_resolver_(value);
    (*this)(column, basic_operation, string_value);
  }

  auto operator()([[maybe_unused]] predicate::SubExpressionBegin lexeme)
      -> void {
    lexemes_.emplace_back("(");
  }

  auto operator()([[maybe_unused]] predicate::SubExpressionEnd lexeme) -> void {
    lexemes_.emplace_back(")");
  }

  auto operator()(predicate::CompositeOperation composite_operation) -> void {
    const std::string_view operation = format(composite_operation);
    lexemes_.emplace_back(fmt::format("{}", operation));
  }

 private:
  constexpr static auto format(predicate::BasicOperation basic_operation)
      -> std::string_view {
    std::string_view value{};
    switch (basic_operation) {
      case predicate::BasicOperation::Eq:
        value = "=";
        break;
      case predicate::BasicOperation::Neq:
        value = "!=";
        break;
      case predicate::BasicOperation::Less:
        value = "<";
        break;
      case predicate::BasicOperation::Greater:
        value = ">";
        break;
      case predicate::BasicOperation::LessEq:
        value = "<=";
        break;
      case predicate::BasicOperation::GreaterEq:
        value = ">=";
        break;
    }
    return value;
  }

  constexpr static auto format(
      predicate::CompositeOperation composite_operation) -> std::string_view {
    std::string_view value{};
    switch (composite_operation) {
      case predicate::CompositeOperation::And:
        value = "AND";
        break;
      case predicate::CompositeOperation::Or:
        value = "OR";
        break;
    }
    return value;
  }

  std::vector<std::string> lexemes_;

  std::reference_wrapper<ColumnResolver> column_resolver_;
  std::reference_wrapper<EnumerationResolver> enum_resolver_;
  std::reference_wrapper<ValueSanitizer> sanitizer_;
};

}  // namespace simulator::data_layer::internal_pqxx::detail

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_PREDICATE_FORMATTER_HPP_
