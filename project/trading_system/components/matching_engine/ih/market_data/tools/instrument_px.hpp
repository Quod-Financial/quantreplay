#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_INSTRUMENT_PX_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_INSTRUMENT_PX_HPP_

#include <optional>

#include "core/domain/attributes.hpp"
#include "core/domain/enumerators.hpp"

namespace simulator::trading_system::matching_engine::mdata {

template <enum core::enumerators::MdEntryType Type>
class InstrumentPx {
 public:
  constexpr InstrumentPx() = default;

  constexpr explicit InstrumentPx(Price price) : price_{price} {}

  [[nodiscard]]
  constexpr auto price() const -> std::optional<Price> {
    return price_;
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

  constexpr auto mark_deleted() -> void {
    if (price_.has_value()) {
      action_ = MarketEntryAction::Option::Delete;
    }
  }

 private:
  std::optional<Price> price_ = std::nullopt;
  MarketEntryAction action_ = MarketEntryAction::Option::New;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_INSTRUMENT_PX_HPP_
