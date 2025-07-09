#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_MARKET_DATA_REQUEST_PROCESSOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_MARKET_DATA_REQUEST_PROCESSOR_HPP_

#include "common/market_state/snapshot.hpp"
#include "common/trade.hpp"
#include "protocol/app/instrument_state_request.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

class MarketDataRequestProcessor {
 public:
  MarketDataRequestProcessor() = default;
  MarketDataRequestProcessor(const MarketDataRequestProcessor&) = default;
  MarketDataRequestProcessor(MarketDataRequestProcessor&&) = default;
  virtual ~MarketDataRequestProcessor() = default;

  // clang-format off
  auto operator=(const MarketDataRequestProcessor&)
      -> MarketDataRequestProcessor& = default;
  auto operator=(MarketDataRequestProcessor&&)
      -> MarketDataRequestProcessor& = default;
  // clang-format on

  virtual auto process(const protocol::MarketDataRequest& request) -> void = 0;

  virtual auto capture(protocol::InstrumentState& state) -> void = 0;

  virtual auto store_state(std::optional<Trade>& last_trade,
                           std::optional<market_state::InstrumentInfo>& info)
      -> void = 0;

  virtual auto recover_state(std::optional<Trade> last_trade,
                             std::optional<market_state::InstrumentInfo> info) -> void = 0;

  virtual auto stop_streaming(const protocol::Session& client_session)
      -> void = 0;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_MARKET_DATA_REQUEST_PROCESSOR_HPP_
