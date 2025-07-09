#include <gtest/gtest.h>

#include "api/models/column_mapping.hpp"
#include "api/models/datasource.hpp"
#include "api/models/market_phase.hpp"
#include "api/models/price_seed.hpp"
#include "api/models/venue.hpp"
#include "ih/common/exceptions.hpp"
#include "ih/pqxx/common/column_resolver.hpp"

namespace simulator::data_layer::internal_pqxx::test {
namespace {

using Resolver = internal_pqxx::ColumnResolver;

struct DataLayerColumnMappingResolver : public ::testing::Test {
  using Column = ColumnMapping::Attribute;

  const ColumnResolver resolver{};
};

TEST_F(DataLayerColumnMappingResolver, ResolvesUndefined) {
  EXPECT_THROW(resolver(static_cast<Column>(-1)), ColumnNameEncodingError);
}

TEST_F(DataLayerColumnMappingResolver, ResolvesColumnFrom) {
  EXPECT_EQ(resolver(Column::ColumnFrom), "column_from");
}

TEST_F(DataLayerColumnMappingResolver, ResolvesColumnTo) {
  EXPECT_EQ(resolver(Column::ColumnTo), "column_to");
}

TEST_F(DataLayerColumnMappingResolver, ResolvesDatasourceId) {
  EXPECT_EQ(resolver(Column::DatasourceId), "data_source_id");
}

struct DataLayerDatasourceResolver : public ::testing::Test {
  using Column = Datasource::Attribute;

  const ColumnResolver resolver{};
};

TEST_F(DataLayerDatasourceResolver, ResolvesUndefined) {
  EXPECT_THROW(resolver(static_cast<Column>(-1)), ColumnNameEncodingError);
}

TEST_F(DataLayerDatasourceResolver, ResolvesDatasourceId) {
  EXPECT_EQ(resolver(Column::DatasourceId), "data_source_id");
}

TEST_F(DataLayerDatasourceResolver, ResolvesEnabled) {
  EXPECT_EQ(resolver(Column::Enabled), "enabled");
}

TEST_F(DataLayerDatasourceResolver, ResolvesName) {
  EXPECT_EQ(resolver(Column::Name), "name");
}

TEST_F(DataLayerDatasourceResolver, ResolvesVenueID) {
  EXPECT_EQ(resolver(Column::VenueId), "venue_id");
}

TEST_F(DataLayerDatasourceResolver, ResolvesConnection) {
  EXPECT_EQ(resolver(Column::Connection), "connection");
}

TEST_F(DataLayerDatasourceResolver, ResolvesFormat) {
  EXPECT_EQ(resolver(Column::Format), "format");
}

TEST_F(DataLayerDatasourceResolver, ResolvesType) {
  EXPECT_EQ(resolver(Column::Type), "type");
}

TEST_F(DataLayerDatasourceResolver, ResolvesRepeat) {
  EXPECT_EQ(resolver(Column::Repeat), "repeat");
}

TEST_F(DataLayerDatasourceResolver, ResolvesTextDelimiter) {
  EXPECT_EQ(resolver(Column::TextDelimiter), "text_delimiter");
}

TEST_F(DataLayerDatasourceResolver, ResolvesTextHeaderRow) {
  EXPECT_EQ(resolver(Column::TextHeaderRow), "text_header_row");
}

TEST_F(DataLayerDatasourceResolver, ResolvesTextDataRow) {
  EXPECT_EQ(resolver(Column::TextDataRow), "text_data_row");
}

TEST_F(DataLayerDatasourceResolver, ResolvesTableName) {
  EXPECT_EQ(resolver(Column::TableName), "table_name");
}

TEST_F(DataLayerDatasourceResolver, ResolvesMaxDepthLevels) {
  EXPECT_EQ(resolver(Column::MaxDepthLevels), "max_depth_levels");
}

struct DataLayerListingResolver : public ::testing::Test {
  using Column = Listing::Attribute;

