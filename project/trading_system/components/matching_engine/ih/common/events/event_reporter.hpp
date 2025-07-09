#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_EVENT_REPORTER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_EVENT_REPORTER_HPP_

#include <utility>

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event.hpp"

namespace simulator::trading_system::matching_engine {

class EventReporter {
 public:
  EventReporter() = delete;
  EventReporter(const EventReporter&) = default;
  EventReporter(EventReporter&&) = default;
  virtual ~EventReporter() = default;

  auto operator=(const EventReporter&) -> EventReporter& = default;
  auto operator=(EventReporter&&) -> EventReporter& = default;

  explicit EventReporter(EventListener& listener) : listener_(&listener) {}

  template <typename EventType>
  auto emit(EventType&& event) const -> void {
    listener_->on(Event(std::forward<EventType>(event)));
  }

  auto listener() const -> EventListener& { return *listener_; }

 private:
  EventListener* listener_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_EVENT_REPORTER_HPP_