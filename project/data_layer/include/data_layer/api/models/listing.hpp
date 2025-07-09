#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_LISTING_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_LISTING_HPP_

#include <cstdint>
#include <optional>
#include <string>

#include "data_layer/api/predicate/definitions.hpp"

namespace simulator::data_layer {

class Listing {
 public:
  class Patch;

  using Predicate = predicate::Expression<Listing>;

  enum class Attribute {
    ListingId,
    Symbol,
    VenueId,
    SecurityType,
    PriceCurrency,
    FxBaseCurrency,
    InstrSymbol,
    QtyMinimum,
    QtyMaximum,
    QtyMultiple,
    PriceTickSize,
    Enabled,
    RandomQtyMaximum,
    RandomDepthLevels,
    RandomOrdersSpread,
    RandomOrdersRate,
    RandomTickRange,
    SecurityExchange,
    PartyId,
    PartyRole,
    CusipId,
    SedolId,
    IsinId,
    RicId,
    ExchangeSymbolId,
    BloombergSymbolId,
    RandomQtyMinimum,
    RandomAmtMinimum,
    RandomAmtMaximum,
    RandomOrdersEnabled,
    RandomAggressiveQtyMinimum,
    RandomAggressiveQtyMaximum,
    RandomAggressiveAmtMinimum,
    RandomAggressiveAmtMaximum
  };

  [[nodiscard]]
  static auto create(Listing::Patch snapshot, std::uint64_t id) -> Listing;

  [[nodiscard]]
  auto listing_id() const noexcept -> std::uint64_t;

  [[nodiscard]]
  auto symbol() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto venue_id() const noexcept -> const std::string&;

  [[nodiscard]]
  auto security_type() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto price_currency() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto fx_base_currency() const noexcept
      -> const std::optional<std::string>&;

  [[nodiscard]]
  auto instr_symbol() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto qty_minimum() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto qty_maximum() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto qty_multiple() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto price_tick_size() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto enabled_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto random_qty_maximum() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto random_depth_levels() const noexcept -> std::optional<std::uint32_t>;

  [[nodiscard]]
  auto random_orders_spread() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto random_orders_rate() const noexcept -> std::optional<std::uint32_t>;

  [[nodiscard]]
  auto random_tick_range() const noexcept -> std::optional<std::uint32_t>;

  [[nodiscard]]
  auto security_exchange() const noexcept
      -> const std::optional<std::string>&;

  [[nodiscard]]
  auto party_id() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto party_role() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto cusip_id() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto sedol_id() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto isin_id() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto ric_id() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto exchange_symbol_id() const noexcept
      -> const std::optional<std::string>&;

  [[nodiscard]]
  auto bloomberg_symbol_id() const noexcept
      -> const std::optional<std::string>&;

  [[nodiscard]]
  auto random_qty_minimum() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto random_amt_minimum() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto random_amt_maximum() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto random_orders_enabled_flag() const noexcept -> std::optional<bool>;

  [[nodiscard]]
  auto random_aggressive_qty_minimum() const noexcept
      -> std::optional<double>;

  [[nodiscard]]
  auto random_aggressive_qty_maximum() const noexcept
      -> std::optional<double>;

  [[nodiscard]]
  auto random_aggressive_amt_minimum() const noexcept
      -> std::optional<double>;

  [[nodiscard]]
  auto random_aggressive_amt_maximum() const noexcept
      -> std::optional<double>;

 private:
  Listing() = default;

  std::optional<std::string> symbol_;
  std::optional<std::string> security_type_;
  std::optional<std::string> price_currency_;
  std::optional<std::string> fx_base_currency_;
  std::optional<std::string> instr_symbol_;
  std::optional<std::string> security_exchange_;
  std::optional<std::string> party_id_;
  std::optional<std::string> party_role_;
  std::optional<std::string> cusip_id_;
  std::optional<std::string> sedol_id_;
  std::optional<std::string> isin_id_;
  std::optional<std::string> ric_id_;
  std::optional<std::string> exchange_symbol_id_;
  std::optional<std::string> bloomberg_symbol_id_;

  std::string venue_id_;

