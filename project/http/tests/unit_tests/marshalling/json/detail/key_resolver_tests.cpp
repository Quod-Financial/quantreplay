#include <gtest/gtest.h>

#include <stdexcept>

#include "data_layer/api/models/column_mapping.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/market_phase.hpp"
#include "data_layer/api/models/price_seed.hpp"
#include "data_layer/api/models/setting.hpp"
#include "data_layer/api/models/venue.hpp"
#include "ih/marshalling/json/detail/key_resolver.hpp"

namespace simulator::http::json::test {
namespace {

struct HttpJsonKeyResolverColumnMapping : public ::testing::Test {
  using Attribute = data_layer::ColumnMapping::Attribute;
};

TEST_F(HttpJsonKeyResolverColumnMapping,
       ThrowsExceptionOnResolvingUndefinedKey) {
  EXPECT_THROW((void)KeyResolver::resolve_key(static_cast<Attribute>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonKeyResolverColumnMapping, ResolvesDatasourceId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::DatasourceId), "datasourceId");
}

TEST_F(HttpJsonKeyResolverColumnMapping, ResolvesColumnFrom) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::ColumnFrom), "columnFrom");
}

TEST_F(HttpJsonKeyResolverColumnMapping, ResolvesColumnTo) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::ColumnTo), "columnTo");
}

struct HttpJsonKeyResolverDatasource : public ::testing::Test {
  using Attribute = data_layer::Datasource::Attribute;
};

TEST_F(HttpJsonKeyResolverDatasource, ThrowsExceptionOnResolvingUndefinedKey) {
  EXPECT_THROW((void)KeyResolver::resolve_key(static_cast<Attribute>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesDatasourceId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::DatasourceId), "id");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesEnabled) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Enabled), "enabled");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesName) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Name), "name");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesVenueId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::VenueId), "venueId");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesConnection) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Connection), "connection");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesFormat) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Format), "format");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesType) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Type), "type");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesRepeat) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Repeat), "repeat");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesTextDelimiter) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::TextDelimiter),
            "textDelimiter");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesTextHeaderRow) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::TextHeaderRow),
            "textHeaderRow");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesTextDataRow) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::TextDataRow), "textDataRow");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesTableName) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::TableName), "tableName");
}

TEST_F(HttpJsonKeyResolverDatasource, ResolvesMaxDepthLevels) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::MaxDepthLevels),
            "maxDepthLevels");
}

struct HttpJsonKeyResolverListing : public ::testing::Test {
  using Attribute = data_layer::Listing::Attribute;
};

TEST_F(HttpJsonKeyResolverListing, ThrowsExceptionOnResolvingUndefinedKey) {
  EXPECT_THROW((void)KeyResolver::resolve_key(static_cast<Attribute>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonKeyResolverListing, ResolvesListingId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::ListingId), "id");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesSymbol) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Symbol), "symbol");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesVenueId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::VenueId), "venueId");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesSecurityType) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::SecurityType), "securityType");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesPriceCurrency) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::PriceCurrency),
            "priceCurrency");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesFxBaseCurrency) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::FxBaseCurrency),
            "fxBaseCurrency");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesInstrSymbol) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::InstrSymbol), "instrSymbol");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesSecurityExchange) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::SecurityExchange),
            "securityExchange");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesPartyId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::PartyId), "partyId");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesPartyRole) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::PartyRole), "partyRole");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesCusipId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::CusipId), "cusipId");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesSedolId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::SedolId), "sedolId");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesIsinId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::IsinId), "isinId");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRicId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RicId), "ricId");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesExchangeSymbolId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::ExchangeSymbolId),
            "exchangeSymbolId");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesBloombergSymbolId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::BloombergSymbolId),
            "bloombergSymbolId");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesQtyMinimum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::QtyMinimum), "qtyMinimum");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesQtyMaximum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::QtyMaximum), "qtyMaximum");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesQtyMultiple) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::QtyMultiple), "qtyMultiple");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesPriceTickSize) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::PriceTickSize),
            "priceTickSize");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomQtyMinimum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomQtyMinimum),
            "randomQtyMinimum");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomQtyMaximum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomQtyMaximum),
            "randomQtyMaximum");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomDepthLevels) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomDepthLevels),
            "randomDepthLevels");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomOrdersSpread) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomOrdersSpread),
            "randomOrdersSpread");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomOrdersRate) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomOrdersRate),
            "randomOrdersRate");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomTickRange) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomTickRange),
            "randomTickRange");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomAmtMinimum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomAmtMinimum),
            "randomAmtMinimum");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomAmtMaximum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomAmtMaximum),
            "randomAmtMaximum");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesEnabled) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Enabled), "enabled");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomOrdersEnabled) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomOrdersEnabled),
            "randomOrdersEnabled");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomAggressiveQtyMinimum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomAggressiveQtyMinimum),
            "randomAggQtyMinimum");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomAggressiveQtyMaximum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomAggressiveQtyMaximum),
            "randomAggQtyMaximum");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomAggressiveAmtMinimum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomAggressiveAmtMinimum),
            "randomAggAmtMinimum");
}

