#include "ih/repository/repository_accessor.hpp"

#include "ih/repository/trading_engines_repository.hpp"

namespace simulator::trading_system {

namespace {

class TradingEngineRepositoryAccessor : public RepositoryAccessor {
 public:
  explicit TradingEngineRepositoryAccessor(
      const TradingEnginesRepository& repository)
      : repository_(repository) {}

 private:
  auto unicast_impl(InstrumentId instrument_id,
                    Action action) const -> void override {
    action(repository_.find_instrument_engine(instrument_id));
  }

  auto broadcast_impl(Action action) const -> void override {
    repository_.for_each_engine(action);
  }

  const TradingEnginesRepository& repository_;
};

}  // namespace

auto RepositoryAccessor::create(const TradingEnginesRepository& repository)
    -> std::unique_ptr<RepositoryAccessor> {
  return std::make_unique<TradingEngineRepositoryAccessor>(repository);
}

}  // namespace simulator::trading_system