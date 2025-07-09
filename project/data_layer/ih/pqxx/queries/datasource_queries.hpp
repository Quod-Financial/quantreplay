#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DATASOURCE_QUERIES_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DATASOURCE_QUERIES_HPP_

#include <cstdint>
#include <string>

#include "api/inspectors/column_mapping.hpp"
#include "api/inspectors/datasource.hpp"
#include "api/models/column_mapping.hpp"
#include "api/models/datasource.hpp"
#include "ih/pqxx/common/names/database_entries.hpp"
#include "ih/pqxx/queries/detail/delete_query_builder.hpp"
#include "ih/pqxx/queries/detail/insert_query_builder.hpp"
#include "ih/pqxx/queries/detail/select_query_builder.hpp"
#include "ih/pqxx/queries/detail/update_query_builder.hpp"

namespace simulator::data_layer::internal_pqxx {
namespace datasource_query {

class Insert {
 public:
  using PatchType = data_layer::Datasource::Patch;

  template <typename Sanitizer>
  static auto prepare(const PatchType& snapshot, Sanitizer& sanitizer)
      -> Insert {
    Insert query{};
    query.build(snapshot, sanitizer);
    return query;
  }

  auto returning_id() -> Insert& {
    builder_.with_returning(Datasource::Attribute::DatasourceId);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Insert() : builder_(std::string{table::Datasource}) {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = DatasourcePatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::InsertQueryBuilder builder_;
};

class Select {
 public:
  using Predicate = predicate::Expression<Datasource>;

  static auto prepare() -> Select { return Select{}; }

  template <typename Sanitizer>
  auto by(const Predicate& predicate, Sanitizer& sanitizer) -> Select& {
    builder_.with_predicate_expression(predicate, sanitizer);
    return *this;
  }

  template <typename Sanitizer>
  auto by_datasource_id(std::uint64_t datasource_id, Sanitizer& sanitizer)
      -> Select& {
    constexpr auto column = Datasource::Attribute::DatasourceId;
    builder_.with_eq_predicate(column, datasource_id, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Select() : builder_(std::string{table::Datasource}) {}

  detail::SelectQueryBuilder builder_;
};

class Update {
 public:
  using PatchType = Datasource::Patch;
  using Predicate = predicate::Expression<Datasource>;

  template <typename Sanitizer>
  static auto prepare(const PatchType& snapshot, Sanitizer& sanitizer)
      -> Update {
    Update query{};
    query.build(snapshot, sanitizer);
    return query;
  }

  template <typename Sanitizer>
  auto by(const Predicate& predicate, Sanitizer& sanitizer) -> Update& {
    builder_.with_predicate_expression(predicate, sanitizer);
    return *this;
  }

  auto returning_id() -> Update& {
    builder_.with_returning(Datasource::Attribute::DatasourceId);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Update() : builder_(std::string{table::Datasource}) {}

  template <typename Sanitizer>
  auto build(const PatchType& snapshot, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = DatasourcePatchReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(snapshot);

    builder_.build(data_extractor);
  }

  detail::UpdateQueryBuilder builder_;
};

}  // namespace datasource_query

namespace column_mapping_query {

class Insert {
 public:
  template <typename Sanitizer>
  static auto prepare(const ColumnMapping& mapping, Sanitizer& sanitizer)
      -> Insert {
    Insert query{};
    query.build(mapping, sanitizer);
    return query;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Insert() : builder_(std::string{table::ColumnMapping}) {}

  template <typename Sanitizer>
  auto build(const ColumnMapping& mapping, Sanitizer& sanitizer) -> void {
    auto data_extractor = builder_.make_data_extractor(sanitizer);

    using PatchReader = ColumnMappingReader<decltype(data_extractor)>;
    PatchReader reader{data_extractor};
    reader.read(mapping);

    builder_.build(data_extractor);
  }

  detail::InsertQueryBuilder builder_;
};

class Select {
 public:
  static auto prepare() -> Select { return Select{}; }

  template <typename Sanitizer>
  auto by_datasource_id(std::uint64_t datasource_id, Sanitizer& sanitizer)
      -> Select& {
    constexpr auto column = ColumnMapping::Attribute::DatasourceId;
    builder_.with_eq_predicate(column, datasource_id, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Select() : builder_(std::string{table::ColumnMapping}) {}

  detail::SelectQueryBuilder builder_;
};

class Delete {
 public:
  static auto prepare() -> Delete { return Delete{}; }

  template <typename Sanitizer>
  auto by_datasource_id(std::uint64_t datasource_id, Sanitizer& sanitizer)
      -> Delete& {
    constexpr auto column = ColumnMapping::Attribute::DatasourceId;
    builder_.with_eq_predicate(column, datasource_id, sanitizer);
    return *this;
  }

  [[nodiscard]]
  auto compose() const -> std::string {
    return builder_.compose();
  }

 private:
  Delete() : builder_(std::string{table::ColumnMapping}) {}

  detail::DeleteQueryBuilder builder_;
};

}  // namespace column_mapping_query
}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_QUERIES_DATASOURCE_QUERIES_HPP_
