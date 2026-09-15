#ifndef SIMULATOR_FIX_COMMON_COMMUNICATORS_APPLICATION_HPP_
#define SIMULATOR_FIX_COMMON_COMMUNICATORS_APPLICATION_HPP_

#include <quickfix/Application.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <functional>

#include "common/processors/event_processor.hpp"
#include "common/processors/message_processor.hpp"

namespace simulator::fix {

class Application final : public FIX::NullApplication {
 public:
  Application(const MessageProcessor& message_processor,
              const EventProcessor& event_processor) noexcept;

  auto onLogon(const FIX::SessionID& fix_session) -> void override;

  auto onLogout(const FIX::SessionID& fix_session) -> void override;

  auto fromApp(const FIX::Message& fix_message,
               const FIX::SessionID& fix_session) noexcept(false)
      -> void override;

 private:
  static auto process_message(const MessageProcessor& message_processor,
                              const FIX::SessionID& fix_session,
                              const FIX::Message& fix_message) -> void;

  static auto emit_session_connection_event(
      const EventProcessor& event_processor, const FIX::SessionID& fix_session)
      -> void;

  static auto emit_session_disconnection_event(
      const EventProcessor& event_processor, const FIX::SessionID& fix_session)
      -> void;

  std::reference_wrapper<const MessageProcessor> message_processor_;
  std::reference_wrapper<const EventProcessor> event_processor_;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_COMMUNICATORS_APPLICATION_HPP_
