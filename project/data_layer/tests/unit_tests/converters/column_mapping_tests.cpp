#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "api/converters/column_mapping.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::data_layer::converter::test {
namespace {

using namespace ::testing;

MATCHER_P(IsNoDepthColumn, column, "") {
  return ExplainMatchResult(IsTrue(), arg.has_value(), result_listener) &&
         ExplainMatchResult(Eq(column), arg->column(), result_listener) &&
         ExplainMatchResult(VariantWith<ColumnFrom::NoDepth>(_),
                            arg->depth_level(),
                            result_listener);
}

MATCHER_P2(IsColumnWithDepth, column, depth, "") {
  return ExplainMatchResult(IsTrue(), arg.has_value(), result_listener) &&
         ExplainMatchResult(Eq(column), arg->column(), result_listener) &&
         ExplainMatchResult(VariantWith<std::uint32_t>(depth),
                            arg->depth_level(),
                            result_listener);
}

MATCHER_P(IsColumnWithVariableDepth, column, "") {
  return ExplainMatchResult(IsTrue(), arg.has_value(), result_listener) &&
         ExplainMatchResult(Eq(column), arg->column(), result_listener) &&
         ExplainMatchResult(VariantWith<ColumnFrom::VariableDepth>(_),
                            arg->depth_level(),
                            result_listener);
}

TEST(DataLayerColumnMappingConverterColumnFrom, DefinesColumnsThatHaveDepth) {
  ASSERT_TRUE(ColumnFrom::has_depth(ColumnFrom::Column::BidParty));
  ASSERT_TRUE(ColumnFrom::has_depth(ColumnFrom::Column::BidQuantity));
  ASSERT_TRUE(ColumnFrom::has_depth(ColumnFrom::Column::BidPrice));
  ASSERT_TRUE(ColumnFrom::has_depth(ColumnFrom::Column::OfferPrice));
  ASSERT_TRUE(ColumnFrom::has_depth(ColumnFrom::Column::OfferQuantity));
  ASSERT_TRUE(ColumnFrom::has_depth(ColumnFrom::Column::OfferParty));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     DefinesColumnsThatDoNotHaveDepth) {
  ASSERT_FALSE(ColumnFrom::has_depth(ColumnFrom::Column::ReceivedTimestamp));
  ASSERT_FALSE(ColumnFrom::has_depth(ColumnFrom::Column::MessageTimestamp));
  ASSERT_FALSE(ColumnFrom::has_depth(ColumnFrom::Column::Instrument));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ReturnsErrorMessageOnCreationNotExistedColumnFromString) {
  const auto column_from_exp = ColumnFrom::create("NotExistedColumn");
  ASSERT_THAT(column_from_exp,
              IsUnexpected("incorrect Column From value: `NotExistedColumn'"));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromReceivedTimestampNoDepthStr) {
  const auto column_from_exp = ColumnFrom::create("ReceivedTimeStamp");
  ASSERT_THAT(column_from_exp,
              IsNoDepthColumn(ColumnFrom::Column::ReceivedTimestamp));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ReturnsErrorMessageOnCreationReceivedTimestampWithDepthFromString) {
  const auto column_from_exp = ColumnFrom::create("ReceivedTimeStamp1");
  ASSERT_THAT(column_from_exp,
              IsUnexpected("ReceivedTimeStamp cannot have depth"));
}

TEST(
    DataLayerColumnMappingConverterColumnFrom,
    ReturnsErrorMessageOnCreationReceivedTimestampWithVariableDepthFromString) {
  const auto column_from_exp = ColumnFrom::create("ReceivedTimeStamp#");
  ASSERT_THAT(column_from_exp,
              IsUnexpected("ReceivedTimeStamp cannot have depth"));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromMessageTimestampNoDepthStr) {
  const auto column_from_exp = ColumnFrom::create("MessageTimeStamp");
  ASSERT_THAT(column_from_exp,
              IsNoDepthColumn(ColumnFrom::Column::MessageTimestamp));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ReturnsErrorMessageOnCreationMessageTimestampWithDepthFromString) {
  const auto column_from_exp = ColumnFrom::create("MessageTimeStamp1");
  ASSERT_THAT(column_from_exp,
              IsUnexpected("MessageTimeStamp cannot have depth"));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ReturnsErrorMessageOnCreationMessageTimestampWithVariableDepthFromString) {
  const auto column_from_exp = ColumnFrom::create("MessageTimeStamp#");
  ASSERT_THAT(column_from_exp,
              IsUnexpected("MessageTimeStamp cannot have depth"));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromInstrumentNoDepthStr) {
  const auto column_from_exp = ColumnFrom::create("Instrument");
  ASSERT_THAT(column_from_exp, IsNoDepthColumn(ColumnFrom::Column::Instrument));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ReturnsErrorMessageOnCreationInstrumentWithDepthFromString) {
  const auto column_from_exp = ColumnFrom::create("Instrument1");
  ASSERT_THAT(column_from_exp, IsUnexpected("Instrument cannot have depth"));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ReturnsErrorMessageOnCreationInstrumentWithVariableDepthFromString) {
  const auto column_from_exp = ColumnFrom::create("Instrument#");
  ASSERT_THAT(column_from_exp, IsUnexpected("Instrument cannot have depth"));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidPartyWithDepthOneByDefaultStr) {
  const auto column_from_exp = ColumnFrom::create("BidParty");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::BidParty, 1u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidPartyWithDepthStr) {
  const auto column_from_exp = ColumnFrom::create("BidParty42");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::BidParty, 42u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidPartyWithVariableDepthStr) {
  const auto column_from_exp = ColumnFrom::create("BidParty#");
  ASSERT_THAT(column_from_exp,
              IsColumnWithVariableDepth(ColumnFrom::Column::BidParty));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidQuantityWithDepthOneByDefaultStr) {
  const auto column_from_exp = ColumnFrom::create("BidQuantity");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::BidQuantity, 1u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidQuantityWithDepthStr) {
  const auto column_from_exp = ColumnFrom::create("BidQuantity42");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::BidQuantity, 42u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidQuantityWithVariableDepthStr) {
  const auto column_from_exp = ColumnFrom::create("BidQuantity#");
  ASSERT_THAT(column_from_exp,
              IsColumnWithVariableDepth(ColumnFrom::Column::BidQuantity));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidPriceWithDepthOneByDefaultStr) {
  const auto column_from_exp = ColumnFrom::create("BidPrice");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::BidPrice, 1u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidPriceWithDepthStr) {
  const auto column_from_exp = ColumnFrom::create("BidPrice42");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::BidPrice, 42u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidPriceWithVariableDepthStr) {
  const auto column_from_exp = ColumnFrom::create("BidPrice#");
  ASSERT_THAT(column_from_exp,
              IsColumnWithVariableDepth(ColumnFrom::Column::BidPrice));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromOfferPriceWithDepthOneByDefaultStr) {
  const auto column_from_exp = ColumnFrom::create("AskPrice");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::OfferPrice, 1u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromOfferPriceWithDepthStr) {
  const auto column_from_exp = ColumnFrom::create("AskPrice42");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::OfferPrice, 42u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromOfferPriceWithVariableDepth) {
  const auto column_from_exp = ColumnFrom::create("AskPrice#");
  ASSERT_THAT(column_from_exp,
              IsColumnWithVariableDepth(ColumnFrom::Column::OfferPrice));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromOfferQuantityWithDepthOneByDefault) {
  const auto column_from_exp = ColumnFrom::create("AskQuantity");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::OfferQuantity, 1u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromOfferQuantityWithDepth) {
  const auto column_from_exp = ColumnFrom::create("AskQuantity42");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::OfferQuantity, 42u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromOfferQuantityWithVariableDepth) {
  const auto column_from_exp = ColumnFrom::create("AskQuantity#");
  ASSERT_THAT(column_from_exp,
              IsColumnWithVariableDepth(ColumnFrom::Column::OfferQuantity));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromOfferPartyWithDepthOneByDefault) {
  const auto column_from_exp = ColumnFrom::create("AskParty");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::OfferParty, 1u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromOfferPartyWithDepth) {
  const auto column_from_exp = ColumnFrom::create("AskParty42");
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::OfferParty, 42u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromOfferPartyWithVariableDepth) {
  const auto column_from_exp = ColumnFrom::create("AskParty#");
  ASSERT_THAT(column_from_exp,
              IsColumnWithVariableDepth(ColumnFrom::Column::OfferParty));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromReceivedTimestampNoDepth) {
  const auto column_from_exp = ColumnFrom::create(
      ColumnFrom::Column::ReceivedTimestamp, ColumnFrom::NoDepth{});
  ASSERT_THAT(column_from_exp,
              IsNoDepthColumn(ColumnFrom::Column::ReceivedTimestamp));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ReturnsErrorMessageOnCreationReceivedTimestampWithDepth) {
  const auto column_from_exp =
      ColumnFrom::create(ColumnFrom::Column::ReceivedTimestamp, 42u);
  ASSERT_THAT(column_from_exp,
              IsUnexpected("ReceivedTimeStamp cannot have depth"));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ReturnsErrorMessageOnCreationReceivedTimestampWithVariableDepth) {
  const auto column_from_exp = ColumnFrom::create(
      ColumnFrom::Column::ReceivedTimestamp, ColumnFrom::VariableDepth{});
  ASSERT_THAT(column_from_exp,
              IsUnexpected("ReceivedTimeStamp cannot have depth"));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidPartyWithDepthOneByDefault) {
  const auto column_from_exp =
      ColumnFrom::create(ColumnFrom::Column::BidParty, ColumnFrom::NoDepth{});
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::BidParty, 1u));
}

