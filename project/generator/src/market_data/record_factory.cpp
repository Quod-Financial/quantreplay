#include "ih/market_data/record_factory.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "core/common/std_formatter.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "log/logging.hpp"
#include "protocol/types/session.hpp"

namespace simulator::generator::mdata {
namespace {

[[nodiscard]]
auto encode_connection(const protocol::Session& session) -> std::string {
  const auto* fix_session = std::get_if<protocol::fix::Session>(&session.value);
  if (fix_session == nullptr) {
    return fmt::to_string(session);
  }
  if (fix_session->session_qualifier.has_value()) {
    return fmt::format("{}:{}->{}:{}",
                       fix_session->begin_string,
                       fix_session->sender_comp_id,
                       fix_session->target_comp_id,
                       *fix_session->session_qualifier);
  }
  return fmt::format("{}:{}->{}",
                     fix_session->begin_string,
                     fix_session->sender_comp_id,
                     fix_session->target_comp_id);
}

[[nodiscard]]
auto first_entry_time(const std::vector<MarketDataEntry>& entries)
    -> std::optional<historical::Timepoint> {
  for (const MarketDataEntry& entry : entries) {
    if (entry.time.has_value()) {
      return std::make_optional<historical::Timepoint>(entry.time->value());
    }
  }
  return std::nullopt;
}

[[nodiscard]]
auto exceeds_depth(std::size_t side_entries, MarketDepth depth) -> bool {
  return depth != AllDepthLevels &&
         side_entries >= static_cast<std::size_t>(depth.value());
}

auto set_bid(historical::Level::Builder& level,
             const MarketDataEntry& entry,
             const PartyId& counterparty) -> void {
  if (entry.price.has_value()) {
    level.with_bid_price(entry.price->value());
  }
  if (entry.quantity.has_value()) {
    level.with_bid_quantity(entry.quantity->value());
  }
  level.with_bid_counterparty(entry.buyer_id.has_value()
                                  ? entry.buyer_id->value()
                                  : counterparty.value());
}

auto set_offer(historical::Level::Builder& level,
               const MarketDataEntry& entry,
               const PartyId& counterparty) -> void {
  if (entry.price.has_value()) {
    level.with_offer_price(entry.price->value());
  }
  if (entry.quantity.has_value()) {
    level.with_offer_quantity(entry.quantity->value());
  }
  level.with_offer_counterparty(entry.seller_id.has_value()
                                    ? entry.seller_id->value()
                                    : counterparty.value());
}

[[nodiscard]]
auto count_levels(const std::vector<MarketDataEntry>& entries,
                  MarketDepth depth) -> std::size_t {
  std::size_t bid_entries = 0;
  std::size_t offer_entries = 0;
  for (const MarketDataEntry& entry : entries) {
    if (entry.type == MdEntryType::Option::Bid) {
      if (!exceeds_depth(bid_entries, depth)) {
        ++bid_entries;
      }
    } else if (entry.type == MdEntryType::Option::Offer) {
      if (!exceeds_depth(offer_entries, depth)) {
        ++offer_entries;
      }
    }
  }
  return std::max(bid_entries, offer_entries);
}

[[nodiscard]]
auto level_at(std::vector<historical::Level::Builder>& levels,
              std::size_t index) -> historical::Level::Builder& {
  while (index >= levels.size()) {
    levels.emplace_back();
  }
  return levels[index];
}

auto add_levels(historical::Record::Builder& builder,
                const std::vector<MarketDataEntry>& entries,
                const OrderDerivationRequirement& requirement) -> void {
  std::vector<historical::Level::Builder> levels;
  levels.reserve(count_levels(entries, requirement.depth));
  std::size_t bid_entries = 0;
  std::size_t offer_entries = 0;

  for (const MarketDataEntry& entry : entries) {
    if (entry.type == MdEntryType::Option::Bid) {
      if (exceeds_depth(bid_entries, requirement.depth)) {
        log::debug("dropping bid entry beyond the requested depth of {}: {}",
                   requirement.depth,
                   entry);
        continue;
      }
      set_bid(level_at(levels, bid_entries++), entry, requirement.counterparty);
    } else if (entry.type == MdEntryType::Option::Offer) {
      if (exceeds_depth(offer_entries, requirement.depth)) {
        log::debug("dropping offer entry beyond the requested depth of {}: {}",
                   requirement.depth,
                   entry);
        continue;
      }
      set_offer(level_at(levels, offer_entries++),
                entry,
                requirement.counterparty);
    } else {
      log::debug(
          "skipping market data entry of type `{}', which does not describe "
          "an order book level",
          entry.type);
    }
  }

  for (std::size_t index = 0; index < levels.size(); ++index) {
    builder.add_level(
        index, historical::Level::Builder::construct(std::move(levels[index])));
  }
}

}  // namespace

auto make_order_book_record(const protocol::MarketDataSnapshot& snapshot,
                            const OrderDerivationRequirement& requirement,
                            std::uint64_t source_row) -> historical::Record {
  historical::Record::BuilderImpl builder;
  builder.with_instrument(requirement.symbol.value())
      .with_received_time(std::chrono::system_clock::now())
      .with_source_connection(encode_connection(snapshot.session))
      .with_source_row(source_row);

  if (snapshot.request_id.has_value()) {
    builder.with_source_name(snapshot.request_id->value());
  }

  if (const auto message_time =
          first_entry_time(snapshot.market_data_entries)) {
    builder.with_message_time(*message_time);
  }

  add_levels(builder, snapshot.market_data_entries, requirement);

  return builder.construct();
}

}  // namespace simulator::generator::mdata
