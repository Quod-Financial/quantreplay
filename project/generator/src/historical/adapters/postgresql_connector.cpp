#include "ih/historical/adapters/postgresql_connector.hpp"

#include <fmt/format.h>

#include <memory>
#include <pqxx/connection>
#include <pqxx/result>
#include <pqxx/row>
#include <pqxx/transaction>
#include <string>
#include <string_view>
#include <vector>

#include "ih/historical/parsing/params.hpp"
#include "ih/historical/parsing/parsing.hpp"
#include "ih/historical/parsing/row.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

PostgresConnector::PostgresConnector(historical::DatabaseParsingParams params,
                                     historical::MappingParams mapping_params,
                                     pqxx::connection connection)
    : mapping_params_{std::move(mapping_params)},
      parsing_params_{std::move(params)} {
  load_data(connection);

  auto depth_config = make_depth_config(connection);
  depth_ = depth_config.datasource_depth;

  init_mapping_params(std::move(depth_config));
  extract_data();
}

auto PostgresConnector::create(historical::DatabaseParsingParams params,
                               historical::MappingParams mapping_params)
    -> std::unique_ptr<PostgresConnector> {
  pqxx::connection connection{params.datasource_connection()};
  return std::make_unique<PostgresConnector>(
      std::move(params), std::move(mapping_params), std::move(connection));
}

auto PostgresConnector::has_next_record() const noexcept -> bool {
  return next_row_index_ < extracted_rows_.size();
}

auto PostgresConnector::parse_next_record(Record::Builder& builder) -> void {
  assert(has_next_record());

  auto& [row_number, pq_row] = extracted_rows_[next_row_index_++];

  builder.with_source_row(row_number)
      .with_source_name(parsing_params_.datasource_name())
      .with_source_connection(parsing_params_.datasource_connection());

  const historical::Row row = historical::Row::from(pq_row);
  historical::parse(row, builder, mapping_params_, depth_);
}

auto PostgresConnector::load_data(pqxx::connection& database_connection)
    -> void {
  pqxx::work transaction{database_connection};
  data_ = transaction.exec(
      fmt::format("SELECT * FROM {};", parsing_params_.table_name()));
}

auto PostgresConnector::make_depth_config(
    pqxx::connection& database_connection) const -> mapping::DepthConfig {
  const auto data_depth =
      mapping::depth_from_columns_number(columns_number(database_connection));
  const auto depth_to_parse = mapping::depth_to_parse(
      data_depth, parsing_params_.datasource_max_depth_levels());
  return {.datasource_depth = data_depth, .depth_to_parse = depth_to_parse};
}

auto PostgresConnector::init_mapping_params(mapping::DepthConfig depth_config)
    -> void {
  const std::int32_t columns_count = data_.columns();

  std::vector<std::string> column_names{};
  column_names.reserve(static_cast<std::uint32_t>(columns_count));

  for (std::int32_t row_idx = 0; row_idx < columns_count; ++row_idx) {
    const std::string_view column_name = data_.column_name(row_idx);
    column_names.emplace_back(column_name);
  }

  mapping_params_.initialize(std::move(column_names), std::move(depth_config));
}

auto PostgresConnector::extract_data() -> void {
  std::uint64_t row_number{1};
  for (pqxx::row row : data_) {
    extracted_rows_.emplace_back(row_number++, std::move(row));
  }
}

auto PostgresConnector::columns_number(
    pqxx::connection& database_connection) const -> std::uint32_t {
  pqxx::work transaction{database_connection};
  pqxx::result result = transaction.exec(
      fmt::format("SELECT * FROM {} LIMIT 1;", parsing_params_.table_name()));

  const auto number = static_cast<std::uint32_t>(result.columns());
  log::debug("Selected number of columns {}.", number);
  return number;
}

}  // namespace simulator::generator::historical
