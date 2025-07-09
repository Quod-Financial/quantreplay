#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <csv.hpp>
#include <initializer_list>
#include <pqxx/result>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>

#include "data_layer/api/models/column_mapping.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/data/time.hpp"
#include "ih/historical/mapping/params.hpp"
#include "ih/historical/parsing/parsing.hpp"
#include "ih/historical/parsing/row.hpp"
#include "tests/test_utils/historical_data_utils.hpp"

namespace simulator::generator::historical::test {
namespace {

using namespace ::testing;

auto make_mapping_param(std::string column_from, std::uint32_t column_to)
    -> data_layer::ColumnMapping {
  data_layer::ColumnMapping::Patch patch;
  patch.with_column_to(std::to_string(column_to))
      .with_column_from(std::move(column_from));
  return data_layer::ColumnMapping::create(std::move(patch), 0);
}

auto make_mapping(std::initializer_list<data_layer::ColumnMapping> params,
                  mapping::DepthConfig depth_config)
    -> historical::MappingParams {
  using historical::MappingParams;

  MappingParams mapping_params{params, DatasourceParams::CsvNoHeader};
  mapping_params.initialize(depth_config);
  return mapping_params;
}

struct GeneratorHistoricalParsingParseLevel : public ::testing::Test {
  static constexpr mapping::DepthConfig DepthOne{1, 1};

  Level::Builder builder;
};

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesNanStringBidPrice) {
  const Row row{"AB"};
  const auto mapping =
      make_mapping({make_mapping_param("BidPrice", 1)}, DepthOne);

  parse_bid_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.bid_price(), Eq(std::nullopt));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesDecimalStringBidPrice) {
  const Row row{"42.42"};
  const auto mapping =
      make_mapping({make_mapping_param("BidPrice", 1)}, DepthOne);

  parse_bid_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.bid_price(), Optional(DoubleEq(42.42)));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesNanStringBidQuantity) {
  const Row row{"AB"};
  const auto mapping =
      make_mapping({make_mapping_param("BidQuantity", 1)}, DepthOne);

  parse_bid_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.bid_quantity(), Eq(std::nullopt));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesDecimalStringBidQuantity) {
  const Row row{"42.42"};
  const auto mapping =
      make_mapping({make_mapping_param("BidQuantity", 1)}, DepthOne);

  parse_bid_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.bid_quantity(), Optional(DoubleEq(42.42)));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesEmptyStringBidParty) {
  const Row row{""};
  const auto mapping =
      make_mapping({make_mapping_param("BidParty", 1)}, DepthOne);

  parse_bid_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.bid_counterparty(), Eq(std::nullopt));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesNotEmptyBidParty) {
  const Row row{"BidCP"};
  const auto mapping =
      make_mapping({make_mapping_param("BidParty", 1)}, DepthOne);

  parse_bid_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.bid_counterparty(), Optional(Eq("BidCP")));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesNanStringOfferPrice) {
  const Row row{"AB"};
  const auto mapping =
      make_mapping({make_mapping_param("AskPrice", 1)}, DepthOne);

  parse_offer_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.offer_price(), Eq(std::nullopt));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesDecimalStringOfferPrice) {
  const Row row{"42.42"};
  const auto mapping =
      make_mapping({make_mapping_param("AskPrice", 1)}, DepthOne);

  parse_offer_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.offer_price(), Optional(DoubleEq(42.42)));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesOfferNanStringQuantity) {
  const Row row{"AB"};
  const auto mapping =
      make_mapping({make_mapping_param("AskQuantity", 1)}, DepthOne);

  parse_offer_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.offer_quantity(), Eq(std::nullopt));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesDecimalStringOfferQuantity) {
  const Row row{"42.42"};
  const auto mapping =
      make_mapping({make_mapping_param("AskQuantity", 1)}, DepthOne);

  parse_offer_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.offer_quantity(), Optional(DoubleEq(42.42)));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesEmptyStringOfferParty) {
  const Row row{""};
  const auto mapping =
      make_mapping({make_mapping_param("AskParty", 1)}, DepthOne);

  parse_offer_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.offer_counterparty(), Eq(std::nullopt));
}

