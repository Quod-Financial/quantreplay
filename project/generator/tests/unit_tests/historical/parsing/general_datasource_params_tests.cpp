#include <gtest/gtest.h>

#include "ih/constants.hpp"
#include "ih/historical/parsing/params.hpp"

namespace simulator::generator::historical::test {
namespace {

struct GeneratorHistoricalParsingGeneralDatasourceParams
    : public testing::Test {
  GeneralDatasourceParams general_params;
};

TEST_F(GeneratorHistoricalParsingGeneralDatasourceParams,
       DoesNotStoreDefaultName) {
  ASSERT_TRUE(general_params.name().empty());
}

TEST_F(GeneratorHistoricalParsingGeneralDatasourceParams, StoresName) {
  general_params.set_name("dummy");
  ASSERT_EQ(general_params.name(), "dummy");
}

TEST_F(GeneratorHistoricalParsingGeneralDatasourceParams,
       DoesNotStoreDefaultConnection) {
  ASSERT_TRUE(general_params.connection().empty());
}

TEST_F(GeneratorHistoricalParsingGeneralDatasourceParams, StoresConnection) {
  general_params.set_connection("dummy");
  ASSERT_EQ(general_params.connection(), "dummy");
}

TEST_F(GeneratorHistoricalParsingGeneralDatasourceParams,
       StoresAllDepthLevelsAsDefault) {
  ASSERT_EQ(general_params.max_depth_levels(),
            constant::historical::AllDepthLevels);
}

TEST_F(GeneratorHistoricalParsingGeneralDatasourceParams,
       StoresMaxDepthLevels) {
  general_params.set_max_depth_levels(42);
  ASSERT_EQ(general_params.max_depth_levels(), 42);
}

}  // namespace
}  // namespace simulator::generator::historical::test