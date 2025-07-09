#include "ih/marshalling/json/detail/key_resolver.hpp"

#include <fmt/format.h>

#include <stdexcept>
#include <string_view>
#include <type_traits>

#include "ih/marshalling/json/detail/keys.hpp"

namespace simulator::http::json {

namespace {

template <typename Attribute>
[[noreturn]]
auto raise_bad_attribute_error(std::string_view model_name, Attribute attribute)
    -> void {
  static_assert(std::is_enum_v<Attribute>);
  using Underlying = std::underlying_type_t<Attribute>;

  throw std::logic_error{fmt::format(
      "unable to resolve a JSON key by an undefined attribute enumeration "
      "value `{:#x}' for a {} model ",
      static_cast<Underlying>(attribute),
      model_name)};
}

}  // namespace

auto KeyResolver::resolve_key(data_layer::ColumnMapping::Attribute attribute)
    -> std::string_view {
  switch (attribute) {
    case data_layer::ColumnMapping::Attribute::ColumnFrom:
      return column_mapping_key::ColumnFrom;
    case data_layer::ColumnMapping::Attribute::ColumnTo:
      return column_mapping_key::ColumnTo;
    case data_layer::ColumnMapping::Attribute::DatasourceId:
      return column_mapping_key::DatasourceId;
  }

  raise_bad_attribute_error("ColumnMapping", attribute);
}

auto KeyResolver::resolve_key(data_layer::Datasource::Attribute attribute)
    -> std::string_view {
  switch (attribute) {
    case data_layer::Datasource::Attribute::DatasourceId:
      return datasource_key::DatasourceId;
    case data_layer::Datasource::Attribute::Enabled:
      return datasource_key::Enabled;
    case data_layer::Datasource::Attribute::Name:
      return datasource_key::Name;
    case data_layer::Datasource::Attribute::VenueId:
      return datasource_key::VenueId;
    case data_layer::Datasource::Attribute::Connection:
      return datasource_key::Connection;
    case data_layer::Datasource::Attribute::Format:
      return datasource_key::Format;
    case data_layer::Datasource::Attribute::Type:
      return datasource_key::Type;
    case data_layer::Datasource::Attribute::Repeat:
      return datasource_key::Repeat;
    case data_layer::Datasource::Attribute::TextDelimiter:
      return datasource_key::TextDelimiter;
    case data_layer::Datasource::Attribute::TextHeaderRow:
      return datasource_key::TextHeaderRow;
    case data_layer::Datasource::Attribute::TextDataRow:
      return datasource_key::TextDataRow;
    case data_layer::Datasource::Attribute::TableName:
      return datasource_key::TableName;
    case data_layer::Datasource::Attribute::MaxDepthLevels:
      return datasource_key::MaxDepthLevels;
  }

  raise_bad_attribute_error("Datasource", attribute);
}

auto KeyResolver::resolve_key(data_layer::Listing::Attribute attribute)
    -> std::string_view {
  switch (attribute) {
    case data_layer::Listing::Attribute::ListingId:
      return listing_key::ListingId;
    case data_layer::Listing::Attribute::Symbol:
      return listing_key::Symbol;
    case data_layer::Listing::Attribute::VenueId:
      return listing_key::VenueId;
    case data_layer::Listing::Attribute::SecurityType:
      return listing_key::SecurityType;
    case data_layer::Listing::Attribute::PriceCurrency:
      return listing_key::PriceCurrency;
    case data_layer::Listing::Attribute::FxBaseCurrency:
      return listing_key::FxBaseCurrency;
    case data_layer::Listing::Attribute::InstrSymbol:
      return listing_key::InstrSymbol;
    case data_layer::Listing::Attribute::QtyMinimum:
      return listing_key::QtyMinimum;
    case data_layer::Listing::Attribute::QtyMaximum:
      return listing_key::QtyMaximum;
    case data_layer::Listing::Attribute::QtyMultiple:
      return listing_key::QtyMultiple;
    case data_layer::Listing::Attribute::PriceTickSize:
      return listing_key::PriceTickSize;
    case data_layer::Listing::Attribute::Enabled:
      return listing_key::Enabled;
    case data_layer::Listing::Attribute::RandomQtyMaximum:
      return listing_key::RandomQtyMaximum;
    case data_layer::Listing::Attribute::RandomDepthLevels:
      return listing_key::RandomDepthLevels;
    case data_layer::Listing::Attribute::RandomOrdersSpread:
      return listing_key::RandomOrdersSpread;
    case data_layer::Listing::Attribute::RandomOrdersRate:
      return listing_key::RandomOrdersRate;
    case data_layer::Listing::Attribute::RandomTickRange:
      return listing_key::RandomTickRange;
    case data_layer::Listing::Attribute::SecurityExchange:
      return listing_key::SecurityExchange;
    case data_layer::Listing::Attribute::PartyId:
      return listing_key::PartyId;
    case data_layer::Listing::Attribute::PartyRole:
      return listing_key::PartyRole;
    case data_layer::Listing::Attribute::CusipId:
      return listing_key::CusipId;
    case data_layer::Listing::Attribute::SedolId:
      return listing_key::SedolId;
    case data_layer::Listing::Attribute::IsinId:
      return listing_key::IsinId;
    case data_layer::Listing::Attribute::RicId:
      return listing_key::RicId;
    case data_layer::Listing::Attribute::ExchangeSymbolId:
      return listing_key::ExchangeSymbolId;
    case data_layer::Listing::Attribute::BloombergSymbolId:
      return listing_key::BloombergSymbolId;
    case data_layer::Listing::Attribute::RandomQtyMinimum:
      return listing_key::RandomQtyMinimum;
    case data_layer::Listing::Attribute::RandomAmtMinimum:
      return listing_key::RandomAmtMinimum;
    case data_layer::Listing::Attribute::RandomAmtMaximum:
      return listing_key::RandomAmtMaximum;
    case data_layer::Listing::Attribute::RandomOrdersEnabled:
      return listing_key::RandomOrdersEnabled;
    case data_layer::Listing::Attribute::RandomAggressiveQtyMinimum:
      return listing_key::RandomAggressiveQtyMinimum;
    case data_layer::Listing::Attribute::RandomAggressiveQtyMaximum:
      return listing_key::RandomAggressiveQtyMaximum;
    case data_layer::Listing::Attribute::RandomAggressiveAmtMinimum:
      return listing_key::RandomAggressiveAmtMinimum;
    case data_layer::Listing::Attribute::RandomAggressiveAmtMaximum:
      return listing_key::RandomAggressiveAmtMaximum;
  }

  raise_bad_attribute_error("Listing", attribute);
}

auto KeyResolver::resolve_key(data_layer::MarketPhase::Attribute attribute)
    -> std::string_view {
  switch (attribute) {
    case data_layer::MarketPhase::Attribute::Phase:
      return market_phase_key::Phase;
    case data_layer::MarketPhase::Attribute::StartTime:
      return market_phase_key::StartTime;
    case data_layer::MarketPhase::Attribute::EndTime:
      return market_phase_key::EndTime;
    case data_layer::MarketPhase::Attribute::VenueId:
      return market_phase_key::VenueId;
    case data_layer::MarketPhase::Attribute::EndTimeRange:
      return market_phase_key::EndTimeRange;
    case data_layer::MarketPhase::Attribute::AllowCancels:
      return market_phase_key::AllowCancels;
  }

  raise_bad_attribute_error("MarketPhase", attribute);
}

auto KeyResolver::resolve_key(data_layer::PriceSeed::Attribute attribute)
    -> std::string_view {
  switch (attribute) {
    case data_layer::PriceSeed::Attribute::PriceSeedId:
      return price_seed_key::PriceSeedId;
    case data_layer::PriceSeed::Attribute::Symbol:
      return price_seed_key::Symbol;
    case data_layer::PriceSeed::Attribute::SecurityType:
      return price_seed_key::SecurityType;
    case data_layer::PriceSeed::Attribute::PriceCurrency:
      return price_seed_key::PriceCurrency;
    case data_layer::PriceSeed::Attribute::SecurityId:
      return price_seed_key::SecurityId;
    case data_layer::PriceSeed::Attribute::SecurityIdSource:
      return price_seed_key::SecurityIdSource;
    case data_layer::PriceSeed::Attribute::InstrumentSymbol:
      return price_seed_key::InstrumentSymbol;
    case data_layer::PriceSeed::Attribute::MidPrice:
      return price_seed_key::MidPrice;
    case data_layer::PriceSeed::Attribute::BidPrice:
      return price_seed_key::BidPrice;
    case data_layer::PriceSeed::Attribute::OfferPrice:
      return price_seed_key::OfferPrice;
    case data_layer::PriceSeed::Attribute::LastUpdate:
      return price_seed_key::LastUpdate;
  }

  raise_bad_attribute_error("PriceSeed", attribute);
}

auto KeyResolver::resolve_key(data_layer::Setting::Attribute attribute)
    -> std::string_view {
  switch (attribute) {
    case data_layer::Setting::Attribute::Key:
      return setting_key::Key;
    case data_layer::Setting::Attribute::Value:
      return setting_key::Value;
  }

  raise_bad_attribute_error("Setting", attribute);
}

auto KeyResolver::resolve_key(data_layer::Venue::Attribute attribute)
    -> std::string_view {
  switch (attribute) {
    case data_layer::Venue::Attribute::VenueId:
      return venue_key::VenueId;
    case data_layer::Venue::Attribute::Name:
      return venue_key::Name;
    case data_layer::Venue::Attribute::EngineType:
      return venue_key::EngineType;
    case data_layer::Venue::Attribute::SupportTifIoc:
      return venue_key::SupportTifIoc;
    case data_layer::Venue::Attribute::SupportTifFok:
      return venue_key::SupportTifFok;
    case data_layer::Venue::Attribute::SupportTifDay:
      return venue_key::SupportTifDay;
    case data_layer::Venue::Attribute::IncludeOwnOrders:
      return venue_key::IncludeOwnOrders;
    case data_layer::Venue::Attribute::RestPort:
      return venue_key::RestPort;
    case data_layer::Venue::Attribute::OrderOnStartup:
      return venue_key::OrderOnStartup;
    case data_layer::Venue::Attribute::RandomPartiesCount:
      return venue_key::RandomPartiesCount;
    case data_layer::Venue::Attribute::TnsEnabled:
      return venue_key::TnsEnabled;
    case data_layer::Venue::Attribute::TnsQtyEnabled:
      return venue_key::TnsQtyEnabled;
    case data_layer::Venue::Attribute::TnsSideEnabled:
      return venue_key::TnsSideEnabled;
    case data_layer::Venue::Attribute::TnsPartiesEnabled:
      return venue_key::TnsPartiesEnabled;
    case data_layer::Venue::Attribute::Timezone:
      return venue_key::Timezone;
    case data_layer::Venue::Attribute::CancelOnDisconnect:
      return venue_key::CancelOnDisconnect;
    case data_layer::Venue::Attribute::PersistenceEnabled:
      return venue_key::PersistenceEnabled;
    case data_layer::Venue::Attribute::PersistenceFilePath:
      return venue_key::PersistenceFilePath;
  }

  raise_bad_attribute_error("Venue", attribute);
}

}  // namespace simulator::http::json