  const ColumnResolver resolver{};
};

TEST_F(DataLayerListingResolver, ResolvesUndefined) {
  EXPECT_THROW(resolver(static_cast<Column>(-1)), ColumnNameEncodingError);
}

TEST_F(DataLayerListingResolver, ResolvesListingID) {
  EXPECT_EQ(resolver(Column::ListingId), "listing_id");
}

TEST_F(DataLayerListingResolver, ResolvesSymbol) {
  EXPECT_EQ(resolver(Column::Symbol), "symbol");
}

TEST_F(DataLayerListingResolver, ResolvesVenueID) {
  EXPECT_EQ(resolver(Column::VenueId), "venue_id");
}

TEST_F(DataLayerListingResolver, ResolvesSecurityType) {
  EXPECT_EQ(resolver(Column::SecurityType), "security_type");
}

TEST_F(DataLayerListingResolver, ResolvesPriceCurrency) {
  EXPECT_EQ(resolver(Column::PriceCurrency), "price_currency");
}

TEST_F(DataLayerListingResolver, ResolvesFxBaseCurrency) {
  EXPECT_EQ(resolver(Column::FxBaseCurrency), "fx_base_currency");
}

TEST_F(DataLayerListingResolver, ResolvesInstrSymbol) {
  EXPECT_EQ(resolver(Column::InstrSymbol), "instr_symbol");
}

TEST_F(DataLayerListingResolver, ResolvesQtyMinimum) {
  EXPECT_EQ(resolver(Column::QtyMinimum), "qty_minimum");
}

TEST_F(DataLayerListingResolver, ResolvesQtyMaximum) {
  EXPECT_EQ(resolver(Column::QtyMaximum), "qty_maximum");
}

TEST_F(DataLayerListingResolver, ResolvesQtyMultiple) {
  EXPECT_EQ(resolver(Column::QtyMultiple), "qty_multiple");
}

TEST_F(DataLayerListingResolver, ResolvesPriceTickSize) {
  EXPECT_EQ(resolver(Column::PriceTickSize), "price_tick_size");
}

TEST_F(DataLayerListingResolver, ResolvesEnabled) {
  EXPECT_EQ(resolver(Column::Enabled), "enabled");
}

TEST_F(DataLayerListingResolver, ResolvesRandomQtyMaximum) {
  EXPECT_EQ(resolver(Column::RandomQtyMaximum), "random_qty_maximum");
}

TEST_F(DataLayerListingResolver, ResolvesRandomDepthLevels) {
  EXPECT_EQ(resolver(Column::RandomDepthLevels), "random_depth_levels");
}

TEST_F(DataLayerListingResolver, ResolvesRandomOrdersSpread) {
  EXPECT_EQ(resolver(Column::RandomOrdersSpread), "random_orders_spread");
}

TEST_F(DataLayerListingResolver, ResolvesRandomOrdersRate) {
  EXPECT_EQ(resolver(Column::RandomOrdersRate), "random_orders_rate");
}

TEST_F(DataLayerListingResolver, ResolvesRandomTickRange) {
  EXPECT_EQ(resolver(Column::RandomTickRange), "random_tick_range");
}

TEST_F(DataLayerListingResolver, ResolvesSecurityExchange) {
  EXPECT_EQ(resolver(Column::SecurityExchange), "security_exchange");
}

TEST_F(DataLayerListingResolver, ResolvesPartyID) {
  EXPECT_EQ(resolver(Column::PartyId), "party_id");
}

TEST_F(DataLayerListingResolver, ResolvesPartyRole) {
  EXPECT_EQ(resolver(Column::PartyRole), "party_role");
}

TEST_F(DataLayerListingResolver, ResolvesCusipID) {
  EXPECT_EQ(resolver(Column::CusipId), "cusip_id");
}

TEST_F(DataLayerListingResolver, ResolvesSedolID) {
  EXPECT_EQ(resolver(Column::SedolId), "sedol_id");
}

TEST_F(DataLayerListingResolver, ResolvesIsinID) {
  EXPECT_EQ(resolver(Column::IsinId), "isin_id");
}

TEST_F(DataLayerListingResolver, ResolvesRicID) {
  EXPECT_EQ(resolver(Column::RicId), "ric_id");
}

TEST_F(DataLayerListingResolver, ResolvesExchangeSymbolID) {
  EXPECT_EQ(resolver(Column::ExchangeSymbolId), "exchange_symbol_id");
}

TEST_F(DataLayerListingResolver, ResolvesBloombergSymbolID) {
  EXPECT_EQ(resolver(Column::BloombergSymbolId), "bloomberg_symbol_id");
}

TEST_F(DataLayerListingResolver, ResolvesRandomQtyMinimum) {
  EXPECT_EQ(resolver(Column::RandomQtyMinimum), "random_qty_minimum");
}

TEST_F(DataLayerListingResolver, ResolvesRandomAmtMinimum) {
  EXPECT_EQ(resolver(Column::RandomAmtMinimum), "random_amt_minimum");
}

TEST_F(DataLayerListingResolver, ResolvesRandomAmtMaximum) {
  EXPECT_EQ(resolver(Column::RandomAmtMaximum), "random_amt_maximum");
}

TEST_F(DataLayerListingResolver, ResolvesRandomOrdersEnabled) {
  EXPECT_EQ(resolver(Column::RandomOrdersEnabled), "random_orders_enabled");
}

TEST_F(DataLayerListingResolver, ResolvesRandomAggressiveQtyMinimum) {
  EXPECT_EQ(resolver(Column::RandomAggressiveQtyMinimum),
            "random_aggressive_qty_minimum");
}

TEST_F(DataLayerListingResolver, ResolvesRandomAggressiveQtyMaximum) {
  EXPECT_EQ(resolver(Column::RandomAggressiveQtyMaximum),
            "random_aggressive_qty_maximum");
}

TEST_F(DataLayerListingResolver, ResolvesRandomAggressiveAmtMinimum) {
  EXPECT_EQ(resolver(Column::RandomAggressiveAmtMinimum),
            "random_aggressive_amt_minimum");
}

TEST_F(DataLayerListingResolver, ResolvesRandomAggressiveAmtMaximum) {
  EXPECT_EQ(resolver(Column::RandomAggressiveAmtMaximum),
            "random_aggressive_amt_maximum");
}

struct DataLayerMarketPhaseResolver : public ::testing::Test {
  using Column = MarketPhase::Attribute;