TEST_F(GeneratorHistoricalParsingParseLevel, ParsesOfferNotEmptyStringParty) {
  const Row row{"AskCP"};
  const auto mapping =
      make_mapping({make_mapping_param("AskParty", 1)}, DepthOne);

  parse_offer_level_part(row, builder, mapping, 1);

  const auto level = Level::Builder::construct(builder);
  EXPECT_THAT(level.offer_counterparty(), Optional(Eq("AskCP")));
}

class GeneratorHistoricalRowParsing : public ::testing::Test {
 public:
  static constexpr mapping::DepthConfig DepthOne{1, 1};

  static constexpr std::string_view TestTimeString = "2023-01-20 12:00:32.345";

  static constexpr auto TestTimeValue =
      historical::Timepoint{historical::Duration{1674216032345}};

  historical::Record::Builder builder;

  GeneratorHistoricalRowParsing() {
    builder.with_source_row(1).with_source_connection("dummy");
  }
};

TEST_F(GeneratorHistoricalRowParsing, DoesNotSetEmptyReceiveTimeStamp) {
  const Row row{"", "dummy"};
  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("Instrument", 2)},
                                    DepthOne);

  parse(row, builder, mapping, 1);
  EXPECT_THROW((void)Record::Builder::construct(builder),
               std::invalid_argument);
}

TEST_F(GeneratorHistoricalRowParsing, DoesNotSetIncorrectReceiveTimeStamp) {
  const Row row{"AABBCC", "dummy"};
  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("Instrument", 2)},
                                    DepthOne);

  parse(row, builder, mapping, 1);
  EXPECT_THROW((void)Record::Builder::construct(builder),
               std::invalid_argument);
}

TEST_F(GeneratorHistoricalRowParsing, SetsReceivedTimeStamp) {
  const Row row{TestTimeString, "dummy"};
  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("Instrument", 2)},
                                    DepthOne);

  parse(row, builder, mapping, 1);

  const auto record = Record::Builder::construct(builder);
  EXPECT_EQ(record.receive_time(), TestTimeValue);
}

TEST_F(GeneratorHistoricalRowParsing, DoesNotSetEmptyMessageTimeStamp) {
  const Row row{TestTimeString, "", "dummy"};
  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("MessageTimeStamp", 2),
                                     make_mapping_param("Instrument", 3)},
                                    DepthOne);

  parse(row, builder, mapping, 1);

  const auto record = Record::Builder::construct(builder);
  EXPECT_EQ(record.message_time(), std::nullopt);
}

TEST_F(GeneratorHistoricalRowParsing, DoesNotSetIncorrecMessageTimeStamp) {
  const Row row{TestTimeString, "AABBCC", "dummy"};
  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("MessageTimeStamp", 2),
                                     make_mapping_param("Instrument", 3)},
                                    DepthOne);

  parse(row, builder, mapping, 1);

  const auto record = Record::Builder::construct(builder);
  EXPECT_EQ(record.message_time(), std::nullopt);
}

TEST_F(GeneratorHistoricalRowParsing, SetsMessageTimeStamp) {
  const Row row{TestTimeString, TestTimeString, "dummy"};
  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("MessageTimeStamp", 2),
                                     make_mapping_param("Instrument", 3)},
                                    DepthOne);

  parse(row, builder, mapping, 1);

  const auto record = Record::Builder::construct(builder);
  EXPECT_THAT(record.message_time(), Optional(Eq(TestTimeValue)));
}

TEST_F(GeneratorHistoricalRowParsing, DoesNotSetEmptyInstrument) {
  const Row row{TestTimeString, ""};
  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("Instrument", 2)},
                                    DepthOne);

  parse(row, builder, mapping, 1);
  EXPECT_THROW((void)Record::Builder::construct(builder),
               std::invalid_argument);
}