TEST(DataLayerColumnMappingConverterColumnFrom, CreatesFromBidPartyWithDepth) {
  const auto column_from_exp =
      ColumnFrom::create(ColumnFrom::Column::BidParty, 42u);
  ASSERT_THAT(column_from_exp,
              IsColumnWithDepth(ColumnFrom::Column::BidParty, 42u));
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     CreatesFromBidPartyWithVariableDepth) {
  const auto column_from_exp = ColumnFrom::create(ColumnFrom::Column::BidParty,
                                                  ColumnFrom::VariableDepth{});
  ASSERT_THAT(column_from_exp,
              IsColumnWithVariableDepth(ColumnFrom::Column::BidParty));
}

TEST(DataLayerColumnMappingConverterExtractColumnName,
     ReturnsColumnNameFromColumnWithVariableDepth) {
  const auto name = extract_column_name_from_variable_depth("column#");
  ASSERT_EQ(name, "column");
}

TEST(DataLayerColumnMappingConverterExtractColumnName,
     ReturnsNulloptIfColumnNotEndWithVariableDepth) {
  const auto name = extract_column_name_from_variable_depth("column");
  ASSERT_EQ(name, std::nullopt);
}

TEST(DataLayerColumnMappingConverterExtractColumnName,
     ReturnsNulloptIfColumnContainsOnlyVariableDepth) {
  const auto name = extract_column_name_from_variable_depth("#");
  ASSERT_EQ(name, std::nullopt);
}

