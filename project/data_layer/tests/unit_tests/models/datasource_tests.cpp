#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

#include "api/models/datasource.hpp"
#include "ih/common/exceptions.hpp"

namespace simulator::data_layer::test {
namespace {

using namespace simulator;
using namespace testing;

using Format = Datasource::Format;
using Type = Datasource::Type;

// NOLINTBEGIN(*magic-numbers*)

struct DataLayerModelsDatasourcePatch : public Test {
  Datasource::Patch patch;
};

TEST_F(DataLayerModelsDatasourcePatch, DoesNotContainDefaultValues) {
  ASSERT_FALSE(patch.enabled_flag().has_value());
  ASSERT_FALSE(patch.name().has_value());
  ASSERT_FALSE(patch.venue_id().has_value());
  ASSERT_FALSE(patch.connection().has_value());
  ASSERT_FALSE(patch.format().has_value());
  ASSERT_FALSE(patch.type().has_value());
  ASSERT_FALSE(patch.repeat_flag().has_value());
  ASSERT_FALSE(patch.text_delimiter().has_value());
  ASSERT_FALSE(patch.text_header_row().has_value());
  ASSERT_FALSE(patch.text_data_row().has_value());
  ASSERT_FALSE(patch.table_name().has_value());
  ASSERT_FALSE(patch.columns_mapping().has_value());
}

TEST_F(DataLayerModelsDatasourcePatch, SetsEnabledFlag) {
  patch.with_enabled_flag(false);
  EXPECT_THAT(patch.enabled_flag(), Optional(Eq(false)));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsName) {
  patch.with_name("LSE Data");
  EXPECT_THAT(patch.name(), Optional(Eq("LSE Data")));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsVenueID) {
  patch.with_venue_id("LSE");
  EXPECT_THAT(patch.venue_id(), Optional(Eq("LSE")));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsConnection) {
  patch.with_connection("/path/to/file.csv");
  EXPECT_THAT(patch.connection(), Optional(Eq("/path/to/file.csv")));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsFormat) {
  patch.with_format(Format::Postgres);
  EXPECT_THAT(patch.format(), Optional(Eq(Format::Postgres)));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsType) {
  patch.with_type(Type::OrderBook);
  EXPECT_THAT(patch.type(), Optional(Eq(Type::OrderBook)));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsRepeatFlag) {
  patch.with_repeat_flag(true);
  EXPECT_THAT(patch.repeat_flag(), Optional(Eq(true)));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsTextDelimiter) {
  patch.with_text_delimiter('\t');
  EXPECT_THAT(patch.text_delimiter(), Optional(Eq('\t')));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsTextHeaderRow) {
  patch.with_text_header_row(42);
  EXPECT_THAT(patch.text_header_row(), Optional(Eq(42)));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsTextDataRow) {
  patch.with_text_data_row(42);
  EXPECT_THAT(patch.text_data_row(), Optional(Eq(42)));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsTableName) {
  patch.with_table_name("historical_table");
  EXPECT_THAT(patch.table_name(), Optional(Eq("historical_table")));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsColumnMapping) {
  ColumnMapping::Patch column_mapping_patch0;
  column_mapping_patch0.with_column_from("first_from")
      .with_column_to("first_to");

  ColumnMapping::Patch column_mapping_patch1;
  column_mapping_patch1.with_column_from("second_from")
      .with_column_to("second_to");

  patch.with_column_mapping(column_mapping_patch0)
      .with_column_mapping(column_mapping_patch1);

  ASSERT_THAT(
      patch.columns_mapping(),
      Optional(ElementsAre(column_mapping_patch0, column_mapping_patch1)));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsWithoutColumnMapping) {
  patch.without_column_mapping();
  ASSERT_THAT(patch.columns_mapping(), Optional(IsEmpty()));
}

TEST_F(DataLayerModelsDatasourcePatch, SetsMaxDepthLevels) {
  patch.with_max_depth_levels(42);
  EXPECT_THAT(patch.max_depth_levels(), Optional(Eq(42)));
}

struct DataLayerModelsDatasource : public Test {
  Datasource::Patch patch;

  static auto fill_required_fields(Datasource::Patch& patch) -> void {
    patch.with_name("dummy")
        .with_venue_id("dummy")
        .with_connection("dummy")
        .with_format(Format::Postgres)
        .with_type(Type::OrderBook);
  }
};

TEST_F(DataLayerModelsDatasource, StoresDatasourceID) {
  fill_required_fields(patch);
  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.datasource_id(), 42);
}

TEST_F(DataLayerModelsDatasource, StoresNullOptionalEnableFlagIfNotSet) {
  fill_required_fields(patch);
  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.enabled_flag(), std::nullopt);
}

TEST_F(DataLayerModelsDatasource, StoresEnableFlag) {
  fill_required_fields(patch);
  patch.with_enabled_flag(true);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.enabled_flag(), true);
}

TEST_F(DataLayerModelsDatasource, ThrowsExceptionIfNameIsNotSet) {
  patch.with_venue_id("dummy")
      .with_connection("dummy")
      .with_format(Format::Postgres)
      .with_type(Type::OrderBook);

  ASSERT_FALSE(patch.name().has_value());

  EXPECT_THROW((void)Datasource::create(patch, 42), RequiredAttributeMissing);
}

TEST_F(DataLayerModelsDatasource, StoresName) {
  fill_required_fields(patch);
  patch.with_name("LSE Data");

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.name(), "LSE Data");
}

