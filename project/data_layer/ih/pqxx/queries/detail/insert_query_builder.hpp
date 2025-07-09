#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_INSERT_QUERY_BUILDER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_INSERT_QUERY_BUILDER_HPP_

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

namespace simulator::data_layer::internal_pqxx::detail {

class InsertQueryBuilder {
  template <typename Sanitizer>
  using CustomExtractorType =
      data_layer::DataExtractor<internal_pqxx::ColumnResolver,
                                internal_pqxx::EnumerationResolver,
                                Sanitizer>;

  template <typename T>
  constexpr static bool can_be_resolved_as_column_v =
      std::is_invocable_r_v<std::string, internal_pqxx::ColumnResolver, T>;

 public:
  InsertQueryBuilder() = delete;

  explicit InsertQueryBuilder(std::string table_name);

  template <typename Sanitizer>
  auto make_data_extractor(Sanitizer& value_sanitizer)
      -> CustomExtractorType<Sanitizer>;

  template <typename Sanitizer>
  auto build(const CustomExtractorType<Sanitizer>& extractor) -> void;

  template <typename Column>
  auto with_returning(Column column) -> void;

  [[nodiscard]]
  auto compose() const -> std::string;

 private:
  std::string table_name_;
  std::string returning_;
  std::vector<std::string> columns_;
  std::vector<std::string> values_;

  internal_pqxx::ColumnResolver column_resolver_;
  internal_pqxx::EnumerationResolver enum_resolver_;
};

inline InsertQueryBuilder::InsertQueryBuilder(std::string table_name)
    : table_name_(std::move(table_name)) {
  if (table_name_.empty()) {
    throw InternalError(
        "INSERT query builder has not been supplied with a target "
        "table name");
  }
}

template <typename Sanitizer>
inline auto InsertQueryBuilder::make_data_extractor(Sanitizer& value_sanitizer)
    -> CustomExtractorType<Sanitizer> {
  return CustomExtractorType<Sanitizer>{
      column_resolver_, enum_resolver_, value_sanitizer};
}

template <typename Column>
inline auto InsertQueryBuilder::with_returning(Column column) -> void {
  static_assert(can_be_resolved_as_column_v<Column>,
                "Can not resolve a column name for a given type");
  returning_ = column_resolver_(column);
}

template <typename Sanitizer>
inline auto InsertQueryBuilder::build(
    const CustomExtractorType<Sanitizer>& extractor) -> void {
  const auto& extracted_data = extractor.extracted_data();
  columns_.reserve(extracted_data.size());
  values_.reserve(extracted_data.size());

  for (auto [column, value] : extractor.extracted_data()) {
    columns_.emplace_back(column);
    values_.emplace_back(value);
  }
}

inline auto InsertQueryBuilder::compose() const -> std::string {
  assert(!table_name_.empty());  // Must be validated in c-tor

  if (columns_.empty() && values_.empty()) {
    throw MalformedPatch(
        "a patch with no fields is passed into INSERT query builder");
  }

  const std::string returning =
      returning_.empty() ? "" : fmt::format(" RETURNING {}", returning_);

  constexpr std::string_view separator = ", ";
  return fmt::format("INSERT INTO {} ({}) VALUES ({}){}",
                     table_name_,
                     fmt::join(columns_, separator),
                     fmt::join(values_, separator),
                     returning);
}

}  // namespace simulator::data_layer::internal_pqxx::detail

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DETAIL_INSERT_QUERY_BUILDER_HPP_
