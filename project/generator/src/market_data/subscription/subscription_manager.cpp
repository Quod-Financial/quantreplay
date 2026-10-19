#include "ih/market_data/subscription/subscription_manager.hpp"

#include <algorithm>
#include <cassert>
#include <exception>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

#include "core/domain/instrument_descriptor.hpp"
#include "log/logging.hpp"
#include "middleware/routing/market_data_request_channel.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::generator::mdata {
namespace {

[[nodiscard]]
auto merge_depth(MarketDepth lhs, MarketDepth rhs) noexcept -> MarketDepth {
  if (lhs == AllDepthLevels || rhs == AllDepthLevels) {
    return AllDepthLevels;
  }
  return std::max(lhs, rhs);
}

[[nodiscard]]
auto merge_update_type(MarketDataUpdateType lhs,
                       MarketDataUpdateType rhs) noexcept
    -> MarketDataUpdateType {
  if (lhs == MarketDataUpdateType::Option::Incremental &&
      rhs == MarketDataUpdateType::Option::Incremental) {
    return MarketDataUpdateType::Option::Incremental;
  }
  return MarketDataUpdateType::Option::Snapshot;
}

[[nodiscard]]
auto make_request(const Subscription& subscription,
                  MdSubscriptionRequestType request_type)
    -> protocol::MarketDataRequest {
  protocol::MarketDataRequest request{subscription.session()};
  request.request_id = subscription.request_id();
  request.instruments.push_back(subscription.instrument());
  request.market_data_types = {MdEntryType::Option::Bid,
                               MdEntryType::Option::Offer};
  request.market_depth = subscription.depth();
  request.request_type = request_type;
  request.update_type = subscription.update_type();
  return request;
}

auto try_send(const Subscription& subscription,
              MdSubscriptionRequestType request_type) -> bool {
  try {
    middleware::send_market_data_request(
        make_request(subscription, request_type));
    return true;
  } catch (const std::exception& exception) {
    log::err("failed to send the `{}' market data {} request for {}: {}",
             subscription.request_id(),
             request_type,
             subscription.instrument(),
             exception.what());
  } catch (...) {
    log::err(
        "failed to send the `{}' market data {} request for {}, unknown "
        "error occurred",
        subscription.request_id(),
        request_type,
        subscription.instrument());
  }
  return false;
}

}  // namespace

SubscriptionManager::SubscriptionManager(
    std::shared_ptr<PriceCache> price_cache,
    std::shared_ptr<MarketDataBookCache> book_cache,
    std::shared_ptr<ComponentContext> context)
    : price_cache_{std::move(price_cache)},
      book_cache_{std::move(book_cache)},
      context_{std::move(context)} {
  assert(price_cache_);
  assert(book_cache_);
  assert(context_);
}

auto SubscriptionManager::add_datasource_subscription(
    const SubscriptionRequirement& requirement)
    -> std::optional<MergedSubscription> {
  if (requirement.symbol.value().empty()) {
    log::warn(
        "ignoring a market data subscription requirement with an empty symbol");
    return std::nullopt;
  }

  const auto existing =
      std::ranges::find_if(subscriptions_, [&](const Subscription& candidate) {
        return candidate.session() == requirement.session &&
               candidate.instrument().symbol == requirement.symbol;
      });

  if (existing != subscriptions_.end()) {
    merge(*existing, requirement);
    return std::make_optional<MergedSubscription>(existing->request_id(),
                                                  existing->depth());
  }

  InstrumentDescriptor instrument;
  instrument.symbol = requirement.symbol;

  auto request_id = MdRequestId{context_->generate_identifier()};
  subscriptions_.emplace_back(request_id,
                              requirement.session,
                              std::move(instrument),
                              requirement.depth,
                              requirement.update_type);
  subscriptions_.back().add_datasource_id(requirement.datasource_id);

  return std::make_optional<MergedSubscription>(std::move(request_id),
                                                requirement.depth);
}

auto SubscriptionManager::subscribe() -> void {
  {
    const std::lock_guard lock{mutex_};
    subscription_requested_ = true;
  }

  for (Subscription& subscription : subscriptions_) {
    send_subscribe(subscription);
  }
}

auto SubscriptionManager::unsubscribe() -> void {
  {
    const std::lock_guard lock{mutex_};
    subscription_requested_ = false;
  }

  for (Subscription& subscription : subscriptions_) {
    send_unsubscribe(subscription);
  }
}

auto SubscriptionManager::handle_session_connected(
    const protocol::Session& session) -> void {
  bool subscription_requested = false;
  {
    const std::lock_guard lock{mutex_};
    subscription_requested = subscription_requested_;
    for (Subscription& subscription : subscriptions_) {
      if (subscription.session() == session) {
        subscription.mark_unsubscribed();
      }
    }
  }

  if (!subscription_requested) {
    log::debug(
        "the generator has no market data subscriptions to request on the {} "
        "session, which has been established",
        session);
    return;
  }

  log::info(
      "requesting the market data subscriptions of the generator again on the "
      "{} session, which has been established",
      session);

  for (Subscription& subscription : subscriptions_) {
    if (subscription.session() == session) {
      send_subscribe(subscription);
    }
  }
}

auto SubscriptionManager::handle_session_disconnected(
    const protocol::Session& session) -> void {
  std::vector<MdRequestId> dropped;
  {
    const std::lock_guard lock{mutex_};
    for (Subscription& subscription : subscriptions_) {
      if (subscription.session() != session || !subscription.subscribed()) {
        continue;
      }

      subscription.mark_unsubscribed();
      dropped.push_back(subscription.request_id());
    }
  }

  if (dropped.empty()) {
    return;
  }

  for (const MdRequestId& request_id : dropped) {
    drop_market_data(request_id);
  }

  log::info(
      "dropped {} market data subscription(s) of the generator on the {} "
      "session, which has been disconnected",
      dropped.size(),
      session);
}

