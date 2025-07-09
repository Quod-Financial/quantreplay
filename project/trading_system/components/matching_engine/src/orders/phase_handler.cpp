#include "ih/orders/phase_handler.hpp"

#include <cassert>
#include <map>
#include <memory>

#include "core/domain/attributes.hpp"
#include "core/domain/market_phase.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/security_status.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine::order {

struct PhaseHandler::Subscription {
  protocol::Session session;
  InstrumentDescriptor instrument;
  SecurityStatusReqId identifier;
};

struct PhaseHandler::Index {
  using Container =
      std::multimap<SecurityStatusReqId, std::shared_ptr<Subscription>>;
  using iterator = Container::iterator;

 public:
  auto emplace(const Subscription& subscription)
      -> std::shared_ptr<Subscription> {
    const auto existing_iter =
        find(subscription.identifier, subscription.session);

    if (existing_iter == subscriptions_.end()) {
      auto added = std::make_shared<Subscription>(std::move(subscription));
      subscriptions_.emplace(added->identifier, added);
      return added;
    }
    return nullptr;
  }

  auto erase(const SecurityStatusReqId& identifier,
             const protocol::Session& session)
      -> std::shared_ptr<Subscription> {
    const auto iter = find(identifier, session);
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
  auto find(const SecurityStatusReqId& identifier,
            const protocol::Session& subscriber_session) -> iterator {
    auto [begin, end] = subscriptions_.equal_range(identifier);
    for (auto iter = begin; iter != end; ++iter) {
      if (iter->second->session == subscriber_session) {
        return iter;
      }
    }
    return subscriptions_.end();
  }

  Container subscriptions_;
};

PhaseHandler::PhaseHandler(EventListener& event_listener)
    : EventReporter(event_listener),
      subscriptions_(std::make_unique<Index>()) {}

PhaseHandler::~PhaseHandler() = default;

auto PhaseHandler::handle(event::PhaseTransition transition) -> void {
  const MarketPhase market_phase{transition.phase.phase(), transition.phase.status()};
  if (current_state_ != market_phase) {
    current_state_ = market_phase;
    subscriptions_->for_each(
        [this](const Subscription& subscription) { publish(subscription); });
  }
}

auto PhaseHandler::process(const protocol::SecurityStatusRequest& request)
    -> void {
  if (!request.request_type) {
    reject(request,
           BusinessRejectReason::Option::Other,
           "security status subscription request type is missing");
    return;
  }

  if (!request.request_id) {
    reject(request,
           BusinessRejectReason::Option::Other,
           "security status subscription request id is missing");
    return;
  }

  switch (*request.request_type) {
    case MdSubscriptionRequestType::Option::Subscribe:
      subscribe(request);
      break;
    case MdSubscriptionRequestType::Option::Unsubscribe:
      unsubscribe(request);
      break;
    case MdSubscriptionRequestType::Option::Snapshot:
      snapshot(request);
      break;
  }
}

auto PhaseHandler::unsubscribe(const protocol::Session& session) -> void {
  subscriptions_->erase_if([&session](const Subscription& subscription) {
    return subscription.session == session;
  });
}

auto PhaseHandler::subscribe(const protocol::SecurityStatusRequest& request)
    -> void {
  assert(request.request_id);  // must be guaranteed by a caller

  const auto subscription =
      subscriptions_->emplace({.session = request.session,
                               .instrument = request.instrument,
                               .identifier = *request.request_id});
  if (subscription) {
    publish(*subscription);
    return;
  }

  reject(request,
         BusinessRejectReason::Option::Other,
         "security status subscription request id is already in use");
}

auto PhaseHandler::snapshot(const protocol::SecurityStatusRequest& request)
    -> void {
  assert(request.request_id);  // must be guaranteed by a caller
  publish({.session = request.session,
           .instrument = request.instrument,
           .identifier = *request.request_id});
}

auto PhaseHandler::unsubscribe(const protocol::SecurityStatusRequest& request)
    -> void {
  assert(request.request_id);  // must be guaranteed by a caller

  auto subscription =
      subscriptions_->erase(*request.request_id, request.session);
  if (!subscription) {
    reject(request,
           BusinessRejectReason::Option::UnknownId,
           "no subscription found for the security status request id");
  }
}

auto PhaseHandler::publish(const Subscription& subscription) -> void {
  protocol::SecurityStatus report{subscription.session};
  report.instrument = subscription.instrument;
  report.request_id = subscription.identifier;
  report.trading_phase = current_state_.trading_phase();
  report.trading_status = current_state_.trading_status();
  emit(ClientNotification(report));
}

auto PhaseHandler::reject(const protocol::SecurityStatusRequest& request,
                          BusinessRejectReason reason,
                          std::string text) const -> void {
  protocol::BusinessMessageReject reject{request.session};
  reject.business_reject_reason = reason;
  reject.text = RejectText(std::move(text));
  reject.ref_message_type = RejectedMessageType::Option::SecurityStatusRequest;
  reject.ref_seq_num = request.seq_num;
  if (const auto& rejected_id = request.request_id) {
    reject.ref_id = BusinessRejectRefId(rejected_id->value());
  }

  emit(ClientNotification(reject));
}

}  // namespace simulator::trading_system::matching_engine::order