#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_EXPRESSION_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_EXPRESSION_HPP_

#include <algorithm>
#include <cassert>
#include <utility>
#include <variant>
#include <vector>

#include "data_layer/api/predicate/definitions.hpp"
#include "data_layer/api/predicate/lexeme.hpp"
#include "data_layer/api/predicate/traits.hpp"

namespace simulator::data_layer::predicate {

template <typename Model>
class Expression {
 public:
  using BasicLexeme = BasicPredicate<Model>;
  using CompositeLexeme = CompositePredicate;
  using SubExpressionBeginLexeme = SubExpressionBegin;
  using SubExpressionEndLexeme = SubExpressionEnd;

 private:
  using Lexeme = std::variant<BasicLexeme,
                              CompositeLexeme,
                              SubExpressionBeginLexeme,
                              SubExpressionEndLexeme>;

 public:
  Expression() = delete;  // Empty predicate expressions are not allowed

  // Creates a basic expression with a single BasicPredicate lexeme
  explicit Expression(BasicLexeme basic_lexeme)
      : lexemes_{std::move(basic_lexeme)} {}

  template <typename Formatter>
  auto accept(Formatter& formatter) const -> void {
    const auto lexeme_visitor = [&](const auto& concrete_lexeme) {
      concrete_lexeme.accept(formatter);
    };

    for (const Lexeme& lexeme : lexemes_) {
      std::visit(lexeme_visitor, lexeme);
    }
  }

  [[nodiscard]]
  auto is_basic() const noexcept -> bool {
    assert(!lexemes_.empty());
    return lexemes_.size() == 1 &&
           std::holds_alternative<BasicLexeme>(lexemes_.front());
  }

  [[nodiscard]]
  auto is_composite() const noexcept -> bool {
    return !is_basic();
  }

  static auto compose(Expression left,
                      CompositeOperation operation,
                      Expression right) -> Expression {
    const std::size_t composed_size = calculate_extended_size(left, right);
    std::vector<Lexeme> composed_lexemes;
    composed_lexemes.reserve(composed_size);

    compose_into(std::move(left), composed_lexemes);
    composed_lexemes.emplace_back(CompositeLexeme{operation});
    compose_into(std::move(right), composed_lexemes);

    return Expression{std::move(composed_lexemes)};
  }

 private:
  explicit Expression(std::vector<Lexeme> lexemes) noexcept
      : lexemes_(std::move(lexemes)) {}

  static auto calculate_extended_size(const Expression& left,
                                      const Expression& right) -> std::size_t {
    // An extended size is size(left) + size(right) + 1
    // for a composite lexeme
    std::size_t extended_expression_size =
        left.lexemes_.size() + right.lexemes_.size() + 1;

    if (left.is_composite()) {
      extended_expression_size += 2;
    }

    if (right.is_composite()) {
      extended_expression_size += 2;
    }

    return extended_expression_size;
  }

  static auto compose_into(Expression expression,
                           std::vector<Lexeme>& composed_lexemes) -> void {
    const bool is_composite = expression.is_composite();
    auto& old_lexemes = expression.lexemes_;

    if (is_composite) {
      composed_lexemes.emplace_back(SubExpressionBeginLexeme{});
    }

    std::copy(std::make_move_iterator(std::begin(old_lexemes)),
              std::make_move_iterator(std::end(old_lexemes)),
              std::back_inserter(composed_lexemes));

    if (is_composite) {
      composed_lexemes.emplace_back(SubExpressionEndLexeme{});
    }
  }

  std::vector<Lexeme> lexemes_;
};

// Boolean-type expressions

template <typename Model, typename Traits = ModelTraits<Model>>
inline auto eq(typename Traits::AttributeType field, bool value)
    -> Expression<Model> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{field, BasicOperation::Eq, value}};
}

template <typename Model, typename Traits = ModelTraits<Model>>
inline auto neq(typename Traits::AttributeType field, bool value)
    -> Expression<Model> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{field, BasicOperation::Neq, value}};
}

// UnsignedInt-type expressions

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_unsigned_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Eq, static_cast<std::uint64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto neq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_unsigned_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Neq, static_cast<std::uint64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto less(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_unsigned_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Less, static_cast<std::uint64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto greater(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_unsigned_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{
      field, BasicOperation::Greater, static_cast<std::uint64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto less_eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_unsigned_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::LessEq, static_cast<std::uint64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto greater_eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_unsigned_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{
      field, BasicOperation::GreaterEq, static_cast<std::uint64_t>(value)}};
}

// UnsignedInt-type expressions

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_signed_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Eq, static_cast<std::int64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto neq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_signed_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Neq, static_cast<std::int64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto less(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_signed_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Less, static_cast<std::int64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto greater(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_signed_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Greater, static_cast<std::int64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto less_eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_signed_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::LessEq, static_cast<std::int64_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto greater_eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<is_signed_int_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{
      field, BasicOperation::GreaterEq, static_cast<std::int64_t>(value)}};
}

// Floating-point-type expressions

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<std::is_floating_point_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Eq, static_cast<std::double_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto neq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<std::is_floating_point_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Neq, static_cast<std::double_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto less(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<std::is_floating_point_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::Less, static_cast<std::double_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto greater(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<std::is_floating_point_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{
      field, BasicOperation::Greater, static_cast<std::double_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto less_eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<std::is_floating_point_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{
      Lexeme{field, BasicOperation::LessEq, static_cast<std::double_t>(value)}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto greater_eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<std::is_floating_point_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{
      field, BasicOperation::GreaterEq, static_cast<std::double_t>(value)}};
}

// String-type expressions

template <typename Model, typename Traits = ModelTraits<Model>>
inline auto eq(typename Traits::AttributeType field, std::string value)
    -> Expression<Model> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{field, BasicOperation::Eq, std::move(value)}};
}

template <typename Model, typename Traits = ModelTraits<Model>>
inline auto neq(typename Traits::AttributeType field, std::string value)
    -> Expression<Model> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{field, BasicOperation::Neq, std::move(value)}};
}

// Custom-type (enumeration) expressions

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto eq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<std::is_enum_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{field, BasicOperation::Eq, value}};
}

template <typename Model, typename T, typename Traits = ModelTraits<Model>>
inline auto neq(typename Traits::AttributeType field, T value)
    -> std::enable_if_t<std::is_enum_v<T>, Expression<Model>> {
  using Expression = Expression<Model>;
  using Lexeme = typename Expression::BasicLexeme;

  return Expression{Lexeme{field, BasicOperation::Neq, value}};
}

// Deleted functions (to avoid bugs with implicit conversions)

template <typename Model, typename Traits = ModelTraits<Model>>
inline auto eq(typename Traits::AttributeType field,
               const char* value) = delete;

template <typename Model, typename Traits = ModelTraits<Model>>
inline auto neq(typename Traits::AttributeType field,
                const char* value) = delete;

// Composite operations on expressions

template <typename Model>
auto operator&&(Expression<Model> left, Expression<Model> right)
    -> Expression<Model> {
  return Expression<Model>::compose(
      std::move(left), CompositeOperation::And, std::move(right));
}

template <typename Model>
auto operator||(Expression<Model> left, Expression<Model> right)
    -> Expression<Model> {
  return Expression<Model>::compose(
      std::move(left), CompositeOperation::Or, std::move(right));
}

}  // namespace simulator::data_layer::predicate

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_EXPRESSION_HPP_
