#include "ih/market_data/market_data_provider.hpp"

#include <cassert>
#include <utility>
#include <vector>

namespace simulator::generator::mdata {

CachedMarketDataProvider::CachedMarketDataProvider(
    std::shared_ptr<PriceCache> price_cache,
    MdRequestId order_book,
    std::vector<MdRequestId> external)
    : price_cache_{std::move(price_cache)},
      order_book_id_{std::move(order_book)},
      external_ids_{std::move(external)} {
  assert(price_cache_);
}

auto CachedMarketDataProvider::market_state() const -> MarketState {
  return price_cache_->market_state(order_book_id_, external_ids_);
}

}  // namespace simulator::generator::mdata