  std::optional<double> qty_minimum_;
  std::optional<double> qty_maximum_;
  std::optional<double> qty_multiple_;
  std::optional<double> price_tick_size_;
  std::optional<double> random_qty_maximum_;
  std::optional<double> random_orders_spread_;
  std::optional<double> random_qty_minimum_;
  std::optional<double> random_amt_minimum_;
  std::optional<double> random_amt_maximum_;
  std::optional<double> random_aggressive_qty_minimum_;
  std::optional<double> random_aggressive_qty_maximum_;
  std::optional<double> random_aggressive_amt_minimum_;
  std::optional<double> random_aggressive_amt_maximum_;

  std::optional<std::uint32_t> random_depth_levels_;
  std::optional<std::uint32_t> random_orders_rate_;
  std::optional<std::uint32_t> random_tick_range_;

  std::uint64_t listing_id_{0};

  std::optional<bool> enabled_flag_;
  std::optional<bool> random_orders_enabled_flag_;
};

class Listing::Patch {
  friend class Listing;

 public:
  using Attribute = Listing::Attribute;

  [[nodiscard]]
  auto symbol() const noexcept -> const std::optional<std::string>&;
  auto with_symbol(std::string symbol) noexcept -> Patch&;

  [[nodiscard]]
  auto venue_id() const noexcept -> const std::optional<std::string>&;
  auto with_venue_id(std::string id) noexcept -> Patch&;

  [[nodiscard]]
  auto security_type() const noexcept -> const std::optional<std::string>&;
  auto with_security_type(std::string type) noexcept -> Patch&;

  [[nodiscard]]
  auto price_currency() const noexcept -> const std::optional<std::string>&;
  auto with_price_currency(std::string currency) noexcept -> Patch&;

  [[nodiscard]]
  auto fx_base_currency() const noexcept
      -> const std::optional<std::string>&;
  auto with_fx_base_currency(std::string currency) noexcept -> Patch&;

  [[nodiscard]]
  auto instr_symbol() const noexcept -> const std::optional<std::string>&;
  auto with_instr_symbol(std::string symbol) noexcept -> Patch&;

  [[nodiscard]]
  auto qty_minimum() const noexcept -> std::optional<double>;
  auto with_qty_minimum(double qty) noexcept -> Patch&;

  [[nodiscard]]
  auto qty_maximum() const noexcept -> std::optional<double>;
  auto with_qty_maximum(double qty) noexcept -> Patch&;

  [[nodiscard]]
  auto qty_multiple() const noexcept -> std::optional<double>;
  auto with_qty_multiple(double multiple) noexcept -> Patch&;

  [[nodiscard]]
  auto price_tick_size() const noexcept -> std::optional<double>;
  auto with_price_tick_size(double size) noexcept -> Patch&;

  [[nodiscard]]
  auto enabled_flag() const noexcept -> std::optional<bool>;
  auto with_enabled_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto random_qty_maximum() const noexcept -> std::optional<double>;
  auto with_random_qty_maximum(double qty) noexcept -> Patch&;

  [[nodiscard]]
  auto random_depth_levels() const noexcept -> std::optional<std::uint32_t>;
  auto with_random_depth_levels(std::uint32_t num_levels) noexcept -> Patch&;

  [[nodiscard]]
  auto random_orders_spread() const noexcept -> std::optional<double>;
  auto with_random_orders_spread(double spread) noexcept -> Patch&;

  [[nodiscard]]
  auto random_orders_rate() const noexcept -> std::optional<std::uint32_t>;
  auto with_random_orders_rate(std::uint32_t rate) noexcept -> Patch&;

  [[nodiscard]]
  auto random_tick_range() const noexcept -> std::optional<std::uint32_t>;
  auto with_random_tick_range(std::uint32_t range) noexcept -> Patch&;

  [[nodiscard]]
  auto security_exchange() const noexcept
      -> const std::optional<std::string>&;
  auto with_security_exchange(std::string exchange) noexcept -> Patch&;

  [[nodiscard]]
  auto party_id() const noexcept -> const std::optional<std::string>&;
  auto with_party_id(std::string id) noexcept -> Patch&;

  [[nodiscard]]
  auto party_role() const noexcept -> const std::optional<std::string>&;
  auto with_party_role(std::string role) noexcept -> Patch&;

