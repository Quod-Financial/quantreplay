#include <gmock/gmock.h>

#include "ih/historical/mapping/configurator.hpp"
#include "ih/historical/mapping/specification.hpp"
#include "test_utils/historical_mapping_utils.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {

using namespace ::testing;

struct GeneratorHistoricalMappingConfiguratorDefaultAssociation
    : public testing::Test {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  auto make_configurator(DepthConfig depth_config) -> Configurator {
    return Configurator{
        spec, {}, std::move(depth_config), DatasourceParams::CsvNoHeader};
  }

  Specification spec;
};

TEST_F(GeneratorHistoricalMappingConfiguratorDefaultAssociation,
       ThrowsExceptionIfDatasourceDepthLevelIsZero) {
  ASSERT_THROW(make_configurator({0, 1}), std::invalid_argument);
}

TEST_F(GeneratorHistoricalMappingConfiguratorDefaultAssociation,
       ThrowsExceptionIfDepthToParseIsZero) {
  ASSERT_THROW(make_configurator({1, 0}), std::invalid_argument);
}

TEST_F(GeneratorHistoricalMappingConfiguratorDefaultAssociation,
       ThrowsExceptionIfDataSourceDepthIsLessThenDepthToParse) {
  ASSERT_THROW(make_configurator({1, 42}), std::invalid_argument);
}

TEST_F(GeneratorHistoricalMappingConfiguratorDefaultAssociation,
       ConfiguresDefaultSpecificationWhenDataSourceDepthAndDepthToParseAreOne) {
  const auto configurator = make_configurator({1, 1});

  // clang-format off
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 3));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 5));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 6));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 7));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingConfiguratorDefaultAssociation,
    ConfiguresDefaultSpecificationWhenDataSourceDepthIsTwoAndDepthToParseIsOne) {
  const auto configurator = make_configurator({2, 1});

  // clang-format off
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 3));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 5));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 9));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 10));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 11));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorDefaultAssociation,
       DoesNotConfigureDefaultSpecificationForColumnsOutOfDepthToParse) {
  const auto configurator = make_configurator({2, 1});

  ASSERT_THAT(
      spec,
      Not(Resolves(
          ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value())));
  ASSERT_THAT(
      spec,
      Not(Resolves(
          ColumnFrom::create(ColumnFrom::Column::BidQuantity, 2u).value())));
  ASSERT_THAT(
      spec,
      Not(Resolves(
          ColumnFrom::create(ColumnFrom::Column::BidPrice, 2u).value())));
  ASSERT_THAT(
      spec,
      Not(Resolves(
          ColumnFrom::create(ColumnFrom::Column::OfferPrice, 2u).value())));
  ASSERT_THAT(
      spec,
      Not(Resolves(
          ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 2u).value())));
  ASSERT_THAT(
      spec,
      Not(Resolves(
          ColumnFrom::create(ColumnFrom::Column::OfferParty, 2u).value())));
}

TEST_F(GeneratorHistoricalMappingConfiguratorDefaultAssociation,
       ConfiguresDefaultSpecificationWhenDataSourceDepthAndDepthToParseAreTwo) {
  const auto configurator = make_configurator({2, 2});

  // clang-format off
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 3));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 5));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value(), 6));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 2u).value(), 7));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 2u).value(), 8));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 9));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 10));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 11));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 2u).value(), 12));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 2u).value(), 13));
  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 2u).value(), 14));
  // clang-format on
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test