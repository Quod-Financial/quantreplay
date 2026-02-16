#include <gmock/gmock.h>

#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/mapping/csv_no_header_configurator.hpp"
#include "ih/historical/mapping/specification.hpp"
#include "test_utils/historical_mapping_utils.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {

using namespace ::testing;

struct GeneratorHistoricalMappingCsvNoHeaderConfigurator
    : public testing::Test {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  static constexpr auto MaxColumnCount =
      std::numeric_limits<std::uint32_t>::max();

  auto make_configurator(
      std::uint32_t column_count = MaxColumnCount,
      std::uint32_t max_depth_levels = data_layer::Datasource::AllDepthLevels)
      -> CsvNoHeaderConfigurator {
    return CsvNoHeaderConfigurator{column_count, max_depth_levels};
  }

  tl::expected<Specification, std::string> spec;
};

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       ThrowsExceptionWhenColumnCountIsZero) {
  ASSERT_THROW(make_configurator(0), std::invalid_argument);
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       ReturnsErrorWhenColumnToIsNotANumber) {
  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"Not234A321Number"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsUnexpected("cannot convert Column To from string "
                           "`Not234A321Number' to a column position"));
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       ReturnsErrorWhenColumnToIsNegativeNumber) {
  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"-1"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsUnexpected("cannot convert Column To from string `-1' to a "
                           "column position"));
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       ReturnsErrorWhenColumnToIsZero) {
  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"0"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  // Zero column number is not valid as a 1-based column_to column number
  ASSERT_THAT(spec,
              IsUnexpected("cannot convert Column To from string `0' to a "
                           "column position"));
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       ReturnsErrorWhenColumnToStartsWithNumber) {
  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"6T"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsUnexpected("cannot convert Column To from string `6T' to a "
                           "column position"));
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       ReturnsErrorWhenColumnToExceedsColumnCount) {
  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"11"};

  auto configurator = make_configurator(10);
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(
      spec, IsUnexpected("Column To value `11' exceeds the column count `10'"));
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       AppliesAssociationForReceivedTimestamp) {
  const auto target = ColumnFrom::ReceivedTimestamp;

  const auto column_from = ColumnFrom::create("ReceivedTimeStamp").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsExpected(ResolvesColumnFromAndIndexIs(target, new_source_idx)));
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       AppliesAssociationForMessageTimestamp) {
  const auto target = ColumnFrom::MessageTimestamp;

  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsExpected(ResolvesColumnFromAndIndexIs(target, new_source_idx)));
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       AppliesAssociationForInstrument) {
  const auto target = ColumnFrom::Instrument;

  const auto column_from = ColumnFrom::create("Instrument").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsExpected(ResolvesColumnFromAndIndexIs(target, new_source_idx)));
}

TEST_F(
    GeneratorHistoricalMappingCsvNoHeaderConfigurator,
    ApplyingAssociationForReceivedTimestampKeepsDefaultAssociationForOthers) {
  const auto column_from = ColumnFrom::create("ReceivedTimeStamp").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, new_source_idx));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       ApplyingAssociationForMessageTimestampKeepsDefaultAssociationForOthers) {
  const auto column_from = ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, new_source_idx));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       ApplyingAssociationForInstrumentKeepsDefaultAssociationForOthers) {
  const auto column_from = ColumnFrom::create("Instrument").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, new_source_idx));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       KeepsDefaultNonLevelColumnAssociationWhenLevelColumnConfigured) {
  const auto column_from = ColumnFrom::create("BidParty1").value();
  const std::string column_to{"6"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingCsvNoHeaderConfigurator,
    ApplyingAssociationForLevelColumnKeepsDefaultAssociationForNotLevelColumns) {
  const auto column_from = ColumnFrom::create("BidParty").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), new_source_idx));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       AppliesAssociationForFirstLevelColumn) {
  const auto column_from = ColumnFrom::create("BidParty").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), new_source_idx));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       AppliesAssociationForSecondLevelColumn) {
  const auto column_from = ColumnFrom::create("BidParty2").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value(), new_source_idx));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingCsvNoHeaderConfigurator,
    DoesNotKeepDefaultColumnLevelAssociationWhenLevelColumnBidPartyInColumnMapping) {
  const auto column_from = ColumnFrom::create("BidParty").value();
  const std::string column_to{"6"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingCsvNoHeaderConfigurator,
    DoesNotKeepDefaultColumnLevelAssociationWhenLevelColumnBidQuantityInColumnMapping) {
  const auto column_from = ColumnFrom::create("BidQuantity").value();
  const std::string column_to{"6"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingCsvNoHeaderConfigurator,
    DoesNotKeepDefaultColumnLevelAssociationWhenLevelColumnBidPriceInColumnMapping) {
  const auto column_from = ColumnFrom::create("BidPrice").value();
  const std::string column_to{"6"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingCsvNoHeaderConfigurator,
    DoesNotKeepDefaultColumnLevelAssociationWhenLevelColumnAskPriceInColumnMapping) {
  const auto column_from = ColumnFrom::create("AskPrice").value();
  const std::string column_to{"6"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingCsvNoHeaderConfigurator,
    DoesNotKeepDefaultColumnLevelAssociationWhenLevelColumnAskQuantityInColumnMapping) {
  const auto column_from = ColumnFrom::create("AskQuantity").value();
  const std::string column_to{"6"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(
    GeneratorHistoricalMappingCsvNoHeaderConfigurator,
    DoesNotKeepDefaultColumnLevelAssociationWhenLevelColumnAskPartyInColumnMapping) {
  const auto column_from = ColumnFrom::create("AskParty").value();
  const std::string column_to{"6"};

  auto configurator = make_configurator();
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingCsvNoHeaderConfigurator,
       AppliesAssociationForLevelColumnWhenItsDepthIsEqualMaxDepthLevels) {
  const auto column_from = ColumnFrom::create("BidParty2").value();
  const std::string column_to{"6"};
  constexpr std::uint64_t new_source_idx = 5;

  auto configurator = make_configurator(MaxColumnCount, 2);
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsExpected(ResolvesColumnFromAndIndexIs(
                  ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value(),
                  new_source_idx)));
}

TEST_F(
    GeneratorHistoricalMappingCsvNoHeaderConfigurator,
    DoesNotApplyAssociationForLevelColumnWhenItsDepthIsGreaterThanMaxDepthLevels) {
  const auto column_from = ColumnFrom::create("BidParty2").value();
  const std::string column_to{"6"};

  auto configurator = make_configurator(MaxColumnCount, 1);
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(
      spec,
      IsExpected(Not(Resolves(
          ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value()))));
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test