TEST_F(HttpJsonKeyResolverListing, ResolvesRandomAggressiveAmtMaximum) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomAggressiveAmtMaximum),
            "randomAggAmtMaximum");
}

struct HttpJsonKeyResolverMarketPhase : public ::testing::Test {
  using Attribute = data_layer::MarketPhase::Attribute;
};

TEST_F(HttpJsonKeyResolverMarketPhase, ThrowsExceptionOnResolvingUndefinedKey) {
  EXPECT_THROW((void)KeyResolver::resolve_key(static_cast<Attribute>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonKeyResolverMarketPhase, ResolvesPhase) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Phase), "phase");
}

TEST_F(HttpJsonKeyResolverMarketPhase, ResolvesStartTime) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::StartTime), "startTime");
}

TEST_F(HttpJsonKeyResolverMarketPhase, ResolvesEndTime) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::EndTime), "endTime");
}

TEST_F(HttpJsonKeyResolverMarketPhase, ResolvesVenueId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::VenueId), "venueId");
}

TEST_F(HttpJsonKeyResolverMarketPhase, ResolvesEndTimeRange) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::EndTimeRange), "endTimeRange");
}

TEST_F(HttpJsonKeyResolverMarketPhase, ResolvesAllowCancels) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::AllowCancels), "allowCancels");
}

struct HttpJsonKeyResolverPriceSeed : public ::testing::Test {
  using Attribute = data_layer::PriceSeed::Attribute;
};

TEST_F(HttpJsonKeyResolverPriceSeed, ThrowsExceptionOnResolvingUndefinedKey) {
  EXPECT_THROW((void)KeyResolver::resolve_key(static_cast<Attribute>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesPriceSeedId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::PriceSeedId), "id");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesSymbol) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Symbol), "symbol");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesSecurityType) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::SecurityType), "securityType");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesPriceCurrency) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::PriceCurrency),
            "priceCurrency");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesSecurityId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::SecurityId), "securityId");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesSecurityIdSource) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::SecurityIdSource),
            "securityIdSource");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesInstrumentSymbol) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::InstrumentSymbol),
            "instrumentSymbol");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesMidPrice) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::MidPrice), "midPrice");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesBidPrice) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::BidPrice), "bidPrice");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesOfferPrice) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::OfferPrice), "offerPrice");
}

TEST_F(HttpJsonKeyResolverPriceSeed, ResolvesLastUpdate) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::LastUpdate), "lastUpdate");
}

struct HttpJsonKeyResolverSetting : public ::testing::Test {
  using Attribute = data_layer::Setting::Attribute;
};

TEST_F(HttpJsonKeyResolverSetting, ThrowsExceptionOnResolvingUndefinedKey) {
  EXPECT_THROW((void)KeyResolver::resolve_key(static_cast<Attribute>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonKeyResolverSetting, ResolvesKey) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Key), "key");
}

TEST_F(HttpJsonKeyResolverSetting, ResolvesValue) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Value), "value");
}

struct HttpJsonKeyResolverVenue : public ::testing::Test {
  using Attribute = data_layer::Venue::Attribute;
};

TEST_F(HttpJsonKeyResolverVenue, ThrowsExceptionOnResolvingUndefinedKey) {
  EXPECT_THROW((void)KeyResolver::resolve_key(static_cast<Attribute>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesVenueId) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::VenueId), "id");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesName) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Name), "name");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesEngineType) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::EngineType), "engineType");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesSupportTifIoc) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::SupportTifIoc),
            "supportTifIoc");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesSupportTifFok) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::SupportTifFok),
            "supportTifFok");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesSupportTifDay) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::SupportTifDay),
            "supportTifDay");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesIncludeOwnOrders) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::IncludeOwnOrders),
            "includeOwnOrders");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesRestPort) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RestPort), "restPort");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesOrderOnStartup) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::OrderOnStartup),
            "orderOnStartup");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesTnsEnabled) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::TnsEnabled),
            "timeAndSalesEnabled");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesTnsQtyEnabled) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::TnsQtyEnabled),
            "timeAndSalesQuantityEnabled");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesTnsSideEnabled) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::TnsSideEnabled),
            "timeAndSalesSideEnabled");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesTnsPartiesEnabled) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::TnsPartiesEnabled),
            "timeAndSalesPartiesEnabled");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesRandomPartiesCount) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::RandomPartiesCount),
            "randomPartyCount");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesTimezone) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::Timezone), "timezone");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesCancelOnDisconnect) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::CancelOnDisconnect),
            "cancelOnDisconnect");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesPersistenceEnabled) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::PersistenceEnabled),
            "persistenceEnabled");
}

TEST_F(HttpJsonKeyResolverVenue, ResolvesPersistenceFilePath) {
  EXPECT_EQ(KeyResolver::resolve_key(Attribute::PersistenceFilePath),
            "persistenceFilePath");
}

}  // namespace
}  // namespace simulator::http::json::test
