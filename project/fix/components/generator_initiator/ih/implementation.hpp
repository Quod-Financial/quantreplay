#ifndef SIMULATOR_GENERATOR_INITIATOR_IH_IMPLEMENTATION_HPP_
#define SIMULATOR_GENERATOR_INITIATOR_IH_IMPLEMENTATION_HPP_

#include <quickfix/Initiator.h>
#include <quickfix/Log.h>
#include <quickfix/MessageStore.h>
#include <quickfix/SessionSettings.h>

#include <memory>

#include "common/communicators/application.hpp"
#include "common/communicators/fix_message_sender.hpp"
#include "fix/generator_initiator/generator_initiator.hpp"
#include "ih/mapping/from_fix_mapper.hpp"
#include "ih/mapping/to_fix_mapper.hpp"
#include "ih/processors/market_data_reply_processor.hpp"
#include "ih/processors/market_data_request_sender.hpp"
#include "ih/processors/session_event_processor.hpp"

namespace simulator::fix {

struct GeneratorInitiator::Implementation {
  using ReplyProcessor = generator_initiator::MarketDataReplyProcessor<
      generator_initiator::FromFixMapper>;
  using RequestSender = generator_initiator::MarketDataRequestSender<
      generator_initiator::ToFixMapper>;
  using EventProcessor = generator_initiator::SessionEventProcessor;

  explicit Implementation(const FIX::SessionSettings& settings);
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = delete;
  ~Implementation() noexcept;

  auto operator=(const Implementation&) -> Implementation& = delete;
  auto operator=(Implementation&&) -> Implementation& = delete;

  auto requester() noexcept -> RequestSender&;

  auto start_connection() -> void;

  auto stop_connection() noexcept -> void;

 private:
  auto fix_initiator_connection() noexcept -> FIX::Initiator&;

  FixMessageSender message_sender_;
  ReplyProcessor reply_processor_;
  EventProcessor event_processor_;

  std::unique_ptr<Application> application_;
  std::unique_ptr<FIX::MessageStoreFactory> persistence_factory_;
  std::unique_ptr<FIX::LogFactory> logger_factory_;
  std::unique_ptr<FIX::Initiator> connection_;
  RequestSender request_sender_;
};

}  // namespace simulator::fix

#endif  // SIMULATOR_GENERATOR_INITIATOR_IH_IMPLEMENTATION_HPP_
