#include "ih/historical/parsing/params.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/datasource_listing.hpp"
#include "ih/historical/data/instrument_filter.hpp"

namespace simulator::generator::historical {
namespace {

auto make_instrument_filter(const data_layer::Datasource& datasource)
    -> InstrumentFilter {
  const std::vector<data_layer::DatasourceListing>& listings =
      datasource.listings();
  std::vector<std::string> listed_instruments;
  listed_instruments.reserve(listings.size());
  std::ranges::transform(listings,
                         std::back_inserter(listed_instruments),
                         &data_layer::DatasourceListing::symbol);
  return InstrumentFilter{std::move(listed_instruments)};
}

auto make_general_params(const data_layer::Datasource& datasource)
    -> GeneralDatasourceParams {
  GeneralDatasourceParams general_params;
  general_params.set_name(datasource.name());
  general_params.set_connection(datasource.connection());
  general_params.set_instrument_filter(make_instrument_filter(datasource));

  if (const auto& depth = datasource.max_depth_levels()) {
    general_params.set_max_depth_levels(depth.value());
  }
  return general_params;
}

}  // namespace

auto make_csv_parsing_params(const data_layer::Datasource& datasource)
    -> CsvParsingParams {
  CsvParsingParams params{make_general_params(datasource)};

  if (const auto header_row = datasource.text_header_row()) {
    params.set_header_row(*header_row);
  }
  if (const auto data_row = datasource.text_data_row()) {
    params.set_data_row(*data_row);
  }
  if (const auto delim = datasource.text_delimiter()) {
    params.set_delimiter(*delim);
  }

  if (params.data_row() <= params.header_row()) {
    throw std::runtime_error{fmt::format(
        "can not create csv parsing params, "
        "bad data row value `{}' (value must be greater than header row)",
        params.data_row())};
  }

  return params;
}

auto make_database_parsing_params(const data_layer::Datasource& datasource)
    -> DatabaseParsingParams {
  DatabaseParsingParams params{make_general_params(datasource)};

  if (const auto& table_name = datasource.table_name()) {
    params.set_table_name(*table_name);
  } else {
    throw std::runtime_error{
        "can not create database parsing params, "
        "required Datasouce::TableName attribute is missing"};
  }

  return params;
}

}  // namespace simulator::generator::historical