TEST_F(DataLayerModelsDatasource, ThrowsExceptionIfVenueIdIsNotSet) {
  patch.with_name("dummy")
      .with_connection("dummy")
      .with_format(Format::Postgres)
      .with_type(Type::OrderBook);

  ASSERT_FALSE(patch.venue_id().has_value());

  EXPECT_THROW((void)Datasource::create(patch, 42), RequiredAttributeMissing);
}

TEST_F(DataLayerModelsDatasource, StoresVenueId) {
  fill_required_fields(patch);
  patch.with_venue_id("LSE");

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.venue_id(), "LSE");
}

TEST_F(DataLayerModelsDatasource, ThrowsExceptionIfConnectionIsNotSet) {
  patch.with_name("dummy")
      .with_venue_id("dummy")
      .with_format(Format::Postgres)
      .with_type(Type::OrderBook);

  ASSERT_FALSE(patch.connection().has_value());

  EXPECT_THROW((void)Datasource::create(patch, 42), RequiredAttributeMissing);
}

TEST_F(DataLayerModelsDatasource, StoresConnection) {
  fill_required_fields(patch);
  patch.with_connection("/ro/file.csv");

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.connection(), "/ro/file.csv");
}

TEST_F(DataLayerModelsDatasource, ThrowsExceptionIfFormatIsNotSet) {
  patch.with_name("dummy")
      .with_venue_id("dummy")
      .with_connection("dummy")
      .with_type(Type::OrderBook);

  ASSERT_FALSE(patch.format().has_value());

  EXPECT_THROW((void)Datasource::create(patch, 42), RequiredAttributeMissing);
}

TEST_F(DataLayerModelsDatasource, StoresFormat) {
  fill_required_fields(patch);
  patch.with_format(Format::Postgres);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.format(), Format::Postgres);
}

TEST_F(DataLayerModelsDatasource, ThrowsExceptionIfTypeIsNotSet) {
  patch.with_name("dummy")
      .with_venue_id("dummy")
      .with_connection("dummy")
      .with_format(Format::Postgres);

  ASSERT_FALSE(patch.type().has_value());

  EXPECT_THROW((void)Datasource::create(patch, 42), RequiredAttributeMissing);
}

TEST_F(DataLayerModelsDatasource, StoresType) {
  fill_required_fields(patch);
  patch.with_type(Type::OrderBook);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.type(), Type::OrderBook);
}

TEST_F(DataLayerModelsDatasource, StoresNullOptionalRepeatFlagIfNotSet) {
  fill_required_fields(patch);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.repeat_flag(), std::nullopt);
}

TEST_F(DataLayerModelsDatasource, StoresRepeatFlag) {
  fill_required_fields(patch);
  patch.with_repeat_flag(true);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.repeat_flag(), true);
}

TEST_F(DataLayerModelsDatasource, StoresNullOptionalTextDelimiterIfNotSet) {
  fill_required_fields(patch);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.text_delimiter(), std::nullopt);
}

TEST_F(DataLayerModelsDatasource, StoresTextDelimiter) {
  fill_required_fields(patch);
  patch.with_text_delimiter('\t');
  ASSERT_TRUE(patch.text_delimiter().has_value());

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.text_delimiter(), '\t');
}

TEST_F(DataLayerModelsDatasource, StoresNullOptionalTextHeaderRowIfNotSet) {
  fill_required_fields(patch);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.text_header_row(), std::nullopt);
}

TEST_F(DataLayerModelsDatasource, StoresTextHeader) {
  fill_required_fields(patch);
  patch.with_text_header_row(42);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.text_header_row(), 42);
}

TEST_F(DataLayerModelsDatasource, StoresNullOptionalTextDataRowIfNotSet) {
  fill_required_fields(patch);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.text_data_row(), std::nullopt);
}

TEST_F(DataLayerModelsDatasource, StoresTextDataRow) {
  fill_required_fields(patch);
  patch.with_text_data_row(43);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.text_data_row(), 43);
}

TEST_F(DataLayerModelsDatasource, StoresEmptyColumnsMappingIfNotSet) {
  fill_required_fields(patch);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_TRUE(datasource.columns_mapping().empty());
}

TEST_F(DataLayerModelsDatasource, StoresEmptyColumnsMappingIfEmpty) {
  fill_required_fields(patch);
  patch.without_column_mapping();

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_TRUE(datasource.columns_mapping().empty());
}

TEST_F(DataLayerModelsDatasource, StoresColumnsMapping) {
  fill_required_fields(patch);

  ColumnMapping::Patch timestamp_patch;
  timestamp_patch.with_column_from("ReceivedTimestamp")
      .with_column_to("timestamp");
  ColumnMapping::Patch bid_price_patch;
  bid_price_patch.with_column_from("BidPrice2").with_column_to("bid_price2");
  patch.with_column_mapping(timestamp_patch)
      .with_column_mapping(bid_price_patch);

  const auto datasource = Datasource::create(patch, 42);
  const auto timestamp_mapping = ColumnMapping::create(timestamp_patch, 42);
  const auto bid_price_mapping = ColumnMapping::create(bid_price_patch, 42);
  ASSERT_THAT(datasource.columns_mapping(),
              ElementsAre(timestamp_mapping, bid_price_mapping));
}

TEST_F(DataLayerModelsDatasource, StoresNullOptionalMaxDepthLevelIfNotSet) {
  fill_required_fields(patch);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.max_depth_levels(), std::nullopt);
}

TEST_F(DataLayerModelsDatasource, StoresMaxDepthLevel) {
  fill_required_fields(patch);
  patch.with_max_depth_levels(11);

  const auto datasource = Datasource::create(patch, 42);
  EXPECT_EQ(datasource.max_depth_levels(), 11);
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::data_layer::test