#ifndef SIMULATOR_ACCEPTOR_IH_COMMUNICATORS_APPLICATION_HPP_
#define SIMULATOR_ACCEPTOR_IH_COMMUNICATORS_APPLICATION_HPP_

#include <quickfix/Application.h>
#include <quickfix/Message.h>
#include <quickfix/SessionID.h>

#include <functional>

#include "ih/communicators/reply_sender.hpp"
#include "ih/processors/event_processor.hpp"
#include "ih/processors/request_processor.hpp"

namespace simulator::fix::acceptor {

class Application final : public FIX::NullApplication {
 public:
  Application(const RequestProcessor& request_processor,
              const EventProcessor& event_processor) noexcept;

  auto onLogout(const FIX::SessionID& fix_session) -> void override;

  auto fromApp(const FIX::Message& fix_message,
               const FIX::SessionID& fix_session) noexcept(false)
      -> void override;

 private:
  static auto process_request(const RequestProcessor& request_processor,
                              const FIX::SessionID& fix_session,
                              const FIX::Message& fix_message) -> void;

  static auto emit_session_disconnection_event(
      const EventProcessor& event_processor, const FIX::SessionID& fix_session)
      -> void;

  std::reference_wrapper<const RequestProcessor> request_processor_;
  std::reference_wrapper<const EventProcessor> event_processor_;
};

}  // namespace simulator::fix::acceptor

#endif  // SIMULATOR_ACCEPTOR_IH_COMMUNICATORS_APPLICATION_HPP_