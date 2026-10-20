#include "ih/market_data/market_data_router.hpp"

#include <cassert>
#include <memory>
#include <utility>

namespace simulator::generator::mdata {

MarketDataRouter::MarketDataRouter(
    std::shared_ptr<MarketDataBookCache> book_cache,
    std::shared_ptr<PriceCache> price_cache,
    std::shared_ptr<OrderBookDerivation> order_book_derivation,
    DivergenceHandler handle_divergence)
    : book_cache_{std::move(book_cache)},
      price_cache_{std::move(price_cache)},
      order_book_derivation_{std::move(order_book_derivation)},
      handle_divergence_{std::move(handle_divergence)} {
  assert(book_cache_);
  assert(price_cache_);
  assert(order_book_derivation_);
  assert(handle_divergence_);
}

auto MarketDataRouter::process(const protocol::MarketDataSnapshot& snapshot)
    -> void {
  if (book_cache_->process(snapshot)) {
    publish(snapshot);
  }
}

auto MarketDataRouter::process(const protocol::MarketDataUpdate& update)
    -> void {
  const auto outcome = book_cache_->process(update);
  if (outcome.snapshot.has_value()) {
    publish(*outcome.snapshot);
  }
  if (outcome.diverged && update.request_id.has_value()) {
    handle_divergence_(*update.request_id);
  }
}

auto MarketDataRouter::publish(const protocol::MarketDataSnapshot& snapshot)
    -> void {
  price_cache_->process(snapshot);
  order_book_derivation_->process(snapshot);
}

}  // namespace simulator::generator::mdata
