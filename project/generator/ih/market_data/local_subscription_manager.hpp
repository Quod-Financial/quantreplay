#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_LOCAL_SUBSCRIPTION_MANAGER_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_LOCAL_SUBSCRIPTION_MANAGER_HPP_

#include <memory>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "ih/context/component_context.hpp"
#include "ih/market_data/price_cache.hpp"
#include "ih/market_data/subscription.hpp"

namespace simulator::generator::mdata {

class LocalSubscriptionManager {
 public:
  LocalSubscriptionManager() = delete;

  LocalSubscriptionManager(std::shared_ptr<PriceCache> price_cache,
                           std::shared_ptr<ComponentContext> context);

  LocalSubscriptionManager(const LocalSubscriptionManager&) = delete;
  LocalSubscriptionManager(LocalSubscriptionManager&&) = delete;
  ~LocalSubscriptionManager() noexcept = default;

  auto operator=(const LocalSubscriptionManager&)
      -> LocalSubscriptionManager& = delete;
  auto operator=(LocalSubscriptionManager&&)
      -> LocalSubscriptionManager& = delete;

  auto add(InstrumentDescriptor instrument) -> MdRequestId;

  auto subscribe() -> void;

  auto unsubscribe() -> void;

 private:
  auto send_subscribe(Subscription& subscription) -> void;

  auto send_unsubscribe(Subscription& subscription) -> void;

  std::vector<Subscription> subscriptions_;
  std::shared_ptr<PriceCache> price_cache_;
  std::shared_ptr<ComponentContext> context_;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_LOCAL_SUBSCRIPTION_MANAGER_HPP_
