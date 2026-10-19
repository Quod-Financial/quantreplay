#include "ih/market_data/historical_random_price_source_registry.hpp"

#include <cassert>
#include <ranges>
#include <string>
#include <unordered_map>
#include <utility>

#include "log/logging.hpp"

namespace simulator::generator::mdata {

HistoricalRandomPriceSourceRegistry::HistoricalRandomPriceSourceRegistry(
    std::shared_ptr<PriceCache> price_cache,
    std::shared_ptr<ComponentContext> context)
    : price_cache_{std::move(price_cache)}, context_{std::move(context)} {
  assert(price_cache_);
  assert(context_);
}

auto HistoricalRandomPriceSourceRegistry::add(
    const HistoricalPriceSourceKey& key) -> MdRequestId {
  if (const auto source = sources_.find(key); source != sources_.end()) {
    return source->second;
  }

  MdRequestId request_id{context_->generate_identifier()};
  price_cache_->add(request_id);
  sources_.emplace(key, request_id);

  log::debug(
      "configured the `{}' market data entry for the `{}' symbol of the "
      "datasource (id: {})",
      request_id,
      key.symbol,
      key.datasource_id);

  return request_id;
}

auto HistoricalRandomPriceSourceRegistry::symbols_of(DatasourceId datasource_id)
    const -> std::unordered_map<std::string, MdRequestId> {
  std::unordered_map<std::string, MdRequestId> symbols;

  for (const auto& [key, request_id] : sources_) {
    if (key.datasource_id == datasource_id) {
      symbols.emplace(key.symbol.value(), request_id);
    }
  }

  return symbols;
}

auto HistoricalRandomPriceSourceRegistry::reset() -> void {
  for (const auto& request_id : sources_ | std::views::values) {
    price_cache_->reset(request_id);
  }
}

}  // namespace simulator::generator::mdata
