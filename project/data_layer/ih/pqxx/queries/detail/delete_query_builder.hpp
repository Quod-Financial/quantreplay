#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_DELETE_QUERY_BUILDER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_DELETE_QUERY_BUILDER_HPP_

#include <fmt/format.h>

#include <string>

#include "api/exceptions/exceptions.hpp"
#include "ih/pqxx/common/column_resolver.hpp"
#include "ih/pqxx/common/enumeration_resolver.hpp"
#include "ih/pqxx/queries/detail/predicate_formatter.hpp"

namespace simulator::data_layer::internal_pqxx::detail {

class DeleteQueryBuilder {
  template <typename Sanitizer>
  using PredicateBuilder = internal_pqxx::detail::PredicateFormatter<
      internal_pqxx::ColumnResolver,
      internal_pqxx::EnumerationResolver,
      Sanitizer>;

 public:
  DeleteQueryBuilder() = delete;

  explicit DeleteQueryBuilder(std::string table_name);

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

inline DeleteQueryBuilder::DeleteQueryBuilder(std::string table_name)
    : table_name_(std::move(table_name)) {
  if (table_name_.empty()) {
    throw InternalError(
        "DELETE query builder has not been supplied with a target "
        "table name");
  }
}

template <typename Model, typename Sanitizer>
inline auto DeleteQueryBuilder::with_predicate_expression(
    const predicate::Expression<Model>& predicate, Sanitizer& sanitizer)
    -> void {
  PredicateBuilder<Sanitizer> builder{
      column_resolver_, enum_resolver_, sanitizer};

  builder.accept(predicate);
  predicate_ = builder.compose();
}

template <typename Column, typename Value, typename Sanitizer>
inline auto DeleteQueryBuilder::with_eq_predicate(Column column,
                                                Value&& value,
                                                Sanitizer& sanitizer) -> void {
  predicate_ = fmt::format("{} = {}",
                           column_resolver_(column),
                           sanitizer(std::forward<Value>(value)));
}

inline auto DeleteQueryBuilder::compose() const -> std::string {
  std::string res{};
  if (predicate_.empty()) {
    res = fmt::format("DELETE FROM {}", table_name_);
  } else {
    res = fmt::format("DELETE FROM {} WHERE {}", table_name_, predicate_);
  }
  return res;
}

}  // namespace simulator::data_layer::internal_pqxx::detail

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_DELETE_QUERY_BUILDER_HPP_
