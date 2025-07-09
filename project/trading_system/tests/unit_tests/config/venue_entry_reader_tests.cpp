#include <gtest/gtest.h>

#include "data_layer/api/models/venue.hpp"
#include "ih/config/config.hpp"
#include "ih/config/venue_entry_reader.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace testing;  // NOLINT

struct TradingSystemVenueEntryReader : public Test {
  using Venue = data_layer::Venue;

  auto SetUp() -> void override { patch.with_venue_id("LSE"); }

  Config config;
  VenueEntryReader reader{config};

  Venue::Patch patch;
};

TEST_F(TradingSystemVenueEntryReader, SetsSupportDayOrdersByDefaultTrue) {
  reader(Venue::create(patch));
  ASSERT_TRUE(config.support_day_orders_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsSupportDayOrdersFromVenue) {
  patch.with_support_tif_day_flag(false);
  reader(Venue::create(patch));
  ASSERT_FALSE(config.support_day_orders_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsSupportIocOrdersByDefaultTrue) {
  reader(Venue::create(patch));
  ASSERT_TRUE(config.support_ioc_orders_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsSupportIocOrdersFromVenue) {
  patch.with_support_tif_ioc_flag(false);
  reader(Venue::create(patch));
  ASSERT_FALSE(config.support_ioc_orders_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsSupportFokOrdersByDefaultTrue) {
  reader(Venue::create(patch));
  ASSERT_TRUE(config.support_fok_orders_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsSupportFokOrdersFromVenue) {
  patch.with_support_tif_fok_flag(false);
  reader(Venue::create(patch));
  ASSERT_FALSE(config.support_fok_orders_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsCancelOnDisconnectByDefaultFalse) {
  reader(Venue::create(patch));
  ASSERT_FALSE(config.cod_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsCancelOnDisconnectFromVenue) {
  patch.with_cancel_on_disconnect_flag(true);
  reader(Venue::create(patch));
  ASSERT_TRUE(config.cod_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsTradeStreamingByDefaultFalse) {
  reader(Venue::create(patch));
  ASSERT_FALSE(config.trade_streaming_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsTradeStreamingFromVenue) {
  patch.with_tns_enabled_flag(true);
  reader(Venue::create(patch));
  ASSERT_TRUE(config.trade_streaming_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsTradeVolumeStreamingByDefaultFalse) {
  reader(Venue::create(patch));
  ASSERT_FALSE(config.trade_volume_streaming_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsTradeVolumeStreamingFromVenue) {
  patch.with_tns_qty_enabled_flag(true);
  reader(Venue::create(patch));
  ASSERT_TRUE(config.trade_volume_streaming_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsTradePartiesStreamingByDefaultFalse) {
  reader(Venue::create(patch));
  ASSERT_FALSE(config.trade_parties_streaming_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsTradePartiesStreamingFromVenue) {
  patch.with_tns_parties_enabled_flag(true);
  reader(Venue::create(patch));
  ASSERT_TRUE(config.trade_parties_streaming_enabled());
}

TEST_F(TradingSystemVenueEntryReader,
       SetsTradeAggressorStreamingByDefaultFalse) {
  reader(Venue::create(patch));
  ASSERT_FALSE(config.trade_aggressor_streaming_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsTradeAggressorStreamingFromVenue) {
  patch.with_tns_side_enabled_flag(true);
  reader(Venue::create(patch));
  ASSERT_TRUE(config.trade_aggressor_streaming_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsDepthOrdersExclusionByDefaultFalse) {
  reader(Venue::create(patch));
  ASSERT_FALSE(config.depth_orders_exclusion_enabled());
}

TEST_F(TradingSystemVenueEntryReader,
       SetsDepthOrdersExclusionFromVenueAsInvertedIncludeOwnOrders) {
  patch.with_include_own_orders_flag(false);
  reader(Venue::create(patch));
  ASSERT_TRUE(config.depth_orders_exclusion_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsPersistenceByDefaultFalse) {
  reader(Venue::create(patch));
  ASSERT_FALSE(config.persistence_enabled());
}

TEST_F(TradingSystemVenueEntryReader, SetsPersistenceFromVenue) {
  patch.with_persistence_enabled_flag(true);
  reader(Venue::create(patch));
  ASSERT_TRUE(config.persistence_enabled());
}

TEST_F(TradingSystemVenueEntryReader, LeavesPersistenceFilePathByDefaultEmpty) {
  reader(Venue::create(patch));
  ASSERT_TRUE(config.persistence_file_path().empty());
}

TEST_F(TradingSystemVenueEntryReader, SetsPersistenceFilePathFromVenueAsEmpty) {
  patch.with_persistence_file_path({});
  reader(Venue::create(patch));
  ASSERT_TRUE(config.persistence_file_path().empty());
}

TEST_F(TradingSystemVenueEntryReader, SetsPersistenceFilePathFromVenue) {
  patch.with_persistence_file_path("persistence_path");
  reader(Venue::create(patch));
  ASSERT_EQ(config.persistence_file_path(), "persistence_path");
}

TEST_F(TradingSystemVenueEntryReader,
       LeavesTzClockDefaultConstructableByDefault) {
  reader(Venue::create(patch));
  ASSERT_EQ(config.timezone_clock(), core::TzClock{});
}

TEST_F(TradingSystemVenueEntryReader, SetsTzClockWithTimezoneFromVenue) {
  patch.with_timezone("Europe/Kyiv");
  reader(Venue::create(patch));
  ASSERT_EQ(config.timezone_clock(), core::TzClock{"Europe/Kyiv"});
}

TEST_F(TradingSystemVenueEntryReader, SetsTzClockWithDefaultTimezone) {
  patch.with_timezone("UTC");
  reader(Venue::create(patch));
  ASSERT_EQ(config.timezone_clock(), core::TzClock{"UTC"});
}

}  // namespace
}  // namespace simulator::trading_system::test
