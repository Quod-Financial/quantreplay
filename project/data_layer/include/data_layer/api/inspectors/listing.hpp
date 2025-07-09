#ifndef SIMULATOR_PROJECT_DATA_LAYER_API_COMMON_INSPECTORS_LISTING_HPP_
#define SIMULATOR_PROJECT_DATA_LAYER_API_COMMON_INSPECTORS_LISTING_HPP_

#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "data_layer/api/models/listing.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class ListingReader final {
  using Attribute = Listing::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit ListingReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const Listing& listing) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Marshaller>
class ListingPatchReader final {
  using Attribute = Listing::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit ListingPatchReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const Listing::Patch& patch) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class ListingPatchWriter final {
  using Attribute = Listing::Attribute;

  template <typename T>
  constexpr inline static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit ListingPatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_(unmarshaller) {}

  auto write(Listing::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
inline auto ListingReader<Marshaller>::read(const Listing& listing) -> void {
  const std::uint64_t listing_id = listing.listing_id();
  static_assert(can_marshall_v<decltype(listing_id)>);
  marshaller_(Attribute::ListingId, listing_id);

  const std::string& venue_id = listing.venue_id();
  static_assert(can_marshall_v<decltype(venue_id)>);
  marshaller_(Attribute::VenueId, venue_id);

  if (const auto& value = listing.symbol()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Symbol, *value);
  }

  if (const auto& value = listing.security_type()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityType, *value);
  }

  if (const auto& value = listing.price_currency()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PriceCurrency, *value);
  }

  if (const auto& value = listing.fx_base_currency()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::FxBaseCurrency, *value);
  }

  if (const auto& value = listing.instr_symbol()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::InstrSymbol, *value);
  }

  if (const auto& value = listing.qty_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::QtyMinimum, *value);
  }

  if (const auto& value = listing.qty_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::QtyMaximum, *value);
  }

  if (const auto& value = listing.qty_multiple()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::QtyMultiple, *value);
  }

  if (const auto& value = listing.price_tick_size()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PriceTickSize, *value);
  }

  if (const auto& value = listing.enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Enabled, *value);
  }

  if (const auto& value = listing.random_qty_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomQtyMaximum, *value);
  }

  if (const auto& value = listing.random_depth_levels()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomDepthLevels, *value);
  }

  if (const auto& value = listing.random_orders_spread()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomOrdersSpread, *value);
  }

  if (const auto& value = listing.random_orders_rate()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomOrdersRate, *value);
  }

  if (const auto& value = listing.random_tick_range()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomTickRange, *value);
  }

  if (const auto& value = listing.security_exchange()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityExchange, *value);
  }

  if (const auto& value = listing.party_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PartyId, *value);
  }

  if (const auto& value = listing.party_role()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PartyRole, *value);
  }

  if (const auto& value = listing.cusip_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::CusipId, *value);
  }

  if (const auto& value = listing.sedol_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SedolId, *value);
  }

  if (const auto& value = listing.isin_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::IsinId, *value);
  }

  if (const auto& value = listing.ric_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RicId, *value);
  }

  if (const auto& value = listing.exchange_symbol_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::ExchangeSymbolId, *value);
  }

  if (const auto& value = listing.bloomberg_symbol_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::BloombergSymbolId, *value);
  }

  if (const auto& value = listing.random_qty_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomQtyMinimum, *value);
  }

  if (const auto& value = listing.random_amt_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAmtMinimum, *value);
  }

  if (const auto& value = listing.random_amt_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAmtMaximum, *value);
  }

  if (const auto& value = listing.random_orders_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomOrdersEnabled, *value);
  }

  if (const auto& value = listing.random_aggressive_qty_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAggressiveQtyMinimum, *value);
  }

  if (const auto& value = listing.random_aggressive_qty_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAggressiveQtyMaximum, *value);
  }

  if (const auto& value = listing.random_aggressive_amt_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAggressiveAmtMinimum, *value);
  }

  if (const auto& value = listing.random_aggressive_amt_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAggressiveAmtMaximum, *value);
  }
}