TEST_F(GeneratorHistoricalRowParsing, SetsInstrument) {
  const Row row{TestTimeString, "INSTR-1"};
  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("Instrument", 2)},
                                    DepthOne);

  parse(row, builder, mapping, 1);

  const auto record = Record::Builder::construct(builder);
  EXPECT_EQ(record.instrument(), "INSTR-1");
}

TEST_F(GeneratorHistoricalRowParsing, DoesNotSetLevelDataIfItIsNotPresent) {
  const Row row{TestTimeString, "INSTR-2"};
  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("Instrument", 2)},
                                    DepthOne);

  parse(row, builder, mapping, 1);

  const auto record = Record::Builder::construct(builder);
  EXPECT_FALSE(record.has_levels());
}

TEST_F(GeneratorHistoricalRowParsing, SetsLevel) {
  const Row row{TestTimeString,
                "INSTR-2",
                "Bid_party",
                "10",
                "42.42",
                "52.52",
                "20",
                "Ask_party"};

  MockFunction<void(std::uint64_t, const Level&)> level_visitor;
  EXPECT_CALL(level_visitor,
              Call(0, LevelEq("Bid_party", 10, 42.42, 52.52, 20, "Ask_party")))
      .Times(1);

  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("Instrument", 2),
                                     make_mapping_param("BidParty", 3),
                                     make_mapping_param("BidQuantity", 4),
                                     make_mapping_param("BidPrice", 5),
                                     make_mapping_param("AskPrice", 6),
                                     make_mapping_param("AskQuantity", 7),
                                     make_mapping_param("AskParty", 8)},
                                    DepthOne);

  parse(row, builder, mapping, 1);

  const auto record = Record::Builder::construct(builder);
  ASSERT_TRUE(record.has_levels());
  record.visit_levels(level_visitor.AsStdFunction());
}

TEST_F(GeneratorHistoricalRowParsing, SetsTwoLevels) {
  const Row row{TestTimeString,
                "INSTR-2",
                "Bid_party1",
                "10",
                "42.42",
                "Bid_party2",
                "20",
                "52.52",
                "62.62",
                "30",
                "Ask_party1",
                "72.72",
                "40",
                "Ask_party2"};

  MockFunction<void(std::uint64_t, const Level&)> level_visitor;
  EXPECT_CALL(
      level_visitor,
      Call(0, LevelEq("Bid_party1", 10, 42.42, 62.62, 30, "Ask_party1")))
      .Times(1);
  EXPECT_CALL(
      level_visitor,
      Call(1, LevelEq("Bid_party2", 20, 52.52, 72.72, 40, "Ask_party2")))
      .Times(1);

  const auto mapping = make_mapping({make_mapping_param("ReceivedTimeStamp", 1),
                                     make_mapping_param("Instrument", 2),
                                     make_mapping_param("BidParty1", 3),
                                     make_mapping_param("BidQuantity1", 4),
                                     make_mapping_param("BidPrice1", 5),
                                     make_mapping_param("BidParty2", 6),
                                     make_mapping_param("BidQuantity2", 7),
                                     make_mapping_param("BidPrice2", 8),
                                     make_mapping_param("AskPrice1", 9),
                                     make_mapping_param("AskQuantity1", 10),
                                     make_mapping_param("AskParty1", 11),
                                     make_mapping_param("AskPrice2", 12),
                                     make_mapping_param("AskQuantity2", 13),
                                     make_mapping_param("AskParty2", 14)},
                                    {2, 2});

  parse(row, builder, mapping, 2);

  const auto record = Record::Builder::construct(builder);
  ASSERT_TRUE(record.has_levels());
  record.visit_levels(level_visitor.AsStdFunction());
}

}  // namespace
}  // namespace simulator::generator::historical::test