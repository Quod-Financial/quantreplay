#ifndef SIMULATOR_FIX_COMMON_PROCESSORS_EVENT_PROCESSOR_HPP_
#define SIMULATOR_FIX_COMMON_PROCESSORS_EVENT_PROCESSOR_HPP_

#include <quickfix/SessionID.h>

namespace simulator::fix {

class EventProcessor {
 public:
  EventProcessor() = default;
  EventProcessor(const EventProcessor&) = default;
  EventProcessor(EventProcessor&&) noexcept = default;
  virtual ~EventProcessor() = default;

  auto operator=(const EventProcessor&) -> EventProcessor& = default;
  auto operator=(EventProcessor&&) noexcept -> EventProcessor& = default;

  virtual auto process_session_connection(
      const FIX::SessionID& fix_session) const -> void = 0;

  virtual auto process_session_disconnection(
      const FIX::SessionID& fix_session) const -> void = 0;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_PROCESSORS_EVENT_PROCESSOR_HPP_