template <typename Marshaller>
inline auto ListingPatchReader<Marshaller>::read(const Listing::Patch& patch)
    -> void {
  if (const auto& value = patch.symbol()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Symbol, *value);
  }

  if (const auto& value = patch.venue_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::VenueId, *value);
  }

  if (const auto& value = patch.security_type()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityType, *value);
  }

  if (const auto& value = patch.price_currency()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PriceCurrency, *value);
  }

  if (const auto& value = patch.fx_base_currency()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::FxBaseCurrency, *value);
  }

  if (const auto& value = patch.instr_symbol()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::InstrSymbol, *value);
  }

  if (const auto& value = patch.qty_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::QtyMinimum, *value);
  }

  if (const auto& value = patch.qty_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::QtyMaximum, *value);
  }

  if (const auto& value = patch.qty_multiple()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::QtyMultiple, *value);
  }

  if (const auto& value = patch.price_tick_size()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PriceTickSize, *value);
  }

  if (const auto& value = patch.enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Enabled, *value);
  }

  if (const auto& value = patch.random_qty_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomQtyMaximum, *value);
  }

  if (const auto& value = patch.random_depth_levels()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomDepthLevels, *value);
  }

  if (const auto& value = patch.random_orders_spread()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomOrdersSpread, *value);
  }

  if (const auto& value = patch.random_orders_rate()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomOrdersRate, *value);
  }

  if (const auto& value = patch.random_tick_range()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomTickRange, *value);
  }

  if (const auto& value = patch.security_exchange()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityExchange, *value);
  }

  if (const auto& value = patch.party_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PartyId, *value);
  }

  if (const auto& value = patch.party_role()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PartyRole, *value);
  }

  if (const auto& value = patch.cusip_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::CusipId, *value);
  }

  if (const auto& value = patch.sedol_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SedolId, *value);
  }

  if (const auto& value = patch.isin_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::IsinId, *value);
  }

  if (const auto& value = patch.ric_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RicId, *value);
  }

  if (const auto& value = patch.exchange_symbol_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::ExchangeSymbolId, *value);
  }

  if (const auto& value = patch.bloomberg_symbol_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::BloombergSymbolId, *value);
  }

  if (const auto& value = patch.random_qty_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomQtyMinimum, *value);
  }

  if (const auto& value = patch.random_amt_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAmtMinimum, *value);
  }

  if (const auto& value = patch.random_amt_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAmtMaximum, *value);
  }

  if (const auto& value = patch.random_orders_enabled_flag()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomOrdersEnabled, *value);
  }

  if (const auto& value = patch.random_aggressive_qty_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAggressiveQtyMinimum, *value);
  }

  if (const auto& value = patch.random_aggressive_qty_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAggressiveQtyMaximum, *value);
  }

  if (const auto& value = patch.random_aggressive_amt_minimum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAggressiveAmtMinimum, *value);
  }

  if (const auto& value = patch.random_aggressive_amt_maximum()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::RandomAggressiveAmtMaximum, *value);
  }
}

