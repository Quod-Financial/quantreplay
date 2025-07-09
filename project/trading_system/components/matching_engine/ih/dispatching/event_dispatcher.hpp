#ifndef SIMULATOR_MATCHING_ENGINE_IH_DISPATCHING_EVENT_DISPATCHER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_DISPATCHING_EVENT_DISPATCHER_HPP_

#include <functional>
#include <vector>

#include "ih/common/abstractions/event_listener.hpp"

namespace simulator::trading_system::matching_engine {

struct ClientNotification;
struct OrderBookNotification;

class EventDispatcher : public EventListener {
 public:
  EventDispatcher() = default;
  EventDispatcher(const EventDispatcher&) = delete;
  EventDispatcher(EventDispatcher&&) = delete;
  ~EventDispatcher() override = default;

  auto operator=(const EventDispatcher&) -> EventDispatcher& = delete;
  auto operator=(EventDispatcher&&) -> EventDispatcher& = delete;

  using ClientNotificationHandler =
      std::function<void(ClientNotification)>;

  using OrderBookNotificationHandler =
      std::function<void(OrderBookNotification)>;

  auto on_client_notification(ClientNotificationHandler handler)
      -> EventDispatcher&;

  auto on_order_book_notification(OrderBookNotificationHandler handler)
      -> EventDispatcher&;

 private:
  auto on(Event event) -> void override;

  auto dispatch(ClientNotification notification) -> void;
  auto dispatch(OrderBookNotification notification) -> void;

  ClientNotificationHandler client_notification_handler_;
  OrderBookNotificationHandler order_book_notification_handler_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_DISPATCHING_EVENT_DISPATCHER_HPP_