#include <gtest/gtest.h>

#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/parsing/params.hpp"

namespace simulator::generator::historical::test {
namespace {

auto make_datasource_patch() -> data_layer::Datasource::Patch {
  using Type = data_layer::Datasource::Type;

  data_layer::Datasource::Patch patch;
  patch.with_venue_id("dummy")
      .with_name("dummy")
      .with_connection("dummy")
      .with_type(Type::OrderBook);
  return patch;
}

auto make_datasource(const data_layer::Datasource::Patch& patch)
    -> data_layer::Datasource {
  return data_layer::Datasource::create(patch, 0);
}

struct GeneratorHistoricalParsingMakeCsvParsingParams : public ::testing::Test {
  GeneratorHistoricalParsingMakeCsvParsingParams()
      : patch(make_datasource_patch()) {
    patch.with_format(data_layer::Datasource::Format::Csv);
  }

  data_layer::Datasource::Patch patch;
};

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       ThrowsExceptionIfDataRowEqToHeaderRow) {
  patch.with_text_header_row(1).with_text_data_row(1);

  const auto datasource = make_datasource(patch);
  ASSERT_THROW(historical::make_csv_parsing_params(datasource),
               std::runtime_error);
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       ThrowsExceptionIfDataRowLessThanHeader) {
  patch.with_text_header_row(2).with_text_data_row(1);

  const auto datasource = make_datasource(patch);
  ASSERT_THROW(historical::make_csv_parsing_params(datasource),
               std::runtime_error);
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       MakesWithDefinedNameFromDatasource) {
  patch.with_name("csv_name");

  const auto datasource = make_datasource(patch);
  const auto params = make_csv_parsing_params(datasource);
  ASSERT_EQ(params.datasource_name(), "csv_name");
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       MakesWithDefinedConnectionFromDatasource) {
  patch.with_connection("file.csv");

  const auto datasource = make_datasource(patch);
  const auto params = make_csv_parsing_params(datasource);
  ASSERT_EQ(params.datasource_connection(), "file.csv");
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       MakesWithDefinedMaxDepthLevelsFromDatasource) {
  patch.with_max_depth_levels(42);

  const auto datasource = make_datasource(patch);
  const auto params = make_csv_parsing_params(datasource);
  ASSERT_EQ(params.datasource_max_depth_levels(), 42);
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       MakesWithDefaultHeaderRowIfNotSet) {
  const auto datasource = make_datasource(patch);
  const auto params = make_csv_parsing_params(datasource);
  ASSERT_EQ(params.header_row(), 0);
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       MakesWithDefinedHeaderRow) {
  patch.with_text_header_row(1).with_text_data_row(2);

  const auto datasource = make_datasource(patch);
  const auto params = make_csv_parsing_params(datasource);
  ASSERT_EQ(params.header_row(), 1);
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       MakesWithDefaultDataRowIfNotSet) {
  const auto datasource = make_datasource(patch);
  const auto params = make_csv_parsing_params(datasource);
  ASSERT_EQ(params.data_row(), 1);
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       MakesWithDefinedDataRow) {
  patch.with_text_data_row(2);

  const auto datasource = make_datasource(patch);
  const auto params = make_csv_parsing_params(datasource);
  ASSERT_EQ(params.data_row(), 2);
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       MakesWithDefaultDelimiterIfNotSet) {
  const auto datasource = make_datasource(patch);

  const auto params = make_csv_parsing_params(datasource);
  ASSERT_EQ(params.delimiter(), ',');
}

TEST_F(GeneratorHistoricalParsingMakeCsvParsingParams,
       MakesWithDefinedDelimiter) {
  patch.with_text_delimiter(';');

  const auto datasource = make_datasource(patch);
  const auto params = make_csv_parsing_params(datasource);
  ASSERT_EQ(params.delimiter(), ';');
}

struct GeneratorHistoricalParsingMakeDatabaseParsingParams
    : public ::testing::Test {
  GeneratorHistoricalParsingMakeDatabaseParsingParams()
      : patch(make_datasource_patch()) {
    patch.with_format(data_layer::Datasource::Format::Postgres);
  }

  data_layer::Datasource::Patch patch;
};

TEST_F(GeneratorHistoricalParsingMakeDatabaseParsingParams,
       ThrowsExceptionIfTableNameIsNotSet) {
  const auto datasource = make_datasource(patch);
  ASSERT_THROW(historical::make_database_parsing_params(datasource),
               std::runtime_error);
}

TEST_F(GeneratorHistoricalParsingMakeDatabaseParsingParams,
       MakesWithTableName) {
  patch.with_table_name("my_table");
  const auto datasource = make_datasource(patch);

  const auto params = make_database_parsing_params(datasource);
  ASSERT_EQ(params.table_name(), "my_table");
}

TEST_F(GeneratorHistoricalParsingMakeDatabaseParsingParams,
       MakesWithMaxDepthLevels) {
  patch.with_table_name("my_table").with_max_depth_levels(42);
  const auto datasource = make_datasource(patch);

  const auto params = make_database_parsing_params(datasource);
  ASSERT_EQ(params.datasource_max_depth_levels(), 42);
}

}  // namespace
}  // namespace simulator::generator::historical