template <typename Unmarshaller>
inline auto ListingPatchWriter<Unmarshaller>::write(Listing::Patch& patch)
    -> void {
  std::string symbol{};
  static_assert(can_unmarshall_v<decltype(symbol)>);
  if (unmarshaller_(Attribute::Symbol, symbol)) {
    patch.with_symbol(std::move(symbol));
  }

  std::string venue_id{};
  static_assert(can_unmarshall_v<decltype(venue_id)>);
  if (unmarshaller_(Attribute::VenueId, venue_id)) {
    patch.with_venue_id(std::move(venue_id));
  }

  std::string security_type{};
  static_assert(can_unmarshall_v<decltype(security_type)>);
  if (unmarshaller_(Attribute::SecurityType, security_type)) {
    patch.with_security_type(std::move(security_type));
  }

  std::string price_currency{};
  static_assert(can_unmarshall_v<decltype(price_currency)>);
  if (unmarshaller_(Attribute::PriceCurrency, price_currency)) {
    patch.with_price_currency(std::move(price_currency));
  }

  std::string fx_base_currency{};
  static_assert(can_unmarshall_v<decltype(fx_base_currency)>);
  if (unmarshaller_(Attribute::FxBaseCurrency, fx_base_currency)) {
    patch.with_fx_base_currency(std::move(fx_base_currency));
  }

  std::string instr_symbol{};
  static_assert(can_unmarshall_v<decltype(instr_symbol)>);
  if (unmarshaller_(Attribute::InstrSymbol, instr_symbol)) {
    patch.with_instr_symbol(std::move(instr_symbol));
  }

  double qty_minimum{};
  static_assert(can_unmarshall_v<decltype(qty_minimum)>);
  if (unmarshaller_(Attribute::QtyMinimum, qty_minimum)) {
    patch.with_qty_minimum(qty_minimum);
  }

  double qty_maximum{};
  static_assert(can_unmarshall_v<decltype(qty_maximum)>);
  if (unmarshaller_(Attribute::QtyMaximum, qty_maximum)) {
    patch.with_qty_maximum(qty_maximum);
  }

  double qty_multiple{};
  static_assert(can_unmarshall_v<decltype(qty_multiple)>);
  if (unmarshaller_(Attribute::QtyMultiple, qty_multiple)) {
    patch.with_qty_multiple(qty_multiple);
  }

  double price_tick_size{};
  static_assert(can_unmarshall_v<decltype(price_tick_size)>);
  if (unmarshaller_(Attribute::PriceTickSize, price_tick_size)) {
    patch.with_price_tick_size(price_tick_size);
  }

  bool enabled{};
  static_assert(can_unmarshall_v<decltype(enabled)>);
  if (unmarshaller_(Attribute::Enabled, enabled)) {
    patch.with_enabled_flag(enabled);
  }

  double random_qty_maximum{};
  static_assert(can_unmarshall_v<decltype(random_qty_maximum)>);
  if (unmarshaller_(Attribute::RandomQtyMaximum, random_qty_maximum)) {
    patch.with_random_qty_maximum(random_qty_maximum);
  }

  std::uint32_t random_depth_levels{};
  static_assert(can_unmarshall_v<decltype(random_depth_levels)>);
  if (unmarshaller_(Attribute::RandomDepthLevels, random_depth_levels)) {
    patch.with_random_depth_levels(random_depth_levels);
  }

  double random_orders_spread{};
  static_assert(can_unmarshall_v<decltype(random_orders_spread)>);
  if (unmarshaller_(Attribute::RandomOrdersSpread, random_orders_spread)) {
    patch.with_random_orders_spread(random_orders_spread);
  }

  std::uint32_t random_orders_rate{};
  static_assert(can_unmarshall_v<decltype(random_orders_rate)>);
  if (unmarshaller_(Attribute::RandomOrdersRate, random_orders_rate)) {
    patch.with_random_orders_rate(random_orders_rate);
  }

  std::uint32_t random_tick_range{};
  static_assert(can_unmarshall_v<decltype(random_tick_range)>);
  if (unmarshaller_(Attribute::RandomTickRange, random_tick_range)) {
    patch.with_random_tick_range(random_tick_range);
  }

  std::string security_exchange{};
  static_assert(can_unmarshall_v<decltype(security_exchange)>);
  if (unmarshaller_(Attribute::SecurityExchange, security_exchange)) {
    patch.with_security_exchange(std::move(security_exchange));
  }

  std::string party_id{};
  static_assert(can_unmarshall_v<decltype(party_id)>);
  if (unmarshaller_(Attribute::PartyId, party_id)) {
    patch.with_party_id(std::move(party_id));
  }

  std::string party_role{};
  static_assert(can_unmarshall_v<decltype(party_role)>);
  if (unmarshaller_(Attribute::PartyRole, party_role)) {
    patch.with_party_role(std::move(party_role));
  }

  std::string cusip_id{};
  static_assert(can_unmarshall_v<decltype(cusip_id)>);
  if (unmarshaller_(Attribute::CusipId, cusip_id)) {
    patch.with_cusip_id(std::move(cusip_id));
  }

  std::string sedol_id{};
  static_assert(can_unmarshall_v<decltype(sedol_id)>);
  if (unmarshaller_(Attribute::SedolId, sedol_id)) {
    patch.with_sedol_id(std::move(sedol_id));
  }

  std::string isin_id{};
  static_assert(can_unmarshall_v<decltype(isin_id)>);
  if (unmarshaller_(Attribute::IsinId, isin_id)) {
    patch.with_isin_id(std::move(isin_id));
  }

  std::string ric_id{};
  static_assert(can_unmarshall_v<decltype(ric_id)>);
  if (unmarshaller_(Attribute::RicId, ric_id)) {
    patch.with_ric_id(std::move(ric_id));
  }

  std::string exchange_symbol_id{};
  static_assert(can_unmarshall_v<decltype(exchange_symbol_id)>);
  if (unmarshaller_(Attribute::ExchangeSymbolId, exchange_symbol_id)) {
    patch.with_exchange_symbol_id(std::move(exchange_symbol_id));
  }

  std::string bloomberg_symbol_id{};
  static_assert(can_unmarshall_v<decltype(bloomberg_symbol_id)>);
  if (unmarshaller_(Attribute::BloombergSymbolId, bloomberg_symbol_id)) {
    patch.with_bloomberg_symbol_id(std::move(bloomberg_symbol_id));
  }

  double random_qty_minimum{};
  static_assert(can_unmarshall_v<decltype(random_qty_minimum)>);
  if (unmarshaller_(Attribute::RandomQtyMinimum, random_qty_minimum)) {
    patch.with_random_qty_minimum(random_qty_minimum);
  }

  double random_amt_minimum{};
  static_assert(can_unmarshall_v<decltype(random_amt_minimum)>);
  if (unmarshaller_(Attribute::RandomAmtMinimum, random_amt_minimum)) {
    patch.with_random_amt_minimum(random_amt_minimum);
  }

  double random_amt_maximum{};
  static_assert(can_unmarshall_v<decltype(random_amt_maximum)>);
  if (unmarshaller_(Attribute::RandomAmtMaximum, random_amt_maximum)) {
    patch.with_random_amt_maximum(random_amt_maximum);
  }

  bool random_orders_enabled{};
  static_assert(can_unmarshall_v<decltype(random_orders_enabled)>);
  if (unmarshaller_(Attribute::RandomOrdersEnabled, random_orders_enabled)) {
    patch.with_random_orders_enabled_flag(random_orders_enabled);
  }

  double random_aggressive_qty_minimum{};
  static_assert(can_unmarshall_v<decltype(random_aggressive_qty_minimum)>);
  if (unmarshaller_(Attribute::RandomAggressiveQtyMinimum,
                    random_aggressive_qty_minimum)) {
    patch.with_random_aggressive_qty_minimum(random_aggressive_qty_minimum);
  }

  double random_aggressive_qty_maximum{};
  static_assert(can_unmarshall_v<decltype(random_aggressive_qty_maximum)>);
  if (unmarshaller_(Attribute::RandomAggressiveQtyMaximum,
                    random_aggressive_qty_maximum)) {
    patch.with_random_aggressive_qty_maximum(random_aggressive_qty_maximum);
  }

  double random_aggressive_amt_minimum{};
  static_assert(can_unmarshall_v<decltype(random_aggressive_amt_minimum)>);
  if (unmarshaller_(Attribute::RandomAggressiveAmtMinimum,
                    random_aggressive_amt_minimum)) {
    patch.with_random_aggressive_amt_minimum(random_aggressive_amt_minimum);
  }

  double random_aggressive_amt_maximum{};
  static_assert(can_unmarshall_v<decltype(random_aggressive_amt_maximum)>);
  if (unmarshaller_(Attribute::RandomAggressiveAmtMaximum,
                    random_aggressive_amt_maximum)) {
    patch.with_random_aggressive_amt_maximum(random_aggressive_amt_maximum);
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_PROJECT_DATA_LAYER_API_COMMON_INSPECTORS_LISTING_HPP_
