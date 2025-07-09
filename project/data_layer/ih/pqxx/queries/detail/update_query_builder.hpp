#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_UPDATE_QUERY_BUILDER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_UPDATE_QUERY_BUILDER_HPP_

#include <fmt/ranges.h>

#include <cassert>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "api/exceptions/exceptions.hpp"
#include "ih/common/queries/data_extractor.hpp"
#include "ih/pqxx/common/column_resolver.hpp"
#include "ih/pqxx/common/enumeration_resolver.hpp"
#include "ih/pqxx/queries/detail/predicate_formatter.hpp"

namespace simulator::data_layer::internal_pqxx::detail {

class UpdateQueryBuilder {
  template <typename Sanitizer>
  using CustomExtractorType =
      data_layer::DataExtractor<internal_pqxx::ColumnResolver,
                                internal_pqxx::EnumerationResolver,
                                Sanitizer>;

  template <typename Sanitizer>
  using PredicateBuilder = internal_pqxx::detail::PredicateFormatter<
      internal_pqxx::ColumnResolver,
      internal_pqxx::EnumerationResolver,
      Sanitizer>;

  template <typename T>
  constexpr static bool can_be_resolved_as_column_v =
      std::is_invocable_r_v<std::string, internal_pqxx::ColumnResolver, T>;

 public:
  UpdateQueryBuilder() = delete;

  explicit UpdateQueryBuilder(std::string table_name);

  template <typename Sanitizer>
  auto make_data_extractor(Sanitizer& value_sanitizer)
      -> CustomExtractorType<Sanitizer>;

  template <typename Sanitizer>
  auto build(const CustomExtractorType<Sanitizer>& extractor) -> void;

  template <typename Column>
  auto with_returning(Column column) -> void;

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
  std::string returning_;
  std::string predicate_;
  std::vector<std::string> column_value_updates_;

  internal_pqxx::ColumnResolver column_resolver_;
  internal_pqxx::EnumerationResolver enum_resolver_;
};

inline UpdateQueryBuilder::UpdateQueryBuilder(std::string table_name)
    : table_name_(std::move(table_name)) {
  if (table_name_.empty()) {
    throw InternalError(
        "UPDATE query builder has not been supplied with a target "
        "table name");
  }
}

template <typename Sanitizer>
inline auto UpdateQueryBuilder::make_data_extractor(Sanitizer& value_sanitizer)
    -> UpdateQueryBuilder::CustomExtractorType<Sanitizer> {
  return CustomExtractorType<Sanitizer>{
      column_resolver_, enum_resolver_, value_sanitizer};
}

template <typename Sanitizer>
inline auto UpdateQueryBuilder::build(
    const CustomExtractorType<Sanitizer>& extractor) -> void {
  const auto& extracted_data = extractor.extracted_data();
  column_value_updates_.reserve(extracted_data.size());

  for (auto [column, value] : extractor.extracted_data()) {
    column_value_updates_.emplace_back(fmt::format("{} = {}", column, value));
  }
}

template <typename Model, typename Sanitizer>
inline auto UpdateQueryBuilder::with_predicate_expression(
    const predicate::Expression<Model>& predicate, Sanitizer& sanitizer)
    -> void {
  PredicateBuilder<Sanitizer> builder{
      column_resolver_, enum_resolver_, sanitizer};

  builder.accept(predicate);
  predicate_ = builder.compose();
}

template <typename Column, typename Value, typename Sanitizer>
inline auto UpdateQueryBuilder::with_eq_predicate(Column column,
                                                Value&& value,
                                                Sanitizer& sanitizer) -> void {
  predicate_ = fmt::format("{} = {}",
                           column_resolver_(column),
                           sanitizer(std::forward<Value>(value)));
}

inline auto UpdateQueryBuilder::compose() const -> std::string {
  assert(!table_name_.empty());  // Must be validated in c-tor

  if (column_value_updates_.empty()) {
    throw MalformedPatch(
        "a patch with no fields is passed into UPDATE query builder");
  }

  const std::string predicate =
      predicate_.empty() ? "" : fmt::format(" WHERE {}", predicate_);
  const std::string returning =
      returning_.empty() ? "" : fmt::format(" RETURNING {}", returning_);

  constexpr std::string_view separator = ", ";
  return fmt::format("UPDATE {} SET {}{}{}",
                     table_name_,
                     fmt::join(column_value_updates_, separator),
                     predicate,
                     returning);
}
template <typename Column>
auto UpdateQueryBuilder::with_returning(Column column) -> void {
  static_assert(can_be_resolved_as_column_v<Column>,
                "Can not resolve a column name for a given type");
  returning_ = column_resolver_(column);
}

}  // namespace simulator::data_layer::internal_pqxx::detail

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_UPDATE_QUERY_BUILDER_HPP_
