#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_SELECT_QUERY_BUILDER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_SELECT_QUERY_BUILDER_HPP_

#include <fmt/format.h>

#include <cassert>
#include <string>

#include "api/exceptions/exceptions.hpp"
#include "ih/pqxx/common/column_resolver.hpp"
#include "ih/pqxx/common/enumeration_resolver.hpp"
#include "ih/pqxx/queries/detail/predicate_formatter.hpp"

namespace simulator::data_layer::internal_pqxx::detail {

class SelectQueryBuilder {
  template <typename Sanitizer>
  using PredicateBuilder = internal_pqxx::detail::PredicateFormatter<
      internal_pqxx::ColumnResolver,
      internal_pqxx::EnumerationResolver,
      Sanitizer>;

 public:
  SelectQueryBuilder() = delete;

  explicit SelectQueryBuilder(std::string table_name);

  template <typename Model, typename Sanitizer>
  auto with_predicate_expression(const predicate::Expression<Model>& predicate,
                                 Sanitizer& sanitizer) -> void;

  template <typename Column, typename Value, typename Sanitizer>
  auto with_eq_predicate(Column column, Value&& value, Sanitizer& sanitizer)
      -> void;

  [[nodiscard]]
  auto compose() const -> std::string;

 private:
  std::string table_name_;
  std::string predicate_;

  internal_pqxx::ColumnResolver column_resolver_;
  internal_pqxx::EnumerationResolver enum_resolver_;
};

inline SelectQueryBuilder::SelectQueryBuilder(std::string table_name)
    : table_name_(std::move(table_name)) {
  if (table_name_.empty()) {
    throw InternalError(
        "SELECT query builder has not been supplied with a target "
        "table name");
  }
}

template <typename Model, typename Sanitizer>
inline auto SelectQueryBuilder::with_predicate_expression(
    const predicate::Expression<Model>& predicate, Sanitizer& sanitizer)
    -> void {
  PredicateBuilder<Sanitizer> builder{
      column_resolver_, enum_resolver_, sanitizer};

  builder.accept(predicate);
  predicate_ = builder.compose();
}

template <typename Column, typename Value, typename Sanitizer>
inline auto SelectQueryBuilder::with_eq_predicate(Column column,
                                                  Value&& value,
                                                  Sanitizer& sanitizer)
    -> void {
  predicate_ = fmt::format("{} = {}",
                           column_resolver_(column),
                           sanitizer(std::forward<Value>(value)));
}

inline auto SelectQueryBuilder::compose() const -> std::string {
  assert(!table_name_.empty());  // Must be validated in c-tor

  const std::string predicate =
      predicate_.empty() ? "" : fmt::format(" WHERE {}", predicate_);

  return fmt::format("SELECT * FROM {}{}", table_name_, predicate);
}

}  // namespace simulator::data_layer::internal_pqxx::detail

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_SELECT_QUERY_BUILDER_HPP_
