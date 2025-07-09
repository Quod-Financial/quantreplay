#include "api/models/listing.hpp"

#include "ih/common/exceptions.hpp"

namespace simulator::data_layer {

// NOLINTNEXTLINE: No way to split it, it's an atomic model creation operation
auto Listing::create(Listing::Patch snapshot, std::uint64_t id) -> Listing {
  if (!snapshot.venue_id_.has_value()) {
    throw RequiredAttributeMissing("Listing", "VenueID");
  }

  Listing listing{};
  listing.listing_id_ = id;

#define SIM_ASSIGN_FIELD(LISTING_FIELD, SNAPSHOT_FIELD)            \
  if (snapshot.SNAPSHOT_FIELD.has_value()) {                       \
    listing.LISTING_FIELD = std::move(*(snapshot.SNAPSHOT_FIELD)); \
  }                                                                \
  void(0)

  SIM_ASSIGN_FIELD(symbol_, symbol_);
  SIM_ASSIGN_FIELD(venue_id_, venue_id_);
  SIM_ASSIGN_FIELD(security_type_, security_type_);
  SIM_ASSIGN_FIELD(price_currency_, price_currency_);
  SIM_ASSIGN_FIELD(fx_base_currency_, fx_base_currency_);
  SIM_ASSIGN_FIELD(instr_symbol_, instr_symbol_);
  SIM_ASSIGN_FIELD(security_exchange_, security_exchange_);
  SIM_ASSIGN_FIELD(party_id_, party_id_);
  SIM_ASSIGN_FIELD(party_role_, party_role_);
  SIM_ASSIGN_FIELD(cusip_id_, cusip_id_);
  SIM_ASSIGN_FIELD(sedol_id_, sedol_id_);
  SIM_ASSIGN_FIELD(isin_id_, isin_id_);
  SIM_ASSIGN_FIELD(ric_id_, ric_id_);
  SIM_ASSIGN_FIELD(exchange_symbol_id_, exchange_symbol_id_);
  SIM_ASSIGN_FIELD(bloomberg_symbol_id_, bloomberg_symbol_id_);

  SIM_ASSIGN_FIELD(qty_minimum_, qty_minimum_);
  SIM_ASSIGN_FIELD(qty_maximum_, qty_maximum_);
  SIM_ASSIGN_FIELD(qty_multiple_, qty_multiple_);
  SIM_ASSIGN_FIELD(price_tick_size_, price_tick_size_);
  SIM_ASSIGN_FIELD(random_qty_maximum_, random_qty_maximum_);
  SIM_ASSIGN_FIELD(random_orders_spread_, random_orders_spread_);
  SIM_ASSIGN_FIELD(random_qty_minimum_, random_qty_minimum_);
  SIM_ASSIGN_FIELD(random_amt_minimum_, random_amt_minimum_);
  SIM_ASSIGN_FIELD(random_amt_maximum_, random_amt_maximum_);
  SIM_ASSIGN_FIELD(random_aggressive_qty_minimum_,
                   random_aggressive_qty_minimum_);
  SIM_ASSIGN_FIELD(random_aggressive_qty_maximum_,
                   random_aggressive_qty_maximum_);
  SIM_ASSIGN_FIELD(random_aggressive_amt_minimum_,
                   random_aggressive_amt_minimum_);
  SIM_ASSIGN_FIELD(random_aggressive_amt_maximum_,
                   random_aggressive_amt_maximum_);

  SIM_ASSIGN_FIELD(random_depth_levels_, random_depth_levels_);
  SIM_ASSIGN_FIELD(random_orders_rate_, random_orders_rate_);
  SIM_ASSIGN_FIELD(random_tick_range_, random_tick_range_);

  SIM_ASSIGN_FIELD(enabled_flag_, enabled_flag_);
  SIM_ASSIGN_FIELD(random_orders_enabled_flag_, random_orders_enabled_flag_);

#undef SIM_ASSIGN_FIELD
  return listing;
}

auto Listing::listing_id() const noexcept -> std::uint64_t {
  return listing_id_;
}

auto Listing::symbol() const noexcept -> const std::optional<std::string>& {
  return symbol_;
}

auto Listing::venue_id() const noexcept -> const std::string& {
  return venue_id_;
}

auto Listing::security_type() const noexcept
    -> const std::optional<std::string>& {
  return security_type_;
}

auto Listing::price_currency() const noexcept
    -> const std::optional<std::string>& {
  return price_currency_;
}

auto Listing::fx_base_currency() const noexcept
    -> const std::optional<std::string>& {
  return fx_base_currency_;
}

auto Listing::instr_symbol() const noexcept
    -> const std::optional<std::string>& {
  return instr_symbol_;
}

auto Listing::qty_minimum() const noexcept -> std::optional<double> {
  return qty_minimum_;
}

auto Listing::qty_maximum() const noexcept -> std::optional<double> {
  return qty_maximum_;
}

auto Listing::qty_multiple() const noexcept -> std::optional<double> {
  return qty_multiple_;
}

auto Listing::price_tick_size() const noexcept -> std::optional<double> {
  return price_tick_size_;
}

auto Listing::enabled_flag() const noexcept -> std::optional<bool> {
  return enabled_flag_;
}

auto Listing::random_qty_maximum() const noexcept -> std::optional<double> {
  return random_qty_maximum_;
}

auto Listing::random_depth_levels() const noexcept
    -> std::optional<std::uint32_t> {
  return random_depth_levels_;
}

auto Listing::random_orders_spread() const noexcept -> std::optional<double> {
  return random_orders_spread_;
}

auto Listing::random_orders_rate() const noexcept
    -> std::optional<std::uint32_t> {
  return random_orders_rate_;
}

auto Listing::random_tick_range() const noexcept
    -> std::optional<std::uint32_t> {
  return random_tick_range_;
}

auto Listing::security_exchange() const noexcept
    -> const std::optional<std::string>& {
  return security_exchange_;
}

auto Listing::party_id() const noexcept -> const std::optional<std::string>& {
  return party_id_;
}

auto Listing::party_role() const noexcept -> const std::optional<std::string>& {
  return party_role_;
}

auto Listing::cusip_id() const noexcept -> const std::optional<std::string>& {
  return cusip_id_;
}

auto Listing::sedol_id() const noexcept -> const std::optional<std::string>& {
  return sedol_id_;
}

auto Listing::isin_id() const noexcept -> const std::optional<std::string>& {
  return isin_id_;
}

auto Listing::ric_id() const noexcept -> const std::optional<std::string>& {
  return ric_id_;
}

auto Listing::exchange_symbol_id() const noexcept
    -> const std::optional<std::string>& {
  return exchange_symbol_id_;
}

auto Listing::bloomberg_symbol_id() const noexcept
    -> const std::optional<std::string>& {
  return bloomberg_symbol_id_;
}

auto Listing::random_qty_minimum() const noexcept -> std::optional<double> {
  return random_qty_minimum_;
}

auto Listing::random_amt_minimum() const noexcept -> std::optional<double> {
  return random_amt_minimum_;
}

auto Listing::random_amt_maximum() const noexcept -> std::optional<double> {
  return random_amt_maximum_;
}

auto Listing::random_orders_enabled_flag() const noexcept
    -> std::optional<bool> {
  return random_orders_enabled_flag_;
}

auto Listing::random_aggressive_qty_minimum() const noexcept
    -> std::optional<double> {
  return random_aggressive_qty_minimum_;
}

auto Listing::random_aggressive_qty_maximum() const noexcept
    -> std::optional<double> {
  return random_aggressive_qty_maximum_;
}

auto Listing::random_aggressive_amt_minimum() const noexcept
    -> std::optional<double> {
  return random_aggressive_amt_minimum_;
}

auto Listing::random_aggressive_amt_maximum() const noexcept
    -> std::optional<double> {
  return random_aggressive_amt_maximum_;
}

auto Listing::Patch::symbol() const noexcept
    -> const std::optional<std::string>& {
  return symbol_;
}

auto Listing::Patch::with_symbol(std::string symbol) noexcept -> Patch& {
  symbol_ = std::move(symbol);
  return *this;
}

auto Listing::Patch::venue_id() const noexcept
    -> const std::optional<std::string>& {
  return venue_id_;
}

auto Listing::Patch::with_venue_id(std::string id) noexcept -> Patch& {
  venue_id_ = std::move(id);
  return *this;
}

auto Listing::Patch::security_type() const noexcept
    -> const std::optional<std::string>& {
  return security_type_;
}

auto Listing::Patch::with_security_type(std::string type) noexcept -> Patch& {
  security_type_ = std::move(type);
  return *this;
}

auto Listing::Patch::price_currency() const noexcept
    -> const std::optional<std::string>& {
  return price_currency_;
}

auto Listing::Patch::with_price_currency(std::string currency) noexcept
    -> Patch& {
  price_currency_ = std::move(currency);
  return *this;
}

auto Listing::Patch::fx_base_currency() const noexcept
    -> const std::optional<std::string>& {
  return fx_base_currency_;
}

auto Listing::Patch::with_fx_base_currency(std::string currency) noexcept
    -> Patch& {
  fx_base_currency_ = std::move(currency);
  return *this;
}

auto Listing::Patch::instr_symbol() const noexcept
    -> const std::optional<std::string>& {
  return instr_symbol_;
}

auto Listing::Patch::with_instr_symbol(std::string symbol) noexcept -> Patch& {
  instr_symbol_ = std::move(symbol);
  return *this;
}

auto Listing::Patch::qty_minimum() const noexcept -> std::optional<double> {
  return qty_minimum_;
}

auto Listing::Patch::with_qty_minimum(double qty) noexcept -> Patch& {
  qty_minimum_ = qty;
  return *this;
}

auto Listing::Patch::qty_maximum() const noexcept -> std::optional<double> {
  return qty_maximum_;
}

auto Listing::Patch::with_qty_maximum(double qty) noexcept -> Patch& {
  qty_maximum_ = qty;
  return *this;
}

auto Listing::Patch::qty_multiple() const noexcept -> std::optional<double> {
  return qty_multiple_;
}

auto Listing::Patch::with_qty_multiple(double multiple) noexcept -> Patch& {
  qty_multiple_ = multiple;
  return *this;
}

auto Listing::Patch::price_tick_size() const noexcept -> std::optional<double> {
  return price_tick_size_;
}

auto Listing::Patch::with_price_tick_size(double size) noexcept -> Patch& {
  price_tick_size_ = size;
  return *this;
}

auto Listing::Patch::enabled_flag() const noexcept -> std::optional<bool> {
  return enabled_flag_;
}

auto Listing::Patch::with_enabled_flag(bool flag) noexcept -> Patch& {
  enabled_flag_ = flag;
  return *this;
}

auto Listing::Patch::random_qty_maximum() const noexcept
    -> std::optional<double> {
  return random_qty_maximum_;
}

auto Listing::Patch::with_random_qty_maximum(double qty) noexcept -> Patch& {
  random_qty_maximum_ = qty;
  return *this;
}

auto Listing::Patch::random_depth_levels() const noexcept
    -> std::optional<std::uint32_t> {
  return random_depth_levels_;
}

auto Listing::Patch::with_random_depth_levels(std::uint32_t num_levels) noexcept
    -> Patch& {
  random_depth_levels_ = num_levels;
  return *this;
}

auto Listing::Patch::random_orders_spread() const noexcept
    -> std::optional<double> {
  return random_orders_spread_;
}

auto Listing::Patch::with_random_orders_spread(double spread) noexcept
    -> Patch& {
  random_orders_spread_ = spread;
  return *this;
}

auto Listing::Patch::random_orders_rate() const noexcept
    -> std::optional<std::uint32_t> {
  return random_orders_rate_;
}

auto Listing::Patch::with_random_orders_rate(std::uint32_t rate) noexcept
    -> Patch& {
  random_orders_rate_ = rate;
  return *this;
}

auto Listing::Patch::random_tick_range() const noexcept
    -> std::optional<std::uint32_t> {
  return random_tick_range_;
}

auto Listing::Patch::with_random_tick_range(std::uint32_t range) noexcept
    -> Patch& {
  random_tick_range_ = range;
  return *this;
}

auto Listing::Patch::security_exchange() const noexcept
    -> const std::optional<std::string>& {
  return security_exchange_;
}

auto Listing::Patch::with_security_exchange(std::string exchange) noexcept
    -> Patch& {
  security_exchange_ = std::move(exchange);
  return *this;
}

auto Listing::Patch::party_id() const noexcept
    -> const std::optional<std::string>& {
  return party_id_;
}

auto Listing::Patch::with_party_id(std::string id) noexcept -> Patch& {
  party_id_ = std::move(id);
  return *this;
}

auto Listing::Patch::party_role() const noexcept
    -> const std::optional<std::string>& {
  return party_role_;
}

auto Listing::Patch::with_party_role(std::string role) noexcept -> Patch& {
  party_role_ = std::move(role);
  return *this;
}

auto Listing::Patch::cusip_id() const noexcept
    -> const std::optional<std::string>& {
  return cusip_id_;
}

auto Listing::Patch::with_cusip_id(std::string id) noexcept -> Patch& {
  cusip_id_ = std::move(id);
  return *this;
}

auto Listing::Patch::sedol_id() const noexcept
    -> const std::optional<std::string>& {
  return sedol_id_;
}

auto Listing::Patch::with_sedol_id(std::string id) noexcept -> Patch& {
  sedol_id_ = std::move(id);
  return *this;
}

auto Listing::Patch::isin_id() const noexcept
    -> const std::optional<std::string>& {
  return isin_id_;
}

auto Listing::Patch::with_isin_id(std::string id) noexcept -> Patch& {
  isin_id_ = std::move(id);
  return *this;
}

auto Listing::Patch::ric_id() const noexcept
    -> const std::optional<std::string>& {
  return ric_id_;
}

auto Listing::Patch::with_ric_id(std::string id) noexcept -> Patch& {
  ric_id_ = std::move(id);
  return *this;
}

auto Listing::Patch::exchange_symbol_id() const noexcept
    -> const std::optional<std::string>& {
  return exchange_symbol_id_;
}

auto Listing::Patch::with_exchange_symbol_id(std::string id) noexcept
    -> Patch& {
  exchange_symbol_id_ = std::move(id);
  return *this;
}

auto Listing::Patch::bloomberg_symbol_id() const noexcept
    -> const std::optional<std::string>& {
  return bloomberg_symbol_id_;
}

auto Listing::Patch::with_bloomberg_symbol_id(std::string id) noexcept
    -> Patch& {
  bloomberg_symbol_id_ = std::move(id);
  return *this;
}

auto Listing::Patch::random_qty_minimum() const noexcept
    -> std::optional<double> {
  return random_qty_minimum_;
}

auto Listing::Patch::with_random_qty_minimum(double qty) noexcept -> Patch& {
  random_qty_minimum_ = qty;
  return *this;
}

auto Listing::Patch::random_amt_minimum() const noexcept
    -> std::optional<double> {
  return random_amt_minimum_;
}

auto Listing::Patch::with_random_amt_minimum(double amount) noexcept -> Patch& {
  random_amt_minimum_ = amount;
  return *this;
}

auto Listing::Patch::random_amt_maximum() const noexcept
    -> std::optional<double> {
  return random_amt_maximum_;
}

auto Listing::Patch::with_random_amt_maximum(double amount) noexcept -> Patch& {
  random_amt_maximum_ = amount;
  return *this;
}

auto Listing::Patch::random_orders_enabled_flag() const noexcept
    -> std::optional<bool> {
  return random_orders_enabled_flag_;
}

auto Listing::Patch::with_random_orders_enabled_flag(bool flag) noexcept
    -> Patch& {
  random_orders_enabled_flag_ = flag;
  return *this;
}

auto Listing::Patch::random_aggressive_qty_minimum() const noexcept
    -> std::optional<double> {
  return random_aggressive_qty_minimum_;
}

auto Listing::Patch::with_random_aggressive_qty_minimum(double qty) noexcept
    -> Patch& {
  random_aggressive_qty_minimum_ = qty;
  return *this;
}

auto Listing::Patch::random_aggressive_qty_maximum() const noexcept
    -> std::optional<double> {
  return random_aggressive_qty_maximum_;
}

auto Listing::Patch::with_random_aggressive_qty_maximum(double qty) noexcept
    -> Patch& {
  random_aggressive_qty_maximum_ = qty;
  return *this;
}

auto Listing::Patch::random_aggressive_amt_minimum() const noexcept
    -> std::optional<double> {
  return random_aggressive_amt_minimum_;
}

auto Listing::Patch::with_random_aggressive_amt_minimum(double amount) noexcept
    -> Patch& {
  random_aggressive_amt_minimum_ = amount;
  return *this;
}

auto Listing::Patch::random_aggressive_amt_maximum() const noexcept
    -> std::optional<double> {
  return random_aggressive_amt_maximum_;
}

auto Listing::Patch::with_random_aggressive_amt_maximum(double amount) noexcept
    -> Patch& {
  random_aggressive_amt_maximum_ = amount;
  return *this;
}

}  // namespace simulator::data_layer
