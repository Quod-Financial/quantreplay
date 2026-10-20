#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_DATA_ROUTER_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_DATA_ROUTER_HPP_

#include <functional>
#include <memory>

#include "core/domain/attributes.hpp"
#include "ih/market_data/market_data_book_cache.hpp"
#include "ih/market_data/order_book_derivation.hpp"
#include "ih/market_data/price_cache.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"

namespace simulator::generator::mdata {

// Turns an incremental refresh into the full refresh of its book before it
// reaches the price cache and the order derivation.
class MarketDataRouter {
 public:
  MarketDataRouter() = delete;

  using DivergenceHandler = std::function<void(const MdRequestId&)>;

  MarketDataRouter(std::shared_ptr<MarketDataBookCache> book_cache,
                   std::shared_ptr<PriceCache> price_cache,
                   std::shared_ptr<OrderBookDerivation> order_book_derivation,
                   DivergenceHandler handle_divergence);

  auto process(const protocol::MarketDataSnapshot& snapshot) -> void;

  auto process(const protocol::MarketDataUpdate& update) -> void;

 private:
  auto publish(const protocol::MarketDataSnapshot& snapshot) -> void;

  std::shared_ptr<MarketDataBookCache> book_cache_;
  std::shared_ptr<PriceCache> price_cache_;
  std::shared_ptr<OrderBookDerivation> order_book_derivation_;
  DivergenceHandler handle_divergence_;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_DATA_ROUTER_HPP_
