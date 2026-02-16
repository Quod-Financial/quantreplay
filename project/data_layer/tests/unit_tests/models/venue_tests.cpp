#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "api/models/venue.hpp"
#include "ih/common/exceptions.hpp"
#include "tests/test_utils/matchers.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace ::testing;

using EngineType = Venue::EngineType;

struct DataLayerModelsVenuePatch : public ::testing::Test {
  Venue::Patch patch;
};

TEST_F(DataLayerModelsVenuePatch, SetsVenueID) {
  ASSERT_FALSE(patch.venue_id().has_value());

  patch.with_venue_id("FASTMATCH");
  EXPECT_THAT(patch.venue_id(), Optional(Eq("FASTMATCH")));
}

TEST_F(DataLayerModelsVenuePatch, SetsNameNull) {
  ASSERT_FALSE(patch.name().has_value());

  patch.with_name(std::nullopt);
  EXPECT_THAT(patch.name(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsName) {
  ASSERT_FALSE(patch.name().has_value());

  patch.with_name("London Stock Exchange");
  EXPECT_THAT(patch.name(),
              IsPatchFieldWithValue(Optional(Eq("London Stock Exchange"))));
}

TEST_F(DataLayerModelsVenuePatch, SetsEngineTypeNull) {
  ASSERT_FALSE(patch.engine_type().has_value());

  patch.with_engine_type(std::nullopt);
  EXPECT_THAT(patch.engine_type(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsEngineType) {
  ASSERT_FALSE(patch.engine_type().has_value());

  patch.with_engine_type(EngineType::Quoting);
  EXPECT_THAT(patch.engine_type(),
              IsPatchFieldWithValue(Optional(Eq(EngineType::Quoting))));
}

TEST_F(DataLayerModelsVenuePatch, SetsSupportTifIocFlagNull) {
  ASSERT_FALSE(patch.support_tif_ioc_flag().has_value());

  patch.with_support_tif_ioc_flag(std::nullopt);
  EXPECT_THAT(patch.support_tif_ioc_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsSupportTifIocFlag) {
  ASSERT_FALSE(patch.support_tif_ioc_flag().has_value());

  patch.with_support_tif_ioc_flag(true);
  EXPECT_THAT(patch.support_tif_ioc_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerModelsVenuePatch, SetsSupportTifFokFlagNull) {
  ASSERT_FALSE(patch.support_tif_fok_flag().has_value());

  patch.with_support_tif_fok_flag(std::nullopt);
  EXPECT_THAT(patch.support_tif_fok_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsSupportTifFokFlag) {
  ASSERT_FALSE(patch.support_tif_fok_flag().has_value());

  patch.with_support_tif_fok_flag(false);
  EXPECT_THAT(patch.support_tif_fok_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerModelsVenuePatch, SetsSupportTifDayFlagNull) {
  ASSERT_FALSE(patch.support_tif_day_flag().has_value());

  patch.with_support_tif_day_flag(std::nullopt);
  EXPECT_THAT(patch.support_tif_day_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsSupportTifDayFlag) {
  ASSERT_FALSE(patch.support_tif_day_flag().has_value());

  patch.with_support_tif_day_flag(true);
  EXPECT_THAT(patch.support_tif_day_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerModelsVenuePatch, SetsIncludeOwnOrdersFlagNull) {
  ASSERT_FALSE(patch.include_own_orders_flag().has_value());

  patch.with_include_own_orders_flag(std::nullopt);
  EXPECT_THAT(patch.include_own_orders_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsIncludeOwnOrdersFlag) {
  ASSERT_FALSE(patch.include_own_orders_flag().has_value());

  patch.with_include_own_orders_flag(false);
  EXPECT_THAT(patch.include_own_orders_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerModelsVenuePatch, SetsRestPortNull) {
  ASSERT_FALSE(patch.rest_port().has_value());

  patch.with_rest_port(std::nullopt);
  EXPECT_THAT(patch.rest_port(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsRestPort) {
  ASSERT_FALSE(patch.rest_port().has_value());

  patch.with_rest_port(9001);  // NOLINT: Test value
  EXPECT_THAT(patch.rest_port(), IsPatchFieldWithValue(Optional(Eq(9001))));
}

TEST_F(DataLayerModelsVenuePatch, SetsOrdersOnStartupFlagNull) {
  ASSERT_FALSE(patch.orders_on_startup_flag().has_value());

  patch.with_orders_on_startup_flag(std::nullopt);
  EXPECT_THAT(patch.orders_on_startup_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsOrdersOnStartupFlag) {
  ASSERT_FALSE(patch.orders_on_startup_flag().has_value());

  patch.with_orders_on_startup_flag(true);
  EXPECT_THAT(patch.orders_on_startup_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerModelsVenuePatch, SetsRandomPartiesCountNull) {
  ASSERT_FALSE(patch.random_parties_count().has_value());

  patch.with_random_parties_count(std::nullopt);
  EXPECT_THAT(patch.random_parties_count(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsRandomPartiesCount) {
  ASSERT_FALSE(patch.random_parties_count().has_value());

  patch.with_random_parties_count(50);  // NOLINT: Test value
  EXPECT_THAT(patch.random_parties_count(),
              IsPatchFieldWithValue(Optional(Eq(50))));
}

TEST_F(DataLayerModelsVenuePatch, SetsTnsEnabledFlagNull) {
  ASSERT_FALSE(patch.tns_enabled_flag().has_value());

  patch.with_tns_enabled_flag(std::nullopt);
  EXPECT_THAT(patch.tns_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsTnsEnabledFlag) {
  ASSERT_FALSE(patch.tns_enabled_flag().has_value());

  patch.with_tns_enabled_flag(true);
  EXPECT_THAT(patch.tns_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerModelsVenuePatch, SetsTnsQtyEnabledFlagNull) {
  ASSERT_FALSE(patch.tns_qty_enabled_flag().has_value());

  patch.with_tns_qty_enabled_flag(std::nullopt);
  EXPECT_THAT(patch.tns_qty_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsTnsQtyEnabledFlag) {
  ASSERT_FALSE(patch.tns_qty_enabled_flag().has_value());

  patch.with_tns_qty_enabled_flag(true);
  EXPECT_THAT(patch.tns_qty_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerModelsVenuePatch, SetsTnsSideEnabledFlagNull) {
  ASSERT_FALSE(patch.tns_side_enabled_flag().has_value());

  patch.with_tns_side_enabled_flag(std::nullopt);
  EXPECT_THAT(patch.tns_side_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsTnsSideEnabledFlag) {
  ASSERT_FALSE(patch.tns_side_enabled_flag().has_value());

  patch.with_tns_side_enabled_flag(false);
  EXPECT_THAT(patch.tns_side_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerModelsVenuePatch, SetsTnsPartiesEnabledFlagNull) {
  ASSERT_FALSE(patch.tns_parties_enabled_flag().has_value());

  patch.with_tns_parties_enabled_flag(std::nullopt);
  EXPECT_THAT(patch.tns_parties_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsTnsPartiesEnabledFlag) {
  ASSERT_FALSE(patch.tns_parties_enabled_flag().has_value());

  patch.with_tns_parties_enabled_flag(false);
  EXPECT_THAT(patch.tns_parties_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(false))));
}

TEST_F(DataLayerModelsVenuePatch, SetsTimezoneNull) {
  ASSERT_FALSE(patch.timezone().has_value());

  patch.with_timezone(std::nullopt);
  EXPECT_THAT(patch.timezone(), IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsTimezone) {
  ASSERT_FALSE(patch.timezone().has_value());

  patch.with_timezone("GMT+3");
  EXPECT_THAT(patch.timezone(), IsPatchFieldWithValue(Optional(Eq("GMT+3"))));
}

TEST_F(DataLayerModelsVenuePatch, SetsCancelOnDiconnectFlagNull) {
  ASSERT_FALSE(patch.cancel_on_disconnect_flag().has_value());

  patch.with_cancel_on_disconnect_flag(std::nullopt);
  EXPECT_THAT(patch.cancel_on_disconnect_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsCancelOnDiconnectFlag) {
  ASSERT_FALSE(patch.cancel_on_disconnect_flag().has_value());

  patch.with_cancel_on_disconnect_flag(true);
  EXPECT_THAT(patch.cancel_on_disconnect_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerModelsVenuePatch, SetsPersistenceEnabledFlagNull) {
  ASSERT_FALSE(patch.persistence_enabled_flag().has_value());

  patch.with_persistence_enabled_flag(std::nullopt);
  EXPECT_THAT(patch.persistence_enabled_flag(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsPersistenceEnabledFlag) {
  ASSERT_FALSE(patch.persistence_enabled_flag().has_value());

  patch.with_persistence_enabled_flag(true);
  EXPECT_THAT(patch.persistence_enabled_flag(),
              IsPatchFieldWithValue(Optional(Eq(true))));
}

TEST_F(DataLayerModelsVenuePatch, SetsPersistenceFilePathNull) {
  ASSERT_FALSE(patch.persistence_file_path().has_value());

  patch.with_persistence_file_path(std::nullopt);
  EXPECT_THAT(patch.persistence_file_path(),
              IsPatchFieldWithValue(Eq(std::nullopt)));
}

TEST_F(DataLayerModelsVenuePatch, SetsPersistenceFilePath) {
  ASSERT_FALSE(patch.persistence_file_path().has_value());

  patch.with_persistence_file_path("/rw/sim-storage.json");
  EXPECT_THAT(patch.persistence_file_path(),
              IsPatchFieldWithValue(Optional(Eq("/rw/sim-storage.json"))));
}

TEST_F(DataLayerModelsVenuePatch, SetsMarketPhase) {
  const MarketPhase::Patch market_phase;

  patch.with_market_phase(market_phase);
  patch.with_market_phase(market_phase);

  ASSERT_TRUE(patch.market_phases().has_value());
  EXPECT_EQ(patch.market_phases()->size(), 2);  // NOLINT: check is upper
}

TEST_F(DataLayerModelsVenuePatch, SetsWithoutMarketPhases) {
  patch.without_market_phases();
  ASSERT_TRUE(patch.market_phases().has_value());
  EXPECT_TRUE(patch.market_phases()->empty());  // NOLINT: check is upper
}

TEST_F(DataLayerModelsVenuePatch, SetsWithoutMarketPhasesAfterInsertion) {
  const MarketPhase::Patch market_phase;

  patch.with_market_phase(market_phase);
  patch.with_market_phase(market_phase);
  ASSERT_TRUE(patch.market_phases().has_value());
  ASSERT_EQ(patch.market_phases()->size(), 2);  // NOLINT: check is upper

  patch.without_market_phases();
  ASSERT_TRUE(patch.market_phases().has_value());
  EXPECT_TRUE(patch.market_phases()->empty());  // NOLINT: check is upper
}

struct DataLayerModelsVenue : public ::testing::Test {
  Venue::Patch patch;

  static auto fill_required_fields(Venue::Patch& patch) -> void {
    patch.with_venue_id("dummy");
  }
};

TEST_F(DataLayerModelsVenue, ThrowsExceptionIfVenueIDIsNotSet) {
  ASSERT_FALSE(patch.venue_id().has_value());

  EXPECT_THROW((void)Venue::create(patch), RequiredAttributeMissing);
}

TEST_F(DataLayerModelsVenue, StoresVenueID) {
  patch.with_venue_id("XETRA");
  ASSERT_TRUE(patch.venue_id().has_value());

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.venue_id(), "XETRA");
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalNameIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.name(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresName) {
  fill_required_fields(patch);
  patch.with_name("London Stock Exchange");

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.name(), Optional(Eq("London Stock Exchange")));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalEngineTypeIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.engine_type(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresEngineType) {
  fill_required_fields(patch);
  patch.with_engine_type(EngineType::Matching);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.engine_type(), Optional(Eq(EngineType::Matching)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalSupportTifIocFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.support_tif_ioc_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresSupportTifIocFlag) {
  fill_required_fields(patch);
  patch.with_support_tif_ioc_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.support_tif_ioc_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalSupportTifFokFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.support_tif_fok_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresSupportTifFokFlag) {
  fill_required_fields(patch);
  patch.with_support_tif_fok_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.support_tif_fok_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalSupportTifDayFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.support_tif_day_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresSupportTifDayFlag) {
  fill_required_fields(patch);
  patch.with_support_tif_day_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.support_tif_day_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalIncludeOwnOrdersFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.include_own_orders_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresIncludeOwnOrdersFlag) {
  fill_required_fields(patch);
  patch.with_include_own_orders_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.include_own_orders_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalRestPortIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.rest_port(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresRestPort) {
  fill_required_fields(patch);
  patch.with_rest_port(9001);  // NOLINT: Test value

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.rest_port(), Optional(Eq(9001)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalOrdersOnStartupFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.order_on_startup_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresOrdersOnStartupFlag) {
  fill_required_fields(patch);
  patch.with_orders_on_startup_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.order_on_startup_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalRandomPartiesCountIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.random_parties_count(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresRandomPartiesCount) {
  fill_required_fields(patch);
  patch.with_random_parties_count(42);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.random_parties_count(), Optional(Eq(42)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalTnsEnabledFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.tns_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresTnsEnabledFlag) {
  fill_required_fields(patch);
  patch.with_tns_enabled_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.tns_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalTnsQtyEnabledFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.tns_qty_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresTnsQtyEnabledFlag) {
  fill_required_fields(patch);
  patch.with_tns_qty_enabled_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.tns_qty_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalTnsSideEnabledFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.tns_side_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresTnsSideEnabledFlag) {
  fill_required_fields(patch);
  patch.with_tns_side_enabled_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.tns_side_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalTnsPartiesEnabledFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.tns_parties_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresTnsPartiesEnabledFlag) {
  fill_required_fields(patch);
  patch.with_tns_parties_enabled_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.tns_parties_enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalTimezoneIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.timezone(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresTimezone) {
  fill_required_fields(patch);
  patch.with_timezone("GMT+3");

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.timezone(), Optional(Eq("GMT+3")));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalCancelOnDisconnectFlagIfNotSet) {
  fill_required_fields(patch);
  ASSERT_FALSE(patch.cancel_on_disconnect_flag().has_value());

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.cancel_on_disconnect_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresCancelOnDisconnectFlag) {
  fill_required_fields(patch);
  patch.with_cancel_on_disconnect_flag(false);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.cancel_on_disconnect_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalPersistenceEnabledFlagIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.persistence_enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresPersistenceEnabledFlag) {
  fill_required_fields(patch);
  patch.with_persistence_enabled_flag(true);

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.persistence_enabled_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsVenue, StoresNullOptionalPersistenceFilePathIfNotSet) {
  fill_required_fields(patch);

  const Venue venue = Venue::create(patch);
  EXPECT_EQ(venue.persistence_file_path(), std::nullopt);
}

TEST_F(DataLayerModelsVenue, StoresPersistenceFilePath) {
  fill_required_fields(patch);
  patch.with_persistence_file_path("/rw/state.json");

  const Venue venue = Venue::create(patch);
  EXPECT_THAT(venue.persistence_file_path(), Optional(Eq("/rw/state.json")));
}

TEST_F(DataLayerModelsVenue, StoresNotDefinedMarketPhasesAsEmpty) {
  fill_required_fields(patch);
  ASSERT_FALSE(patch.market_phases().has_value());

  const Venue venue = Venue::create(patch);
  EXPECT_TRUE(venue.market_phases().empty());
}

TEST_F(DataLayerModelsVenue, StoresEmptyMarketPhasesAsEmpty) {
  fill_required_fields(patch);
  patch.without_market_phases();
  ASSERT_TRUE(patch.market_phases().has_value());
  ASSERT_TRUE(patch.market_phases()->empty());  // NOLINT: check is upper

  const Venue venue = Venue::create(patch);
  EXPECT_TRUE(venue.market_phases().empty());
}

TEST_F(DataLayerModelsVenue, StoresMarketPhases) {
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
