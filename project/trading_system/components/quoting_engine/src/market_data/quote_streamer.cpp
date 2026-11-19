#include "ih/market_data/quote_streamer.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <iterator>
#include <optional>
#include <string>
#include <utility>

#include "common/market_data_rejects.hpp"
#include "core/domain/market_data_entry.hpp"
#include "log/logging.hpp"
#include "middleware/routing/trading_reply_channel.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"

namespace simulator::trading_system::quoting_engine {
namespace {

[[nodiscard]]
auto is_quoted_side(MdEntryType type) -> bool {
  return type.value() == MdEntryType::Option::Bid ||
         type.value() == MdEntryType::Option::Offer;
}

[[nodiscard]]
auto is_requested(const std::vector<MdEntryType>& requested_types,
                  MdEntryType::Option type) -> bool {
  return std::ranges::any_of(requested_types,
                             [type](MdEntryType requested_type) {
                               return requested_type.value() == type;
                             });
}

struct Violation {
  std::optional<MdRejectReason> reason;
  std::string text;
};

[[nodiscard]]
auto find_violation(const protocol::MarketDataRequest& request)
    -> std::optional<Violation> {
  if (const auto violation = find_market_data_request_violation(request)) {
    return std::make_optional<Violation>(std::nullopt, std::string{*violation});
  }

  const auto unsupported_type =
      std::ranges::find_if_not(request.market_data_types, is_quoted_side);
  if (unsupported_type != request.market_data_types.end()) {
    return std::make_optional<Violation>(
        MdRejectReason::Option::UnsupportedMdEntryType,
        fmt::format(
            "market data entry type {} is not supported on a quoting listing",
            *unsupported_type));
  }

  if (request.market_depth.has_value() && request.market_depth->value() != 0) {
    return std::make_optional<Violation>(
        MdRejectReason::Option::UnsupportedMarketDepth,
        fmt::format("market depth {} is not supported on a quoting listing",
                    *request.market_depth));
  }

  if (request.request_type == MdSubscriptionRequestType::Option::Subscribe) {
    if (!request.update_type.has_value()) {
      return std::make_optional<Violation>(
          MdRejectReason::Option::UnsupportedMdUpdateType,
          "required market data update type missing");
    }
    if (request.update_type->value() !=
        MarketDataUpdateType::Option::Snapshot) {
      return std::make_optional<Violation>(
          MdRejectReason::Option::UnsupportedMdUpdateType,
          fmt::format(
              "market data update type {} is not supported on a quoting "
              "listing",
              *request.update_type));
    }
  }

  return std::nullopt;
}

[[nodiscard]]
auto to_reject(const protocol::MarketDataRequest& request,
               const Violation& violation) -> protocol::MarketDataReject {
  return violation.reason.has_value()
             ? make_reject(request, *violation.reason, violation.text)
             : make_reject(request, violation.text);
}

[[nodiscard]]
auto make_entry(const PublishedLevel& level, MdEntryType::Option type)
    -> MarketDataEntry {
  MarketDataEntry entry;
  entry.id = level.entry_id;
  entry.type = type;
  entry.price = level.price;
  entry.quantity = level.quantity;
  entry.quote_condition = QuoteCondition::Option::Open;
  entry.quote_type = MdQuoteType::Option::Tradeable;
  return entry;
}

auto append_entries(const std::vector<PublishedLevel>& levels,
                    MdEntryType::Option type,
                    std::vector<MarketDataEntry>& entries) -> void {
  std::ranges::transform(
      levels, std::back_inserter(entries), [type](const PublishedLevel& level) {
        return make_entry(level, type);
      });
}

[[nodiscard]]
auto compose_entries(const Quote& quote,
                     const std::vector<MdEntryType>& requested_types)
    -> std::vector<MarketDataEntry> {
  std::vector<MarketDataEntry> entries;
  if (is_requested(requested_types, MdEntryType::Option::Bid)) {
    append_entries(quote.bids, MdEntryType::Option::Bid, entries);
  }
  if (is_requested(requested_types, MdEntryType::Option::Offer)) {
    append_entries(quote.asks, MdEntryType::Option::Offer, entries);
  }
  return entries;
}

auto send(protocol::MarketDataSnapshot snapshot) -> void {
  log::debug("sending - {}", snapshot);
  middleware::send_trading_reply(std::move(snapshot));
}

auto send(protocol::MarketDataReject reject) -> void {
  log::debug("sending - {}", reject);
  middleware::send_trading_reply(std::move(reject));
}

}  // namespace

auto QuoteStreamer::process(const protocol::MarketDataRequest& request,
                            const Quote* latest_quote) -> void {
  if (const auto violation = find_violation(request)) {
    log::debug("rejecting a market data request on a quoting listing: {}",
               violation->text);
    send(to_reject(request, *violation));
    return;
  }

  if (request.request_type == MdSubscriptionRequestType::Option::Subscribe) {
    subscribe(request, latest_quote);
  } else if (request.request_type ==
             MdSubscriptionRequestType::Option::Unsubscribe) {
    unsubscribe(request);
  } else if (request.request_type ==
             MdSubscriptionRequestType::Option::Snapshot) {
    send_snapshot(make_subscription(request), latest_quote);
  }
}

auto QuoteStreamer::publish(const Quote& quote) const -> void {
  for (const Subscription& subscription : subscriptions_) {
    send_snapshot(subscription, &quote);
  }
}

auto QuoteStreamer::unsubscribe(const protocol::Session& session) -> void {
  std::erase_if(subscriptions_, [&session](const Subscription& subscription) {
    return subscription.session == session;
  });
}

auto QuoteStreamer::subscribe(const protocol::MarketDataRequest& request,
                              const Quote* latest_quote) -> void {
  log::debug("subscribing a client to quote stream, subscription id: {}",
             request.request_id);

  const bool already_subscribed = std::ranges::any_of(
      subscriptions_, [&request](const Subscription& subscription) {
        return is_identified_by(subscription, request);
      });
  if (already_subscribed) {
    send(make_reject(request, MdRejectReason::Option::DuplicateMdReqId));
    return;
  }

  send_snapshot(subscriptions_.emplace_back(make_subscription(request)),
                latest_quote);
}

auto QuoteStreamer::unsubscribe(const protocol::MarketDataRequest& request)
    -> void {
  log::debug("unsubscribing a client from quote stream, subscription id: {}",
             request.request_id);

  const auto erased = std::erase_if(
      subscriptions_, [&request](const Subscription& subscription) {
        return is_identified_by(subscription, request);
      });
  if (erased == 0) {
    send(make_unknown_subscription_reject(request));
  }
}

auto QuoteStreamer::make_subscription(
    const protocol::MarketDataRequest& request) -> Subscription {
  return Subscription{.session = request.session,
                      .instrument = request.instruments.front(),
                      .request_id = request.request_id,
                      .entry_types = request.market_data_types};
}

auto QuoteStreamer::is_identified_by(const Subscription& subscription,
                                     const protocol::MarketDataRequest& request)
    -> bool {
  return subscription.request_id == request.request_id &&
         subscription.session == request.session;
}

auto QuoteStreamer::send_snapshot(const Subscription& subscription,
                                  const Quote* latest_quote) -> void {
  protocol::MarketDataSnapshot snapshot{subscription.session};
  snapshot.request_id = subscription.request_id;
  snapshot.instrument = subscription.instrument;
  if (latest_quote != nullptr) {
    snapshot.market_data_entries =
        compose_entries(*latest_quote, subscription.entry_types);
  }
  send(std::move(snapshot));
}

}  // namespace simulator::trading_system::quoting_engine
