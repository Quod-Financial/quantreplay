#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_HISTORICAL_RANDOM_PRICE_SOURCE_REGISTRY_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_HISTORICAL_RANDOM_PRICE_SOURCE_REGISTRY_HPP_

#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "core/domain/attributes.hpp"
#include "ih/context/component_context.hpp"
#include "ih/market_data/price_cache.hpp"
#include "ih/market_data/subscription_requirement.hpp"

namespace simulator::generator::mdata {

class HistoricalRandomPriceSourceRegistry final {
 public:
  HistoricalRandomPriceSourceRegistry() = delete;

  HistoricalRandomPriceSourceRegistry(
      std::shared_ptr<PriceCache> price_cache,
      std::shared_ptr<ComponentContext> context);

  HistoricalRandomPriceSourceRegistry(
      const HistoricalRandomPriceSourceRegistry&) = delete;
  HistoricalRandomPriceSourceRegistry(HistoricalRandomPriceSourceRegistry&&) =
      delete;
  ~HistoricalRandomPriceSourceRegistry() noexcept = default;

  auto operator=(const HistoricalRandomPriceSourceRegistry&)
      -> HistoricalRandomPriceSourceRegistry& = delete;
  auto operator=(HistoricalRandomPriceSourceRegistry&&)
      -> HistoricalRandomPriceSourceRegistry& = delete;

  auto add(const RandomPriceSourceKey& key) -> MdRequestId;

  [[nodiscard]]
  auto symbols_of(DatasourceId datasource_id) const
      -> std::unordered_map<std::string, MdRequestId>;

  auto reset() -> void;

 private:
  std::map<RandomPriceSourceKey, MdRequestId> sources_;
  std::shared_ptr<PriceCache> price_cache_;
  std::shared_ptr<ComponentContext> context_;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_HISTORICAL_RANDOM_PRICE_SOURCE_REGISTRY_HPP_
