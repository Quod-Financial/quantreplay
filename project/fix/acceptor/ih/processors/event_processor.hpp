#ifndef SIMULATOR_ACCEPTOR_IH_PROCESSORS_EVENT_PROCESSOR_HPP_
#define SIMULATOR_ACCEPTOR_IH_PROCESSORS_EVENT_PROCESSOR_HPP_

#include <quickfix/SessionID.h>

namespace simulator::fix::acceptor {

class EventProcessor {
 public:
  EventProcessor() = default;
  EventProcessor(const EventProcessor&) = default;
  EventProcessor(EventProcessor&&) noexcept = default;
  virtual ~EventProcessor() = default;

  auto operator=(const EventProcessor&) -> EventProcessor& = default;
  auto operator=(EventProcessor&&) noexcept -> EventProcessor& = default;

  virtual auto process_session_disconnection(
      const FIX::SessionID& fix_session) const -> void = 0;
};

}  // namespace simulator::fix::acceptor

#endif  // SIMULATOR_ACCEPTOR_IH_PROCESSORS_EVENT_PROCESSOR_HPP_