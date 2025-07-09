#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_ADAPTERS_POSTGRESQL_CONNECTOR_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_ADAPTERS_POSTGRESQL_CONNECTOR_HPP_

#include <cstdint>
#include <memory>
#include <pqxx/connection>
#include <pqxx/row>
#include <utility>
#include <vector>

#include "ih/historical/adapters/data_access_adapter.hpp"
#include "ih/historical/mapping/params.hpp"
#include "ih/historical/parsing/params.hpp"

namespace simulator::generator::historical {

class PostgresConnector : public historical::DataAccessAdapter {
 public:
  PostgresConnector() = delete;

  PostgresConnector(historical::DatabaseParsingParams params,
                    historical::MappingParams mapping_params,
                    pqxx::connection connection);

  [[nodiscard]]
  static auto create(historical::DatabaseParsingParams params,
                     historical::MappingParams mapping_params)
      -> std::unique_ptr<PostgresConnector>;

 private:
  [[nodiscard]]
  auto has_next_record() const noexcept -> bool override;

  auto parse_next_record(Record::Builder& builder) -> void override;

  auto load_data(pqxx::connection& database_connection) -> void;

  auto make_depth_config(pqxx::connection& database_connection) const
      -> mapping::DepthConfig;

  auto init_mapping_params(mapping::DepthConfig depth_config) -> void;

  auto extract_data() -> void;

  auto columns_number(pqxx::connection& database_connection) const
      -> std::uint32_t;

  MappingParams mapping_params_;
  DatabaseParsingParams parsing_params_;
  std::uint32_t depth_{0};

  pqxx::result data_;

  std::vector<std::pair<std::uint64_t, pqxx::row>> extracted_rows_;
  std::uint64_t next_row_index_{0};
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_ADAPTERS_POSTGRESQL_CONNECTOR_HPP_
