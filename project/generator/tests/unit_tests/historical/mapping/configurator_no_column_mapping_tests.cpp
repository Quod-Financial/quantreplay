#include <gmock/gmock.h>

#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/mapping/configurator.hpp"
#include "ih/historical/mapping/specification.hpp"
#include "test_utils/historical_mapping_utils.hpp"
#include "tests/test_utils/matchers.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {

using namespace ::testing;

struct GeneratorHistoricalMappingConfiguratorNoColumnMapping
    : public testing::Test {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  inline static const Configurator::SourceColumnNames FullOneLevelColumnNames{
      "ReceivedTimeStamp",
      "MessageTimeStamp",
      "Instrument",
      "BidParty",
      "BidQuantity",
      "BidPrice",
      "AskPrice",
      "AskQuantity",
      "AskParty"};

  static auto make_configurator(Configurator::SourceColumnNames column_names)
      -> Configurator {
    return Configurator{std::move(column_names),
                        data_layer::Datasource::AllDepthLevels};
  }

  tl::expected<Specification, std::string> spec;
};

TEST_F(GeneratorHistoricalMappingConfiguratorNoColumnMapping,
       ReturnsErrorWhenAtLeastOneColumnNameIsNotDefault) {
  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidParty",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "not_default_ask_qty",
                                         "AskParty"});
  spec = configurator.configure({});

  ASSERT_THAT(spec, IsUnexpected("Column To `AskQuantity' does not exist"));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoColumnMapping,
       ConfiguresSpecificationIfPartyColumnsExist) {
  auto configurator = make_configurator(FullOneLevelColumnNames);
  spec = configurator.configure({});

  ASSERT_TRUE(spec.has_value());
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoColumnMapping,
       ConfiguresSpecificationIfPartyColumnsDoNotExist) {
  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({});

  ASSERT_TRUE(spec.has_value());
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoColumnMapping,
       ConfiguresReceivedTimeStampUsingDefaultColumnName) {
  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({});

  ASSERT_THAT(spec,
              IsExpected(ResolvesColumnFromAndIndexIs(
                  ColumnFrom::ReceivedTimestamp, 0)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoColumnMapping,
       ConfiguresMessageTimeStampUsingDefaultColumnName) {
  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({});

  ASSERT_THAT(spec,
              IsExpected(ResolvesColumnFromAndIndexIs(
                  ColumnFrom::MessageTimestamp, 1)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoColumnMapping,
       ConfiguresInstrumentUsingDefaultColumnName) {
  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({});

  ASSERT_THAT(
      spec,
      IsExpected(ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoColumnMapping,
       ConfiguresLevelOneWithoutPartiesUsingDefaultColumnNames) {
  auto configurator = make_configurator({"Instrument",
                                         "MessageTimeStamp",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity",
                                         "BidParty",
                                         "AskParty",
                                         "ReceivedTimeStamp"});
  spec = configurator.configure({});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 7));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorNoColumnMapping,
       DoesNotConfigureDefaultSpecificationForLevelMoreThanOne) {
  auto configurator = make_configurator(FullOneLevelColumnNames);
  spec = configurator.configure({});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 2u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 2u).value())));
  // clang-format on
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test