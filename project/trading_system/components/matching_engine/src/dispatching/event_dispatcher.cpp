#include "ih/dispatching/event_dispatcher.hpp"

#include <stdexcept>
#include <utility>
#include <variant>

#include "ih/common/events/client_notification.hpp"
#include "ih/common/events/event.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

auto EventDispatcher::on_client_notification(ClientNotificationHandler handler)
    -> EventDispatcher& {
  if (handler) [[likely]] {
    client_notification_handler_ = std::move(handler);
    return *this;
  }

  throw std::invalid_argument(
      "EventDispatcher can not accept "
      "an uninitialized ClientNotificationHandler");
}

auto EventDispatcher::on_order_book_notification(
    OrderBookNotificationHandler handler) -> EventDispatcher& {
  if (handler) [[likely]] {
    order_book_notification_handler_ = std::move(handler);
    return *this;
  }

  throw std::invalid_argument(
      "EventDispatcher can not accept "
      "an uninitialized OrderBookNotificationHandler");
}

auto EventDispatcher::on(Event event) -> void {
  log::debug("received an event");

  std::visit([this](auto category) { dispatch(std::move(category)); },
             std::move(event.value));
}

auto EventDispatcher::dispatch(ClientNotification notification) -> void {
  log::debug("dispatching a ClientNotification");

  if (client_notification_handler_) [[likely]] {
    client_notification_handler_(std::move(notification));
    return;
  }

  log::warn(
      "cannot dispatch a ClientNotification, no handler is registered to "
      "handle such events");
}

auto EventDispatcher::dispatch(OrderBookNotification notification) -> void {
  log::debug("dispatching an OrderBookNotification: {}", notification);

  if (order_book_notification_handler_) [[likely]] {
    order_book_notification_handler_(std::move(notification));
    return;
  }

  log::warn(
      "cannot dispatch an OrderBookNotification, no handler is registered to "
      "handle such events");
}

}  // namespace simulator::trading_system::matching_engine