  const ColumnResolver resolver{};
};

TEST_F(DataLayerMarketPhaseResolver, ResolvesUndefined) {
  EXPECT_THROW(resolver(static_cast<Column>(-1)), ColumnNameEncodingError);
}

TEST_F(DataLayerMarketPhaseResolver, ResolvesPhase) {
  EXPECT_EQ(resolver(Column::Phase), "phase");
}

TEST_F(DataLayerMarketPhaseResolver, ResolvesStartTime) {
  EXPECT_EQ(resolver(Column::StartTime), "start_time");
}

TEST_F(DataLayerMarketPhaseResolver, ResolvesEndTime) {
  EXPECT_EQ(resolver(Column::EndTime), "end_time");
}

TEST_F(DataLayerMarketPhaseResolver, ResolvesVenueID) {
  EXPECT_EQ(resolver(Column::VenueId), "venue_id");
}

TEST_F(DataLayerMarketPhaseResolver, ResolvesEndTimeRange) {
  EXPECT_EQ(resolver(Column::EndTimeRange), "end_time_range");
}

TEST_F(DataLayerMarketPhaseResolver, ResolvesAllowCancels) {
  EXPECT_EQ(resolver(Column::AllowCancels), "allow_cancels");
}

struct DataLayerPriceSeedResolver : public ::testing::Test {
  using Column = PriceSeed::Attribute;

  const ColumnResolver resolver{};
};

TEST_F(DataLayerPriceSeedResolver, ResolvesUndefined) {
  EXPECT_THROW(resolver(static_cast<Column>(-1)), ColumnNameEncodingError);
}

TEST_F(DataLayerPriceSeedResolver, ResolvesPriceSeedId) {
  EXPECT_EQ(resolver(Column::PriceSeedId), "price_seed_id");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesSymbol) {
  EXPECT_EQ(resolver(Column::Symbol), "symbol");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesSecurityType) {
  EXPECT_EQ(resolver(Column::SecurityType), "security_type");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesPriceCurreny) {
  EXPECT_EQ(resolver(Column::PriceCurrency), "price_currency");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesSecurityID) {
  EXPECT_EQ(resolver(Column::SecurityId), "security_id");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesSecurityIDSource) {
  EXPECT_EQ(resolver(Column::SecurityIdSource), "security_id_source");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesInstrumentSymbol) {
  EXPECT_EQ(resolver(Column::InstrumentSymbol), "instrument_symbol");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesMidPrice) {
  EXPECT_EQ(resolver(Column::MidPrice), "mid_price");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesBidPrice) {
  EXPECT_EQ(resolver(Column::BidPrice), "bid_price");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesOfferPrice) {
  EXPECT_EQ(resolver(Column::OfferPrice), "offer_price");
}

TEST_F(DataLayerPriceSeedResolver, ResolvesLastUpdate) {
  EXPECT_EQ(resolver(Column::LastUpdate), "last_update");
}

struct DataLayerSettingResolver : public ::testing::Test {
  using Column = Setting::Attribute;

