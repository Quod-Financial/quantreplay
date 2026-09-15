#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_MANAGER_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_MANAGER_HPP_

#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "ih/context/component_context.hpp"
#include "ih/market_data/price_cache.hpp"
#include "ih/market_data/subscription.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata {

// Merges the configured market data requirements into one subscription per
// session and symbol, and sends subscribe/unsubscribe requests for them.
// All the requirements have to be added before the manager is used, as the
// requests are sent and the session events are handled by different threads.
class SubscriptionManager {
 public:
  SubscriptionManager() = delete;

  SubscriptionManager(std::shared_ptr<PriceCache> price_cache,
                      std::shared_ptr<ComponentContext> context);

  SubscriptionManager(const SubscriptionManager&) = delete;
  SubscriptionManager(SubscriptionManager&&) = delete;
  ~SubscriptionManager() noexcept = default;

  auto operator=(const SubscriptionManager&) -> SubscriptionManager& = delete;
  auto operator=(SubscriptionManager&&) -> SubscriptionManager& = delete;

  auto add_datasource_subscription(const SubscriptionRequirement& requirement)
      -> std::optional<MdRequestId>;

  auto subscribe() -> void;

  auto unsubscribe() -> void;

  // Requests the subscriptions of a session that has just been established
  // again, as a counterparty keeps no subscription state across sessions.
  auto handle_session_connected(const protocol::Session& session) -> void;

  // Drops the subscriptions of a session that is gone without requesting an
  // unsubscription, which can no longer be delivered over that session.
  auto handle_session_disconnected(const protocol::Session& session) -> void;

  // Disables the rejected subscription, so no unsubscription is requested for
  // it, until subscribe() or the connection of its session requests it again.
  auto handle_reject(const protocol::MarketDataReject& reject) -> void;

  [[nodiscard]]
  auto find_request_id(const protocol::Session& session,
                       const Symbol& symbol) const
      -> std::optional<MdRequestId>;

 private:
  auto merge(Subscription& subscription,
             const SubscriptionRequirement& requirement) -> void;

  auto send_subscribe(Subscription& subscription) -> void;

  auto send_unsubscribe(Subscription& subscription) -> void;

  auto mark_unsubscribed(Subscription& subscription) -> void;

  std::vector<Subscription> subscriptions_;
  std::shared_ptr<PriceCache> price_cache_;
  std::shared_ptr<ComponentContext> context_;
  bool subscription_requested_ = false;
  mutable std::mutex mutex_;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_SUBSCRIPTION_MANAGER_HPP_
