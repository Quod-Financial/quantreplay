#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/mapping/configurator.hpp"
#include "ih/historical/mapping/specification.hpp"
#include "test_utils/historical_mapping_utils.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {
using namespace ::testing;

struct GeneratorHistoricalMappingConfiguratorVariableDepth
    : public testing::Test {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  inline static const std::map<data_layer::converter::ColumnFrom, std::string>
      FullColumnsConfig{
          make_column_config(ColumnFrom::create("Instrument").value(),
                             "Symbol"),
          make_column_config(ColumnFrom::create("ReceivedTimeStamp").value(),
                             "Time"),
          make_column_config(ColumnFrom::create("MessageTimeStamp").value(),
                             "MsgTime"),
          make_column_config(ColumnFrom::create("BidPrice#").value(), "BidPx#"),
          make_column_config(ColumnFrom::create("BidQuantity#").value(),
                             "BidQty#"),
          make_column_config(ColumnFrom::create("AskPrice#").value(), "AskPx#"),
          make_column_config(ColumnFrom::create("AskQuantity#").value(),
                             "AskQty#"),
          make_column_config(ColumnFrom::create("BidParty#").value(), "BidCP#"),
          make_column_config(ColumnFrom::create("AskParty#").value(), "AskCP#"),
      };

  inline static const Configurator::SourceColumnNames FullOneLevelColumnNames{
      "Symbol",
      "Time",
      "MsgTime",
      "BidPx1",
      "BidQty1",
      "AskPx1",
      "AskQty1",
      "BidCP1",
      "AskCP1"};

  inline static const Configurator::SourceColumnNames
      FullLevelOnePartialLevelTwoColumnNames{"Symbol",
                                             "Time",
                                             "MsgTime",
                                             "BidPx1",
                                             "BidQty1",
                                             "AskPx1",
                                             "AskQty1",
                                             "BidCP1",
                                             "AskCP1",
                                             "BidPx2",
                                             "BidQty2",
                                             "AskPx2",
                                             "BidCP2",
                                             "AskCP2"};

  inline static const Configurator::SourceColumnNames FullTwoLevelsColumnNames{
      "Symbol",
      "Time",
      "MsgTime",
      "BidPx1",
      "BidQty1",
      "AskPx1",
      "AskQty1",
      "BidCP1",
      "AskCP1",
      "BidPx2",
      "BidQty2",
      "AskPx2",
      "AskQty2",
      "BidCP2",
      "AskCP2"};

  static auto make_configurator(Configurator::SourceColumnNames columns_names,
                                std::uint32_t max_depth_levels)
      -> Configurator {
    return Configurator{std::move(columns_names), max_depth_levels};
  }

  tl::expected<Specification, std::string> spec;
};

TEST_F(GeneratorHistoricalMappingConfiguratorVariableDepth,
       ReturnsErrorWhenNotAllRequiredLevelColumnsMappingAreDefined) {
  auto configurator = make_configurator(
      {
          "Symbol",
          "Time",
          "MsgTime",
          "BidPx#",
          "BidQty#",
          "AskPx#",
          "AskQty#",
      },
      data_layer::Datasource::AllDepthLevels);

  // clang-format off
  spec = configurator.configure({
    make_column_config(ColumnFrom::create("Instrument").value(), "Symbol"),
    make_column_config(ColumnFrom::create("ReceivedTimeStamp").value(), "Time"),
    make_column_config(ColumnFrom::create("MessageTimeStamp").value(), "MsgTime"),
    make_column_config(ColumnFrom::create("BidQuantity#").value(), "BidQty#"),
    make_column_config(ColumnFrom::create("AskPrice#").value(), "AskPx#"),
    make_column_config(ColumnFrom::create("AskQuantity#").value(), "AskQty#"),
  });
  // clang-format on

  ASSERT_THAT(spec,
              IsUnexpected("variable depth must be set for all level columns: "
                           "missing Column From `BidPrice'"));
}

