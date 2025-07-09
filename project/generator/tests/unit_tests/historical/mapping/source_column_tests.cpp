#include <gtest/gtest.h>

#include "ih/historical/mapping/specification.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {

TEST(GeneratorHistoricalMappingSourceColumn, StoresAssignedIndex) {
  constexpr SourceColumn source_column{42};
  ASSERT_EQ(source_column.index(), 42);
}

TEST(GeneratorHistoricalMappingSourceColumn, NumberIsOneMoreThanIndex) {
  constexpr SourceColumn source_column{42};
  ASSERT_EQ(source_column.number(), 43);
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test