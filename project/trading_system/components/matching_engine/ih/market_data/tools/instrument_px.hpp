#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_INSTRUMENT_PX_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_INSTRUMENT_PX_HPP_

#include <optional>
#include <type_traits>
#include <variant>

#include "core/domain/attributes.hpp"
#include "core/domain/enumerators.hpp"

namespace simulator::trading_system::matching_engine::mdata {

// Carry quantity for 269=Q
template <enum core::enumerators::MdEntryType Type, bool HasQuantity = false>
class InstrumentPx {
 public:
  constexpr InstrumentPx() = default;

  constexpr explicit InstrumentPx(Price price) : price_{price} {}

  [[nodiscard]]
  constexpr auto price() const -> std::optional<Price> {
    return price_;
  }

  [[nodiscard]]
  constexpr auto quantity() const -> std::optional<Quantity>
    requires HasQuantity
  {
    return quantity_;
  }

  [[nodiscard]]
  constexpr auto type() const -> core::enumerators::MdEntryType {
    return Type;
  }

  [[nodiscard]]
  constexpr auto action() const -> MarketEntryAction {
    return action_;
  }

  [[nodiscard]]
  constexpr explicit operator std::optional<double>() const {
    return price_.has_value() ? std::make_optional<double>(*price_)
                              : std::nullopt;
  }

  constexpr auto update(Price price) -> bool {
    if (price_ != price && action_ != MarketEntryAction::Option::Delete) {
      action_ = price_.has_value() ? MarketEntryAction::Option::Change
                                   : MarketEntryAction::Option::New;
      price_ = price;
      return true;
    }
    return false;
  }

  constexpr auto update(std::optional<Price> price) -> bool {
    if (price.has_value()) {
      return update(*price);
    }
    return false;
  }

  constexpr auto update(Price price, std::optional<Quantity> quantity) -> bool
    requires HasQuantity
  {
    if ((price_ != price || quantity_ != quantity) &&
        action_ != MarketEntryAction::Option::Delete) {
      action_ = price_.has_value() ? MarketEntryAction::Option::Change
                                   : MarketEntryAction::Option::New;
      price_ = price;
      quantity_ = quantity;
      return true;
    }
    return false;
  }

  constexpr auto update(std::optional<Price> price,
                        std::optional<Quantity> quantity) -> bool
    requires HasQuantity
  {
    if (price.has_value()) {
      return update(*price, quantity);
    }
    return false;
  }

  constexpr auto force_update(Price price,
                              std::optional<Quantity> quantity) -> void
    requires HasQuantity
  {
    action_ = price_.has_value() ? MarketEntryAction::Option::Change
                                 : MarketEntryAction::Option::New;
    price_ = price;
    quantity_ = quantity;
  }

  constexpr auto mark_deleted() -> void {
    if (price_.has_value()) {
      action_ = MarketEntryAction::Option::Delete;
    }
  }

 private:
  std::optional<Price> price_ = std::nullopt;
  [[no_unique_address]] std::conditional_t<HasQuantity,
                                           std::optional<Quantity>,
                                           std::monostate> quantity_{};
  MarketEntryAction action_ = MarketEntryAction::Option::New;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_INSTRUMENT_PX_HPP_
