#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_EVENT_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_EVENT_HPP_

#include <concepts>
#include <utility>
#include <variant>

#include "ih/common/events/client_notification.hpp"
#include "ih/common/events/order_book_notification.hpp"

namespace simulator::trading_system::matching_engine {

struct Event {
  using Category = std::variant<ClientNotification, OrderBookNotification>;

  template <typename EventCategory>
    requires std::constructible_from<Category, EventCategory>
  explicit Event(EventCategory&& category)
      : value(std::forward<EventCategory>(category)) {}

  Category value;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_EVENT_HPP_