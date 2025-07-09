#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_KEYS_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_KEYS_HPP_

#include <string_view>

namespace simulator::http::json {

namespace column_mapping_key {

constexpr std::string_view DatasourceId{"datasourceId"};
constexpr std::string_view ColumnFrom{"columnFrom"};
constexpr std::string_view ColumnTo{"columnTo"};

}  // namespace column_mapping_key

namespace datasource_key {

constexpr std::string_view DatasourceId{"id"};
constexpr std::string_view Enabled{"enabled"};
constexpr std::string_view Name{"name"};
constexpr std::string_view VenueId{"venueId"};
constexpr std::string_view Connection{"connection"};
constexpr std::string_view Format{"format"};
constexpr std::string_view Type{"type"};
constexpr std::string_view Repeat{"repeat"};
constexpr std::string_view TextDelimiter{"textDelimiter"};
constexpr std::string_view TextHeaderRow{"textHeaderRow"};
constexpr std::string_view TextDataRow{"textDataRow"};
constexpr std::string_view TableName{"tableName"};
constexpr std::string_view ColumnMapping{"columnMapping"};
constexpr std::string_view MaxDepthLevels{"maxDepthLevels"};
constexpr std::string_view Datasources{"dataSources"};

}  // namespace datasource_key

namespace listing_key {

constexpr std::string_view ListingId{"id"};
constexpr std::string_view Symbol{"symbol"};
constexpr std::string_view VenueId{"venueId"};
constexpr std::string_view SecurityType{"securityType"};
constexpr std::string_view PriceCurrency{"priceCurrency"};
constexpr std::string_view FxBaseCurrency{"fxBaseCurrency"};
constexpr std::string_view InstrSymbol{"instrSymbol"};
constexpr std::string_view SecurityExchange{"securityExchange"};
constexpr std::string_view PartyId{"partyId"};
constexpr std::string_view PartyRole{"partyRole"};
constexpr std::string_view CusipId{"cusipId"};
constexpr std::string_view SedolId{"sedolId"};
constexpr std::string_view IsinId{"isinId"};
constexpr std::string_view RicId{"ricId"};
constexpr std::string_view ExchangeSymbolId{"exchangeSymbolId"};
constexpr std::string_view BloombergSymbolId{"bloombergSymbolId"};
constexpr std::string_view QtyMinimum{"qtyMinimum"};
constexpr std::string_view QtyMaximum{"qtyMaximum"};
constexpr std::string_view QtyMultiple{"qtyMultiple"};
constexpr std::string_view PriceTickSize{"priceTickSize"};
constexpr std::string_view RandomQtyMinimum{"randomQtyMinimum"};
constexpr std::string_view RandomQtyMaximum{"randomQtyMaximum"};
constexpr std::string_view RandomDepthLevels{"randomDepthLevels"};
constexpr std::string_view RandomOrdersSpread{"randomOrdersSpread"};
constexpr std::string_view RandomOrdersRate{"randomOrdersRate"};
constexpr std::string_view RandomTickRange{"randomTickRange"};
constexpr std::string_view RandomAmtMinimum{"randomAmtMinimum"};
constexpr std::string_view RandomAmtMaximum{"randomAmtMaximum"};
constexpr std::string_view RandomAggressiveQtyMinimum{"randomAggQtyMinimum"};
constexpr std::string_view RandomAggressiveQtyMaximum{"randomAggQtyMaximum"};
constexpr std::string_view RandomAggressiveAmtMinimum{"randomAggAmtMinimum"};
constexpr std::string_view RandomAggressiveAmtMaximum{"randomAggAmtMaximum"};
constexpr std::string_view Enabled{"enabled"};
constexpr std::string_view RandomOrdersEnabled{"randomOrdersEnabled"};
constexpr std::string_view Listings{"listings"};

}  // namespace listing_key

namespace market_phase_key {

constexpr std::string_view Phase{"phase"};
constexpr std::string_view StartTime{"startTime"};
constexpr std::string_view EndTime{"endTime"};
constexpr std::string_view VenueId{"venueId"};
constexpr std::string_view EndTimeRange{"endTimeRange"};
constexpr std::string_view AllowCancels{"allowCancels"};

}  // namespace market_phase_key

namespace price_seed_key {

constexpr std::string_view PriceSeedId{"id"};
constexpr std::string_view Symbol{"symbol"};
constexpr std::string_view SecurityType{"securityType"};
constexpr std::string_view PriceCurrency{"priceCurrency"};
constexpr std::string_view SecurityId{"securityId"};
constexpr std::string_view SecurityIdSource{"securityIdSource"};
constexpr std::string_view InstrumentSymbol{"instrumentSymbol"};
constexpr std::string_view MidPrice{"midPrice"};
constexpr std::string_view BidPrice{"bidPrice"};
constexpr std::string_view OfferPrice{"offerPrice"};
constexpr std::string_view LastUpdate{"lastUpdate"};
constexpr std::string_view PriceSeeds{"priceSeeds"};

}  // namespace price_seed_key

namespace setting_key {

constexpr std::string_view Key{"key"};
constexpr std::string_view Value{"value"};
constexpr std::string_view Settings{"settings"};

}  // namespace setting_key

namespace venue_key {

constexpr std::string_view VenueId{"id"};
constexpr std::string_view Name{"name"};
constexpr std::string_view EngineType{"engineType"};
constexpr std::string_view SupportTifIoc{"supportTifIoc"};
constexpr std::string_view SupportTifFok{"supportTifFok"};
constexpr std::string_view SupportTifDay{"supportTifDay"};
constexpr std::string_view IncludeOwnOrders{"includeOwnOrders"};
constexpr std::string_view RestPort{"restPort"};
constexpr std::string_view OrderOnStartup{"orderOnStartup"};
constexpr std::string_view RandomPartiesCount{"randomPartyCount"};
constexpr std::string_view TnsEnabled{"timeAndSalesEnabled"};
constexpr std::string_view TnsQtyEnabled{"timeAndSalesQuantityEnabled"};
constexpr std::string_view TnsSideEnabled{"timeAndSalesSideEnabled"};
constexpr std::string_view TnsPartiesEnabled{"timeAndSalesPartiesEnabled"};
constexpr std::string_view Timezone{"timezone"};
constexpr std::string_view CancelOnDisconnect{"cancelOnDisconnect"};
constexpr std::string_view PersistenceEnabled{"persistenceEnabled"};
constexpr std::string_view PersistenceFilePath{"persistenceFilePath"};
constexpr std::string_view MarketPhases{"phases"};
constexpr std::string_view Venues{"venues"};

}  // namespace venue_key

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_KEYS_HPP_