TEST(DataLayerColumnMappingConverterColumnFrom, NoDepthIsUnordered) {
  const ColumnFrom::Depth no_depth{ColumnFrom::NoDepth{}};
  ASSERT_EQ(no_depth <=> no_depth, std::partial_ordering::unordered);
}

TEST(DataLayerColumnMappingConverterColumnFrom, NoDepthIsEqualToItself) {
  const ColumnFrom::Depth no_depth{ColumnFrom::NoDepth{}};
  ASSERT_EQ(no_depth, no_depth);
}

TEST(DataLayerColumnMappingConverterColumnFrom, VariableDepthIsUnordered) {
  const ColumnFrom::Depth variable_depth{ColumnFrom::VariableDepth{}};
  ASSERT_EQ(variable_depth <=> variable_depth,
            std::partial_ordering::unordered);
}

TEST(DataLayerColumnMappingConverterColumnFrom, VariableDepthIsEqualToItself) {
  const ColumnFrom::Depth variable_depth{ColumnFrom::VariableDepth{}};
  ASSERT_EQ(variable_depth, variable_depth);
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     NoDepthIsLessThanVariableDepth) {
  const ColumnFrom::Depth no_depth{ColumnFrom::NoDepth{}};
  const ColumnFrom::Depth variable_depth{ColumnFrom::VariableDepth{}};
  ASSERT_LT(no_depth, variable_depth);
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     VariantDepthIsLessThanIntegerDepth) {
  const ColumnFrom::Depth variable_depth{ColumnFrom::VariableDepth{}};
  const ColumnFrom::Depth depth{42u};
  ASSERT_LT(variable_depth, depth);
}

