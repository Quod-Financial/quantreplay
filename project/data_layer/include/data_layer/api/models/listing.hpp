#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_LISTING_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_LISTING_HPP_

#include <cstdint>
#include <optional>
#include <string>

#include "data_layer/api/models/patch_field.hpp"
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

  static constexpr bool DefaultEnabledFlag{true};
  static constexpr bool DefaultRandomOrdersEnabledFlag{true};

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
  auto fx_base_currency() const noexcept -> const std::optional<std::string>&;

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
  auto security_exchange() const noexcept -> const std::optional<std::string>&;

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
  auto exchange_symbol_id() const noexcept -> const std::optional<std::string>&;

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
  auto random_aggressive_qty_minimum() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto random_aggressive_qty_maximum() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto random_aggressive_amt_minimum() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto random_aggressive_amt_maximum() const noexcept -> std::optional<double>;

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
  auto security_type() const noexcept -> const PatchField<std::string>&;
  auto with_security_type(std::optional<std::string> type) noexcept -> Patch&;

  [[nodiscard]]
  auto price_currency() const noexcept -> const PatchField<std::string>&;
  auto with_price_currency(std::optional<std::string> currency) noexcept
      -> Patch&;

  [[nodiscard]]
  auto fx_base_currency() const noexcept -> const PatchField<std::string>&;
  auto with_fx_base_currency(std::optional<std::string> currency) noexcept
      -> Patch&;

  [[nodiscard]]
  auto instr_symbol() const noexcept -> const PatchField<std::string>&;
  auto with_instr_symbol(std::optional<std::string> symbol) noexcept -> Patch&;

  [[nodiscard]]
  auto qty_minimum() const noexcept -> PatchField<double>;
  auto with_qty_minimum(std::optional<double> qty) noexcept -> Patch&;

  [[nodiscard]]
  auto qty_maximum() const noexcept -> PatchField<double>;
  auto with_qty_maximum(std::optional<double> qty) noexcept -> Patch&;

  [[nodiscard]]
  auto qty_multiple() const noexcept -> PatchField<double>;
  auto with_qty_multiple(std::optional<double> multiple) noexcept -> Patch&;

  [[nodiscard]]
  auto price_tick_size() const noexcept -> PatchField<double>;
  auto with_price_tick_size(std::optional<double> size) noexcept -> Patch&;

  [[nodiscard]]
  auto enabled_flag() const noexcept -> PatchField<bool>;
  auto with_enabled_flag(std::optional<bool> flag) noexcept -> Patch&;

  [[nodiscard]]
  auto random_qty_maximum() const noexcept -> PatchField<double>;
  auto with_random_qty_maximum(std::optional<double> qty) noexcept -> Patch&;

  [[nodiscard]]
  auto random_depth_levels() const noexcept -> PatchField<std::uint32_t>;
  auto with_random_depth_levels(std::optional<uint32_t> num_levels) noexcept
      -> Patch&;

  [[nodiscard]]
  auto random_orders_spread() const noexcept -> PatchField<double>;
  auto with_random_orders_spread(std::optional<double> spread) noexcept
      -> Patch&;

  [[nodiscard]]
  auto random_orders_rate() const noexcept -> PatchField<std::uint32_t>;
  auto with_random_orders_rate(std::optional<uint32_t> rate) noexcept -> Patch&;

  [[nodiscard]]
  auto random_tick_range() const noexcept -> PatchField<std::uint32_t>;
  auto with_random_tick_range(std::optional<uint32_t> range) noexcept -> Patch&;

  [[nodiscard]]
  auto security_exchange() const noexcept -> const PatchField<std::string>&;
  auto with_security_exchange(std::optional<std::string> exchange) noexcept
      -> Patch&;

  [[nodiscard]]
  auto party_id() const noexcept -> const PatchField<std::string>&;
  auto with_party_id(std::optional<std::string> id) noexcept -> Patch&;

  [[nodiscard]]
  auto party_role() const noexcept -> const PatchField<std::string>&;
  auto with_party_role(std::optional<std::string> role) noexcept -> Patch&;

  [[nodiscard]]
  auto cusip_id() const noexcept -> const PatchField<std::string>&;
  auto with_cusip_id(std::optional<std::string> id) noexcept -> Patch&;

  [[nodiscard]]
  auto sedol_id() const noexcept -> const PatchField<std::string>&;
  auto with_sedol_id(std::optional<std::string> id) noexcept -> Patch&;

  [[nodiscard]]
  auto isin_id() const noexcept -> const PatchField<std::string>&;
  auto with_isin_id(std::optional<std::string> id) noexcept -> Patch&;

  [[nodiscard]]
  auto ric_id() const noexcept -> const PatchField<std::string>&;
  auto with_ric_id(std::optional<std::string> id) noexcept -> Patch&;

  [[nodiscard]]
  auto exchange_symbol_id() const noexcept -> const PatchField<std::string>&;
  auto with_exchange_symbol_id(std::optional<std::string> id) noexcept
      -> Patch&;

  [[nodiscard]]
  auto bloomberg_symbol_id() const noexcept -> const PatchField<std::string>&;
  auto with_bloomberg_symbol_id(std::optional<std::string> id) noexcept
      -> Patch&;

  [[nodiscard]]
  auto random_qty_minimum() const noexcept -> PatchField<double>;
  auto with_random_qty_minimum(std::optional<double> qty) noexcept -> Patch&;

  [[nodiscard]]
  auto random_amt_minimum() const noexcept -> PatchField<double>;
  auto with_random_amt_minimum(std::optional<double> amount) noexcept -> Patch&;

  [[nodiscard]]
  auto random_amt_maximum() const noexcept -> PatchField<double>;
  auto with_random_amt_maximum(std::optional<double> amount) noexcept -> Patch&;

  [[nodiscard]]
  auto random_orders_enabled_flag() const noexcept -> PatchField<bool>;
  auto with_random_orders_enabled_flag(std::optional<bool> flag) noexcept
      -> Patch&;

  [[nodiscard]]
  auto random_aggressive_qty_minimum() const noexcept -> PatchField<double>;
  auto with_random_aggressive_qty_minimum(std::optional<double> qty) noexcept
      -> Patch&;

  [[nodiscard]]
  auto random_aggressive_qty_maximum() const noexcept -> PatchField<double>;
  auto with_random_aggressive_qty_maximum(std::optional<double> qty) noexcept
      -> Patch&;

  [[nodiscard]]
  auto random_aggressive_amt_minimum() const noexcept -> PatchField<double>;
  auto with_random_aggressive_amt_minimum(std::optional<double> amount) noexcept
      -> Patch&;

  [[nodiscard]]
  auto random_aggressive_amt_maximum() const noexcept -> PatchField<double>;
  auto with_random_aggressive_amt_maximum(std::optional<double> amount) noexcept
      -> Patch&;

 private:
  std::optional<std::string> symbol_;
  std::optional<std::string> venue_id_;
  PatchField<std::string> security_type_;
  PatchField<std::string> price_currency_;
  PatchField<std::string> fx_base_currency_;
  PatchField<std::string> instr_symbol_;
  PatchField<std::string> security_exchange_;
  PatchField<std::string> party_id_;
  PatchField<std::string> party_role_;
  PatchField<std::string> cusip_id_;
  PatchField<std::string> sedol_id_;
  PatchField<std::string> isin_id_;
  PatchField<std::string> ric_id_;
  PatchField<std::string> exchange_symbol_id_;
  PatchField<std::string> bloomberg_symbol_id_;

  PatchField<double> qty_minimum_;
  PatchField<double> qty_maximum_;
  PatchField<double> qty_multiple_;
  PatchField<double> price_tick_size_;
  PatchField<double> random_qty_maximum_;
  PatchField<double> random_orders_spread_;
  PatchField<double> random_qty_minimum_;
  PatchField<double> random_amt_minimum_;
  PatchField<double> random_amt_maximum_;
  PatchField<double> random_aggressive_qty_minimum_;
  PatchField<double> random_aggressive_qty_maximum_;
  PatchField<double> random_aggressive_amt_minimum_;
  PatchField<double> random_aggressive_amt_maximum_;

  PatchField<std::uint32_t> random_depth_levels_;
  PatchField<std::uint32_t> random_orders_rate_;
  PatchField<std::uint32_t> random_tick_range_;

  PatchField<bool> enabled_flag_;
  PatchField<bool> random_orders_enabled_flag_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_LISTING_HPP_
