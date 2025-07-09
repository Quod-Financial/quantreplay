#ifndef SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_NAMES_DATABASE_ENTRIES_HPP_
#define SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_NAMES_DATABASE_ENTRIES_HPP_

#include <string_view>

namespace simulator::data_layer::internal_pqxx {
namespace table {

constexpr std::string_view ColumnMapping{"column_mapping"};
constexpr std::string_view Datasource{"data_source"};
constexpr std::string_view Listing{"listing"};
constexpr std::string_view MarketPhase{"market_phase"};
constexpr std::string_view PriceSeed{"price_seed"};
constexpr std::string_view Setting{"setting"};
constexpr std::string_view Venue{"venue"};

}  // namespace table

namespace column_mapping_column {

constexpr std::string_view ColumnFrom{"column_from"};
constexpr std::string_view ColumnTo{"column_to"};
constexpr std::string_view DatasourceId{"data_source_id"};

}  // namespace column_mapping_column

namespace datasource_column {

constexpr std::string_view DatasourceId{"data_source_id"};
constexpr std::string_view Enabled{"enabled"};
constexpr std::string_view Name{"name"};
constexpr std::string_view VenueId{"venue_id"};
constexpr std::string_view Connection{"connection"};
constexpr std::string_view Format{"format"};
constexpr std::string_view Type{"type"};
constexpr std::string_view Repeat{"repeat"};
constexpr std::string_view TextDelimiter{"text_delimiter"};
constexpr std::string_view TextHeaderRow{"text_header_row"};
constexpr std::string_view TextDataRow{"text_data_row"};
constexpr std::string_view TableName{"table_name"};
constexpr std::string_view MaxDepthLevels{"max_depth_levels"};

}  // namespace datasource_column

namespace listing_column {

constexpr std::string_view ListingId{"listing_id"};
constexpr std::string_view Symbol{"symbol"};
constexpr std::string_view VenueId{"venue_id"};
constexpr std::string_view SecurityType{"security_type"};
constexpr std::string_view PriceCurrency{"price_currency"};
constexpr std::string_view FxBaseCurrency{"fx_base_currency"};
constexpr std::string_view InstrSymbol{"instr_symbol"};
constexpr std::string_view QtyMinimum{"qty_minimum"};
constexpr std::string_view QtyMaximum{"qty_maximum"};
constexpr std::string_view QtyMultiple{"qty_multiple"};
constexpr std::string_view PriceTickSize{"price_tick_size"};
constexpr std::string_view Enabled{"enabled"};
constexpr std::string_view RandomQtyMaximum{"random_qty_maximum"};
constexpr std::string_view RandomDepthLevels{"random_depth_levels"};
constexpr std::string_view RandomOrdersSpread{"random_orders_spread"};
constexpr std::string_view RandomOrdersRate{"random_orders_rate"};
constexpr std::string_view RandomTickRange{"random_tick_range"};
constexpr std::string_view SecurityExchange{"security_exchange"};
constexpr std::string_view PartyId{"party_id"};
constexpr std::string_view PartyRole{"party_role"};
constexpr std::string_view CusipId{"cusip_id"};
constexpr std::string_view SedolId{"sedol_id"};
constexpr std::string_view IsinId{"isin_id"};
constexpr std::string_view RicId{"ric_id"};
constexpr std::string_view ExchangeSymbolId{"exchange_symbol_id"};
constexpr std::string_view BloombergSymbolId{"bloomberg_symbol_id"};
constexpr std::string_view RandomQtyMinimum{"random_qty_minimum"};
constexpr std::string_view RandomAmtMinimum{"random_amt_minimum"};
constexpr std::string_view RandomAmtMaximum{"random_amt_maximum"};
constexpr std::string_view RandomOrdersEnabled{"random_orders_enabled"};
constexpr std::string_view RandomAggressiveQtyMinimum{
    "random_aggressive_qty_minimum"};
constexpr std::string_view RandomAggressiveQtyMaximum{
    "random_aggressive_qty_maximum"};
constexpr std::string_view RandomAggressiveAmtMinimum{
    "random_aggressive_amt_minimum"};
constexpr std::string_view RandomAggressiveAmtMaximum{
    "random_aggressive_amt_maximum"};

}  // namespace listing_column

namespace market_phase_column {

constexpr std::string_view Phase{"phase"};
constexpr std::string_view StartTime{"start_time"};
constexpr std::string_view EndTime{"end_time"};
constexpr std::string_view VenueId{"venue_id"};
constexpr std::string_view EndTimeRange{"end_time_range"};
constexpr std::string_view AllowCancels{"allow_cancels"};

}  // namespace market_phase_column

namespace price_seed_column {

constexpr std::string_view PriceSeedId{"price_seed_id"};
constexpr std::string_view Symbol{"symbol"};
constexpr std::string_view SecurityType{"security_type"};
constexpr std::string_view PriceCurrency{"price_currency"};
constexpr std::string_view SecurityId{"security_id"};
constexpr std::string_view SecurityIdSource{"security_id_source"};
constexpr std::string_view InstrumentSymbol{"instrument_symbol"};
constexpr std::string_view MidPrice{"mid_price"};
constexpr std::string_view BidPrice{"bid_price"};
constexpr std::string_view OfferPrice{"offer_price"};
constexpr std::string_view LastUpdate{"last_update"};

}  // namespace price_seed_column

namespace setting_column {

constexpr std::string_view Key{"key"};
constexpr std::string_view Value{"value"};

}  // namespace setting_column

namespace venue_column {

constexpr std::string_view VenueId{"venue_id"};
constexpr std::string_view Name{"name"};
constexpr std::string_view EngineType{"engine_type"};
constexpr std::string_view SupportTifIoc{"support_tif_ioc"};
constexpr std::string_view SupportTifFok{"support_tif_fok"};
constexpr std::string_view SupportTifDay{"support_tif_day"};
constexpr std::string_view IncludeOwnOrders{"include_own_orders"};
constexpr std::string_view RestPort{"rest_port"};
constexpr std::string_view OrderOnStartup{"order_on_startup"};
constexpr std::string_view RandomPartyCount{"random_party_count"};
constexpr std::string_view TnsEnabled{"tns_enabled"};
constexpr std::string_view TnsQtyEnabled{"tns_qty_enabled"};
constexpr std::string_view TnsSideEnabled{"tns_side_enabled"};
constexpr std::string_view TnsPartiesEnabled{"tns_parties_enabled"};
constexpr std::string_view Timezone{"timezone"};
constexpr std::string_view CancelOnDisconnect{"cancel_on_disconnect"};
constexpr std::string_view PersistenceEnabled{"persistence_enabled"};
constexpr std::string_view PersistenceFilePath{"persistence_file_path"};

}  // namespace venue_column
}  // namespace simulator::data_layer::internal_pqxx

#endif  // SIMULATOR_DATA_LAYER_IH_PQXX_DATABASE_NAMES_DATABASE_ENTRIES_HPP_
