#include "ih/historical/parsing/params.hpp"

#include <fmt/format.h>

#include <stdexcept>

#include "data_layer/api/models/datasource.hpp"

namespace simulator::generator::historical {
namespace {

auto make_general_params(const data_layer::Datasource& datasource)
    -> GeneralDatasourceParams {
  GeneralDatasourceParams general_params;
  general_params.set_name(datasource.name());
  general_params.set_connection(datasource.connection());

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