  const ColumnResolver resolver{};
};

TEST_F(DataLayerSettingResolver, Resolve_Undefined) {
  EXPECT_THROW(resolver(static_cast<Column>(-1)), ColumnNameEncodingError);
}

TEST_F(DataLayerSettingResolver, Resolve_Key) {
  EXPECT_EQ(resolver(Column::Key), "key");
}

TEST_F(DataLayerSettingResolver, Resolve_Value) {
  EXPECT_EQ(resolver(Column::Value), "value");
}

struct DataLayerVenueResolver : public ::testing::Test {
  using Column = Venue::Attribute;

  const ColumnResolver resolver{};
};

TEST_F(DataLayerVenueResolver, ResolvesUndefined) {
  EXPECT_THROW(resolver(static_cast<Column>(-1)), ColumnNameEncodingError);
}

TEST_F(DataLayerVenueResolver, ResolvesVenueId) {
  EXPECT_EQ(resolver(Column::VenueId), "venue_id");
}

TEST_F(DataLayerVenueResolver, ResolvesName) {
  EXPECT_EQ(resolver(Column::Name), "name");
}

TEST_F(DataLayerVenueResolver, ResolvesEngineType) {
  EXPECT_EQ(resolver(Column::EngineType), "engine_type");
}

TEST_F(DataLayerVenueResolver, ResolvesSupportTifIoc) {
  EXPECT_EQ(resolver(Column::SupportTifIoc), "support_tif_ioc");
}

TEST_F(DataLayerVenueResolver, ResolvesSupportTifFok) {
  EXPECT_EQ(resolver(Column::SupportTifFok), "support_tif_fok");
}

TEST_F(DataLayerVenueResolver, ResolvesSupportTifDay) {
  EXPECT_EQ(resolver(Column::SupportTifDay), "support_tif_day");
}

TEST_F(DataLayerVenueResolver, ResolvesIncludeOwnOrders) {
  EXPECT_EQ(resolver(Column::IncludeOwnOrders), "include_own_orders");
}

TEST_F(DataLayerVenueResolver, ResolvesRestPort) {
  EXPECT_EQ(resolver(Column::RestPort), "rest_port");
}

TEST_F(DataLayerVenueResolver, ResolvesOrderOnStartup) {
  EXPECT_EQ(resolver(Column::OrderOnStartup), "order_on_startup");
}

TEST_F(DataLayerVenueResolver, ResolvesRandomPartiesCount) {
  EXPECT_EQ(resolver(Column::RandomPartiesCount), "random_party_count");
}

TEST_F(DataLayerVenueResolver, ResolvesTnsEnabled) {
  EXPECT_EQ(resolver(Column::TnsEnabled), "tns_enabled");
}

TEST_F(DataLayerVenueResolver, ResolvesTnsQtyEnabled) {
  EXPECT_EQ(resolver(Column::TnsQtyEnabled), "tns_qty_enabled");
}

TEST_F(DataLayerVenueResolver, ResolvesTnsSideEnabled) {
  EXPECT_EQ(resolver(Column::TnsSideEnabled), "tns_side_enabled");
}

TEST_F(DataLayerVenueResolver, ResolvesTnsPartiesEnabled) {
  EXPECT_EQ(resolver(Column::TnsPartiesEnabled), "tns_parties_enabled");
}

TEST_F(DataLayerVenueResolver, ResolvesTimezone) {
  EXPECT_EQ(resolver(Column::Timezone), "timezone");
}

TEST_F(DataLayerVenueResolver, ResolvesCancelOnDisconnect) {
  EXPECT_EQ(resolver(Column::CancelOnDisconnect), "cancel_on_disconnect");
}

TEST_F(DataLayerVenueResolver, ResolvesPersistenceEnabled) {
  EXPECT_EQ(resolver(Column::PersistenceEnabled), "persistence_enabled");
}

TEST_F(DataLayerVenueResolver, ResolvesPersistenceFilePath) {
  EXPECT_EQ(resolver(Column::PersistenceFilePath), "persistence_file_path");
}

}  // namespace
}  // namespace simulator::data_layer::internal_pqxx::test