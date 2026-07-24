#include <fmt/chrono.h>
#include <gmock/gmock.h>

#include <optional>

#include "common/instrument_state.hpp"
#include "ih/state_persistence/json/detail/reader.hpp"
#include "ih/state_persistence/json/detail/writer.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::json::test {
namespace {

using namespace ::testing;

struct TradingSystemJsonInstrumentInfo : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value json_value;
};

TEST_F(TradingSystemJsonInstrumentInfo, ReadsFromJson) {
  json_value.SetObject();
  json_value.AddMember("TradingSessionLowPrice", 100.1, doc.GetAllocator());
  json_value.AddMember("TradingSessionHighPrice", 300.3, doc.GetAllocator());
  json_value.AddMember("OpeningPrice", 110.5, doc.GetAllocator());
  json_value.AddMember("ClosingPrice", 120.6, doc.GetAllocator());
  json_value.AddMember("AuctionClearingPrice", 130.7, doc.GetAllocator());
  json_value.AddMember("AuctionClearingQuantity", 500.0, doc.GetAllocator());
  json_value.AddMember("PreviousClosingPrice", 95.4, doc.GetAllocator());

  market_state::InstrumentInfo info;
  ASSERT_TRUE(json::read(json_value, info).has_value());

  EXPECT_EQ(info.low_price, Price{100.1});
  EXPECT_EQ(info.high_price, Price{300.3});
  EXPECT_EQ(info.opening_price, Price{110.5});
  EXPECT_EQ(info.closing_price, Price{120.6});
  EXPECT_EQ(info.auction_clearing_price, Price{130.7});
  EXPECT_EQ(info.auction_clearing_quantity, Quantity{500.0});
  EXPECT_EQ(info.previous_closing_price, Price{95.4});
}

TEST_F(TradingSystemJsonInstrumentInfo, WritingSetsJsonValueTypeToObject) {
  using namespace simulator::trading_system::test;

  const market_state::InstrumentInfo instrument_info{
      .low_price = Price{100.1}, .high_price = Price{300.3}};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_info).has_value());
  ASSERT_TRUE(json_value.IsObject());
}

TEST_F(TradingSystemJsonInstrumentInfo, WritesToJson) {
  using namespace simulator::trading_system::test;

  const market_state::InstrumentInfo instrument_info{
      .low_price = Price{100.1},
      .high_price = Price{300.3},
      .opening_price = Price{110.5},
      .closing_price = Price{120.6},
      .auction_clearing_price = Price{130.7},
      .auction_clearing_quantity = Quantity{500.0},
      .previous_closing_price = Price{95.4}};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_info).has_value());

  EXPECT_THAT(json_value, HasDouble("TradingSessionLowPrice", 100.1));
  EXPECT_THAT(json_value, HasDouble("TradingSessionHighPrice", 300.3));
  EXPECT_THAT(json_value, HasDouble("OpeningPrice", 110.5));
  EXPECT_THAT(json_value, HasDouble("ClosingPrice", 120.6));
  EXPECT_THAT(json_value, HasDouble("AuctionClearingPrice", 130.7));
  EXPECT_THAT(json_value, HasDouble("AuctionClearingQuantity", 500.0));
  EXPECT_THAT(json_value, HasDouble("PreviousClosingPrice", 95.4));
}

TEST_F(TradingSystemJsonInstrumentInfo, WritesUnsetAuctionFieldsAsNull) {
  using namespace simulator::trading_system::test;

  const market_state::InstrumentInfo instrument_info{
      .low_price = Price{100.1}, .high_price = Price{300.3}};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_info).has_value());

  EXPECT_THAT(json_value, HasNull("OpeningPrice"));
  EXPECT_THAT(json_value, HasNull("AuctionClearingPrice"));
  EXPECT_THAT(json_value, HasNull("AuctionClearingQuantity"));
}

TEST_F(TradingSystemJsonInstrumentInfo, ReadsMissingAuctionFieldsAsUnset) {
  json_value.SetObject();
  json_value.AddMember("TradingSessionLowPrice", 100.1, doc.GetAllocator());
  json_value.AddMember("TradingSessionHighPrice", 300.3, doc.GetAllocator());

  market_state::InstrumentInfo info;
  ASSERT_TRUE(json::read(json_value, info).has_value());

  EXPECT_EQ(info.opening_price, std::nullopt);
  EXPECT_EQ(info.closing_price, std::nullopt);
  EXPECT_EQ(info.auction_clearing_price, std::nullopt);
  EXPECT_EQ(info.auction_clearing_quantity, std::nullopt);
  EXPECT_EQ(info.previous_closing_price, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentInfo, ReadsNullAuctionFieldAsUnset) {
  rapidjson::Value null_value;
  json_value.SetObject();
  json_value.AddMember("OpeningPrice", null_value, doc.GetAllocator());

  market_state::InstrumentInfo info;
  ASSERT_TRUE(json::read(json_value, info).has_value());

  EXPECT_EQ(info.opening_price, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentInfo, WritesOpeningPriceTimestampsToJson) {
  using namespace std::chrono_literals;
  using namespace simulator::trading_system::test;

  constexpr auto opening_time =
      core::sys_us{core::sys_days{2025y / 12 / 31} + 9h + 30min};
  const market_state::InstrumentInfo instrument_info{
      .opening_price = Price{110.5}, .opening_price_time = opening_time};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_info).has_value());

  EXPECT_THAT(
      json_value,
      HasString("OpeningPriceTime", fmt::format("{:%Y%m%d-%T}", opening_time)));
}

