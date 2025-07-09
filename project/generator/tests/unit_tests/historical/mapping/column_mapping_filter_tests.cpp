#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/historical/mapping/column_mapping_filter.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {

using namespace ::testing;

auto column_mapping(std::string column_from, std::string column_to)
    -> data_layer::ColumnMapping {
  data_layer::ColumnMapping::Patch patch;
  patch.with_column_from(std::move(column_from))
      .with_column_to(std::move(column_to));
  return data_layer::ColumnMapping::create(std::move(patch), 0);
}

auto column_config(data_layer::converter::ColumnFrom::Column column_from,
                   data_layer::converter::ColumnFrom::Depth depth,
                   std::string column_to)
    -> data_layer::converter::ColumnConfig {
  return data_layer::converter::ColumnConfig{
      data_layer::converter::ColumnFrom::create(column_from, std::move(depth))
          .value(),
      std::move(column_to),
      0};
}

TEST(GeneratorHistoricalMappingColumnMappingFilter,
     ReturnsTheSameMappingWhenNoVariableDepth) {
  using ColumnFrom = data_layer::converter::ColumnFrom;
  const std::vector<data_layer::ColumnMapping> mappings{
      column_mapping("ReceivedTimeStamp", "timestamp"),
      column_mapping("BidParty42", "bid_party")};

  const auto filtered = filter(mappings, 42);
  ASSERT_THAT(filtered,
              ElementsAre(column_config(ColumnFrom::Column::ReceivedTimestamp,
                                        ColumnFrom::NoDepth{},
                                        "timestamp"),
                          column_config(
                              ColumnFrom::Column::BidParty, 42u, "bid_party")));
}

TEST(GeneratorHistoricalMappingColumnMappingFilter, UnfoldsVariableDepth) {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  const std::vector<data_layer::ColumnMapping> mappings{
      column_mapping("BidParty#", "bid_party#")};

  const auto filtered = filter(mappings, 2);
  ASSERT_THAT(
      filtered,
      ElementsAre(
          column_config(ColumnFrom::Column::BidParty, 1u, "bid_party1"),
          column_config(ColumnFrom::Column::BidParty, 2u, "bid_party2")));
}

TEST(GeneratorHistoricalMappingColumnMappingFilter,
     IgnoresDepthMappingIfVariableDepthIsSet) {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  const std::vector<data_layer::ColumnMapping> mappings{
      column_mapping("BidParty#", "bid_party#"),
      column_mapping("BidParty1", "ignored1"),
      column_mapping("BidParty2", "ignored2"),
  };

  const auto filtered = filter(mappings, 2);
  ASSERT_THAT(
      filtered,
      ElementsAre(
          column_config(ColumnFrom::Column::BidParty, 1u, "bid_party1"),
          column_config(ColumnFrom::Column::BidParty, 2u, "bid_party2")));
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test
