#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <utility>

#include "ih/historical/mapping/configurator.hpp"
#include "ih/historical/mapping/depth_config.hpp"
#include "ih/historical/mapping/specification.hpp"
#include "test_utils/historical_mapping_utils.hpp"

namespace simulator::generator::historical::mapping::test {
namespace {

using namespace ::testing;

struct GeneratorHistoricalMappingConfiguratorSourceColumnNamesPostgres
    : public testing::Test {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  auto make_configurator(std::initializer_list<std::string> columns_names,
                         DepthConfig depth_config) -> Configurator {
    return Configurator{spec,
                        columns_names,
                        std::move(depth_config),
                        DatasourceParams::Postgres};
  }

  Specification spec;
};

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNamesPostgres,
       KeepsDefaultAssociationWhenColumnToIsNotInSourceColumnNames) {
  const std::initializer_list<std::string> names{"Symbol",
                                                 "Time",
                                                 "MsgTime",
                                                 "BidPx1",
                                                 "BidQty",
                                                 "AskPx",
                                                 "AskQty",
                                                 "BidCP",
                                                 "AskCP"};

  const auto target = ColumnFrom::MessageTimestamp;
  constexpr std::uint64_t default_message_timestamp_source_idx = 1;

  const auto column_from =
      data_layer::converter::ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"NotInColumnNamesNumber"};

  auto configurator = make_configurator(names, {1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec,
              ResolvesColumnFromAndIndexIs(
                  target, default_message_timestamp_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNamesPostgres,
       KeepsDefaultAssociationWhenColumnToIsNumber) {
  const std::initializer_list<std::string> names{"Symbol",
                                                 "Time",
                                                 "MsgTime",
                                                 "BidPx1",
                                                 "BidQty",
                                                 "AskPx",
                                                 "AskQty",
                                                 "BidCP",
                                                 "AskCP"};

  const auto target = ColumnFrom::Instrument;
  constexpr std::uint64_t default_instrument_source_idx = 2;

  const auto column_from = ColumnFrom::create("Instrument").value();
  const std::string column_to{"1"};

  auto configurator = make_configurator(names, {1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(
      spec,
      ResolvesColumnFromAndIndexIs(target, default_instrument_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNamesPostgres,
       AppliesAssociationWhenColumnToIsInSourceColumnNames) {
  const std::initializer_list<std::string> names{"Symbol",
                                                 "Time",
                                                 "MsgTime",
                                                 "BidPx1",
                                                 "BidQty",
                                                 "AskPx",
                                                 "AskQty",
                                                 "BidCP",
                                                 "AskCP"};

  const auto target =
      ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value();
  constexpr std::uint64_t bid_cp_index = 7;

  const auto column_from =
      data_layer::converter::ColumnFrom::create("BidParty2").value();
  const std::string column_to{"BidCP"};

  auto configurator = make_configurator(names, {4, 3});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, bid_cp_index));
}

struct GeneratorHistoricalMappingConfiguratorSourceColumnNamesCSV
    : public testing::Test {
  using ColumnFrom = data_layer::converter::ColumnFrom;

  auto make_configurator(std::initializer_list<std::string> columns_names,
                         DepthConfig depth_config) -> Configurator {
    return Configurator{spec,
                        columns_names,
                        std::move(depth_config),
                        DatasourceParams::CsvHasHeader};
  }

  Specification spec;
};

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNamesCSV,
       KeepsDefaultAssociationWhenColumnToIsNotInSourceColumnNames) {
  const std::initializer_list<std::string> names{"Symbol",
                                                 "Time",
                                                 "MsgTime",
                                                 "BidPx1",
                                                 "BidQty",
                                                 "AskPx",
                                                 "AskQty",
                                                 "BidCP",
                                                 "AskCP"};

  const auto target = ColumnFrom::MessageTimestamp;
  constexpr std::uint64_t default_message_timestamp_source_idx = 1;

  const auto column_from =
      data_layer::converter::ColumnFrom::create("MessageTimeStamp").value();
  const std::string column_to{"NotInColumnNamesNumber"};

  auto configurator = make_configurator(names, {1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec,
              ResolvesColumnFromAndIndexIs(
                  target, default_message_timestamp_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNamesCSV,
       KeepsDefaultAssociationWhenColumnToIsNumber) {
  const std::initializer_list<std::string> names{"Symbol",
                                                 "Time",
                                                 "MsgTime",
                                                 "BidPx1",
                                                 "BidQty",
                                                 "AskPx",
                                                 "AskQty",
                                                 "BidCP",
                                                 "AskCP"};

  const auto target = ColumnFrom::Instrument;
  constexpr std::uint64_t default_instrument_source_idx = 2;

  const auto column_from = ColumnFrom::create("Instrument").value();
  const std::string column_to{"1"};

  auto configurator = make_configurator(names, {1, 1});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(
      spec,
      ResolvesColumnFromAndIndexIs(target, default_instrument_source_idx));
}

TEST_F(GeneratorHistoricalMappingConfiguratorSourceColumnNamesCSV,
       AppliesAssociationWhenColumnToIsInSourceColumnNames) {
  const std::initializer_list<std::string> names{"Symbol",
                                                 "Time",
                                                 "MsgTime",
                                                 "BidPx1",
                                                 "BidQty",
                                                 "AskPx",
                                                 "AskQty",
                                                 "BidCP",
                                                 "AskCP"};

  const auto target =
      ColumnFrom::create(ColumnFrom::Column::BidParty, 2u).value();
  constexpr std::uint64_t bid_cp_index = 7;

  const auto column_from =
      data_layer::converter::ColumnFrom::create("BidParty2").value();
  const std::string column_to{"BidCP"};

  auto configurator = make_configurator(names, {4, 3});
  configurator.configure(make_column_config(column_from, column_to));

  ASSERT_THAT(spec, ResolvesColumnFromAndIndexIs(target, bid_cp_index));
}

}  // namespace
}  // namespace simulator::generator::historical::mapping::test
