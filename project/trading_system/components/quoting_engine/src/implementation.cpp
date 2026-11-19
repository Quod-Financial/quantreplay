#include "ih/implementation.hpp"

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/market_phase.hpp"
#include "ih/quotes/quote.hpp"
#include "log/logging.hpp"
#include "middleware/routing/trading_reply_channel.hpp"

namespace simulator::trading_system::quoting_engine {
namespace {

[[nodiscard]]
auto to_levels(const std::vector<protocol::QuoteEntry>& entries)
    -> std::vector<QuoteLevel> {
  std::vector<QuoteLevel> levels;
  levels.reserve(entries.size());
  std::ranges::transform(
      entries, std::back_inserter(levels), [](const auto& entry) {
        return QuoteLevel{.price = entry.price, .quantity = entry.quantity};
      });
  return levels;
}

}  // namespace

QuotingEngine::Implementation::Implementation(
    const Instrument& instrument,
    std::shared_ptr<QuoteIdGenerator> quote_id_generator)
    : security_status_{MarketPhase::open(), *this},
      quote_validator_{instrument},
      quotes_{std::move(quote_id_generator)} {}

auto QuotingEngine::Implementation::process(
    const protocol::OrderPlacementRequest& request) -> void {
  order_rejecter_.reject(request);
}

auto QuotingEngine::Implementation::process(
    const protocol::MarketDataRequest& request) -> void {
  quote_streamer_.process(request, quotes_.latest());
}

auto QuotingEngine::Implementation::process(
    const protocol::SecurityStatusRequest& request) -> void {
  security_status_.process(request);
}

auto QuotingEngine::Implementation::process(
    const protocol::QuotePublicationRequest& request,
    protocol::QuotePublicationReply& reply) -> void {
  if (auto violation = quote_validator_.find_violation(request)) {
    log::debug("refusing an invalid quote: {}", *violation);
    reply.result = protocol::QuotePublicationReply::Result::InvalidRequest;
    reply.error_message = std::move(*violation);
    return;
  }

  const Quote& quote =
      quotes_.add(to_levels(request.bids), to_levels(request.asks));
  log::info("published quote `{}'", quote.identifier);

  reply.result = protocol::QuotePublicationReply::Result::Published;
  reply.quote_id = quote.identifier;
  quote_streamer_.publish(quote);
}

auto QuotingEngine::Implementation::handle(
    const protocol::SessionTerminatedEvent& event) -> void {
  security_status_.unsubscribe(event.session);
  quote_streamer_.unsubscribe(event.session);
}

auto QuotingEngine::Implementation::send(protocol::SecurityStatus status)
    -> void {
  log::debug("sending - {}", status);
  middleware::send_trading_reply(std::move(status));
}

auto QuotingEngine::Implementation::send(protocol::BusinessMessageReject reject)
    -> void {
  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

}  // namespace simulator::trading_system::quoting_engine
