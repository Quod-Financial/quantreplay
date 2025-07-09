#ifndef SIMULATOR_TRADING_SYSTEM_IH_EXECUTION_REJECT_NOTIFIER_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_EXECUTION_REJECT_NOTIFIER_HPP_

#include <memory>
#include <string_view>

#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"

namespace simulator::trading_system {

class RejectNotifier {
  class OrderIdentifiersGenerator;

 public:
  RejectNotifier();
  RejectNotifier(const RejectNotifier&) = delete;
  RejectNotifier(RejectNotifier&&) noexcept;
  ~RejectNotifier() noexcept;

  auto operator=(const RejectNotifier&) -> RejectNotifier& = delete;
  auto operator=(RejectNotifier&&) noexcept -> RejectNotifier&;

  auto reject(const protocol::OrderPlacementRequest& request,
              std::string_view reason) const -> void;

  auto reject(const protocol::OrderModificationRequest& request,
              std::string_view reason) const -> void;

  auto reject(const protocol::OrderCancellationRequest& request,
              std::string_view reason) const -> void;

  auto reject(const protocol::MarketDataRequest& request,
              std::string_view reason) const -> void;

  auto reject(const protocol::SecurityStatusRequest& request,
              std::string_view reason) const -> void;

  auto notify_no_instruments_requested(
      const protocol::MarketDataRequest& request) const -> void;

  auto notify_multiple_instruments_requested(
      const protocol::MarketDataRequest& request) const -> void;

 private:
  std::unique_ptr<OrderIdentifiersGenerator> id_generator_;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_EXECUTION_REJECT_NOTIFIER_HPP_