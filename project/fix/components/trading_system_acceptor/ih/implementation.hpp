#ifndef SIMULATOR_TRADING_SYSTEM_ACCEPTOR_IH_IMPLEMENTATION_HPP_
#define SIMULATOR_TRADING_SYSTEM_ACCEPTOR_IH_IMPLEMENTATION_HPP_

#include <quickfix/Acceptor.h>
#include <quickfix/Log.h>
#include <quickfix/MessageStore.h>
#include <quickfix/SessionSettings.h>

#include <memory>

#include "common/communicators/application.hpp"
#include "common/communicators/fix_message_sender.hpp"
#include "fix/trading_system_acceptor/trading_system_acceptor.hpp"
#include "ih/mapping/from_fix_mapper.hpp"
#include "ih/mapping/to_fix_mapper.hpp"
#include "ih/processors/app_event_processor.hpp"
#include "ih/processors/app_reply_processor.hpp"
#include "ih/processors/app_request_processor.hpp"

namespace simulator::fix {

struct TradingSystemAcceptor::Implementation {
  using RequestProcessor = trading_system_acceptor::AppRequestProcessor<
      trading_system_acceptor::FromFixMapper>;
  using ReplyProcessor = trading_system_acceptor::AppReplyProcessor<
      trading_system_acceptor::ToFixMapper>;
  using EventProcessor = trading_system_acceptor::AppEventProcessor;

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

  FixMessageSender reply_sender_;
  RequestProcessor request_processor_;
  EventProcessor event_processor_;

  std::unique_ptr<Application> application_;
  std::unique_ptr<FIX::MessageStoreFactory> persistence_factory_;
  std::unique_ptr<FIX::LogFactory> logger_factory_;
  std::unique_ptr<FIX::Acceptor> server_;
  ReplyProcessor reply_processor_;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_TRADING_SYSTEM_ACCEPTOR_IH_IMPLEMENTATION_HPP_