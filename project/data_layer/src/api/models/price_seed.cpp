#include "api/models/price_seed.hpp"

#include <utility>

namespace simulator::data_layer {

auto PriceSeed::create(PriceSeed::Patch snapshot, std::uint64_t id) noexcept
    -> PriceSeed {
  PriceSeed seed{};
  seed.price_seed_id_ = id;

#define SIM_ASSIGN_FIELD(SEED_FIELD, SNAPSHOT_FIELD)         \
  if (snapshot.SNAPSHOT_FIELD.has_value()) {                 \
    seed.SEED_FIELD = std::move(*(snapshot.SNAPSHOT_FIELD)); \
  }                                                          \
  void(0)

  SIM_ASSIGN_FIELD(symbol_, symbol_);
  SIM_ASSIGN_FIELD(security_type_, security_type_);
  SIM_ASSIGN_FIELD(price_currency_, price_currency_);
  SIM_ASSIGN_FIELD(security_id_, security_id_);
  SIM_ASSIGN_FIELD(security_id_source_, security_id_source_);
  SIM_ASSIGN_FIELD(instrument_symbol_, instrument_symbol_);
  SIM_ASSIGN_FIELD(last_update_, last_update_);
  SIM_ASSIGN_FIELD(mid_price_, mid_price_);
  SIM_ASSIGN_FIELD(bid_price_, bid_price_);
  SIM_ASSIGN_FIELD(offer_price_, offer_price_);

#undef SIM_ASSIGN_FIELD

  return seed;
}

auto PriceSeed::price_seed_id() const noexcept -> std::uint64_t {
  return price_seed_id_;
}

auto PriceSeed::symbol() const noexcept -> const std::optional<std::string>& {
  return symbol_;
}

auto PriceSeed::security_type() const noexcept
    -> const std::optional<std::string>& {
  return security_type_;
}

auto PriceSeed::price_currency() const noexcept
    -> const std::optional<std::string>& {
  return price_currency_;
}

auto PriceSeed::security_id() const noexcept
    -> const std::optional<std::string>& {
  return security_id_;
}

auto PriceSeed::security_id_source() const noexcept
    -> const std::optional<std::string>& {
  return security_id_source_;
}

auto PriceSeed::instrument_symbol() const noexcept
    -> const std::optional<std::string>& {
  return instrument_symbol_;
}

auto PriceSeed::mid_price() const noexcept -> std::optional<double> {
  return mid_price_;
}

auto PriceSeed::bid_price() const noexcept -> std::optional<double> {
  return bid_price_;
}

auto PriceSeed::offer_price() const noexcept -> std::optional<double> {
  return offer_price_;
}

auto PriceSeed::last_update() const noexcept
    -> const std::optional<std::string>& {
  return last_update_;
}

auto PriceSeed::Patch::symbol() const noexcept
    -> const std::optional<std::string>& {
  return symbol_;
}

auto PriceSeed::Patch::with_symbol(std::string symbol) noexcept -> Patch& {
  symbol_ = std::move(symbol);
  return *this;
}

auto PriceSeed::Patch::security_type() const noexcept
    -> const std::optional<std::string>& {
  return security_type_;
}

auto PriceSeed::Patch::with_security_type(std::string type) noexcept -> Patch& {
  security_type_ = std::move(type);
  return *this;
}

auto PriceSeed::Patch::price_currency() const noexcept
    -> const std::optional<std::string>& {
  return price_currency_;
}

auto PriceSeed::Patch::with_price_currency(std::string currency) noexcept
    -> Patch& {
  price_currency_ = std::move(currency);
  return *this;
}

auto PriceSeed::Patch::security_id() const noexcept
    -> const std::optional<std::string>& {
  return security_id_;
}

auto PriceSeed::Patch::with_security_id(std::string id) noexcept -> Patch& {
  security_id_ = std::move(id);
  return *this;
}

auto PriceSeed::Patch::security_id_source() const noexcept
    -> const std::optional<std::string>& {
  return security_id_source_;
}

auto PriceSeed::Patch::with_security_id_source(std::string source) noexcept
    -> Patch& {
  security_id_source_ = std::move(source);
  return *this;
}

auto PriceSeed::Patch::instrument_symbol() const noexcept
    -> const std::optional<std::string>& {
  return instrument_symbol_;
}

auto PriceSeed::Patch::with_instrument_symbol(std::string symbol) noexcept
    -> Patch& {
  instrument_symbol_ = std::move(symbol);
  return *this;
}

auto PriceSeed::Patch::mid_price() const noexcept -> std::optional<double> {
  return mid_price_;
}

auto PriceSeed::Patch::with_mid_price(double price) noexcept -> Patch& {
  mid_price_ = price;
  return *this;
}

auto PriceSeed::Patch::bid_price() const noexcept -> std::optional<double> {
  return bid_price_;
}

auto PriceSeed::Patch::with_bid_price(double price) noexcept -> Patch& {
  bid_price_ = price;
  return *this;
}

auto PriceSeed::Patch::offer_price() const noexcept -> std::optional<double> {
  return offer_price_;
}

auto PriceSeed::Patch::with_offer_price(double price) noexcept -> Patch& {
  offer_price_ = price;
  return *this;
}

auto PriceSeed::Patch::last_update() const noexcept
    -> const std::optional<std::string>& {
  return last_update_;
}

auto PriceSeed::Patch::with_last_update(std::string update_timestamp) noexcept
    -> Patch& {
  last_update_ = std::move(update_timestamp);
  return *this;
}

}  // namespace simulator::data_layer
