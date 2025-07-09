#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "api/models/venue.hpp"
#include "ih/common/exceptions.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

using EngineType = Venue::EngineType;

struct DataLayerModelsVenue : public ::testing::Test {
  Venue::Patch patch;
};

TEST_F(DataLayerModelsVenue, Patch_Set_VenueID) {
  ASSERT_FALSE(patch.venue_id().has_value());

  patch.with_venue_id("FASTMATCH");
  EXPECT_THAT(patch.venue_id(), Optional(Eq("FASTMATCH")));
}

TEST_F(DataLayerModelsVenue, Patch_Set_Name) {
  ASSERT_FALSE(patch.name().has_value());

  patch.with_name("London Stock Exchange");
  EXPECT_THAT(patch.name(), Optional(Eq("London Stock Exchange")));
}

TEST_F(DataLayerModelsVenue, Patch_Set_EngineType) {
  ASSERT_FALSE(patch.engine_type().has_value());

  patch.with_engine_type(EngineType::Quoting);
  EXPECT_THAT(patch.engine_type(), Optional(Eq(EngineType::Quoting)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_SupportTifIocFlag) {
  ASSERT_FALSE(patch.support_tif_ioc_flag().has_value());

  patch.with_support_tif_ioc_flag(true);
  EXPECT_THAT(patch.support_tif_ioc_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_SupportTifFokFlag) {
  ASSERT_FALSE(patch.support_tif_fok_flag().has_value());

  patch.with_support_tif_fok_flag(false);
  EXPECT_THAT(patch.support_tif_fok_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_SupportTifDayFlag) {
  ASSERT_FALSE(patch.support_tif_day_flag().has_value());

  patch.with_support_tif_day_flag(true);
  EXPECT_THAT(patch.support_tif_day_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_IncludeOwnOrdersFlag) {
  ASSERT_FALSE(patch.include_own_orders_flag().has_value());

  patch.with_include_own_orders_flag(false);
  EXPECT_THAT(patch.include_own_orders_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_RestPort) {
  ASSERT_FALSE(patch.rest_port().has_value());

  patch.with_rest_port(9001);  // NOLINT: Test value
  EXPECT_THAT(patch.rest_port(), Optional(Eq(9001)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_OrdersOnStartupFlag) {
  ASSERT_FALSE(patch.orders_on_startup_flag().has_value());

  patch.with_orders_on_startup_flag(true);
  EXPECT_THAT(patch.orders_on_startup_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_RandomPartiesCount) {
  ASSERT_FALSE(patch.random_parties_count().has_value());

  patch.with_random_parties_count(50);  // NOLINT: Test value
  EXPECT_THAT(patch.random_parties_count(), Optional(Eq(50)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_TnsEnabledFlag) {
  ASSERT_FALSE(patch.tns_enabled_flag().has_value());

  patch.with_tns_enabled_flag(true);
  EXPECT_THAT(patch.tns_enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_TnsQtyEnabledFlag) {
  ASSERT_FALSE(patch.tns_qty_enabled_flag().has_value());

  patch.with_tns_qty_enabled_flag(true);
  EXPECT_THAT(patch.tns_qty_enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_TnsSideEnabledFlag) {
  ASSERT_FALSE(patch.tns_side_enabled_flag().has_value());

  patch.with_tns_side_enabled_flag(false);
  EXPECT_THAT(patch.tns_side_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_TnsPartiesEnabledFlag) {
  ASSERT_FALSE(patch.tns_parties_enabled_flag().has_value());

  patch.with_tns_parties_enabled_flag(false);
  EXPECT_THAT(patch.tns_parties_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_Timezone) {
  ASSERT_FALSE(patch.timezone().has_value());

  patch.with_timezone("GMT+3");
  EXPECT_THAT(patch.timezone(), Optional(Eq("GMT+3")));
}

TEST_F(DataLayerModelsVenue, Patch_Set_CancelOnDiconnectFlag) {
  ASSERT_FALSE(patch.cancel_on_disconnect_flag().has_value());

  patch.with_cancel_on_disconnect_flag(true);
  EXPECT_THAT(patch.cancel_on_disconnect_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_PersistenceEnabledFlag) {
  ASSERT_FALSE(patch.persistence_enabled_flag().has_value());

  patch.with_persistence_enabled_flag(true);
  EXPECT_THAT(patch.persistence_enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsVenue, Patch_Set_PersistenceFilePath) {
  ASSERT_FALSE(patch.persistence_file_path().has_value());

  patch.with_persistence_file_path("/rw/sim-storage.json");
  EXPECT_THAT(patch.persistence_file_path(),
              Optional(Eq("/rw/sim-storage.json")));
}

TEST_F(DataLayerModelsVenue, Patch_Set_MarketPhases_Add) {
  const MarketPhase::Patch market_phase;

  ASSERT_FALSE(patch.persistence_file_path().has_value());

  patch.with_market_phase(market_phase);
  patch.with_market_phase(market_phase);

  ASSERT_TRUE(patch.market_phases().has_value());
  EXPECT_EQ(patch.market_phases()->size(), 2);  // NOLINT: check is upper
}

TEST_F(DataLayerModelsVenue, Patch_Set_MarketPhases_Clean) {
  ASSERT_FALSE(patch.persistence_file_path().has_value());

  patch.without_market_phases();
  ASSERT_TRUE(patch.market_phases().has_value());
  EXPECT_TRUE(patch.market_phases()->empty());  // NOLINT: check is upper
}

TEST_F(DataLayerModelsVenue, Patch_Set_MarketPhases_Clean_AssignedPreviously) {
  const MarketPhase::Patch market_phase;

  patch.with_market_phase(market_phase);
  patch.with_market_phase(market_phase);
  ASSERT_TRUE(patch.market_phases().has_value());
  ASSERT_EQ(patch.market_phases()->size(), 2);  // NOLINT: check is upper

  patch.without_market_phases();
  ASSERT_TRUE(patch.market_phases().has_value());
  EXPECT_TRUE(patch.market_phases()->empty());  // NOLINT: check is upper
}

TEST_F(DataLayerModelsVenue, Get_VenueID_Missing) {
  ASSERT_FALSE(patch.venue_id().has_value());

  EXPECT_THROW((void)Venue::create(patch), RequiredAttributeMissing);
}

TEST_F(DataLayerModelsVenue, Get_VenueID_Specified) {
  patch.with_venue_id("XETRA");
  ASSERT_TRUE(patch.venue_id().has_value());

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.venue_id(), "XETRA");
}

TEST_F(DataLayerModelsVenue, Get_Name_Missing) {
  patch.with_venue_id("XETRA");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.name(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_Name_Specified) {
  patch.with_venue_id("LSE");
  patch.with_name("London Stock Exchange");

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.name(), Optional(Eq("London Stock Exchange")));
}

TEST_F(DataLayerModelsVenue, Get_EngineType_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.engine_type(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_EngineType_Specified) {
  patch.with_venue_id("LSE");
  patch.with_engine_type(EngineType::Matching);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.engine_type(), Optional(Eq(EngineType::Matching)));
}

TEST_F(DataLayerModelsVenue, Get_SupportTifIocFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.support_tif_ioc_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_SupportTifIocFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_support_tif_ioc_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.support_tif_ioc_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_SupportTifFokFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.support_tif_fok_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_SupportTifFokFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_support_tif_fok_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.support_tif_fok_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_SupportTifDayFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.support_tif_day_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_SupportTifDayFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_support_tif_day_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.support_tif_day_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_IncludeOwnOrdersFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.include_own_orders_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_IncludeOwnOrdersFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_include_own_orders_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.include_own_orders_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_RestPort_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.rest_port(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_RestPort_Specified) {
  patch.with_venue_id("LSE");
  patch.with_rest_port(9001);  // NOLINT: Test value

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.rest_port(), Optional(Eq(9001)));
}

TEST_F(DataLayerModelsVenue, Get_OrdersOnStartupFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.order_on_startup_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_OrdersOnStartupFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_orders_on_startup_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.order_on_startup_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_RandomPartiesCount_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.random_parties_count(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_RandomPartiesCount_Specified) {
  patch.with_venue_id("LSE");
  patch.with_random_parties_count(42);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.random_parties_count(), Optional(Eq(42)));
}

TEST_F(DataLayerModelsVenue, Get_TnsEnabledFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.tns_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_TnsEnabledFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_tns_enabled_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.tns_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_TnsQtyEnabledFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.tns_qty_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_TnsQtyEnabledFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_tns_qty_enabled_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.tns_qty_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_TnsSideEnabledFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.tns_side_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_TnsSideEnabledFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_tns_side_enabled_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.tns_side_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_TnsPartiesEnabledFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.tns_parties_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_TnsPartiesEnabledFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_tns_parties_enabled_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.tns_parties_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_Timezone_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.timezone(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_Timezone_Specified) {
  patch.with_venue_id("LSE");
  patch.with_timezone("GMT+3");

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.timezone(), Optional(Eq("GMT+3")));
}

TEST_F(DataLayerModelsVenue, Get_CancelOnDisconnectFlag_Missing) {
  patch.with_venue_id("LSE");
  ASSERT_FALSE(patch.cancel_on_disconnect_flag().has_value());

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.cancel_on_disconnect_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_CancelOnDisconnectFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_cancel_on_disconnect_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.cancel_on_disconnect_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, Get_PersistenceEnabledFlag_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.persistence_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_PersistenceEnabledFlag_Specified) {
  patch.with_venue_id("LSE");
  patch.with_persistence_enabled_flag(true);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.persistence_enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsVenue, Get_PersistenceFilePath_Missing) {
  patch.with_venue_id("LSE");

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.persistence_file_path(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, Get_PersistenceFilePath_Specified) {
  patch.with_venue_id("LSE");
  patch.with_persistence_file_path("/rw/state.json");

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.persistence_file_path(), Optional(Eq("/rw/state.json")));
}

TEST_F(DataLayerModelsVenue, Get_MarketPhases_Missing) {
  patch.with_venue_id("LSE");
  ASSERT_FALSE(patch.market_phases().has_value());

  const Venue venue = Venue::create(patch);
  EXPECT_TRUE(venue.market_phases().empty());
}

TEST_F(DataLayerModelsVenue, Get_MarketPhases_Empty) {
  patch.with_venue_id("LSE");
  patch.without_market_phases();
  ASSERT_TRUE(patch.market_phases().has_value());
  ASSERT_TRUE(patch.market_phases()->empty());  // NOLINT: check is upper

  const Venue venue = Venue::create(patch);
  EXPECT_TRUE(venue.market_phases().empty());
}

TEST_F(DataLayerModelsVenue, Get_MarketPhases_Specified) {
  MarketPhase::Patch phase_patch;
  phase_patch.with_phase(MarketPhase::Phase::Closed)
      .with_start_time("20:00:00")
      .with_end_time("24:00:00");

  patch.with_venue_id("FASTMATCH").with_market_phase(phase_patch);
  ASSERT_TRUE(patch.market_phases().has_value());
  ASSERT_EQ(patch.market_phases()->size(), 1);  // NOLINT: check is upper

  const Venue venue = Venue::create(patch);
  ASSERT_EQ(venue.market_phases().size(), 1);
  EXPECT_EQ(venue.market_phases().front().venue_id(), "FASTMATCH");
}

}  // namespace
}  // namespace simulator::data_layer::test