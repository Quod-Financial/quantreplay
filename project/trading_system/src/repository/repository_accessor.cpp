#include "ih/repository/repository_accessor.hpp"

#include "ih/repository/trading_engines_repository.hpp"

namespace simulator::trading_system {

namespace {

template <typename EngineType, typename ActionType>
auto apply_if_found(EngineType* engine, ActionType action) -> bool {
  if (engine == nullptr) {
    return false;
  }
  action(*engine);
  return true;
}

class TradingEngineRepositoryAccessor : public RepositoryAccessor {
 public:
  explicit TradingEngineRepositoryAccessor(
      const TradingEnginesRepository& repository)
      : repository_(repository) {}

 private:
  auto unicast_impl(InstrumentId instrument_id, Action action) const
      -> void override {
    action(repository_.find_instrument_engine(instrument_id));
  }

  auto broadcast_impl(Action action) const -> void override {
    repository_.for_each_engine(action);
  }

  auto unicast_order_modifying_impl(InstrumentId instrument_id,
                                    OrderModifyingAction action) const
      -> bool override {
    return apply_if_found(
        repository_.find_order_modifying_engine(instrument_id), action);
  }

  auto unicast_state_persisting_impl(InstrumentId instrument_id,
                                     StatePersistingAction action) const
      -> bool override {
    return apply_if_found(
        repository_.find_state_persisting_engine(instrument_id), action);
  }

  auto unicast_quote_publishing_impl(InstrumentId instrument_id,
                                     QuotePublishingAction action) const
      -> bool override {
    return apply_if_found(
        repository_.find_quote_publishing_engine(instrument_id), action);
  }

  const TradingEnginesRepository& repository_;
};

}  // namespace

auto RepositoryAccessor::create(const TradingEnginesRepository& repository)
    -> std::unique_ptr<RepositoryAccessor> {
  return std::make_unique<TradingEngineRepositoryAccessor>(repository);
}

}  // namespace simulator::trading_system
