#include "ih/pqxx/common/column_resolver.hpp"

#include <string>
#include <string_view>

#include "ih/common/exceptions.hpp"
#include "ih/pqxx/common/names/database_entries.hpp"

namespace simulator::data_layer::internal_pqxx {

auto ColumnResolver::operator()(ColumnMapping::Attribute attribute) const
    -> std::string {
  return ColumnResolver::to_column_name(attribute);
}

auto ColumnResolver::operator()(Datasource::Attribute attribute) const
    -> std::string {
  return ColumnResolver::to_column_name(attribute);
}

auto ColumnResolver::operator()(Listing::Attribute attribute) const
    -> std::string {
  return ColumnResolver::to_column_name(attribute);
}

auto ColumnResolver::operator()(MarketPhase::Attribute attribute) const
    -> std::string {
  return ColumnResolver::to_column_name(attribute);
}

auto ColumnResolver::operator()(PriceSeed::Attribute attribute) const
    -> std::string {
  return ColumnResolver::to_column_name(attribute);
}

auto ColumnResolver::operator()(Setting::Attribute attribute) const
    -> std::string {
  return ColumnResolver::to_column_name(attribute);
}

auto ColumnResolver::operator()(Venue::Attribute attribute) const
    -> std::string {
  return ColumnResolver::to_column_name(attribute);
}

auto ColumnResolver::to_column_name(ColumnMapping::Attribute attribute)
    -> std::string {
  std::string_view column_name{};
  switch (attribute) {
    case ColumnMapping::Attribute::ColumnFrom:
      column_name = column_mapping_column::ColumnFrom;
      break;
    case ColumnMapping::Attribute::ColumnTo:
      column_name = column_mapping_column::ColumnTo;
      break;
    case ColumnMapping::Attribute::DatasourceId:
      column_name = column_mapping_column::DatasourceId;
      break;
  }

  if (!column_name.empty()) {
    return std::string{column_name};
  }
  throw ColumnNameEncodingError(table::ColumnMapping, attribute);
}

auto ColumnResolver::to_column_name(Datasource::Attribute attribute)
    -> std::string {
  std::string_view column_name{};

  switch (attribute) {
    case Datasource::Attribute::DatasourceId:
      column_name = datasource_column::DatasourceId;
      break;
    case Datasource::Attribute::Enabled:
      column_name = datasource_column::Enabled;
      break;
    case Datasource::Attribute::Name:
      column_name = datasource_column::Name;
      break;
    case Datasource::Attribute::VenueId:
      column_name = datasource_column::VenueId;
      break;
    case Datasource::Attribute::Connection:
      column_name = datasource_column::Connection;
      break;
    case Datasource::Attribute::Format:
      column_name = datasource_column::Format;
      break;
    case Datasource::Attribute::Type:
      column_name = datasource_column::Type;
      break;
    case Datasource::Attribute::Repeat:
      column_name = datasource_column::Repeat;
      break;
    case Datasource::Attribute::TextDelimiter:
      column_name = datasource_column::TextDelimiter;
      break;
    case Datasource::Attribute::TextHeaderRow:
      column_name = datasource_column::TextHeaderRow;
      break;
    case Datasource::Attribute::TextDataRow:
      column_name = datasource_column::TextDataRow;
      break;
    case Datasource::Attribute::TableName:
      column_name = datasource_column::TableName;
      break;
    case Datasource::Attribute::MaxDepthLevels:
      column_name = datasource_column::MaxDepthLevels;
      break;
  }

  if (!column_name.empty()) {
    return std::string{column_name};
  }
  throw ColumnNameEncodingError(table::Datasource, attribute);
}

auto ColumnResolver::to_column_name(Listing::Attribute attribute)
    -> std::string {
  std::string_view column_name{};
  switch (attribute) {
    case Listing::Attribute::ListingId:
      column_name = listing_column::ListingId;
      break;
    case Listing::Attribute::Symbol:
      column_name = listing_column::Symbol;
      break;
    case Listing::Attribute::VenueId:
      column_name = listing_column::VenueId;
      break;
    case Listing::Attribute::SecurityType:
      column_name = listing_column::SecurityType;
      break;
    case Listing::Attribute::PriceCurrency:
      column_name = listing_column::PriceCurrency;
      break;
    case Listing::Attribute::FxBaseCurrency:
      column_name = listing_column::FxBaseCurrency;
      break;
    case Listing::Attribute::InstrSymbol:
      column_name = listing_column::InstrSymbol;
      break;
    case Listing::Attribute::QtyMinimum:
      column_name = listing_column::QtyMinimum;
      break;
    case Listing::Attribute::QtyMaximum:
      column_name = listing_column::QtyMaximum;
      break;
    case Listing::Attribute::QtyMultiple:
      column_name = listing_column::QtyMultiple;
      break;
    case Listing::Attribute::PriceTickSize:
      column_name = listing_column::PriceTickSize;
      break;
    case Listing::Attribute::Enabled:
      column_name = listing_column::Enabled;
      break;
    case Listing::Attribute::RandomQtyMaximum:
      column_name = listing_column::RandomQtyMaximum;
      break;
    case Listing::Attribute::RandomDepthLevels:
      column_name = listing_column::RandomDepthLevels;
      break;
    case Listing::Attribute::RandomOrdersSpread:
      column_name = listing_column::RandomOrdersSpread;
      break;
    case Listing::Attribute::RandomOrdersRate:
      column_name = listing_column::RandomOrdersRate;
      break;
    case Listing::Attribute::RandomTickRange:
      column_name = listing_column::RandomTickRange;
      break;
    case Listing::Attribute::SecurityExchange:
      column_name = listing_column::SecurityExchange;
      break;
    case Listing::Attribute::PartyId:
      column_name = listing_column::PartyId;
      break;
    case Listing::Attribute::PartyRole:
      column_name = listing_column::PartyRole;
      break;
    case Listing::Attribute::CusipId:
      column_name = listing_column::CusipId;
      break;
    case Listing::Attribute::SedolId:
      column_name = listing_column::SedolId;
      break;
    case Listing::Attribute::IsinId:
      column_name = listing_column::IsinId;
      break;
    case Listing::Attribute::RicId:
      column_name = listing_column::RicId;
      break;
    case Listing::Attribute::ExchangeSymbolId:
      column_name = listing_column::ExchangeSymbolId;
      break;
    case Listing::Attribute::BloombergSymbolId:
      column_name = listing_column::BloombergSymbolId;
      break;
    case Listing::Attribute::RandomQtyMinimum:
      column_name = listing_column::RandomQtyMinimum;
      break;
    case Listing::Attribute::RandomAmtMinimum:
      column_name = listing_column::RandomAmtMinimum;
      break;
    case Listing::Attribute::RandomAmtMaximum:
      column_name = listing_column::RandomAmtMaximum;
      break;
    case Listing::Attribute::RandomOrdersEnabled:
      column_name = listing_column::RandomOrdersEnabled;
      break;
    case Listing::Attribute::RandomAggressiveQtyMinimum:
      column_name = listing_column::RandomAggressiveQtyMinimum;
      break;
    case Listing::Attribute::RandomAggressiveQtyMaximum:
      column_name = listing_column::RandomAggressiveQtyMaximum;
      break;
    case Listing::Attribute::RandomAggressiveAmtMinimum:
      column_name = listing_column::RandomAggressiveAmtMinimum;
      break;
    case Listing::Attribute::RandomAggressiveAmtMaximum:
      column_name = listing_column::RandomAggressiveAmtMaximum;
      break;
  }

  if (!column_name.empty()) {
    return std::string{column_name};
  }
  throw ColumnNameEncodingError(table::Listing, attribute);
}

auto ColumnResolver::to_column_name(MarketPhase::Attribute attribute)
    -> std::string {
  std::string_view column_name{};
  switch (attribute) {
    case MarketPhase::Attribute::Phase:
      column_name = market_phase_column::Phase;
      break;
    case MarketPhase::Attribute::StartTime:
      column_name = market_phase_column::StartTime;
      break;
    case MarketPhase::Attribute::EndTime:
      column_name = market_phase_column::EndTime;
      break;
    case MarketPhase::Attribute::VenueId:
      column_name = market_phase_column::VenueId;
      break;
    case MarketPhase::Attribute::EndTimeRange:
      column_name = market_phase_column::EndTimeRange;
      break;
    case MarketPhase::Attribute::AllowCancels:
      column_name = market_phase_column::AllowCancels;
  }

  if (!column_name.empty()) {
    return std::string{column_name};
  }
  throw ColumnNameEncodingError(table::MarketPhase, attribute);
}

auto ColumnResolver::to_column_name(PriceSeed::Attribute attribute)
    -> std::string {
  std::string_view column_name{};
  switch (attribute) {
    case PriceSeed::Attribute::PriceSeedId:
      column_name = price_seed_column::PriceSeedId;
      break;
    case PriceSeed::Attribute::Symbol:
      column_name = price_seed_column::Symbol;
      break;
    case PriceSeed::Attribute::SecurityType:
      column_name = price_seed_column::SecurityType;
      break;
    case PriceSeed::Attribute::PriceCurrency:
      column_name = price_seed_column::PriceCurrency;
      break;
    case PriceSeed::Attribute::SecurityId:
      column_name = price_seed_column::SecurityId;
      break;
    case PriceSeed::Attribute::SecurityIdSource:
      column_name = price_seed_column::SecurityIdSource;
      break;
    case PriceSeed::Attribute::InstrumentSymbol:
      column_name = price_seed_column::InstrumentSymbol;
      break;
    case PriceSeed::Attribute::MidPrice:
      column_name = price_seed_column::MidPrice;
      break;
    case PriceSeed::Attribute::BidPrice:
      column_name = price_seed_column::BidPrice;
      break;
    case PriceSeed::Attribute::OfferPrice:
      column_name = price_seed_column::OfferPrice;
      break;
    case PriceSeed::Attribute::LastUpdate:
      column_name = price_seed_column::LastUpdate;
      break;
  }

  if (!column_name.empty()) {
    return std::string{column_name};
  }
  throw ColumnNameEncodingError(table::PriceSeed, attribute);
}

auto ColumnResolver::to_column_name(Setting::Attribute attribute)
    -> std::string {
  std::string_view column_name{};
  switch (attribute) {
    case Setting::Attribute::Key:
      column_name = setting_column::Key;
      break;
    case Setting::Attribute::Value:
      column_name = setting_column::Value;
      break;
  }

  if (!column_name.empty()) {
    return std::string{column_name};
  }
  throw ColumnNameEncodingError(table::Setting, attribute);
}

auto ColumnResolver::to_column_name(Venue::Attribute attribute) -> std::string {
  std::string_view column_name{};
  switch (attribute) {
    case Venue::Attribute::VenueId:
      column_name = venue_column::VenueId;
      break;
    case Venue::Attribute::Name:
      column_name = venue_column::Name;
      break;
    case Venue::Attribute::EngineType:
      column_name = venue_column::EngineType;
      break;
    case Venue::Attribute::SupportTifIoc:
      column_name = venue_column::SupportTifIoc;
      break;
    case Venue::Attribute::SupportTifFok:
      column_name = venue_column::SupportTifFok;
      break;
    case Venue::Attribute::SupportTifDay:
      column_name = venue_column::SupportTifDay;
      break;
    case Venue::Attribute::IncludeOwnOrders:
      column_name = venue_column::IncludeOwnOrders;
      break;
    case Venue::Attribute::RestPort:
      column_name = venue_column::RestPort;
      break;
    case Venue::Attribute::OrderOnStartup:
      column_name = venue_column::OrderOnStartup;
      break;
    case Venue::Attribute::RandomPartiesCount:
      column_name = venue_column::RandomPartyCount;
      break;
    case Venue::Attribute::TnsEnabled:
      column_name = venue_column::TnsEnabled;
      break;
    case Venue::Attribute::TnsQtyEnabled:
      column_name = venue_column::TnsQtyEnabled;
      break;
    case Venue::Attribute::TnsSideEnabled:
      column_name = venue_column::TnsSideEnabled;
      break;
    case Venue::Attribute::TnsPartiesEnabled:
      column_name = venue_column::TnsPartiesEnabled;
      break;
    case Venue::Attribute::Timezone:
      column_name = venue_column::Timezone;
      break;
    case Venue::Attribute::CancelOnDisconnect:
      column_name = venue_column::CancelOnDisconnect;
      break;
    case Venue::Attribute::PersistenceEnabled:
      column_name = venue_column::PersistenceEnabled;
      break;
    case Venue::Attribute::PersistenceFilePath:
      column_name = venue_column::PersistenceFilePath;
      break;
  }

  if (!column_name.empty()) {
    return std::string{column_name};
  }
  throw ColumnNameEncodingError(table::Venue, attribute);
}

}  // namespace simulator::data_layer::internal_pqxx
