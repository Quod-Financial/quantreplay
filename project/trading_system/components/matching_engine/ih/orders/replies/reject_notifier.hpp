#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_REJECT_NOTIFIER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_REJECT_NOTIFIER_HPP_

#include <string_view>

#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"

namespace simulator::trading_system::matching_engine::order {

class RejectNotifier {
 public:
  RejectNotifier() = default;
  RejectNotifier(const RejectNotifier&) = default;
  RejectNotifier(RejectNotifier&&) = default;
  virtual ~RejectNotifier() = default;

  auto operator=(const RejectNotifier&) -> RejectNotifier& = default;
  auto operator=(RejectNotifier&&) -> RejectNotifier& = default;

  virtual auto notify_rejected(
      const protocol::OrderPlacementRequest& placement_request,
      std::string_view reason) -> void = 0;

  virtual auto notify_rejected(
      const protocol::OrderModificationRequest& modification_request,
      std::string_view reason) -> void = 0;

  virtual auto notify_rejected(
      const protocol::OrderCancellationRequest& cancellation_request,
      std::string_view reason) -> void = 0;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_REJECT_NOTIFIER_HPP_
