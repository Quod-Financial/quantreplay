#include "ih/historical/record_applier.hpp"

#include <algorithm>
#include <cassert>
#include <exception>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ih/constants.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/registry/generated_order_data.hpp"
#include "ih/registry/generated_orders_registry.hpp"
#include "ih/registry/registry_updater.hpp"
#include "ih/utils/request_builder.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

auto RecordApplier::apply(historical::Record record,
                          RecordApplier::ContextPointer context) noexcept
    -> std::vector<GeneratedMessage> {
  bool failed = false;
  std::vector<GeneratedMessage> replies;

  const std::uint64_t row_number = record.source_row();
  const std::optional<std::string> source_name = record.source_name();
  const std::optional<std::string> source_conn = record.source_connection();

  RecordApplier applier{std::move(context)};

  try {
    applier.process(std::move(record));
  } catch (const std::exception& ex) {
    log::err(
        "an error occurred while processing a historical record "
        "from row {} from the `{}' datasource (connection: `{}'): {}. "
        "Discarding all generated historical messages, "
        "internal generated orders registry may be corrupted",
        row_number,
        source_name.value_or("unknown"),
        source_conn.value_or("unknown"),
        ex.what());

    failed = true;
  }

  if (!failed) {
    replies = std::move(applier.request_messages_);

    const std::size_t num_messages = replies.size();
    log::debug(
        "{} messages generated based on historical record from row {} of "
        "`{}' datasource",
        num_messages,
        row_number,
        source_name.value_or("unknown"));
  }

  return replies;
}

RecordApplier::RecordApplier(RecordApplier::ContextPointer context) noexcept
    : context_{std::move(context)} {}

void RecordApplier::process(historical::Record record) {
  if (record.has_levels()) {
    cancel_other_parties(record);

    const std::optional<std::string>& source_name = record.source_name();
    const std::uint64_t source_row = record.source_row();
    std::size_t levels_applied = 0;

    record.steal_levels([this, &levels_applied, &source_name, source_row](
                            std::uint64_t level_idx, historical::Level level) {
      if (process(level, level_idx)) {
        ++levels_applied;
      } else {
        log::warn(
            "level at index {} has been skipped in a record from "
            "`{}' datasource at row {}: {}",
            level_idx,
            source_name.value_or("undefined"),
            source_row,
            level);
      }
    });

    log::debug("{} level applied from historical {}", levels_applied, record);
  } else {
    cancel_bid_part();
    cancel_offer_part();

    log::debug(
        "created cancel messages for all bid and offer generated orders, "
        "no levels are present in historical {}",
        record);
  }
}

auto RecordApplier::process(const historical::Level& level,
                            std::uint64_t level_idx) -> bool {
  if (!RecordChecker::is_processable(level)) {
    return false;
  }

  if (!place_bid(level)) {
    log::warn(
        "no bid data was found at the historical level at "
        "index {}; the bid part of the level has been ignored",
        level_idx);
  }

  if (!place_offer(level)) {
    log::warn(
        "no offer data was found at the historical level at "
        "index {}; the offer part of the level has been ignored",
        level_idx);
  }

  return true;
}

auto RecordApplier::place_bid(const historical::Level& level) -> bool {
  if (!RecordChecker::has_bid_part(level)) {
    return false;
  }

  constexpr auto target_side = Side::Option::Buy;

  assert(level.bid_price().has_value());
  const double price = level.bid_price().value();

  assert(level.bid_quantity().has_value());
  const double quantity = level.bid_quantity().value();

  std::string party = level.bid_counterparty().has_value()
                          ? level.bid_counterparty().value()
                          : next_party_id();

  place(Order{price, target_side, quantity, std::move(party)});
  return true;
}

auto RecordApplier::place_offer(const historical::Level& level) -> bool {
  if (!RecordChecker::has_offer_part(level)) {
    return false;
  }

  constexpr auto target_side = Side::Option::Sell;

  assert(level.offer_price().has_value());
  const double price = level.offer_price().value();

  assert(level.offer_quantity().has_value());
  const double quantity = level.offer_quantity().value();

  std::string party = level.offer_counterparty().has_value()
                          ? level.offer_counterparty().value()
                          : next_party_id();

  place(Order{price, target_side, quantity, std::move(party)});
  return true;
}

