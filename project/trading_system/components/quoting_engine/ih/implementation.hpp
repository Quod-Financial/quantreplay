#ifndef SIMULATOR_QUOTING_ENGINE_IH_IMPLEMENTATION_HPP_
#define SIMULATOR_QUOTING_ENGINE_IH_IMPLEMENTATION_HPP_

#include <memory>

#include "common/instrument.hpp"
#include "common/security_status_publisher.hpp"
#include "ih/market_data/quote_streamer.hpp"
#include "ih/orders/order_rejecter.hpp"
#include "ih/quotes/quote_book.hpp"
#include "ih/quotes/quote_validator.hpp"
#include "protocol/admin/quote.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status.hpp"
#include "protocol/app/security_status_request.hpp"
#include "protocol/app/session_terminated_event.hpp"
#include "quoting_engine/quote_id_generator.hpp"
#include "quoting_engine/quoting_engine.hpp"

namespace simulator::trading_system::quoting_engine {

class QuotingEngine::Implementation
    : private SecurityStatusPublisher::ReplySender {
 public:
  Implementation(const Instrument& instrument,
                 std::shared_ptr<QuoteIdGenerator> quote_id_generator);

  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = delete;
  ~Implementation() noexcept override = default;

  auto operator=(const Implementation&) -> Implementation& = delete;
  auto operator=(Implementation&&) -> Implementation& = delete;

  auto process(const protocol::OrderPlacementRequest& request) -> void;

  auto process(const protocol::MarketDataRequest& request) -> void;

  auto process(const protocol::SecurityStatusRequest& request) -> void;

  auto process(const protocol::QuotePublicationRequest& request,
               protocol::QuotePublicationReply& reply) -> void;

  auto process(const protocol::QuoteCancellationRequest& request,
               protocol::QuoteCancellationReply& reply) -> void;

  auto handle(const protocol::SessionTerminatedEvent& event) -> void;

 private:
  auto send(protocol::SecurityStatus status) -> void override;

  auto send(protocol::BusinessMessageReject reject) -> void override;

  OrderRejecter order_rejecter_;
  SecurityStatusPublisher security_status_;
  QuoteValidator quote_validator_;
  QuoteBook quotes_;
  QuoteStreamer quote_streamer_;
};

}  // namespace simulator::trading_system::quoting_engine

#endif  // SIMULATOR_QUOTING_ENGINE_IH_IMPLEMENTATION_HPP_
