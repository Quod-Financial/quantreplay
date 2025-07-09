#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_MARKET_ENTRY_ID_GENERATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_MARKET_ENTRY_ID_GENERATOR_HPP_

#include <memory>

#include "core/domain/attributes.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class MarketEntryIdGenerator {
 public:
  MarketEntryIdGenerator() = default;
  MarketEntryIdGenerator(const MarketEntryIdGenerator&) = default;
  MarketEntryIdGenerator(MarketEntryIdGenerator&&) noexcept = default;
  virtual ~MarketEntryIdGenerator() = default;

  // clang-format off
  auto operator=(const MarketEntryIdGenerator&)
      -> MarketEntryIdGenerator& = default;
  auto operator=(MarketEntryIdGenerator&&) noexcept
      -> MarketEntryIdGenerator& = default;
  // clang-format on

  [[nodiscard]]
  virtual auto operator()() -> MarketEntryId = 0;

  [[nodiscard]]
  static auto create() -> std::unique_ptr<MarketEntryIdGenerator>;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_MARKET_ENTRY_ID_GENERATOR_HPP_
