#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_MD_ENTRY_VALUE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_MD_ENTRY_VALUE_HPP_

#include <optional>
#include <utility>

#include "core/domain/attributes.hpp"
#include "core/domain/enumerators.hpp"

namespace simulator::trading_system::matching_engine::mdata {

template <enum core::enumerators::MdEntryType Type, typename Value>
class MdEntryValue {
 public:
  constexpr MdEntryValue() = default;

  constexpr explicit MdEntryValue(Value value) : value_{std::move(value)} {}

  [[nodiscard]]
  constexpr auto value() const -> const std::optional<Value>& {
    return value_;
  }

  [[nodiscard]]
  static constexpr auto type() -> core::enumerators::MdEntryType {
    return Type;
  }

  [[nodiscard]]
  constexpr auto action() const -> MarketEntryAction {
    return action_;
  }

  constexpr auto update(Value value) -> bool {
    if (value_ != value && action_ != MarketEntryAction::Option::Delete) {
      action_ = value_.has_value() ? MarketEntryAction::Option::Change
                                   : MarketEntryAction::Option::New;
      value_ = std::move(value);
      return true;
    }
    return false;
  }

  constexpr auto update(std::optional<Value> value) -> bool {
    return value.has_value() ? update(*std::move(value)) : false;
  }

  constexpr auto force_update(Value value) -> void {
    action_ = value_.has_value() ? MarketEntryAction::Option::Change
                                 : MarketEntryAction::Option::New;
    value_ = value;
  }

  constexpr auto mark_deleted() -> void {
    if (value_.has_value()) {
      action_ = MarketEntryAction::Option::Delete;
    }
  }

 private:
  std::optional<Value> value_ = std::nullopt;
  MarketEntryAction action_ = MarketEntryAction::Option::New;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_MD_ENTRY_VALUE_HPP_
