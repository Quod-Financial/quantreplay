#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_LEXEME_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_LEXEME_HPP_

#include <utility>
#include <variant>
#include <vector>

#include "data_layer/api/predicate/definitions.hpp"
#include "data_layer/api/predicate/traits.hpp"

namespace simulator::data_layer::predicate {

// Represents a simple predicate expression, which matches given model attribute
// to a given value with a given binary operation, such as:
//     `listing_id = 10`, where:
//          'listing_id' represents an attribute name (enumeration)
//          '=' represents a binary logical operation (equal)
//          '10' represent a value
// Each attribute must be an enumeration, which is taken from model traits
// Each value must be represented by a type from StandardTypes tuple,
// or by an additional enumerable type specified in model traits in
// CustomFieldTypes tuple
template <typename Model, typename ModelTraits = ModelTraits<Model>>
class BasicPredicate {
  using AttributeType = typename ModelTraits::AttributeType;
  using ExtendedTypes = typename ModelTraits::CustomFieldTypes;
  using FieldTypes = aggregated_tuple_t<StandardTypes, ExtendedTypes>;
  using GenericValue = variant_from_tuple_t<FieldTypes>;

 public:
  BasicPredicate() = delete;

  template <typename V,
            std::enable_if_t<is_in_tuple_v<V, FieldTypes>>* = nullptr>
  BasicPredicate(AttributeType field, BasicOperation basic_operation, V&& value)
      : value_(std::forward<V>(value)),
        operation_(basic_operation),
        field_(field) {}

  template <typename Formatter>
  auto accept(Formatter& formatter) const -> void {
    const auto value_visitor = [&](const auto& concrete_value) {
      formatter(field_, operation_, concrete_value);
    };

    std::visit(value_visitor, value_);
  }

 private:
  GenericValue value_;
  BasicOperation operation_;
  AttributeType field_;
};

// Represents a membership predicate expression, which matches a given model
// attribute against a set of values, equivalent to the SQL `IN` operator:
//     `venue_id IN ('XETRA', 'FASTMATCH')`, where:
//          'venue_id' represents an attribute name (enumeration)
//          'IN' represents the membership operation
//          'XETRA', 'FASTMATCH' represent the set of accepted values
// As with BasicPredicate, the attribute is an enumeration from the model traits
// and each value must be a type from StandardTypes or the model's
// CustomFieldTypes tuple.
template <typename Model, typename ModelTraits = ModelTraits<Model>>
class InPredicate {
  using AttributeType = typename ModelTraits::AttributeType;
  using ExtendedTypes = typename ModelTraits::CustomFieldTypes;
  using FieldTypes = aggregated_tuple_t<StandardTypes, ExtendedTypes>;
  using GenericValue = variant_from_tuple_t<FieldTypes>;

 public:
  InPredicate() = delete;

  template <typename V,
            std::enable_if_t<is_in_tuple_v<V, FieldTypes>>* = nullptr>
  InPredicate(AttributeType field, std::vector<V> values) : field_(field) {
    values_.reserve(values.size());
    for (V& value : values) {
      values_.emplace_back(std::move(value));
    }
  }

  template <typename Formatter>
  auto accept(Formatter& formatter) const -> void {
    formatter(field_, values_);
  }

 private:
  std::vector<GenericValue> values_;
  AttributeType field_;
};

// Represents a logical binary operator applied to basic predicates and/or other
// composite structures, such as:
//     listing_id = 10 AND price <= 20:
//          AND is composite predicate applied on 2 basic predicates
//     (listing_id = 10 AND price <= 20) OR size > 100:
//          OR is a composite predicate applied on basic predicate and another
//          composite predicate
//     (listing_id = 10 AND price <= 20) OR (size > 100 AND name = 'my_name'):
//          OR is a composite predicate applied on 2 composite predicates
class CompositePredicate {
 public:
  CompositePredicate() = delete;

  explicit CompositePredicate(CompositeOperation composite_operation) noexcept
      : operation_(composite_operation) {}

  template <typename Formatter>
  auto accept(Formatter& formatter) const -> void {
    formatter(operation_);
  }

 private:
  CompositeOperation operation_;
};

// Special type, which represents a predicate sub-expression begin.
// Usually represented as "("
class SubExpressionBegin {
 public:
  template <typename Formatter>
  auto accept(Formatter& formatter) const -> void {
    formatter(*this);
  }
};

// Special type, which represents a predicate sub-expression end.
// Usually represented as ")"
class SubExpressionEnd {
 public:
  template <typename Formatter>
  auto accept(Formatter& formatter) const -> void {
    formatter(*this);
  }
};

}  // namespace simulator::data_layer::predicate

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_LEXEME_HPP_
