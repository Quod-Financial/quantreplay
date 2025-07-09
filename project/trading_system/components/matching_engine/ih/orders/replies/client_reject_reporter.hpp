#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_CLIENT_REJECT_REPORTER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_CLIENT_REJECT_REPORTER_HPP_

#include <functional>

#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/replies/reject_notifier.hpp"
#include "ih/orders/tools/order_id_generator.hpp"

namespace simulator::trading_system::matching_engine::order {

class ClientRejectReporter : public RejectNotifier, private EventReporter {
 public:
  explicit ClientRejectReporter(EventListener& event_listener,
                                OrderIdGenerator& order_id_generator);

  auto notify_rejected(const protocol::OrderPlacementRequest& request,
                       std::string_view reason) -> void override;

  auto notify_rejected(const protocol::OrderModificationRequest& request,
                       std::string_view reason) -> void override;

  auto notify_rejected(const protocol::OrderCancellationRequest& request,
                       std::string_view reason) -> void override;

 private:
  std::reference_wrapper<OrderIdGenerator> order_id_generator_;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_CLIENT_REJECT_REPORTER_HPP_
