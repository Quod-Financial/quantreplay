#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_DATASOURCE_PARSER_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_DATASOURCE_PARSER_HPP_

#include <pqxx/row>

#include "api/exceptions/exceptions.hpp"
#include "api/inspectors/column_mapping.hpp"
#include "api/inspectors/datasource.hpp"
#include "api/models/column_mapping.hpp"
#include "api/models/datasource.hpp"
#include "ih/pqxx/result/detail/basic_row_parser.hpp"

namespace simulator::data_layer::internal_pqxx {

class ColumnMappingParser {
 public:
  explicit ColumnMappingParser(const pqxx::row& database_row) noexcept
      : row_parser_(database_row) {}

  auto parse_into(ColumnMapping::Patch& destination_patch) -> void {
    ColumnMappingPatchWriter<decltype(row_parser_)> writer{row_parser_};
    writer.write(destination_patch);
  }

  static auto parse(const pqxx::row& database_row) -> ColumnMapping::Patch {
    ColumnMapping::Patch parsed{};
    ColumnMappingParser parser{database_row};
    parser.parse_into(parsed);
    return parsed;
  }

 private:
  detail::BasicRowParser row_parser_;
};

class DatasourceParser {
 public:
  explicit DatasourceParser(const pqxx::row& database_row) noexcept
      : row_parser_(database_row) {}

  auto parse_into(Datasource::Patch& destination_patch) -> void {
    DatasourcePatchWriter<decltype(row_parser_)> writer{row_parser_};
    writer.write(destination_patch);
  }

  static auto parse(const pqxx::row& database_row) -> Datasource::Patch {
    Datasource::Patch parsed{};
    DatasourceParser parser{database_row};
    parser.parse_into(parsed);
    return parsed;
  }

  static auto parse_identifier(const pqxx::row& database_row) -> std::uint64_t {
    detail::BasicRowParser row_parser{database_row};

    std::uint64_t datasource_id{};
    if (row_parser(Datasource::Attribute::DatasourceId, datasource_id)) {
      return datasource_id;
    }

    throw DataDecodingError(
        "failed to decode a datasource record identifier "
        " from the database row");
  }

 private:
  detail::BasicRowParser row_parser_;
};

}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_RESULT_DATASOURCE_PARSER_HPP_