auto SubscriptionManager::handle_reject(
    const protocol::MarketDataReject& reject) -> void {
  if (!reject.request_id.has_value()) {
    log::warn(
        "ignoring a market data request reject without a request identifier, "
        "{}",
        reject);
    return;
  }

  const auto subscription =
      std::ranges::find_if(subscriptions_, [&](const Subscription& candidate) {
        return candidate.request_id() == *reject.request_id &&
               candidate.session() == reject.session;
      });

  if (subscription == subscriptions_.end()) {
    log::warn(
        "ignoring a market data request reject, which matches no market data "
        "subscription of the generator, {}",
        reject);
    return;
  }

  bool was_subscribed = false;
  {
    const std::lock_guard lock{mutex_};
    was_subscribed = subscription->subscribed();
    subscription->mark_unsubscribed();
  }

  if (!was_subscribed) {
    log::debug(
        "ignoring a market data request reject of the `{}' subscription, which "
        "is not subscribed",
        *reject.request_id);
    return;
  }

  drop_market_data(subscription->request_id());

  log::warn(
      "the market data subscription for {} has been rejected and is disabled "
      "until generation is restarted or its session is established again, {}",
      subscription->instrument(),
      reject);
}

auto SubscriptionManager::handle_divergence(const MdRequestId& request_id)
    -> void {
  const auto subscription =
      std::ranges::find_if(subscriptions_, [&](const Subscription& candidate) {
        return candidate.request_id() == request_id;
      });
  if (subscription == subscriptions_.end()) {
    log::warn(
        "ignoring a diverged book of the `{}' market data subscription, which "
        "is not a subscription of the generator",
        request_id);
    return;
  }

  {
    const std::lock_guard lock{mutex_};
    if (!subscription->subscribed()) {
      log::debug(
          "ignoring a diverged book of the `{}' market data subscription, "
          "which is not subscribed",
          request_id);
      return;
    }

    subscription->mark_unsubscribed();
    // Sent under the lock, so a subscribe request that another thread sends
    // for the subscription is always sent after this one.
    try_send(*subscription, MdSubscriptionRequestType::Option::Unsubscribe);
  }

  // The book cache keeps the diverged book, which ignores the full refreshes
  // still in flight until the subscription is requested again.
  price_cache_->reset(request_id);

  log::err(
      "the book of the `{}' market data subscription for {} has diverged from "
      "the feed, the subscription is cancelled until generation is restarted "
      "or its session is established again; disable incrementalSubscriptions "
      "of the datasource if the feed keeps diverging",
      request_id,
      subscription->instrument());
}

auto SubscriptionManager::find_request_id(DatasourceId datasource_id,
                                          const Symbol& symbol) const
    -> std::optional<MdRequestId> {
  const std::lock_guard lock{mutex_};

  const auto subscription =
      std::ranges::find_if(subscriptions_, [&](const Subscription& candidate) {
        return candidate.has_datasource_id(datasource_id) &&
               candidate.instrument().symbol == symbol;
      });

  if (subscription == subscriptions_.end()) {
    return std::nullopt;
  }
  return std::make_optional<MdRequestId>(subscription->request_id());
}

auto SubscriptionManager::merge(Subscription& subscription,
                                const SubscriptionRequirement& requirement)
    -> void {
  const MarketDepth depth =
      merge_depth(subscription.depth(), requirement.depth);
  subscription.set_depth(depth);
  subscription.set_update_type(
      merge_update_type(subscription.update_type(), requirement.update_type));
  subscription.add_datasource_id(requirement.datasource_id);

  log::debug(
      "merged a market data subscription requirement on `{}' into the `{}' "
      "subscription with the {} depth and the {} update type",
      requirement.symbol,
      subscription.request_id(),
      depth,
      subscription.update_type());
}

auto SubscriptionManager::send_subscribe(Subscription& subscription) -> void {
  bool incremental = false;
  {
    const std::lock_guard lock{mutex_};
    if (subscription.subscribed()) {
      return;
    }
    subscription.mark_subscribed();
    incremental =
        subscription.update_type() == MarketDataUpdateType::Option::Incremental;
  }

  // Tracked before the request is sent, as the full refresh answering it is
  // delivered by another thread.
  if (incremental) {
    book_cache_->track(subscription.request_id(), subscription.depth());
  }

  if (!try_send(subscription, MdSubscriptionRequestType::Option::Subscribe)) {
    mark_unsubscribed(subscription);
    drop_market_data(subscription.request_id());
  }
}

auto SubscriptionManager::send_unsubscribe(Subscription& subscription) -> void {
  {
    const std::lock_guard lock{mutex_};
    if (!subscription.subscribed()) {
      return;
    }
    subscription.mark_unsubscribed();
  }

  drop_market_data(subscription.request_id());
  try_send(subscription, MdSubscriptionRequestType::Option::Unsubscribe);
}

auto SubscriptionManager::mark_unsubscribed(Subscription& subscription)
    -> void {
  const std::lock_guard lock{mutex_};
  subscription.mark_unsubscribed();
}

auto SubscriptionManager::drop_market_data(const MdRequestId& request_id)
    -> void {
  price_cache_->reset(request_id);
  book_cache_->reset(request_id);
}

}  // namespace simulator::generator::mdata
