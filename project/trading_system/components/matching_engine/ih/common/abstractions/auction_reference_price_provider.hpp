#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_AUCTION_REFERENCE_PRICE_PROVIDER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_AUCTION_REFERENCE_PRICE_PROVIDER_HPP_

#include <optional>

#include "core/domain/attributes.hpp"

namespace simulator::trading_system::matching_engine {

class AuctionReferencePriceProvider {
 public:
  AuctionReferencePriceProvider() = default;
  AuctionReferencePriceProvider(const AuctionReferencePriceProvider&) = default;
  AuctionReferencePriceProvider(AuctionReferencePriceProvider&&) = default;
  virtual ~AuctionReferencePriceProvider() = default;

  auto operator=(const AuctionReferencePriceProvider&)
      -> AuctionReferencePriceProvider& = default;
  auto operator=(AuctionReferencePriceProvider&&)
      -> AuctionReferencePriceProvider& = default;

  [[nodiscard]]
  virtual auto last_open_phase_traded_price() const -> std::optional<Price> = 0;

  [[nodiscard]]
  virtual auto closing_price() const -> std::optional<Price> = 0;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_AUCTION_REFERENCE_PRICE_PROVIDER_HPP_
