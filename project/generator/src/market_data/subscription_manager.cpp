#include "ih/market_data/subscription_manager.hpp"

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
  request.update_type = MarketDataUpdateType::Option::Snapshot;
  return request;
}

}  // namespace

SubscriptionManager::SubscriptionManager(
    std::shared_ptr<PriceCache> price_cache,
    std::shared_ptr<ComponentContext> context)
    : price_cache_{std::move(price_cache)}, context_{std::move(context)} {
  assert(price_cache_);
  assert(context_);
}

auto SubscriptionManager::add_datasource_subscription(
    const SubscriptionRequirement& requirement) -> std::optional<MdRequestId> {
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
    return existing->request_id();
  }

  InstrumentDescriptor instrument;
  instrument.symbol = requirement.symbol;

  auto request_id = MdRequestId{context_->generate_identifier()};
  subscriptions_.emplace_back(request_id,
                              requirement.session,
                              std::move(instrument),
                              requirement.depth);

  return request_id;
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
    price_cache_->reset(request_id);
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

  price_cache_->reset(*reject.request_id);

  log::warn(
      "the market data subscription for {} has been rejected and is disabled "
      "until generation is restarted or its session is established again, {}",
      subscription->instrument(),
      reject);
}

auto SubscriptionManager::find_request_id(const protocol::Session& session,
                                          const Symbol& symbol) const
    -> std::optional<MdRequestId> {
  const std::lock_guard lock{mutex_};

  const auto subscription =
      std::ranges::find_if(subscriptions_, [&](const Subscription& candidate) {
        return candidate.session() == session &&
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

  log::debug(
      "merged a market data subscription requirement on `{}' into the `{}' "
      "subscription with the {} depth",
      requirement.symbol,
      subscription.request_id(),
      depth);
}

auto SubscriptionManager::send_subscribe(Subscription& subscription) -> void {
  {
    const std::lock_guard lock{mutex_};
    if (subscription.subscribed()) {
      return;
    }
    subscription.mark_subscribed();
  }

  try {
    middleware::send_market_data_request(make_request(
        subscription, MdSubscriptionRequestType::Option::Subscribe));
  } catch (const std::exception& exception) {
    mark_unsubscribed(subscription);
    log::err(
        "failed to send the `{}' market data subscription request for {}: {}",
        subscription.request_id(),
        subscription.instrument(),
        exception.what());
  } catch (...) {
    mark_unsubscribed(subscription);
    log::err(
        "failed to send the `{}' market data subscription request for {}, "
        "unknown error occurred",
        subscription.request_id(),
        subscription.instrument());
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

  price_cache_->reset(subscription.request_id());

  try {
    middleware::send_market_data_request(make_request(
        subscription, MdSubscriptionRequestType::Option::Unsubscribe));
  } catch (const std::exception& exception) {
    log::err(
        "failed to send the `{}' market data unsubscribe request for {}: {}",
        subscription.request_id(),
        subscription.instrument(),
        exception.what());
  } catch (...) {
    log::err(
        "failed to send the `{}' market data unsubscribe request for {}, "
        "unknown error occurred",
        subscription.request_id(),
        subscription.instrument());
  }
}

auto SubscriptionManager::mark_unsubscribed(Subscription& subscription)
    -> void {
  const std::lock_guard lock{mutex_};
  subscription.mark_unsubscribed();
}

}  // namespace simulator::generator::mdata
