#include "quoting_engine/quoting_engine.hpp"

#include <latch>
#include <memory>
#include <utility>

#include "ih/implementation.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::quoting_engine {

QuotingEngine::QuotingEngine(
    const Instrument& instrument,
    std::shared_ptr<QuoteIdGenerator> quote_id_generator,
    runtime::Service& executor)
    : mux_{runtime::Mux::create_chained_mux(executor)},
      implementation_{std::make_unique<Implementation>(
          instrument, std::move(quote_id_generator))} {}

QuotingEngine::~QuotingEngine() noexcept = default;

template <typename RequestType, typename ReplyType>
auto QuotingEngine::execute_synchronously(const RequestType& request,
                                          ReplyType& reply) -> void {
  std::latch request_processed{1};

  runtime::execute(mux_, [this, &request, &reply, &request_processed] {
    implementation_->process(request, reply);
    request_processed.count_down();
  });

  request_processed.wait();
}

auto QuotingEngine::execute(protocol::OrderPlacementRequest request) -> void {
  log::trace("dispatching order placement request");

  runtime::execute(mux_, [this, request = std::move(request)] {
    implementation_->process(request);
  });
}

auto QuotingEngine::execute(protocol::MarketDataRequest request) -> void {
  log::trace("dispatching market data request");

  runtime::execute(mux_, [this, request = std::move(request)] {
    implementation_->process(request);
  });
}

auto QuotingEngine::execute(protocol::SecurityStatusRequest request) -> void {
  log::trace("dispatching security status request");

  runtime::execute(mux_, [this, request = std::move(request)] {
    implementation_->process(request);
  });
}

auto QuotingEngine::execute(const protocol::QuotePublicationRequest& request,
                            protocol::QuotePublicationReply& reply) -> void {
  log::trace("dispatching synchronous quote publication request");

  execute_synchronously(request, reply);
}

auto QuotingEngine::execute(const protocol::QuoteCancellationRequest& request,
                            protocol::QuoteCancellationReply& reply) -> void {
  log::trace("dispatching synchronous quote cancellation request");

  execute_synchronously(request, reply);
}

auto QuotingEngine::handle(const protocol::SessionTerminatedEvent& event)
    -> void {
  log::trace("dispatching client disconnected notification");

  runtime::execute(mux_, [this, event] { implementation_->handle(event); });
}

}  // namespace simulator::trading_system::quoting_engine