TEST_F(GeneratorHistoricalMappingConfiguratorVariableDepth,
       AppliesAssociationForLevelOneIfMaxDepthLevelsIsAll) {
  auto configurator = make_configurator(FullOneLevelColumnNames,
                                        data_layer::Datasource::AllDepthLevels);

  spec = configurator.configure(FullColumnsConfig);

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorVariableDepth,
       AppliesAssociationForTwoLevelsIfMaxDepthLevelsIsAll) {
  auto configurator = make_configurator(FullTwoLevelsColumnNames,
                                        data_layer::Datasource::AllDepthLevels);

  spec = configurator.configure(FullColumnsConfig);

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 2u).value(), 9));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 2u).value(), 10));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 2u).value(), 11));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 2u).value(), 12));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value(), 13));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 2u).value(), 14));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingConfiguratorVariableDepth,
    ReturnsErrorIfSecondLevelIsNotFullyListedInColumnNamesAndIfMaxDepthLevelsIsAll) {
  auto configurator = make_configurator(FullLevelOnePartialLevelTwoColumnNames,
                                        data_layer::Datasource::AllDepthLevels);

  spec = configurator.configure(FullColumnsConfig);

  ASSERT_THAT(spec, IsUnexpected("Column To `AskQty2' does not exist"));
}

TEST_F(GeneratorHistoricalMappingConfiguratorVariableDepth,
       AppliesAssociationLevelOneIfMaxDepthLevelsIsOne) {
  auto configurator = make_configurator(FullOneLevelColumnNames, 1);

  spec = configurator.configure(FullColumnsConfig);

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorVariableDepth,
       DoesNotAppliesAssociationLevelTwoIfMaxDepthLevelsIsOne) {
  auto configurator = make_configurator(FullTwoLevelsColumnNames, 1);

  spec = configurator.configure(FullColumnsConfig);

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 2u).value())));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingConfiguratorVariableDepth,
    AppliesAssociationForLevelOneIfSecondLevelIsNotFullyListedAndMaxDepthLevelsIsOne) {
  auto configurator =
      make_configurator(FullLevelOnePartialLevelTwoColumnNames, 1);

  spec = configurator.configure(FullColumnsConfig);

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingConfiguratorVariableDepth,
    AppliesAssociationForOneLevelIfOnlyLevelOneDefinedAndMaxDepthLevelsIsTwo) {
  auto configurator = make_configurator(FullOneLevelColumnNames, 2);

  spec = configurator.configure(FullColumnsConfig);

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 2u).value())));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingConfiguratorVariableDepth,
    ReturnsErrorIfSecondLevelIsNotFullyListedInColumnNamesAndIfMaxDepthLevelsIsTwo) {
  auto configurator =
      make_configurator(FullLevelOnePartialLevelTwoColumnNames, 2);

  spec = configurator.configure(FullColumnsConfig);

  ASSERT_THAT(spec, IsUnexpected("Column To `AskQty2' does not exist"));
}

TEST_F(
    GeneratorHistoricalMappingConfiguratorVariableDepth,
    AppliesAssociationForLevelsOneAndThreeIfLevelTwoNotListedInColumnNamesAndMaxDepthLevelsIsAll) {
  auto configurator = make_configurator({"Symbol",
                                         "Time",
                                         "MsgTime",
                                         "BidPx1",
                                         "BidQty1",
                                         "AskPx1",
                                         "AskQty1",
                                         "BidCP1",
                                         "AskCP1",
                                         "BidPx3",
                                         "BidQty3",
                                         "AskPx3",
                                         "AskQty3",
                                         "BidCP3",
                                         "AskCP3"},
                                        data_layer::Datasource::AllDepthLevels);

  spec = configurator.configure(FullColumnsConfig);

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 2u).value())));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 3u).value(), 9));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 3u).value(), 10));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 3u).value(), 11));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 3u).value(), 12));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 3u).value(), 13));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 3u).value(), 14));
  // clang-format on
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test
