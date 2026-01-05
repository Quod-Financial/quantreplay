#ifndef SIMULATOR_GENERATOR_TESTS_TEST_UTILS_HISTORICAL_MAPPING_UTILS_HPP_
#define SIMULATOR_GENERATOR_TESTS_TEST_UTILS_HISTORICAL_MAPPING_UTILS_HPP_

#include <gmock/gmock.h>

#include "data_layer/api/models/column_mapping.hpp"

namespace simulator::generator::historical::mapping {

using namespace ::testing;

inline auto make_column_config(data_layer::converter::ColumnFrom column_from,
                               std::string column_to)
    -> std::pair<const data_layer::converter::ColumnFrom, std::string> {
  return std::make_pair<const data_layer::converter::ColumnFrom, std::string>(
      std::move(column_from), std::move(column_to));
}

MATCHER_P(Resolves, column_from, "") {
  return arg.resolve_by(column_from).has_value();
}

MATCHER_P2(ResolvesColumnFromAndIndexIs, column_from, index, "") {
  const auto resolved = arg.resolve_by(column_from);
  return ExplainMatchResult(Ne(std::nullopt), resolved, result_listener) &&
         ExplainMatchResult(Eq(index), resolved->index(), result_listener);
}

}  // namespace simulator::generator::historical::mapping

#endif  // SIMULATOR_GENERATOR_TESTS_TEST_UTILS_HISTORICAL_MAPPING_UTILS_HPP_
