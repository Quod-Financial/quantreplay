#ifndef SIMULATOR_TRADING_SYSTEM_IH_REPOSITORY_TRADING_ENGINES_REPOSITORY_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_REPOSITORY_TRADING_ENGINES_REPOSITORY_HPP_

#include <concepts>
#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/instrument.hpp"
#include "common/order_modifying_engine.hpp"
#include "common/phase_aware_engine.hpp"
#include "common/quote_publishing_engine.hpp"
#include "common/state_persisting_engine.hpp"
#include "common/trading_engine.hpp"

namespace simulator::trading_system {

// Trading engines repository keeps track of all trading engines in the system.
// The repository is responsible for managing the lifetime of trading engines.
class TradingEnginesRepository {
  struct InstrumentIdHasher {
    auto operator()(InstrumentId identifier) const -> std::size_t;
  };

  template <typename RoleType>
  using ByInstrumentLookupTable =
      std::unordered_map<InstrumentId,
                         std::reference_wrapper<RoleType>,
                         InstrumentIdHasher>;

  using PhaseAwareEngineReference = std::reference_wrapper<PhaseAwareEngine>;
  using Storage = std::vector<std::unique_ptr<TradingEngine>>;

 public:
  TradingEnginesRepository() = default;

  auto size() const noexcept -> std::size_t { return engines_.size(); }

  template <std::derived_from<TradingEngine> EngineType>
  auto add_engine(InstrumentId instrument_id,
                  std::unique_ptr<EngineType> engine) -> EngineType&;

  template <std::derived_from<TradingEngine> EngineType>
  auto add_engine(const Instrument& instrument,
                  std::unique_ptr<EngineType> engine) -> EngineType& {
    return add_engine(instrument.identifier, std::move(engine));
  }

  auto find_instrument_engine(InstrumentId identifier) const -> TradingEngine&;

  [[nodiscard]]
  auto find_order_modifying_engine(InstrumentId identifier) const
      -> OrderModifyingEngine*;

  [[nodiscard]]
  auto find_state_persisting_engine(InstrumentId identifier) const
      -> StatePersistingEngine*;

  [[nodiscard]]
  auto find_quote_publishing_engine(InstrumentId identifier) const
      -> QuotePublishingEngine*;

  template <typename FunctionType>
    requires std::invocable<FunctionType, TradingEngine&>
  auto for_each_engine(const FunctionType& function) const -> void {
    apply_for_each_engine(
        [&function](TradingEngine& engine) { function(engine); });
  }

  template <typename FunctionType>
    requires std::invocable<FunctionType, PhaseAwareEngine&>
  auto for_each_phase_aware_engine(const FunctionType& function) const -> void {
    for (PhaseAwareEngine& engine : phase_aware_engines_) {
      function(engine);
    }
  }

  template <typename ActionType>
    requires std::is_invocable_r_v<bool, ActionType, QuotePublishingEngine&>
  [[nodiscard]]
  auto try_quote_publishing_until_handled(const ActionType& action) const
      -> bool {
    // NOLINTNEXTLINE(readability-use-anyofallof): the action has side effects
    for (const auto& entry : quote_publishing_engines_) {
      if (action(entry.second.get())) {
        return true;
      }
    }
    return false;
  }

 private:
  auto associate_instrument_with_engine(InstrumentId instrument_id,
                                        TradingEngine& engine) -> void;

  auto apply_for_each_engine(
      const std::function<void(TradingEngine&)>& function) const -> void;

  ByInstrumentLookupTable<TradingEngine> by_instrument_lookup_;
  ByInstrumentLookupTable<OrderModifyingEngine> order_modifying_engines_;
  ByInstrumentLookupTable<StatePersistingEngine> state_persisting_engines_;
  ByInstrumentLookupTable<QuotePublishingEngine> quote_publishing_engines_;
  std::vector<PhaseAwareEngineReference> phase_aware_engines_;
  Storage engines_;
};

template <std::derived_from<TradingEngine> EngineType>
auto TradingEnginesRepository::add_engine(InstrumentId instrument_id,
                                          std::unique_ptr<EngineType> engine)
    -> EngineType& {
  if (!engine) [[unlikely]] {
    throw std::invalid_argument{"can not add null engine to the repository"};
  }

  EngineType& engine_reference = *engine;
  associate_instrument_with_engine(instrument_id, engine_reference);
  if constexpr (std::derived_from<EngineType, OrderModifyingEngine>) {
    order_modifying_engines_.emplace(instrument_id, engine_reference);
  }
  if constexpr (std::derived_from<EngineType, StatePersistingEngine>) {
    state_persisting_engines_.emplace(instrument_id, engine_reference);
  }
  if constexpr (std::derived_from<EngineType, QuotePublishingEngine>) {
    quote_publishing_engines_.emplace(instrument_id, engine_reference);
  }
  if constexpr (std::derived_from<EngineType, PhaseAwareEngine>) {
    phase_aware_engines_.emplace_back(engine_reference);
  }
  engines_.emplace_back(std::move(engine));

  return engine_reference;
}

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_REPOSITORY_TRADING_ENGINES_REPOSITORY_HPP_
