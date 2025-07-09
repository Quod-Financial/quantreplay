#include "ih/historical/adapters/data_access_adapter.hpp"

#include <stdexcept>
#include <utility>

#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/adapters/csv_reader.hpp"
#include "ih/historical/adapters/postgresql_connector.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/mapping/params.hpp"
#include "ih/historical/parsing/params.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

auto DataAccessAdapter::accept(const RecordVisitor& visitor) noexcept -> void {
  while (has_next_record()) {
    Record::Builder builder{};

    try {
      parse_next_record(builder);
    } catch (const std::exception& exception) {
      log::warn("failed to parse historical datasource row: {}",
                exception.what());
      continue;
    }

    try {
      Record record = Record::Builder::construct(std::move(builder));
      visitor(std::move(record));
    } catch (const std::exception& exception) {
      log::warn("failed to process historical data record: {}",
                exception.what());
    }
  }
}

auto DataAccessAdapterFactoryImpl::create_data_adapter(
    const data_layer::Datasource& datasource) const
    -> std::unique_ptr<DataAccessAdapter> {
  using Format = data_layer::Datasource::Format;
  switch (datasource.format()) {
    case Format::Csv:
      return create_csv_reader(datasource);
    case Format::Postgres:
      return create_postgres_db_reader(datasource);
  }

  throw std::runtime_error{"unable to determine datasource format"};
}

auto DataAccessAdapterFactoryImpl::create_csv_reader(
    const data_layer::Datasource& datasource)
    -> std::unique_ptr<DataAccessAdapter> {
  CsvParsingParams parsing = make_csv_parsing_params(datasource);
  MappingParams mapping = make_mapping_params(datasource);
  return CsvReader::create(std::move(parsing), std::move(mapping));
}

auto DataAccessAdapterFactoryImpl::create_postgres_db_reader(
    const data_layer::Datasource& datasource)
    -> std::unique_ptr<DataAccessAdapter> {
  DatabaseParsingParams parsing = make_database_parsing_params(datasource);
  MappingParams mapping = make_mapping_params(datasource);
  return PostgresConnector::create(std::move(parsing), std::move(mapping));
}

}  // namespace simulator::generator::historical
