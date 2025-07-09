#ifndef SIMULATOR_ACCEPTOR_IH_ACCEPTOR_HPP_
#define SIMULATOR_ACCEPTOR_IH_ACCEPTOR_HPP_

#include <quickfix/Acceptor.h>
#include <quickfix/Log.h>
#include <quickfix/MessageStore.h>
#include <quickfix/SessionSettings.h>

#include <memory>

#include "acceptor/acceptor.hpp"
#include "ih/communicators/application.hpp"
#include "ih/communicators/fix_reply_sender.hpp"
#include "ih/mapping/from_fix_mapper.hpp"
#include "ih/mapping/to_fix_mapper.hpp"
#include "ih/processors/app_event_processor.hpp"
#include "ih/processors/app_reply_processor.hpp"
#include "ih/processors/app_request_processor.hpp"

namespace simulator::fix {

struct Acceptor::Implementation {
  using RequestProcessor =
      acceptor::AppRequestProcessor<acceptor::FromFixMapper>;
  using ReplyProcessor = acceptor::AppReplyProcessor<acceptor::ToFixMapper>;
  using EventProcessor = acceptor::AppEventProcessor;

  explicit Implementation(const FIX::SessionSettings& settings);
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) noexcept = default;
  ~Implementation() noexcept;

  auto operator=(const Implementation&) -> Implementation& = delete;
  auto operator=(Implementation&&) noexcept -> Implementation& = default;

  auto replier() noexcept -> ReplyProcessor&;

  auto start_server() -> void;

  auto stop_server() noexcept -> void;

 private:
  auto fix_acceptor_server() noexcept -> FIX::Acceptor&;

  acceptor::FixReplySender reply_sender_;
  RequestProcessor request_processor_;
  EventProcessor event_processor_;

  std::unique_ptr<acceptor::Application> application_;
  std::unique_ptr<FIX::MessageStoreFactory> persistence_factory_;
  std::unique_ptr<FIX::LogFactory> logger_factory_;
  std::unique_ptr<FIX::Acceptor> server_;
  ReplyProcessor reply_processor_;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_ACCEPTOR_IH_ACCEPTOR_HPP_