auto RecordApplier::place(RecordApplier::Order order) -> void {
  auto& context = *context_;
  auto& registry = context.take_registry();

  const std::optional<GeneratedOrderData> existing_order =
      registry.find_by_owner(order.counterparty_id);

  RequestBuilder message_builder;
  message_builder.with_resting_attributes()
      .with_price(OrderPrice{order.price})
      .with_quantity(Quantity{order.quantity})
      .with_side(order.side)
      .with_counterparty(PartyId{std::move(order.counterparty_id)});

  if (existing_order.has_value() &&
      existing_order->get_order_side() == order.side) {
    message_builder.make_modification_request()
        .with_clordid(existing_order->get_order_id())
        .with_orig_clordid(existing_order->get_orig_order_id());
  } else {
    if (existing_order.has_value()) {
      const auto& target_ord_id = existing_order->get_order_id();
      cancel([&target_ord_id](const GeneratedOrderData& placed_order) {
        return placed_order.get_order_id() == target_ord_id;
      });
    }

    message_builder.make_new_order_request().with_clordid(
        ClientOrderId{context.get_synthetic_identifier()});
  }

  auto order_message = RequestBuilder::construct(std::move(message_builder));
  OrderRegistryUpdater::update(registry, order_message);
  request_messages_.emplace_back(std::move(order_message));
}

auto RecordApplier::cancel(
    const GeneratedOrdersRegistry::Predicate& cancel_criteria) -> void {
  GeneratedOrdersRegistry& registry = context_->take_registry();

  const std::vector<GeneratedOrderData> orders =
      registry.select_by(cancel_criteria);

  if (orders.empty()) {
    return;
  }

  std::vector<GeneratedMessage> cancel_requests;
  for (const GeneratedOrderData& order : orders) {
    RequestBuilder request_builder;
    request_builder.make_cancel_request()
        .with_resting_attributes()
        .with_clordid(order.get_order_id())
        .with_orig_clordid(order.get_orig_order_id())
        .with_side(order.get_order_side())
        .with_price(order.get_order_px())
        .with_quantity(order.get_order_qty())
        .with_counterparty(order.get_owner_id());

    auto cancel_request = RequestBuilder::construct(std::move(request_builder));
    cancel_requests.emplace_back(std::move(cancel_request));
  }

  for (const GeneratedMessage& cancel_request : cancel_requests) {
    OrderRegistryUpdater::update(registry, cancel_request);
  }

  std::copy(std::make_move_iterator(std::begin(cancel_requests)),
            std::make_move_iterator(std::end(cancel_requests)),
            std::back_inserter(request_messages_));
}

auto RecordApplier::cancel_bid_part() -> void {
  static const GeneratedOrdersRegistry::Predicate all_bid_order =
      [](const GeneratedOrderData& order) {
        return order.get_order_side() == Side::Option::Buy;
      };

  cancel(all_bid_order);
}

auto RecordApplier::cancel_offer_part() -> void {
  static const GeneratedOrdersRegistry::Predicate all_offer_order =
      [](const GeneratedOrderData& order) {
        return order.get_order_side() == Side::Option::Sell;
      };

  cancel(all_offer_order);
}

auto RecordApplier::cancel_other_parties(const historical::Record& record)
    -> void {
  std::unordered_set<std::string> parties;
  record.visit_levels([&parties](std::uint64_t, const Level& level) {
    if (const auto party = level.bid_counterparty()) {
      parties.insert(*party);
    }
    if (const auto party = level.offer_counterparty()) {
      parties.insert(*party);
    }
  });

  cancel([&parties = std::as_const(parties)](const GeneratedOrderData& order) {
    return !parties.contains(order.get_owner_id().value());
  });
}

auto RecordApplier::next_party_id() -> std::string {
  return fmt::format(constant::historical::DefaultCounterpartyPattern,
                     ++party_id_counter_);
}

RecordApplier::Order::Order(double order_price,
                            Side order_side,
                            double order_quantity,
                            std::string order_counterparty_id) noexcept
    : counterparty_id{std::move(order_counterparty_id)},
      price{order_price},
      quantity{order_quantity},
      side{order_side} {}

auto RecordApplier::RecordChecker::is_processable(
    const historical::Level& level) noexcept -> bool {
  const bool has_bid_px = level.bid_price().has_value();
  const bool has_bid_qty = level.bid_quantity().has_value();

  const bool has_offer_px = level.offer_price().has_value();
  const bool has_offer_qty = level.offer_quantity().has_value();

  const bool is_bid_valid = !static_cast<bool>(has_bid_px ^ has_bid_qty);
  const bool is_offer_valid = !static_cast<bool>(has_offer_px ^ has_offer_qty);

  return is_bid_valid && is_offer_valid;
}

auto RecordApplier::RecordChecker::has_bid_part(
    const historical::Level& level) noexcept -> bool {
  if (!is_processable(level)) {
    return false;
  }

  const bool has_price = level.bid_price().has_value();
  const bool has_qty = level.bid_quantity().has_value();

  assert(!(has_price ^ has_qty));
  return has_price && has_qty;
}

auto RecordApplier::RecordChecker::has_offer_part(
    const historical::Level& level) noexcept -> bool {
  if (!is_processable(level)) {
    return false;
  }

  const bool has_price = level.offer_price().has_value();
  const bool has_qty = level.offer_quantity().has_value();

  assert(!(has_price ^ has_qty));
  return has_price && has_qty;
}

}  // namespace simulator::generator::historical
