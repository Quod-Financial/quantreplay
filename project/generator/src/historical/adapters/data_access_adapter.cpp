#include "ih/historical/adapters/data_access_adapter.hpp"

#include <cstddef>
#include <stdexcept>
#include <utility>

#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/adapters/csv_reader.hpp"
#include "ih/historical/adapters/postgresql_connector.hpp"
#include "ih/historical/data/instrument_filter.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/mapping/params.hpp"
#include "ih/historical/parsing/params.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

DataAccessAdapter::DataAccessAdapter(InstrumentFilter instrument_filter)
    : instrument_filter_{std::move(instrument_filter)} {}

auto DataAccessAdapter::accept(const RecordVisitor& visitor) noexcept -> void {
  std::size_t unlisted_records = 0;

  while (has_next_record()) {
    std::unique_ptr<Record::Builder> builder =
        std::make_unique<Record::BuilderImpl>();

    try {
      parse_next_record(builder);
    } catch (const std::exception& exception) {
      log::warn("failed to parse historical datasource row: {}",
                exception.what());
      continue;
    }

    try {
      Record record = builder->construct();
      if (!instrument_filter_.accepts(record.instrument())) {
        ++unlisted_records;
        continue;
      }
      visitor(std::move(record));
    } catch (const std::exception& exception) {
      log::warn("failed to process historical data record: {}",
                exception.what());
    }
  }

  if (unlisted_records != 0) {
    log::debug(
        "skipped {} historical data records of instruments not listed by the "
        "datasource",
        unlisted_records);
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
    case Format::Fix:
      throw std::runtime_error{
          "FIX format is not supported for static historical replay"};
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
