#ifndef SIMULATOR_TRADING_SYSTEM_IH_TOOLS_TRADING_ENGINE_FACTORY_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_TOOLS_TRADING_ENGINE_FACTORY_HPP_

#include <memory>

#include "common/instrument.hpp"
#include "common/trading_engine.hpp"
#include "ih/config/config.hpp"
#include "runtime/service.hpp"

namespace simulator::trading_system {

class TradingEngineFactory {
 public:
  TradingEngineFactory() = default;
  TradingEngineFactory(const TradingEngineFactory&) = default;
  TradingEngineFactory(TradingEngineFactory&&) noexcept = default;
  virtual ~TradingEngineFactory() = default;

  // clang-format off
  auto operator=(const TradingEngineFactory&)
      -> TradingEngineFactory& = default;
  auto operator=(TradingEngineFactory&&) noexcept
      -> TradingEngineFactory& = default;
  // clang-format on

  [[nodiscard]]
  virtual auto create_trading_engine(const Instrument& instrument) const
      -> std::unique_ptr<TradingEngine> = 0;
};

[[nodiscard]]
auto create_matching_engine_factory(const Config& config,
                                    runtime::Service& executor)
    -> std::unique_ptr<TradingEngineFactory>;

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_TOOLS_TRADING_ENGINE_FACTORY_HPP_