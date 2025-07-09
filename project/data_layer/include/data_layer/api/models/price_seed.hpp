#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_PRICE_SEED_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_PRICE_SEED_HPP_

#include <cstdint>
#include <optional>
#include <string>

#include "data_layer/api/predicate/definitions.hpp"

namespace simulator::data_layer {

class PriceSeed {
 public:
  class Patch;

  using Predicate = predicate::Expression<PriceSeed>;

  enum class Attribute {
    PriceSeedId,
    Symbol,
    SecurityType,
    PriceCurrency,
    SecurityId,
    SecurityIdSource,
    InstrumentSymbol,
    MidPrice,
    BidPrice,
    OfferPrice,
    LastUpdate
  };

  [[nodiscard]]
  static auto create(PriceSeed::Patch snapshot, std::uint64_t id) noexcept
      -> PriceSeed;

  [[nodiscard]]
  auto price_seed_id() const noexcept -> std::uint64_t;

  [[nodiscard]]
  auto symbol() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto security_type() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto price_currency() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto security_id() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto security_id_source() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto instrument_symbol() const noexcept -> const std::optional<std::string>&;

  [[nodiscard]]
  auto mid_price() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto bid_price() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto offer_price() const noexcept -> std::optional<double>;

  [[nodiscard]]
  auto last_update() const noexcept -> const std::optional<std::string>&;

 private:
  PriceSeed() = default;

  std::optional<std::string> symbol_;
  std::optional<std::string> security_type_;
  std::optional<std::string> price_currency_;
  std::optional<std::string> security_id_;
  std::optional<std::string> security_id_source_;
  std::optional<std::string> instrument_symbol_;
  std::optional<std::string> last_update_;

  std::optional<double> mid_price_;
  std::optional<double> bid_price_;
  std::optional<double> offer_price_;

  std::uint64_t price_seed_id_{0};
};

class PriceSeed::Patch {
  friend class PriceSeed;

 public:
  using Attribute = PriceSeed::Attribute;

  [[nodiscard]]
  auto symbol() const noexcept -> const std::optional<std::string>&;
  auto with_symbol(std::string symbol) noexcept -> Patch&;

  [[nodiscard]]
  auto security_type() const noexcept -> const std::optional<std::string>&;
  auto with_security_type(std::string type) noexcept -> Patch&;

  [[nodiscard]]
  auto price_currency() const noexcept -> const std::optional<std::string>&;
  auto with_price_currency(std::string currency) noexcept -> Patch&;

  [[nodiscard]]
  auto security_id() const noexcept -> const std::optional<std::string>&;
  auto with_security_id(std::string id) noexcept -> Patch&;

  [[nodiscard]]
  auto security_id_source() const noexcept -> const std::optional<std::string>&;
  auto with_security_id_source(std::string source) noexcept -> Patch&;

  [[nodiscard]]
  auto instrument_symbol() const noexcept -> const std::optional<std::string>&;
  auto with_instrument_symbol(std::string symbol) noexcept -> Patch&;

  [[nodiscard]]
  auto mid_price() const noexcept -> std::optional<double>;
  auto with_mid_price(double price) noexcept -> Patch&;

  [[nodiscard]]
  auto bid_price() const noexcept -> std::optional<double>;
  auto with_bid_price(double price) noexcept -> Patch&;

  [[nodiscard]]
  auto offer_price() const noexcept -> std::optional<double>;
  auto with_offer_price(double price) noexcept -> Patch&;

  [[nodiscard]]
  auto last_update() const noexcept -> const std::optional<std::string>&;
  auto with_last_update(std::string update_timestamp) noexcept -> Patch&;

 private:
  std::optional<std::string> symbol_;
  std::optional<std::string> security_type_;
  std::optional<std::string> price_currency_;
  std::optional<std::string> security_id_;
  std::optional<std::string> security_id_source_;
  std::optional<std::string> instrument_symbol_;
  std::optional<std::string> last_update_;

  std::optional<double> mid_price_;
  std::optional<double> bid_price_;
  std::optional<double> offer_price_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_PRICE_SEED_HPP_
