#ifndef SIMULATOR_TRADING_SYSTEM_IH_TOOLS_MARKET_STATE_PERSISTENCE_CONTROLLER_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_TOOLS_MARKET_STATE_PERSISTENCE_CONTROLLER_HPP_

#include <gsl/pointers>
#include <memory>

#include "core/common/return_code.hpp"
#include "ih/config/config.hpp"
#include "ih/execution/execution_system.hpp"
#include "ih/state_persistence/serializer.hpp"

namespace simulator::trading_system {

class MarketStatePersistenceController {
 public:
  struct RecoverResult {
    core::code::RecoverMarketState code;
    std::string error_message;
  };

  explicit MarketStatePersistenceController(
      const Config& config,
      const Executor& executor,
      std::unique_ptr<Serializer> serializer,
      std::string venue_id,
      std::vector<Instrument>&& instruments);

  auto store() -> core::code::StoreMarketState;

  auto recover() -> RecoverResult;

 private:
  const Config& config_;
  const Executor& executor_;
  gsl::not_null<std::unique_ptr<Serializer>> serializer_;

  std::string venue_id_;
  std::vector<Instrument> instruments_;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_TOOLS_MARKET_STATE_PERSISTENCE_CONTROLLER_HPP_
