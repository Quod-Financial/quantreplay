#include <gtest/gtest.h>

#include "ih/constants.hpp"
#include "ih/historical/parsing/params.hpp"

namespace simulator::generator::historical::test {
namespace {

TEST(GeneratorHistoricalParsingDatabaseParsingParams, ReturnsName) {
  GeneralDatasourceParams general_params;
  general_params.set_name("Name");

  const DatabaseParsingParams params{general_params};
  ASSERT_EQ(params.datasource_name(), "Name");
}

TEST(GeneratorHistoricalParsingDatabaseParsingParams, ReturnsConnection) {
  GeneralDatasourceParams general_params;
  general_params.set_connection("connection_string");

  const DatabaseParsingParams params{general_params};
  ASSERT_EQ(params.datasource_connection(), "connection_string");
}

TEST(GeneratorHistoricalParsingDatabaseParsingParams, ReturnsMaxDepthLevels) {
  GeneralDatasourceParams general_params;
  general_params.set_max_depth_levels(42);

  const DatabaseParsingParams params{general_params};
  ASSERT_EQ(params.datasource_max_depth_levels(), 42);
}

TEST(GeneratorHistoricalParsingDatabaseParsingParams,
     DoesNotStoreDefaultTableName) {
  const DatabaseParsingParams params{{}};
  ASSERT_TRUE(params.table_name().empty());
}

TEST(GeneratorHistoricalParsingDatabaseParsingParams,
     StoresDefaultMaxDepthLevels) {
  const DatabaseParsingParams params{{}};
  ASSERT_EQ(params.datasource_max_depth_levels(),
            constant::historical::AllDepthLevels);
}

TEST(GeneratorHistoricalParsingDatabaseParsingParams, ReturnsTableName) {
  DatabaseParsingParams params{{}};
  params.set_table_name("my_table");

  ASSERT_EQ(params.table_name(), "my_table");
}

}  // namespace
}  // namespace simulator::generator::historical::test