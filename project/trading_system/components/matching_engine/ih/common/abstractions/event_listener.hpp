#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_EVENT_LISTENER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_EVENT_LISTENER_HPP_

namespace simulator::trading_system::matching_engine {

struct Event;

class EventListener {
 public:
  EventListener() = default;
  EventListener(const EventListener&) = default;
  EventListener(EventListener&&) = default;
  virtual ~EventListener() = default;

  auto operator=(const EventListener&) -> EventListener& = default;
  auto operator=(EventListener&&) -> EventListener& = default;

  virtual auto on(Event event) -> void = 0;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_EVENT_LISTENER_HPP_
