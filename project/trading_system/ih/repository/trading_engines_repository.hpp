#ifndef SIMULATOR_TRADING_SYSTEM_IH_REPOSITORY_TRADING_ENGINES_REPOSITORY_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_REPOSITORY_TRADING_ENGINES_REPOSITORY_HPP_

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "common/instrument.hpp"
#include "common/trading_engine.hpp"

namespace simulator::trading_system {

// Trading engines repository keeps track of all trading engines in the system.
// The repository is responsible for managing the lifetime of trading engines.
class TradingEnginesRepository {
  struct InstrumentIdHasher {
    auto operator()(InstrumentId identifier) const -> std::size_t;
  };

  using EngineReference = std::reference_wrapper<TradingEngine>;
  using Storage = std::vector<std::unique_ptr<TradingEngine>>;
  using ByInstrumentLookupTable =
      std::unordered_map<InstrumentId, EngineReference, InstrumentIdHasher>;

 public:
  TradingEnginesRepository() = default;

  auto size() const noexcept -> std::size_t { return engines_.size(); }

  auto add_engine(InstrumentId instrument_id,
                  std::unique_ptr<TradingEngine> engine) -> TradingEngine&;

  auto add_engine(const Instrument& instrument,
                  std::unique_ptr<TradingEngine> engine) -> TradingEngine& {
    return add_engine(instrument.identifier, std::move(engine));
  }

  auto find_instrument_engine(InstrumentId identifier) const -> TradingEngine&;

  template <typename FunctionType>
    requires std::invocable<FunctionType, TradingEngine&>
  auto for_each_engine(const FunctionType& function) const -> void {
    apply_for_each_engine(
        [&function](TradingEngine& engine) { function(engine); });
  }

 private:
  auto associate_instrument_with_engine(InstrumentId instrument_id,
                                        TradingEngine& engine) -> void;

  auto apply_for_each_engine(
      const std::function<void(TradingEngine&)>& function) const -> void;

  ByInstrumentLookupTable by_instrument_lookup_;
  Storage engines_;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_REPOSITORY_TRADING_ENGINES_REPOSITORY_HPP_