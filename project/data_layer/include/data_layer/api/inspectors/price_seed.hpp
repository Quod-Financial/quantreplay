#ifndef SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_PRICE_SEED_HPP_
#define SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_PRICE_SEED_HPP_

#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "data_layer/api/models/price_seed.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class PriceSeedReader final {
  using Attribute = PriceSeed::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit PriceSeedReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const PriceSeed& price_seed) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Marshaller>
class PriceSeedPatchReader final {
  using Attribute = PriceSeed::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit PriceSeedPatchReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const PriceSeed::Patch& patch) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class PriceSeedPatchWriter final {
  using Attribute = PriceSeed::Attribute;

  template <typename T>
  constexpr inline static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit PriceSeedPatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_(unmarshaller) {}

  auto write(PriceSeed::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
inline auto PriceSeedReader<Marshaller>::read(const PriceSeed& price_seed)
    -> void {
  const std::uint64_t price_seed_id = price_seed.price_seed_id();
  static_assert(can_marshall_v<decltype(price_seed_id)>);
  marshaller_(Attribute::PriceSeedId, price_seed_id);

  if (const auto& value = price_seed.symbol()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Symbol, *value);
  }

  if (const auto& value = price_seed.security_type()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityType, *value);
  }

  if (const auto& value = price_seed.price_currency()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PriceCurrency, *value);
  }

  if (const auto& value = price_seed.security_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityId, *value);
  }

  if (const auto& value = price_seed.security_id_source()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityIdSource, *value);
  }

  if (const auto& value = price_seed.instrument_symbol()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::InstrumentSymbol, *value);
  }

  if (const auto& value = price_seed.mid_price()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::MidPrice, *value);
  }

  if (const auto& value = price_seed.bid_price()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::BidPrice, *value);
  }

  if (const auto& value = price_seed.offer_price()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::OfferPrice, *value);
  }

  if (const auto& value = price_seed.last_update()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::LastUpdate, *value);
  }
}

template <typename Marshaller>
auto PriceSeedPatchReader<Marshaller>::read(const PriceSeed::Patch& patch)
    -> void {
  if (const auto& value = patch.symbol()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Symbol, *value);
  }

  if (const auto& value = patch.security_type()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityType, *value);
  }

  if (const auto& value = patch.price_currency()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::PriceCurrency, *value);
  }

  if (const auto& value = patch.security_id()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityId, *value);
  }

  if (const auto& value = patch.security_id_source()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::SecurityIdSource, *value);
  }

  if (const auto& value = patch.instrument_symbol()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::InstrumentSymbol, *value);
  }

  if (const auto& value = patch.mid_price()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::MidPrice, *value);
  }

  if (const auto& value = patch.bid_price()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::BidPrice, *value);
  }

  if (const auto& value = patch.offer_price()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::OfferPrice, *value);
  }

  if (const auto& value = patch.last_update()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::LastUpdate, *value);
  }
}

template <typename Unmarshaller>
auto PriceSeedPatchWriter<Unmarshaller>::write(PriceSeed::Patch& patch)
    -> void {
  std::string symbol{};
  static_assert(can_unmarshall_v<decltype(symbol)>);
  if (unmarshaller_(Attribute::Symbol, symbol)) {
    patch.with_symbol(std::move(symbol));
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

  std::string security_id{};
  static_assert(can_unmarshall_v<decltype(security_id)>);
  if (unmarshaller_(Attribute::SecurityId, security_id)) {
    patch.with_security_id(std::move(security_id));
  }

  std::string security_id_source{};
  static_assert(can_unmarshall_v<decltype(security_id_source)>);
  if (unmarshaller_(Attribute::SecurityIdSource, security_id_source)) {
    patch.with_security_id_source(std::move(security_id_source));
  }

  std::string instrument_symbol{};
  static_assert(can_unmarshall_v<decltype(instrument_symbol)>);
  if (unmarshaller_(Attribute::InstrumentSymbol, instrument_symbol)) {
    patch.with_instrument_symbol(std::move(instrument_symbol));
  }

  double mid_price{};
  static_assert(can_unmarshall_v<decltype(mid_price)>);
  if (unmarshaller_(Attribute::MidPrice, mid_price)) {
    patch.with_mid_price(mid_price);
  }

  double bid_price{};
  static_assert(can_unmarshall_v<decltype(bid_price)>);
  if (unmarshaller_(Attribute::BidPrice, bid_price)) {
    patch.with_bid_price(bid_price);
  }

  double offer_price{};
  static_assert(can_unmarshall_v<decltype(offer_price)>);
  if (unmarshaller_(Attribute::OfferPrice, offer_price)) {
    patch.with_offer_price(offer_price);
  }

  std::string last_update_time{};
  static_assert(can_unmarshall_v<decltype(last_update_time)>);
  if (unmarshaller_(Attribute::LastUpdate, last_update_time)) {
    patch.with_last_update(std::move(last_update_time));
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_PRICE_SEED_HPP_