  [[nodiscard]]
  auto cusip_id() const noexcept -> const std::optional<std::string>&;
  auto with_cusip_id(std::string id) noexcept -> Patch&;

  [[nodiscard]]
  auto sedol_id() const noexcept -> const std::optional<std::string>&;
  auto with_sedol_id(std::string id) noexcept -> Patch&;

  [[nodiscard]]
  auto isin_id() const noexcept -> const std::optional<std::string>&;
  auto with_isin_id(std::string id) noexcept -> Patch&;

  [[nodiscard]]
  auto ric_id() const noexcept -> const std::optional<std::string>&;
  auto with_ric_id(std::string id) noexcept -> Patch&;

  [[nodiscard]]
  auto exchange_symbol_id() const noexcept
      -> const std::optional<std::string>&;
  auto with_exchange_symbol_id(std::string id) noexcept -> Patch&;

  [[nodiscard]]
  auto bloomberg_symbol_id() const noexcept
      -> const std::optional<std::string>&;
  auto with_bloomberg_symbol_id(std::string id) noexcept -> Patch&;

  [[nodiscard]]
  auto random_qty_minimum() const noexcept -> std::optional<double>;
  auto with_random_qty_minimum(double qty) noexcept -> Patch&;

  [[nodiscard]]
  auto random_amt_minimum() const noexcept -> std::optional<double>;
  auto with_random_amt_minimum(double amount) noexcept -> Patch&;

  [[nodiscard]]
  auto random_amt_maximum() const noexcept -> std::optional<double>;
  auto with_random_amt_maximum(double amount) noexcept -> Patch&;

  [[nodiscard]]
  auto random_orders_enabled_flag() const noexcept -> std::optional<bool>;
  auto with_random_orders_enabled_flag(bool flag) noexcept -> Patch&;

  [[nodiscard]]
  auto random_aggressive_qty_minimum() const noexcept
      -> std::optional<double>;
  auto with_random_aggressive_qty_minimum(double qty) noexcept -> Patch&;

  [[nodiscard]]
  auto random_aggressive_qty_maximum() const noexcept
      -> std::optional<double>;
  auto with_random_aggressive_qty_maximum(double qty) noexcept -> Patch&;

  [[nodiscard]]
  auto random_aggressive_amt_minimum() const noexcept
      -> std::optional<double>;
  auto with_random_aggressive_amt_minimum(double amount) noexcept -> Patch&;

  [[nodiscard]]
  auto random_aggressive_amt_maximum() const noexcept
      -> std::optional<double>;
  auto with_random_aggressive_amt_maximum(double amount) noexcept -> Patch&;

 private:
  std::optional<std::string> symbol_;
  std::optional<std::string> venue_id_;
  std::optional<std::string> security_type_;
  std::optional<std::string> price_currency_;
  std::optional<std::string> fx_base_currency_;
  std::optional<std::string> instr_symbol_;
  std::optional<std::string> security_exchange_;
  std::optional<std::string> party_id_;
  std::optional<std::string> party_role_;
  std::optional<std::string> cusip_id_;
  std::optional<std::string> sedol_id_;
  std::optional<std::string> isin_id_;
  std::optional<std::string> ric_id_;
  std::optional<std::string> exchange_symbol_id_;
  std::optional<std::string> bloomberg_symbol_id_;

  std::optional<double> qty_minimum_;
  std::optional<double> qty_maximum_;
  std::optional<double> qty_multiple_;
  std::optional<double> price_tick_size_;
  std::optional<double> random_qty_maximum_;
  std::optional<double> random_orders_spread_;
  std::optional<double> random_qty_minimum_;
  std::optional<double> random_amt_minimum_;
  std::optional<double> random_amt_maximum_;
  std::optional<double> random_aggressive_qty_minimum_;
  std::optional<double> random_aggressive_qty_maximum_;
  std::optional<double> random_aggressive_amt_minimum_;
  std::optional<double> random_aggressive_amt_maximum_;

  std::optional<std::uint32_t> random_depth_levels_;
  std::optional<std::uint32_t> random_orders_rate_;
  std::optional<std::uint32_t> random_tick_range_;

  std::optional<bool> enabled_flag_;
  std::optional<bool> random_orders_enabled_flag_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_LISTING_HPP_