TEST(DataLayerColumnMappingConverterColumnFrom, Depth1IsLessThanDepth2) {
  const ColumnFrom::Depth depth1{1u};
  const ColumnFrom::Depth depth2{2u};
  ASSERT_LT(depth1, depth2);
}

TEST(DataLayerColumnMappingConverterColumnFrom, FmtFormatsNoDepth) {
  ASSERT_EQ(fmt::to_string(ColumnFrom::Depth{ColumnFrom::NoDepth{}}),
            "NoDepth");
}

TEST(DataLayerColumnMappingConverterColumnFrom, FmtFormatsVariableDepth) {
  ASSERT_EQ(fmt::to_string(ColumnFrom::Depth{ColumnFrom::VariableDepth{}}),
            "VariableDepth");
}

TEST(DataLayerColumnMappingConverterColumnFrom, FmtFormatsDepth) {
  ASSERT_EQ(fmt::to_string(ColumnFrom::Depth{42u}), "42");
}

TEST(DataLayerColumnMappingConverterColumnFrom, FmtFormatsColumnWithNoDepth) {
  const auto column_from = ColumnFrom::create("ReceivedTimeStamp");
  ASSERT_TRUE(column_from.has_value());
  ASSERT_EQ(fmt::to_string(column_from.value()),
            R"({ "column": "ReceivedTimeStamp", "depth": "NoDepth" })");
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     FmtFormatsColumnWithVariableDepth) {
  const auto column_from = ColumnFrom::create("BidParty#");
  ASSERT_TRUE(column_from.has_value());
  ASSERT_EQ(fmt::to_string(column_from.value()),
            R"({ "column": "BidParty", "depth": "VariableDepth" })");
}

TEST(DataLayerColumnMappingConverterColumnFrom, FmtFormatsColumnWithDepth) {
  const auto column_from = ColumnFrom::create("BidParty42");
  ASSERT_TRUE(column_from.has_value());
  ASSERT_EQ(fmt::to_string(column_from.value()),
            R"({ "column": "BidParty", "depth": "42" })");
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ReceivedTimestampIsLessThanMessageTimestamp) {
  ASSERT_LT(ColumnFrom::ReceivedTimestamp, ColumnFrom::MessageTimestamp);
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     MessageTimestampIsLessThanInstrument) {
  ASSERT_LT(ColumnFrom::MessageTimestamp, ColumnFrom::Instrument);
}

