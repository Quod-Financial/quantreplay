#include "instruments/sources.hpp"

#include <utility>
#include <vector>

#include "cfg/api/cfg.hpp"
#include "data_layer/api/converters/price_tick_ranges.hpp"
#include "data_layer/api/data_access_layer.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/predicate/predicate.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::instrument {

namespace detail {
namespace {

template <typename Original, typename Converted>
auto convert(const std::optional<Original>& original,
             std::optional<Converted>& converted) -> void {
  if (original.has_value()) {
    converted = Converted{*original};
  }
}

template <>
auto convert(const std::optional<std::string>& original,
             std::optional<SecurityType>& converted) -> void {
  if (!original.has_value()) {
    return;
  }

  const std::string_view security_type_string = *original;
  converted = convert_security_type_from_string(security_type_string);
  if (!converted.has_value()) {
    log::warn("ignoring an unsupported security type value `{}'",
              security_type_string);
  }
}

template <>
auto convert(const std::optional<std::string>& original,
             std::optional<PartyRole>& converted) -> void {
  if (!original.has_value()) {
    return;
  }

  const std::string_view party_role_string = *original;
  converted = convert_party_role_from_string(party_role_string);
  if (!converted.has_value()) {
    log::warn("ignoring an unsupported party role value `{}'",
              party_role_string);
  }
}

auto convert(data_layer::Listing::EngineType original, EngineType& converted)
    -> void {
  switch (original) {
    case data_layer::Listing::EngineType::Matching:
      converted = EngineType::Matching;
      return;
    case data_layer::Listing::EngineType::Quoting:
      converted = EngineType::Quoting;
      return;
  }
}

}  // namespace

auto create_instrument(const data_layer::Listing& listing) noexcept
    -> Instrument {
  Instrument instrument;
  instrument.database_id = DatabaseId{listing.listing_id()};

  convert(listing.symbol(), instrument.symbol);
  convert(listing.security_type(), instrument.security_type);
  convert(listing.price_currency(), instrument.price_currency);
  convert(listing.fx_base_currency(), instrument.base_currency);
  convert(listing.security_exchange(), instrument.security_exchange);
  convert(listing.party_id(), instrument.party_id);
  convert(listing.party_role(), instrument.party_role);
  convert(listing.cusip_id(), instrument.cusip);
  convert(listing.sedol_id(), instrument.sedol);
  convert(listing.isin_id(), instrument.isin);
  convert(listing.ric_id(), instrument.ric);
  convert(listing.exchange_symbol_id(), instrument.exchange_id);
  convert(listing.bloomberg_symbol_id(), instrument.bloomberg_id);
  instrument.price_tick_ranges =
      data_layer::converter::to_price_tick_ranges(listing.price_tick_ranges());
  convert(listing.qty_multiple(), instrument.quantity_tick);
  convert(listing.qty_minimum(), instrument.min_quantity);
  convert(listing.qty_maximum(), instrument.max_quantity);
  convert(listing.engine_type(), instrument.engine_type);

  return instrument;
}

}  // namespace detail

DatabaseSource::DatabaseSource(data_layer::database::Context database) noexcept
    : db_{std::move(database)} {}

auto DatabaseSource::load_instruments(
    const LoadingProcedure& loading_proc) const -> void {
  using data_layer::Listing;
  using data_layer::select_all_listings;

  log::info("loading listings from the database");

  const std::vector<Listing> listings = select_all_listings(db_, predicate());
  log::debug("{} matching listings selected", listings.size());

  for (const auto& listing : listings) {
    loading_proc(detail::create_instrument(listing));
  }
}

auto DatabaseSource::predicate() -> data_layer::Listing::Predicate {
  using data_layer::Listing;
  using data_layer::ListingCmp;

  return ListingCmp::eq(Listing::Attribute::VenueId,
                        simulator::cfg::venue().name) &&
         ListingCmp::eq(Listing::Attribute::Enabled, true);
}

MemorySource::MemorySource(std::vector<Instrument> instruments) noexcept
    : instruments_(std::move(instruments)) {}

auto MemorySource::load_instruments(const LoadingProcedure& loading_proc) const
    -> void {
  for (const auto& instrument : instruments_) {
    loading_proc(instrument);
  }
}

auto MemorySource::add_instrument(Instrument instrument) -> MemorySource& {
  instruments_.emplace_back(std::move(instrument));
  return *this;
}

}  // namespace simulator::trading_system::instrument