#include "ih/market_data/local_subscription_manager.hpp"

#include <cassert>
#include <exception>
#include <utility>

#include "ih/market_data/subscription_requirement.hpp"
#include "log/logging.hpp"
#include "middleware/routing/trading_request_channel.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/types/session.hpp"

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
  // Subscribe on full snapshots to get the top of the book and the depth of
  // each side.
  request.update_type = MarketDataUpdateType::Option::Snapshot;
  return request;
}

}  // namespace

LocalSubscriptionManager::LocalSubscriptionManager(
    std::shared_ptr<PriceCache> price_cache,
    std::shared_ptr<ComponentContext> context)
    : price_cache_{std::move(price_cache)}, context_{std::move(context)} {
  assert(price_cache_);
  assert(context_);
}

auto LocalSubscriptionManager::add(InstrumentDescriptor instrument)
    -> MdRequestId {
  MdRequestId request_id{context_->generate_identifier()};
  price_cache_->add(request_id);

  subscriptions_.emplace_back(request_id,
                              protocol::Session{protocol::generator::Session{}},
                              std::move(instrument),
                              AllDepthLevels);

  log::debug("configured the '{}' order book market data subscription for {}",
             request_id,
             subscriptions_.back().instrument());

  return request_id;
}

auto LocalSubscriptionManager::subscribe() -> void {
  for (Subscription& subscription : subscriptions_) {
    send_subscribe(subscription);
  }
}

auto LocalSubscriptionManager::unsubscribe() -> void {
  for (Subscription& subscription : subscriptions_) {
    send_unsubscribe(subscription);
  }
}

auto LocalSubscriptionManager::send_subscribe(Subscription& subscription)
    -> void {
  if (subscription.subscribed()) {
    return;
  }

  try {
    middleware::send_trading_request(make_request(
        subscription, MdSubscriptionRequestType::Option::Subscribe));
    subscription.mark_subscribed();
  } catch (const std::exception& exception) {
    log::err(
        "failed to send the '{}' order book subscription request for {}: {}",
        subscription.request_id(),
        subscription.instrument(),
        exception.what());
  } catch (...) {
    log::err(
        "failed to send the '{}' order book subscription request for {}, "
        "unknown error occurred",
        subscription.request_id(),
        subscription.instrument());
  }
}

auto LocalSubscriptionManager::send_unsubscribe(Subscription& subscription)
    -> void {
  if (!subscription.subscribed()) {
    return;
  }

  subscription.mark_unsubscribed();
  price_cache_->reset(subscription.request_id());

  try {
    middleware::send_trading_request(make_request(
        subscription, MdSubscriptionRequestType::Option::Unsubscribe));
  } catch (const std::exception& exception) {
    log::err(
        "failed to send the '{}' order book unsubscribe request for {}: {}",
        subscription.request_id(),
        subscription.instrument(),
        exception.what());
  } catch (...) {
    log::err(
        "failed to send the '{}' order book unsubscribe request for {}, "
        "unknown error occurred",
        subscription.request_id(),
        subscription.instrument());
  }
}

}  // namespace simulator::generator::mdata