TEST(DataLayerColumnMappingConverterColumnFrom, InstrumentIsLessThanBidParty) {
  const auto bid_party = ColumnFrom::create("BidParty1");

  ASSERT_LT(ColumnFrom::Instrument, bid_party.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom, BidPartyIsLessThanBidQuantity) {
  const auto bid_party = ColumnFrom::create("BidParty1");
  const auto bid_quantity = ColumnFrom::create("BidQuantity1");

  ASSERT_LT(bid_party.value(), bid_quantity.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom, BidQuantityIsLessThanBidPrice) {
  const auto bid_quantity = ColumnFrom::create("BidQuantity1");
  const auto bid_price = ColumnFrom::create("BidPrice1");

  ASSERT_LT(bid_quantity.value(), bid_price.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom, BidPriceIsLessThanOfferPrice) {
  const auto bid_price = ColumnFrom::create("BidPrice1");
  const auto offer_price = ColumnFrom::create("AskPrice1");

  ASSERT_LT(bid_price.value(), offer_price.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     OfferPriceIsLessThanOfferQuantity) {
  const auto offer_price = ColumnFrom::create("AskPrice1");
  const auto offer_quantity = ColumnFrom::create("AskQuantity1");

  ASSERT_LT(offer_price.value(), offer_quantity.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     OfferQuantityIsLessThanOfferParty) {
  const auto offer_quantity = ColumnFrom::create("AskQuantity1");
  const auto offer_party = ColumnFrom::create("AskParty1");

  ASSERT_LT(offer_quantity.value(), offer_party.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesVariableDepthLevelWithDepthForBidParties) {
  const auto bid_party = ColumnFrom::create("BidParty#");
  const auto bid_party1 = ColumnFrom::create("BidParty1");

  ASSERT_LT(bid_party.value(), bid_party1.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesDepthLevelForBidParties) {
  const auto bid_party1 = ColumnFrom::create("BidParty1");
  const auto bid_party2 = ColumnFrom::create("BidParty2");

  ASSERT_LT(bid_party1.value(), bid_party2.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesVariableDepthLevelWithDepthForBidQuantities) {
  const auto bid_quantity = ColumnFrom::create("BidQuantity#");
  const auto bid_quantity1 = ColumnFrom::create("BidQuantity1");

  ASSERT_LT(bid_quantity.value(), bid_quantity1.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesDepthLevelForBidQuantities) {
  const auto bid_quantity1 = ColumnFrom::create("BidQuantity1");
  const auto bid_quantity2 = ColumnFrom::create("BidQuantity2");

  ASSERT_LT(bid_quantity1.value(), bid_quantity2.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesVariableDepthLevelWithDepthForBidPrices) {
  const auto bid_price = ColumnFrom::create("BidPrice#");
  const auto bid_price1 = ColumnFrom::create("BidPrice1");

  ASSERT_LT(bid_price.value(), bid_price1.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesDepthLevelForBidPrices) {
  const auto bid_price1 = ColumnFrom::create("BidPrice1");
  const auto bid_price2 = ColumnFrom::create("BidPrice2");

  ASSERT_LT(bid_price1.value(), bid_price2.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesVariableDepthLevelWithDepthForOfferPrices) {
  const auto offer_price = ColumnFrom::create("AskPrice#");
  const auto offer_price1 = ColumnFrom::create("AskPrice1");

  ASSERT_LT(offer_price.value(), offer_price1.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesDepthLevelForOfferPrices) {
  const auto offer_price1 = ColumnFrom::create("AskPrice1");
  const auto offer_price2 = ColumnFrom::create("AskPrice2");

  ASSERT_LT(offer_price1.value(), offer_price2.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesVariableDepthLevelWithDepthForOfferQuantities) {
  const auto offer_quantity = ColumnFrom::create("AskQuantity#");
  const auto offer_quantity1 = ColumnFrom::create("AskQuantity1");

  ASSERT_LT(offer_quantity.value(), offer_quantity1.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesDepthLevelForOfferQuantities) {
  const auto offer_quantity1 = ColumnFrom::create("AskQuantity1");
  const auto offer_quantity2 = ColumnFrom::create("AskQuantity2");

  ASSERT_LT(offer_quantity1.value(), offer_quantity2.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesVariableDepthLevelWithDepthForOfferParties) {
  const auto offer_party = ColumnFrom::create("AskParty#");
  const auto offer_party1 = ColumnFrom::create("AskParty1");

  ASSERT_LT(offer_party.value(), offer_party1.value());
}

TEST(DataLayerColumnMappingConverterColumnFrom,
     ComparesDepthLevelForOfferParties) {
  const auto offer_party1 = ColumnFrom::create("AskParty1");
  const auto offer_party2 = ColumnFrom::create("AskParty2");

  ASSERT_LT(offer_party1.value(), offer_party2.value());
}

struct DataLayerColumnMappingConverterColumnFromColumn
    : public TestWithParam<std::pair<ColumnFrom::Column, std::string_view>> {};

TEST_P(DataLayerColumnMappingConverterColumnFromColumn, FmtFormatting) {
  const auto [column, expected] = GetParam();

  ASSERT_EQ(fmt::to_string(column), expected);
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    Formatting,
    DataLayerColumnMappingConverterColumnFromColumn,
    Values(
        std::make_pair(static_cast<ColumnFrom::Column>(0xFF), "undefined"),
        std::make_pair(ColumnFrom::Column::ReceivedTimestamp, "ReceivedTimeStamp"),
        std::make_pair(ColumnFrom::Column::MessageTimestamp, "MessageTimeStamp"),
        std::make_pair(ColumnFrom::Column::Instrument, "Instrument"),
        std::make_pair(ColumnFrom::Column::BidParty, "BidParty"),
        std::make_pair(ColumnFrom::Column::BidQuantity, "BidQuantity"),
        std::make_pair(ColumnFrom::Column::BidPrice, "BidPrice"),
        std::make_pair(ColumnFrom::Column::OfferPrice, "AskPrice"),
        std::make_pair(ColumnFrom::Column::OfferQuantity, "AskQuantity"),
        std::make_pair(ColumnFrom::Column::OfferParty, "AskParty")));
// clang-format on

}  // namespace
}  // namespace simulator::data_layer::converter::test
