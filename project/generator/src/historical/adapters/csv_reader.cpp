#include "ih/historical/adapters/csv_reader.hpp"

#include <algorithm>
#include <csv.hpp>
#include <memory>
#include <string>
#include <vector>

#include "ih/historical/data/record.hpp"
#include "ih/historical/mapping/params.hpp"
#include "ih/historical/parsing/parsing.hpp"
#include "ih/historical/parsing/row.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

CsvReader::CsvReader(historical::CsvParsingParams params,
                     MappingParams mapping_params,
                     csv::CSVReader& reader)
    : parsing_params_(std::move(params)),
      mapping_params_{std::move(mapping_params)} {
  load_csv_data(reader);

  auto depth_config = make_depth_config(reader);
  depth_ = depth_config.depth_to_parse;
  init_mapping_params(reader, std::move(depth_config));
}

auto CsvReader::create(historical::CsvParsingParams parsing_params,
                       historical::MappingParams mapping_params)
    -> std::unique_ptr<CsvReader> {
  csv::CSVFormat format = make_format(parsing_params);
  csv::CSVReader csv_reader{parsing_params.datasource_connection(),
                            std::move(format)};

  return std::make_unique<CsvReader>(
      std::move(parsing_params), std::move(mapping_params), csv_reader);
}

auto CsvReader::make_format(const historical::CsvParsingParams& parsing_params)
    -> csv::CSVFormat {
  csv::CSVFormat format{};
  format.variable_columns(csv::VariableColumnPolicy::KEEP)
      .delimiter(parsing_params.delimiter());

  if (parsing_params.has_header_row()) {
    const auto header_num = parsing_params.header_row();
    const auto header_idx = static_cast<std::int32_t>(header_num - 1);
    format.header_row(header_idx);
  } else {
    format.no_header();
  }

  return format;
}

auto CsvReader::has_next_record() const noexcept -> bool {
  return next_row_index_ < extracted_rows_.size();
}

auto CsvReader::parse_next_record(historical::Record::Builder& builder)
    -> void {
  assert(has_next_record());

  auto [row_number, csv_row] = extracted_rows_[next_row_index_++];

  builder.with_source_row(row_number)
      .with_source_name(parsing_params_.datasource_name())
      .with_source_connection(parsing_params_.datasource_connection());

  const historical::Row row = historical::Row::from(csv_row);
  historical::parse(row, builder, mapping_params_, depth_);
}

auto CsvReader::make_depth_config(const csv::CSVReader& reader) const
    -> mapping::DepthConfig {
  const auto data_depth =
      mapping::depth_from_columns_number(columns_number(reader));
  const auto depth_to_parse = mapping::depth_to_parse(
      data_depth, parsing_params_.datasource_max_depth_levels());
  return {.datasource_depth = data_depth, .depth_to_parse = depth_to_parse};
}

auto CsvReader::init_mapping_params(const csv::CSVReader& reader,
                                    mapping::DepthConfig depth_config) -> void {
  if (parsing_params_.has_header_row()) {
    std::vector<std::string> columns_names = reader.get_col_names();
    mapping_params_.initialize(std::move(columns_names),
                               std::move(depth_config));
  } else {
    mapping_params_.initialize(std::move(depth_config));
  }
}

auto CsvReader::load_csv_data(csv::CSVReader& reader) -> void {
  // The csv parsing library trims all rows before header row and a header row
  // itself. Thus, sentinel represents a 'global' row number with respect to
  // possibly-trimmed rows. Sentinel itself is initialized as index (with 0),
  // but is interpreted as a global row number inside a reading loop.

  const std::uint64_t data_row_number = parsing_params_.data_row();
  std::uint64_t row_sentinel = [&]() -> std::uint64_t {
    if (parsing_params_.has_header_row()) {
      return parsing_params_.header_row();
    }
    return 0;
  }();

  std::for_each(std::begin(reader), std::end(reader), [&](csv::CSVRow& row) {
    if (++row_sentinel >= data_row_number) {
      extracted_rows_.emplace_back(row_sentinel, std::move(row));
    }
  });
}

auto CsvReader::columns_number(const csv::CSVReader& reader) const
    -> std::uint32_t {
  if (parsing_params_.has_header_row()) {
    const auto number =
        static_cast<std::uint32_t>(reader.get_col_names().size());
    log::debug("Got a number of columns {} from the header row.", number);
    return number;
  }

  if (!extracted_rows_.empty()) {
    const auto number =
        static_cast<std::uint32_t>(extracted_rows_.front().second.size());
    log::debug("Got a number of columns {} from the first data row.", number);
    return number;
  }

  log::debug("Header and data rows are absent. Number of columns is 0.");
  return 0;
}

}  // namespace simulator::generator::historical
