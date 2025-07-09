#include "ih/market_data/subscriptions/subscription_manager.hpp"

#include <cassert>
#include <map>
#include <memory>

#include "ih/market_data/subscriptions/subscription.hpp"
#include "ih/market_data/tools/algorithms.hpp"
#include "ih/market_data/tools/notification_creators.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class SubscriptionManager::Index {
  using Container = std::multimap<MdRequestId, std::shared_ptr<Subscription>>;
  using iterator = Container::iterator;

 public:
  auto emplace(Subscription subscription) -> std::shared_ptr<Subscription> {
    const auto existing_iter =
        find(subscription.request_id(), subscription.session());

    if (existing_iter == subscriptions_.end()) {
      auto added = std::make_shared<Subscription>(std::move(subscription));
      subscriptions_.emplace(added->request_id(), added);
      return added;
    }
    return nullptr;
  }

  auto erase(const MdRequestId& request_id,
             const protocol::Session& subscriber_session)
      -> std::shared_ptr<Subscription> {
    const auto iter = find(request_id, subscriber_session);
    if (iter != subscriptions_.end()) {
      const auto subscription = iter->second;
      subscriptions_.erase(iter);
      return subscription;
    }
    return nullptr;
  }

  template <typename F>
    requires std::invocable<F, Subscription&>
  auto for_each(F function) const -> void {
    for (const auto& [_, subscription] : subscriptions_) {
      function(*subscription);
    }
  }

  template <typename P>
    requires std::predicate<P, Subscription&>
  auto erase_if(P predicate) -> void {
    for (auto iter = subscriptions_.begin(); iter != subscriptions_.end();) {
      if (predicate(*iter->second)) {
        const auto subscription = iter->second;
        iter = subscriptions_.erase(iter);
      } else {
        ++iter;
      }
    }
  }

 private:
  auto find(const MdRequestId& request_id,
            const protocol::Session& subscriber_session) -> iterator {
    auto [begin, end] = subscriptions_.equal_range(request_id);
    for (auto iter = begin; iter != end; ++iter) {
      if (iter->second->session() == subscriber_session) {
        return iter;
      }
    }
    return subscriptions_.end();
  }

  Container subscriptions_;
};

SubscriptionManager::SubscriptionManager(
    Configuration configuration,
    EventListener& listener,
    MarketDataProvider& market_data_provider)
    : EventReporter(listener),
      configuration_(configuration),
      index_(std::make_unique<Index>()),
      data_provider_(market_data_provider) {}

SubscriptionManager::~SubscriptionManager() noexcept = default;

auto SubscriptionManager::process(protocol::MarketDataRequest request) -> void {
  if (!validate(request)) {
    return;
  }

  switch (*request.request_type) {
    case MdSubscriptionRequestType::Option::Subscribe:
      subscribe(std::move(request));
      break;
    case MdSubscriptionRequestType::Option::Unsubscribe:
      unsubscribe(std::move(request));
      break;
    case MdSubscriptionRequestType::Option::Snapshot:
      snapshot(std::move(request));
      break;
  }
}

auto SubscriptionManager::unsubscribe(const protocol::Session& client_session)
    -> void {
  index_->erase_if([&](Subscription& subscription) {
    return subscription.session() == client_session;
  });
}

auto SubscriptionManager::publish() -> void {
  index_->for_each([this](Subscription& subscription) {
    subscription.send_update(data_provider_);
  });
}

auto SubscriptionManager::validate(const protocol::MarketDataRequest& request)
    -> bool {
  if (!request.request_id.has_value()) {
    emit(make_request_rejected_notification(
        request, "required market data request id missing"));
    return false;
  }
  if (!request.request_type.has_value()) {
    emit(make_request_rejected_notification(
        request, "required market data request type missing"));
    return false;
  }
  if (request.instruments.size() != 1) {
    emit(make_request_rejected_notification(
        request, "invalid number of instruments in the request"));
    return false;
  }
  if (request.market_data_types.empty()) {
    emit(make_request_rejected_notification(
        request, "no supported market data types specified in the request"));
    return false;
  }
  if (contains(request.market_data_types, MdEntryType::Option::Trade) &&
      !trade_streaming_enabled()) {
    emit(make_request_rejected_notification(
        request,
        "subscriptions on trades are not allowed, streaming is disabled"));
    return false;
  }
  if (request.market_depth.has_value() && request.market_depth->value() > 1) {
    emit(make_request_rejected_notification(
        request, "unsupported market depth value specified in the request"));
    return false;
  }
  return true;
}

auto SubscriptionManager::subscribe(protocol::MarketDataRequest request)
    -> void {
  assert(request.request_id.has_value());
  assert(request.instruments.size() == 1);

  log::debug("subscribing a client to market data stream, subscription id: {}",
             request.request_id);

  if (const auto subscription = index_->emplace(create_subscription(request))) {
    subscription->send_initial(data_provider_);
  } else {
    emit(make_request_rejected_notification(
        request, MdRejectReason::Option::DuplicateMdReqId));
  }
}

auto SubscriptionManager::unsubscribe(protocol::MarketDataRequest request)
    -> void {
  assert(request.request_id.has_value());

  log::debug(
      "unsubscribing a client from market data stream, subscription id: {}",
      request.request_id);

  auto subscription = index_->erase(*request.request_id, request.session);
  if (!subscription) {
    emit(make_request_rejected_notification(
        request, "no subscription found for the request id"));
  }
}

auto SubscriptionManager::snapshot(protocol::MarketDataRequest request) const
    -> void {
  assert(request.request_id.has_value());
  assert(request.instruments.size() == 1);

  log::debug("sending a market data snapshot to a client, subscription id: {}",
             request.request_id);

  create_subscription(request).send_snapshot(data_provider_);
}

auto SubscriptionManager::create_subscription(
    const protocol::MarketDataRequest& request) const -> Subscription {
  if (!request.request_id.has_value()) {
    throw std::invalid_argument(
        "request id is required to create a subscription");
  }
  if (request.instruments.size() != 1) {
    throw std::invalid_argument("invalid number of instruments in the request");
  }

  StreamingSettings settings;
  for (const MdEntryType type : request.market_data_types) {
    settings.enable_data_type_streaming(type);
  }
  if (request.market_depth == MarketDepth{1}) {
    settings.enable_top_of_book_only_streaming();
  }
  if (request.update_type == MarketDataUpdateType::Option::Snapshot) {
    settings.enable_full_update_streaming();
  }

  const auto ord_owner_iter =
      std::ranges::find_if(request.parties, [](const auto& party) {
        return party.role() == PartyRole::Option::ExecutingFirm;
      });
  if (ord_owner_iter != request.parties.end()) {
    settings.filter_orders_by_owner(ord_owner_iter->party_id());
  }

  return Subscription{listener(),
                      *request.request_id,
                      request.session,
                      request.instruments.front(),
                      std::move(settings)};
}

auto SubscriptionManager::trade_streaming_enabled() const -> bool {
  return configuration_.enable_trades_streaming;
}

}  // namespace simulator::trading_system::matching_engine::mdata