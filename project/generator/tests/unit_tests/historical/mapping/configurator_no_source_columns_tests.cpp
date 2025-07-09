#include <gmock/gmock.h>

#include "ih/historical/mapping/configurator.hpp"
#include "ih/historical/mapping/specification.hpp"
#include "test_utils/historical_mapping_utils.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {

using namespace ::testing;

struct GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV
    : public testing::Test {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  auto make_configurator(DepthConfig depth_config) -> Configurator {
    return Configurator{
        spec, {}, std::move(depth_config), DatasourceParams::CsvNoHeader};
  }

  Specification spec;
};

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       KeepsDefaultAssociationWhenColumnToIsNotANumber) {
  const auto target = ColumnFrom::MessageTimestamp;
  constexpr std::uint64_t default_message_timestamp_source_idx = 1;

  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"Not234A321Number"};

  auto configurator = make_configurator({1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec,
              ResolvesColumnFromAndIndexIs(
                  target, default_message_timestamp_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       KeepsDefaultAssociationWhenColumnToIsNegativeNumber) {
  const auto target = ColumnFrom::MessageTimestamp;
  constexpr std::uint64_t default_message_timestamp_source_idx = 1;

  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"-1"};

  auto configurator = make_configurator({1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec,
              ResolvesColumnFromAndIndexIs(
                  target, default_message_timestamp_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       KeepsDefaultAssociationWhenColumnToIsZero) {
  const auto affected_target = ColumnFrom::MessageTimestamp;
  constexpr std::uint64_t default_message_timestamp_source_idx = 1;

  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"0"};

  auto configurator = make_configurator({1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  // Zero column number is not valid as a 1-based column_to column number
  ASSERT_THAT(spec,
              ResolvesColumnFromAndIndexIs(
                  affected_target, default_message_timestamp_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       KeepsDefaultAssociationWhenColumnToStartsWithNumber) {
  const auto affected_target = ColumnFrom::MessageTimestamp;
  constexpr std::uint64_t default_message_timestamp_source_idx = 1;

  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"6T"};

  auto configurator = make_configurator({1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec,
              ResolvesColumnFromAndIndexIs(
                  affected_target, default_message_timestamp_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForReceivedTimestampWithoutDepthAndDepthToParseIsOne) {
  const auto target = ColumnFrom::ReceivedTimestamp;

  const auto column_from = ColumnFrom::create("ReceivedTimeStamp").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForMessageTimestampWithoutDepthAndDepthToParseIsOne) {
  const auto target = ColumnFrom::MessageTimestamp;

  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForInstrumentWithoutDepthAndDepthToParseIsOne) {
  const auto target = ColumnFrom::Instrument;

  const auto column_from = ColumnFrom::create("Instrument").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForBidPartyWithDepthIsLessThanDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value();

  const auto column_from = ColumnFrom::create("BidParty1").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForBidPartyWhenItsDepthIsEqualToDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value();

  const auto column_from = ColumnFrom::create("BidParty2").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForBidQuantityWithDepthIsLessThanDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value();

  const auto column_from = ColumnFrom::create("BidQuantity1").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForBidQuantityWhenItsDepthIsEqualToDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::BidQuantity, 2u).value();

  const auto column_from = ColumnFrom::create("BidQuantity2").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForBidPriceWithDepthIsLessThanDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value();

  const auto column_from = ColumnFrom::create("BidPrice1").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForBidPriceWhenItsDepthIsEqualToDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::BidPrice, 2u).value();

  const auto column_from = ColumnFrom::create("BidPrice2").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForOfferPriceWithDepthIsLessThanDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value();

  const auto column_from = ColumnFrom::create("AskPrice1").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForOfferPriceWhenItsDepthIsEqualToDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::OfferPrice, 2u).value();

  const auto column_from = ColumnFrom::create("AskPrice2").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForOfferQuantityWithDepthIsLessThanDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value();

  const auto column_from = ColumnFrom::create("AskQuantity1").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForOfferQuantityWhenItsDepthIsEqualToDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 2u).value();

  const auto column_from = ColumnFrom::create("AskQuantity2").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForOfferPartyWithDepthIsLessThanDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value();

  const auto column_from = ColumnFrom::create("AskParty1").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoSourceColumnsCSV,
       AppliesAssociationForOfferPartyWhenItsDepthIsEqualToDepthToParse) {
  const auto target =
      ColumnFrom::create(ColumnFrom::Column::OfferParty, 2u).value();

  const auto column_from = ColumnFrom::create("AskParty2").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator({3, 2});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, new_source_idx));
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test
