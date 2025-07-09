#include <gtest/gtest.h>

#include "ih/historical/parsing/params.hpp"

namespace simulator::generator::historical::test {
namespace {

struct GeneratorHistoricalParsingCsvParsingParams : public testing::Test {
  GeneralDatasourceParams general_params;
};

TEST_F(GeneratorHistoricalParsingCsvParsingParams, ReturnsDatasourceName) {
  general_params.set_name("Name");

  const CsvParsingParams params{general_params};
  ASSERT_EQ(params.datasource_name(), "Name");
}

TEST_F(GeneratorHistoricalParsingCsvParsingParams,
       ReturnsDatasourceConnection) {
  general_params.set_connection("Connection");

  const CsvParsingParams params{general_params};
  ASSERT_EQ(params.datasource_connection(), "Connection");
}

TEST_F(GeneratorHistoricalParsingCsvParsingParams,
       ReturnsDefaultMaxDepthLevels) {
  const CsvParsingParams params{general_params};
  ASSERT_EQ(params.datasource_max_depth_levels(),
            constant::historical::AllDepthLevels);
}

TEST_F(GeneratorHistoricalParsingCsvParsingParams, ReturnsMaxDepthLevels) {
  general_params.set_max_depth_levels(42);

  const CsvParsingParams params{general_params};
  ASSERT_EQ(params.datasource_max_depth_levels(), 42);
}

TEST_F(GeneratorHistoricalParsingCsvParsingParams, ReturnsDefaultHeaderRow) {
  const CsvParsingParams params{general_params};

  ASSERT_EQ(params.header_row(), 0);
  ASSERT_FALSE(params.has_header_row());
}

TEST_F(GeneratorHistoricalParsingCsvParsingParams, ReturnsHeaderRow) {
  CsvParsingParams params{general_params};
  params.set_header_row(42);

  ASSERT_EQ(params.header_row(), 42);
  ASSERT_TRUE(params.has_header_row());
}

TEST_F(GeneratorHistoricalParsingCsvParsingParams, ReturnsDefaultDataRow) {
  const CsvParsingParams params{general_params};

  ASSERT_EQ(params.data_row(), 1);
}

TEST_F(GeneratorHistoricalParsingCsvParsingParams, ReturnsDataRow) {
  CsvParsingParams params{general_params};
  params.set_data_row(42);

  ASSERT_EQ(params.data_row(), 42);
}

TEST_F(GeneratorHistoricalParsingCsvParsingParams, ReturnsDefaultDelimiter) {
  const CsvParsingParams params{general_params};

  ASSERT_EQ(params.delimiter(), ',');
}

TEST_F(GeneratorHistoricalParsingCsvParsingParams, ReturnsDelimiter) {
  CsvParsingParams params{general_params};
  params.set_delimiter(';');

  ASSERT_EQ(params.delimiter(), ';');
}

}  // namespace
}  // namespace simulator::generator::historical::test