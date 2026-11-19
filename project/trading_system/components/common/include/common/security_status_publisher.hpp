#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_SECURITY_STATUS_PUBLISHER_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_SECURITY_STATUS_PUBLISHER_HPP_

#include <functional>
#include <memory>
#include <string>

#include "core/domain/attributes.hpp"
#include "core/domain/market_phase.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/security_status.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system {

class SecurityStatusPublisher {
 public:
  class ReplySender {
   public:
    ReplySender() = default;
    ReplySender(const ReplySender&) = default;
    ReplySender(ReplySender&&) noexcept = default;
    virtual ~ReplySender() = default;

    auto operator=(const ReplySender&) -> ReplySender& = default;
    auto operator=(ReplySender&&) noexcept -> ReplySender& = default;

    virtual auto send(protocol::SecurityStatus status) -> void = 0;

    virtual auto send(protocol::BusinessMessageReject reject) -> void = 0;
  };

  SecurityStatusPublisher(MarketPhase market_phase, ReplySender& reply_sender);

  SecurityStatusPublisher(const SecurityStatusPublisher&) = delete;
  SecurityStatusPublisher(SecurityStatusPublisher&&) = delete;
  ~SecurityStatusPublisher() noexcept;

  auto operator=(const SecurityStatusPublisher&)
      -> SecurityStatusPublisher& = delete;
  auto operator=(SecurityStatusPublisher&&)
      -> SecurityStatusPublisher& = delete;

  [[nodiscard]]
  auto market_phase() const -> MarketPhase {
    return market_phase_;
  }

  auto update(MarketPhase market_phase) -> bool;

  auto process(const protocol::SecurityStatusRequest& request) -> void;

  auto unsubscribe(const protocol::Session& session) -> void;

 private:
  struct Subscription;
  class Index;

  auto subscribe(const protocol::SecurityStatusRequest& request) -> void;

  auto snapshot(const protocol::SecurityStatusRequest& request) const -> void;

  auto unsubscribe(const protocol::SecurityStatusRequest& request) -> void;

  auto publish(const Subscription& subscription) const -> void;

  auto reject(const protocol::SecurityStatusRequest& request,
              BusinessRejectReason reason,
              std::string text) const -> void;

  std::unique_ptr<Index> subscriptions_;
  std::reference_wrapper<ReplySender> reply_sender_;
  MarketPhase market_phase_;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_SECURITY_STATUS_PUBLISHER_HPP_