TEST_F(TradingSystemJsonInstrumentInfo, ReadsOpeningPriceTimestampsFromJson) {
  using namespace std::chrono_literals;

  constexpr auto opening_time =
      core::sys_us{core::sys_days{2025y / 12 / 31} + 9h + 30min};
  const auto opening_str = fmt::format("{:%Y%m%d-%T}", opening_time);
  json_value.SetObject();
  json_value.AddMember(
      "OpeningPriceTime",
      rapidjson::Value{opening_str.c_str(), doc.GetAllocator()},
      doc.GetAllocator());

  market_state::InstrumentInfo info;
  ASSERT_TRUE(json::read(json_value, info).has_value());

  EXPECT_EQ(info.opening_price_time, opening_time);
}

TEST_F(TradingSystemJsonInstrumentInfo,
       WritesUnsetOpeningPriceTimestampsAsNull) {
  using namespace simulator::trading_system::test;

  const market_state::InstrumentInfo instrument_info{.opening_price =
                                                         Price{110.5}};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_info).has_value());

  EXPECT_THAT(json_value, HasNull("OpeningPriceTime"));
}

TEST_F(TradingSystemJsonInstrumentInfo,
       ReadsMissingOpeningPriceTimestampsAsUnset) {
  json_value.SetObject();
  json_value.AddMember("OpeningPrice", 110.5, doc.GetAllocator());

  market_state::InstrumentInfo info;
  ASSERT_TRUE(json::read(json_value, info).has_value());

  EXPECT_EQ(info.opening_price_time, std::nullopt);
}

TEST_F(TradingSystemJsonInstrumentInfo, WritesClosingPriceTimestampsToJson) {
  using namespace std::chrono_literals;
  using namespace simulator::trading_system::test;

  constexpr auto closing_time =
      core::sys_us{core::sys_days{2025y / 12 / 31} + 17h + 30min};
  const market_state::InstrumentInfo instrument_info{
      .closing_price = Price{120.5}, .closing_price_time = closing_time};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_info).has_value());

  EXPECT_THAT(
      json_value,
      HasString("ClosingPriceTime", fmt::format("{:%Y%m%d-%T}", closing_time)));
}

TEST_F(TradingSystemJsonInstrumentInfo, ReadsClosingPriceTimestampsFromJson) {
  using namespace std::chrono_literals;

  constexpr auto closing_time =
      core::sys_us{core::sys_days{2025y / 12 / 31} + 17h + 30min};
  const auto closing_str = fmt::format("{:%Y%m%d-%T}", closing_time);
  json_value.SetObject();
  json_value.AddMember(
      "ClosingPriceTime",
      rapidjson::Value{closing_str.c_str(), doc.GetAllocator()},
      doc.GetAllocator());

  market_state::InstrumentInfo info;
  ASSERT_TRUE(json::read(json_value, info).has_value());

  EXPECT_EQ(info.closing_price_time, closing_time);
}

TEST_F(TradingSystemJsonInstrumentInfo,
       WritesUnsetClosingPriceTimestampsAsNull) {
  using namespace simulator::trading_system::test;

  const market_state::InstrumentInfo instrument_info{.closing_price =
                                                         Price{120.5}};

  ASSERT_TRUE(
      json::write(json_value, doc.GetAllocator(), instrument_info).has_value());

  EXPECT_THAT(json_value, HasNull("ClosingPriceTime"));
}

TEST_F(TradingSystemJsonInstrumentInfo,
       ReadsMissingClosingPriceTimestampsAsUnset) {
  json_value.SetObject();
  json_value.AddMember("ClosingPrice", 120.5, doc.GetAllocator());

  market_state::InstrumentInfo info;
  ASSERT_TRUE(json::read(json_value, info).has_value());

  EXPECT_EQ(info.closing_price_time, std::nullopt);
}

}  // namespace
}  // namespace simulator::trading_system::json::test
