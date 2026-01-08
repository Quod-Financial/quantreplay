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

struct GeneratorHistoricalMappingConfiguratorSourceColumnNames
    : public testing::Test {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  static auto make_configurator(
      Configurator::SourceColumnNames columns_names,
      std::uint32_t max_depth_levels = data_layer::Datasource::AllDepthLevels)
      -> Configurator {
    return Configurator{std::move(columns_names), max_depth_levels};
  }

  tl::expected<Specification, std::string> spec;
};

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       ThrowsExceptionWhenSourceColumnNamesIsEmpty) {
  ASSERT_THROW(make_configurator({}), std::invalid_argument);
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       ThrowsMeaningfulExceptionMessageWhenSourceColumnNamesIsEmpty) {
  try {
    [[maybe_unused]] const auto configurator = make_configurator({});
    FAIL() << "std::invalid_argument exception must be thrown";
  } catch (std::invalid_argument& e) {
    ASSERT_STREQ(e.what(), "the list of source column names must not be empty");
  }
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       ReturnsErrorWhenColumnToDoesNotExist) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"NotInColumnNames"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsUnexpected("Column To `NotInColumnNames' does not exist"));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForReceivedTimestamp) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("ReceivedTimeStamp").value();
  const std::string column_to{"Time"};

  auto configurator = make_configurator({"Time",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsExpected(ResolvesColumnFromAndIndexIs(
                  ColumnFrom::ReceivedTimestamp, 0)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       ApplyingAssociationForReceivedTimestampKeepsDefaultMapping) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("ReceivedTimeStamp").value();
  const std::string column_to{"Time"};

  auto configurator = make_configurator({"Time",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidParty",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity",
                                         "AskParty"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
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

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForMessageTimestamp) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"Time"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "Time",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec,
              IsExpected(ResolvesColumnFromAndIndexIs(
                  ColumnFrom::MessageTimestamp, 1)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       ApplyingAssociationForMessageTimestampKeepsDefaultMapping) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"Time"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "Time",
                                         "Instrument",
                                         "BidParty",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity",
                                         "AskParty"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForInstrument) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("Instrument").value();
  const std::string column_to{"Symbol"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Symbol",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(
      spec,
      IsExpected(ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       ApplyingAssociationForInstrumentKeepsDefaultMapping) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("Instrument").value();
  const std::string column_to{"Symbol"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Symbol",
                                         "BidParty",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity",
                                         "AskParty"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 6));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 7));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value(), 8));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       IgnoresAbsenceBidPartyAndAskPartyWhenDefaultLevelMappingApplied) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("Instrument").value();
  const std::string column_to{"Symbol"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Symbol",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value(), 3));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value(), 4));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value(), 5));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value(), 6));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       ReturnsErrorWhenBidPartyConfiguredButNotInSourceColumnNames) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("BidParty").value();
  const std::string column_to{"bid_party"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec, IsUnexpected("Column To `bid_party' does not exist"));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       ReturnsErrorWhenAskPartyConfiguredButNotInSourceColumnNames) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("AskParty").value();
  const std::string column_to{"ask_party"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_THAT(spec, IsUnexpected("Column To `ask_party' does not exist"));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForBidParty) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("BidParty").value();
  const std::string column_to{"bid_party"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "bid_party",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  const auto target = data_layer::converter::ColumnFrom::create(
                          ColumnFrom::Column::BidParty, 1u)
                          .value();

  ASSERT_THAT(spec, IsExpected(ResolvesColumnFromAndIndexIs(target, 3)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       SpecifyingAssociationForBidPartyDisablesDefaultLevelMapping) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("BidParty").value();
  const std::string column_to{"bid_party"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "bid_party",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity",
                                         "AskParty"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForBidQuantity) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("BidQuantity").value();
  const std::string column_to{"bid_quantity"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "bid_quantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  const auto target = data_layer::converter::ColumnFrom::create(
                          ColumnFrom::Column::BidQuantity, 1u)
                          .value();

  ASSERT_THAT(spec, IsExpected(ResolvesColumnFromAndIndexIs(target, 3)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       SpecifyingAssociationForBidQuantityDisablesDefaultLevelMapping) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("BidQuantity").value();
  const std::string column_to{"bid_quantity"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "bid_quantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity",
                                         "AskParty"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForBidPrice) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("BidPrice").value();
  const std::string column_to{"bid_price"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "bid_price",
                                         "AskPrice",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  const auto target = data_layer::converter::ColumnFrom::create(
                          ColumnFrom::Column::BidPrice, 1u)
                          .value();

  ASSERT_THAT(spec, IsExpected(ResolvesColumnFromAndIndexIs(target, 4)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       SpecifyingAssociationForBidPriceDisablesDefaultLevelMapping) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("BidPrice").value();
  const std::string column_to{"bid_price"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "bid_price",
                                         "AskPrice",
                                         "AskQuantity",
                                         "AskParty"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForAskPrice) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("AskPrice").value();
  const std::string column_to{"ask_price"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "ask_price",
                                         "AskQuantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  const auto target = data_layer::converter::ColumnFrom::create(
                          ColumnFrom::Column::OfferPrice, 1u)
                          .value();

  ASSERT_THAT(spec, IsExpected(ResolvesColumnFromAndIndexIs(target, 5)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       SpecifyingAssociationForAskPriceDisablesDefaultLevelMapping) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("AskPrice").value();
  const std::string column_to{"ask_price"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "ask_price",
                                         "AskQuantity",
                                         "AskParty"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForAskQuantity) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("AskQuantity").value();
  const std::string column_to{"ask_quantity"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "ask_quantity"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  const auto target = data_layer::converter::ColumnFrom::create(
                          ColumnFrom::Column::OfferQuantity, 1u)
                          .value();

  ASSERT_THAT(spec, IsExpected(ResolvesColumnFromAndIndexIs(target, 6)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       SpecifyingAssociationForAskQuantityDisablesDefaultLevelMapping) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("AskQuantity").value();
  const std::string column_to{"ask_quantity"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "ask_quantity",
                                         "AskParty"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferParty, 1u).value())));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForAskParty) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("AskParty").value();
  const std::string column_to{"ask_party"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity",
                                         "ask_party"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  const auto target = data_layer::converter::ColumnFrom::create(
                          ColumnFrom::Column::OfferParty, 1u)
                          .value();

  ASSERT_THAT(spec, IsExpected(ResolvesColumnFromAndIndexIs(target, 7)));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       SpecifyingAssociationForAskPartyDisablesDefaultLevelMapping) {
  const auto column_from =
      data_layer::converter::ColumnFrom::create("AskParty").value();
  const std::string column_to{"ask_party"};

  auto configurator = make_configurator({"ReceivedTimeStamp",
                                         "MessageTimeStamp",
                                         "Instrument",
                                         "BidQuantity",
                                         "BidPrice",
                                         "AskPrice",
                                         "AskQuantity",
                                         "ask_party"});
  spec = configurator.configure({make_column_config(column_from, column_to)});

  ASSERT_TRUE(spec.has_value());
  // clang-format off
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::ReceivedTimestamp, 0));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::MessageTimestamp, 1));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::Instrument, 2));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidParty, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::BidPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 1u).value())));
  ASSERT_THAT(*spec, Not(Resolves(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 1u).value())));
  // clang-format on
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForTwoLevels) {
  using data_layer::converter::ColumnFrom;
  auto configurator = make_configurator({"Symbol",
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
                                         "AskCP2"});
  // clang-format off
  spec = configurator.configure({
    make_column_config(ColumnFrom::create("Instrument").value(), "Symbol"),
    make_column_config(ColumnFrom::create("ReceivedTimeStamp").value(), "Time"),
    make_column_config(ColumnFrom::create("MessageTimeStamp").value(), "MsgTime"),
    make_column_config(ColumnFrom::create("BidPrice1").value(), "BidPx1"),
    make_column_config(ColumnFrom::create("BidQuantity1").value(), "BidQty1"),
    make_column_config(ColumnFrom::create("AskPrice1").value(), "AskPx1"),
    make_column_config(ColumnFrom::create("AskQuantity1").value(), "AskQty1"),
    make_column_config(ColumnFrom::create("BidParty1").value(), "BidCP1"),
    make_column_config(ColumnFrom::create("AskParty1").value(), "AskCP1"),
    make_column_config(ColumnFrom::create("BidPrice2").value(), "BidPx2"),
    make_column_config(ColumnFrom::create("BidQuantity2").value(), "BidQty2"),
    make_column_config(ColumnFrom::create("AskPrice2").value(), "AskPx2"),
    make_column_config(ColumnFrom::create("AskQuantity2").value(), "AskQty2"),
    make_column_config(ColumnFrom::create("BidParty2").value(), "BidCP2"),
    make_column_config(ColumnFrom::create("AskParty2").value(), "AskCP2")
  });
  // clang-format on

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
    GeneratorHistoricalMappingConfiguratorSourceColumnNames,
    AppliesAssociationForLevelOneWhenLevelOneConfiguredAndMaxDepthLevelsIsOne) {
  using data_layer::converter::ColumnFrom;
  auto configurator = make_configurator({"Symbol",
                                         "Time",
                                         "MsgTime",
                                         "BidPx1",
                                         "BidQty1",
                                         "AskPx1",
                                         "AskQty1",
                                         "BidCP1",
                                         "AskCP1"},
                                        1);
  // clang-format off
  spec = configurator.configure({
    make_column_config(ColumnFrom::create("Instrument").value(), "Symbol"),
    make_column_config(ColumnFrom::create("ReceivedTimeStamp").value(), "Time"),
    make_column_config(ColumnFrom::create("MessageTimeStamp").value(), "MsgTime"),
    make_column_config(ColumnFrom::create("BidPrice1").value(), "BidPx1"),
    make_column_config(ColumnFrom::create("BidQuantity1").value(), "BidQty1"),
    make_column_config(ColumnFrom::create("AskPrice1").value(), "AskPx1"),
    make_column_config(ColumnFrom::create("AskQuantity1").value(), "AskQty1"),
    make_column_config(ColumnFrom::create("BidParty1").value(), "BidCP1"),
    make_column_config(ColumnFrom::create("AskParty1").value(), "AskCP1")
  });
  // clang-format on

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
    GeneratorHistoricalMappingConfiguratorSourceColumnNames,
    AppliesAssociationForLevelOneWhenTwoLevelsConfiguredAndMaxDepthLevelsIsOne) {
  using data_layer::converter::ColumnFrom;
  auto configurator = make_configurator({"Symbol",
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
                                         "AskCP2"},
                                        1);
  // clang-format off
  spec = configurator.configure({
    make_column_config(ColumnFrom::create("Instrument").value(), "Symbol"),
    make_column_config(ColumnFrom::create("ReceivedTimeStamp").value(), "Time"),
    make_column_config(ColumnFrom::create("MessageTimeStamp").value(), "MsgTime"),
    make_column_config(ColumnFrom::create("BidPrice1").value(), "BidPx1"),
    make_column_config(ColumnFrom::create("BidQuantity1").value(), "BidQty1"),
    make_column_config(ColumnFrom::create("AskPrice1").value(), "AskPx1"),
    make_column_config(ColumnFrom::create("AskQuantity1").value(), "AskQty1"),
    make_column_config(ColumnFrom::create("BidParty1").value(), "BidCP1"),
    make_column_config(ColumnFrom::create("AskParty1").value(), "AskCP1"),
    make_column_config(ColumnFrom::create("BidPrice2").value(), "BidPx2"),
    make_column_config(ColumnFrom::create("BidQuantity2").value(), "BidQty2"),
    make_column_config(ColumnFrom::create("AskPrice2").value(), "AskPx2"),
    make_column_config(ColumnFrom::create("AskQuantity2").value(), "AskQty2"),
    make_column_config(ColumnFrom::create("BidParty2").value(), "BidCP2"),
    make_column_config(ColumnFrom::create("AskParty2").value(), "AskCP2")
  });
  // clang-format on

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
    GeneratorHistoricalMappingConfiguratorSourceColumnNames,
    AppliesAssociationForLevelOneWhenLevelOneConfiguredAndMaxDepthLevelsIsTwo) {
  using data_layer::converter::ColumnFrom;
  auto configurator = make_configurator({"Symbol",
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
                                         "AskCP2"},
                                        2);
  // clang-format off
  spec = configurator.configure({
    make_column_config(ColumnFrom::create("Instrument").value(), "Symbol"),
    make_column_config(ColumnFrom::create("ReceivedTimeStamp").value(), "Time"),
    make_column_config(ColumnFrom::create("MessageTimeStamp").value(), "MsgTime"),
    make_column_config(ColumnFrom::create("BidPrice1").value(), "BidPx1"),
    make_column_config(ColumnFrom::create("BidQuantity1").value(), "BidQty1"),
    make_column_config(ColumnFrom::create("AskPrice1").value(), "AskPx1"),
    make_column_config(ColumnFrom::create("AskQuantity1").value(), "AskQty1"),
    make_column_config(ColumnFrom::create("BidParty1").value(), "BidCP1"),
    make_column_config(ColumnFrom::create("AskParty1").value(), "AskCP1")
  });
  // clang-format on

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

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNames,
       AppliesAssociationForLevelsOneAndThreeIfLevelTwoNotConfigured) {
  using data_layer::converter::ColumnFrom;

  // clang-format off
  auto configurator = make_configurator(
      {"Symbol", "Time", "MsgTime",
       "BidPx1", "BidQty1", "AskPx1", "AskQty1", "BidCP1", "AskCP1",
       "BidPx2", "BidQty2", "AskPx2", "AskQty2", "BidCP2", "AskCP2",
       "BidPx3", "BidQty3", "AskPx3", "AskQty3", "BidCP3", "AskCP3"});

  spec = configurator.configure({
    make_column_config(ColumnFrom::create("Instrument").value(), "Symbol"),
    make_column_config(ColumnFrom::create("ReceivedTimeStamp").value(), "Time"),
    make_column_config(ColumnFrom::create("MessageTimeStamp").value(), "MsgTime"),
    make_column_config(ColumnFrom::create("BidPrice1").value(), "BidPx1"),
    make_column_config(ColumnFrom::create("BidQuantity1").value(), "BidQty1"),
    make_column_config(ColumnFrom::create("AskPrice1").value(), "AskPx1"),
    make_column_config(ColumnFrom::create("AskQuantity1").value(), "AskQty1"),
    make_column_config(ColumnFrom::create("BidParty1").value(), "BidCP1"),
    make_column_config(ColumnFrom::create("AskParty1").value(), "AskCP1"),
    make_column_config(ColumnFrom::create("BidPrice3").value(), "BidPx3"),
    make_column_config(ColumnFrom::create("BidQuantity3").value(), "BidQty3"),
    make_column_config(ColumnFrom::create("AskPrice3").value(), "AskPx3"),
    make_column_config(ColumnFrom::create("AskQuantity3").value(), "AskQty3"),
    make_column_config(ColumnFrom::create("BidParty3").value(), "BidCP3"),
    make_column_config(ColumnFrom::create("AskParty3").value(), "AskCP3")
  });
  // clang-format on

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
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidPrice, 3u).value(), 15));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidQuantity, 3u).value(), 16));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferPrice, 3u).value(), 17));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferQuantity, 3u).value(), 18));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::BidParty, 3u).value(), 19));
  ASSERT_THAT(*spec, ResolvesColumnFromAndIndexIs(ColumnFrom::create(ColumnFrom::Column::OfferParty, 3u).value(), 20));
  // clang-format on
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test
