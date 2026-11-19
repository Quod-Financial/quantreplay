#include "common/security_status_publisher.hpp"

#include <cassert>
#include <concepts>
#include <map>
#include <ranges>
#include <utility>

#include "core/domain/instrument_descriptor.hpp"

namespace simulator::trading_system {

struct SecurityStatusPublisher::Subscription {
  protocol::Session session;
  InstrumentDescriptor instrument;
  SecurityStatusReqId identifier;
};

class SecurityStatusPublisher::Index {
  using Container = std::multimap<SecurityStatusReqId, Subscription>;
  using iterator = Container::iterator;

 public:
  auto emplace(Subscription subscription) -> const Subscription* {
    if (find(subscription.identifier, subscription.session) !=
        subscriptions_.end()) {
      return nullptr;
    }

    const auto added = subscriptions_.emplace(subscription.identifier,
                                              std::move(subscription));
    return &added->second;
  }

  auto erase(const SecurityStatusReqId& identifier,
             const protocol::Session& session) -> bool {
    const auto iter = find(identifier, session);
    if (iter == subscriptions_.end()) {
      return false;
    }

    subscriptions_.erase(iter);
    return true;
  }

  auto erase(const protocol::Session& session) -> void {
    std::erase_if(subscriptions_, [&session](const auto& entry) {
      return entry.second.session == session;
    });
  }

  template <typename F>
    requires std::invocable<F, const Subscription&>
  auto for_each(F function) const -> void {
    for (const auto& subscription : subscriptions_ | std::views::values) {
      function(subscription);
    }
  }

 private:
  auto find(const SecurityStatusReqId& identifier,
            const protocol::Session& session) -> iterator {
    auto [begin, end] = subscriptions_.equal_range(identifier);
    for (auto iter = begin; iter != end; ++iter) {
      if (iter->second.session == session) {
        return iter;
      }
    }
    return subscriptions_.end();
  }

  Container subscriptions_;
};

SecurityStatusPublisher::SecurityStatusPublisher(MarketPhase market_phase,
                                                 ReplySender& reply_sender)
    : subscriptions_{std::make_unique<Index>()},
      reply_sender_{reply_sender},
      market_phase_{market_phase} {}

SecurityStatusPublisher::~SecurityStatusPublisher() noexcept = default;

auto SecurityStatusPublisher::update(MarketPhase market_phase) -> bool {
  if (market_phase_ == market_phase) {
    return false;
  }

  market_phase_ = market_phase;
  subscriptions_->for_each(
      [this](const Subscription& subscription) { publish(subscription); });
  return true;
}

auto SecurityStatusPublisher::process(
    const protocol::SecurityStatusRequest& request) -> void {
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

  switch (request.request_type->value()) {
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

auto SecurityStatusPublisher::unsubscribe(const protocol::Session& session)
    -> void {
  subscriptions_->erase(session);
}

auto SecurityStatusPublisher::subscribe(
    const protocol::SecurityStatusRequest& request) -> void {
  assert(request.request_id);
  const auto* const subscription =
      subscriptions_->emplace({.session = request.session,
                               .instrument = request.instrument,
                               .identifier = *request.request_id});
  if (subscription != nullptr) {
    publish(*subscription);
    return;
  }

  reject(request,
         BusinessRejectReason::Option::Other,
         "security status subscription request id is already in use");
}

auto SecurityStatusPublisher::snapshot(
    const protocol::SecurityStatusRequest& request) const -> void {
  assert(request.request_id);
  publish({.session = request.session,
           .instrument = request.instrument,
           .identifier = *request.request_id});
}

auto SecurityStatusPublisher::unsubscribe(
    const protocol::SecurityStatusRequest& request) -> void {
  assert(request.request_id);
  if (!subscriptions_->erase(*request.request_id, request.session)) {
    reject(request,
           BusinessRejectReason::Option::UnknownId,
           "no subscription found for the security status request id");
  }
}

auto SecurityStatusPublisher::publish(const Subscription& subscription) const
    -> void {
  protocol::SecurityStatus report{subscription.session};
  report.instrument = subscription.instrument;
  report.request_id = subscription.identifier;
  report.trading_phase = market_phase_.trading_phase();
  report.trading_status = market_phase_.trading_status();
  reply_sender_.get().send(std::move(report));
}

auto SecurityStatusPublisher::reject(
    const protocol::SecurityStatusRequest& request,
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

  reply_sender_.get().send(std::move(reject));
}

}  // namespace simulator::trading_system
