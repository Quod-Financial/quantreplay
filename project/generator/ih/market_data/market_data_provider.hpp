#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_DATA_PROVIDER_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_DATA_PROVIDER_HPP_

#include <memory>
#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/market_data/market_state.hpp"
#include "ih/market_data/price_cache.hpp"

namespace simulator::generator::mdata {

class MarketDataProvider {
 public:
  MarketDataProvider() = default;
  MarketDataProvider(const MarketDataProvider&) = default;
  MarketDataProvider(MarketDataProvider&&) noexcept = default;
  virtual ~MarketDataProvider() = default;

  auto operator=(const MarketDataProvider&) -> MarketDataProvider& = default;
  auto operator=(MarketDataProvider&&) noexcept
      -> MarketDataProvider& = default;

  [[nodiscard]]
  virtual auto market_state() const -> MarketState = 0;
};

// Composes the market state of a single listing out of cached market data.
// The market depth always comes from the simulated venue's own order book,
// while the base prices are the best of its top of the book and of the tops of
// the configured random price sources, if there are any.
class CachedMarketDataProvider final : public MarketDataProvider {
 public:
  CachedMarketDataProvider() = delete;

  CachedMarketDataProvider(std::shared_ptr<PriceCache> price_cache,
                           MdRequestId order_book,
                           std::vector<MdRequestId> external);

  [[nodiscard]]
  auto market_state() const -> MarketState override;

 private:
  std::shared_ptr<PriceCache> price_cache_;
  MdRequestId order_book_id_;
  std::vector<MdRequestId> external_ids_;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_DATA_PROVIDER_HPP_
