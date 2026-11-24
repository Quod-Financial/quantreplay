#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_QUOTING_ENGINE_QUOTING_ENGINE_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_QUOTING_ENGINE_QUOTING_ENGINE_HPP_

#include <memory>

#include "common/instrument.hpp"
#include "common/quote_publishing_engine.hpp"
#include "common/trading_engine.hpp"
#include "protocol/admin/quote.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/app/session_terminated_event.hpp"
#include "quoting_engine/quote_id_generator.hpp"
#include "runtime/mux.hpp"
#include "runtime/service.hpp"

namespace simulator::trading_system::quoting_engine {

class QuotingEngine final : public TradingEngine, public QuotePublishingEngine {
 public:
  class Implementation;

  QuotingEngine(const Instrument& instrument,
                std::shared_ptr<QuoteIdGenerator> quote_id_generator,
                runtime::Service& executor);

  QuotingEngine() = delete;
  QuotingEngine(const QuotingEngine&) = delete;
  QuotingEngine(QuotingEngine&&) = delete;
  ~QuotingEngine() noexcept override;

  auto operator=(const QuotingEngine&) -> QuotingEngine& = delete;
  auto operator=(QuotingEngine&&) -> QuotingEngine& = delete;

  auto execute(protocol::OrderPlacementRequest request) -> void override;

  auto execute(protocol::MarketDataRequest request) -> void override;

  auto execute(protocol::SecurityStatusRequest request) -> void override;

  auto execute(const protocol::QuotePublicationRequest& request,
               protocol::QuotePublicationReply& reply) -> void override;

  auto execute(const protocol::QuoteCancellationRequest& request,
               protocol::QuoteCancellationReply& reply) -> void override;

  auto handle(const protocol::SessionTerminatedEvent& event) -> void override;

 private:
  template <typename RequestType, typename ReplyType>
  auto execute_synchronously(const RequestType& request, ReplyType& reply)
      -> void;

  runtime::Mux mux_;
  std::unique_ptr<Implementation> implementation_;
};

}  // namespace simulator::trading_system::quoting_engine

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_QUOTING_ENGINE_QUOTING_ENGINE_HPP_
