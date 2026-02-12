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
    std::vector<Order> incoming_orders = collect_orders(record);

    apply_orders(std::move(incoming_orders));

    cancel_not_placed_orders();

    sort_messages();

    log::debug("{} orders processed from historical record",
               placed_client_order_ids_.size());
  } else {
    cancel_bid_part();
    cancel_offer_part();

    log::debug(
        "created cancel messages for all bid and offer generated orders, "
        "no levels are present in historical {}",
        record);
  }
}

auto RecordApplier::collect_orders(historical::Record& record)
    -> std::vector<Order> {
  std::vector<Order> orders;

  record.steal_levels([this, &orders](std::uint64_t level_idx,
                                      historical::Level level) {
    if (skip_bids_) {
      log::debug(
          "bid side is already marked as invalid, skipping bid at index {}",
          level_idx);
    } else if (RecordChecker::has_valid_bid(level)) {
      std::string party = level.bid_counterparty().has_value()
                              ? *level.bid_counterparty()
                              : next_party_id();
      orders.emplace_back(*level.bid_price(),
                          Side::Option::Buy,
                          *level.bid_quantity(),
                          std::move(party));
    } else {
      skip_bids_ = true;
      log::warn(
          "bid side became invalid at level index {} -> all subsequent bid "
          "levels will be ignored",
          level_idx);
    }

    if (skip_offers_) {
      log::debug(
          "offer side is already marked as invalid, skipping offer at index {}",
          level_idx);
    } else if (RecordChecker::has_valid_offer(level)) {
      std::string party = level.offer_counterparty().has_value()
                              ? *level.offer_counterparty()
                              : next_party_id();
      orders.emplace_back(*level.offer_price(),
                          Side::Option::Sell,
                          *level.offer_quantity(),
                          std::move(party));
    } else {
      skip_offers_ = true;
      log::warn(
          "offer side became invalid at level index {} -> all subsequent "
          "offer levels will be ignored",
          level_idx);
    }
  });

  return orders;
}

auto RecordApplier::apply_orders(std::vector<Order> incoming_orders) -> void {
  filter_already_placed_orders(incoming_orders);

  auto& context = *context_;
  auto& registry = context.take_registry();

  for (auto& incoming_order : incoming_orders) {
    auto reusable_order = find_same_party_side_order(incoming_order);

    RequestBuilder message_builder;
    message_builder.with_resting_attributes()
        .with_price(OrderPrice{incoming_order.price})
        .with_quantity(Quantity{incoming_order.quantity})
        .with_side(incoming_order.side)
        .with_counterparty(PartyId{std::move(incoming_order.counterparty_id)});

    if (reusable_order.has_value()) {
      message_builder.make_modification_request()
          .with_clordid(reusable_order->get_order_id())
          .with_orig_clordid(reusable_order->get_orig_order_id());
    } else {
      message_builder.make_new_order_request().with_clordid(
          ClientOrderId{context.get_synthetic_identifier()});
    }

    auto order_message = RequestBuilder::construct(std::move(message_builder));
    OrderRegistryUpdater::update(registry, order_message);

    const auto& new_order_id = order_message.client_order_id->value();
    matched_order_ids_.insert(new_order_id);
    placed_client_order_ids_.insert(new_order_id);
    request_messages_.emplace_back(std::move(order_message));
  }
}

auto RecordApplier::filter_already_placed_orders(
    std::vector<Order>& incoming_orders) -> void {
  auto& context = *context_;
  auto& registry = context.take_registry();

  auto matches_existing_order = [this,
                                 &registry](const Order& incoming) -> bool {
    for (const auto& existing :
         registry.find_all_by_owner(incoming.counterparty_id)) {
      if (matched_order_ids_.contains(existing.get_order_id().value())) {
        continue;
      }
      if (existing.get_order_side() == incoming.side &&
          existing.get_order_px() == OrderPrice{incoming.price} &&
          existing.get_order_qty() == Quantity{incoming.quantity}) {
        const auto& order_id = existing.get_order_id().value();
        matched_order_ids_.insert(order_id);
        placed_client_order_ids_.insert(order_id);
        return true;
      }
    }
    return false;
  };

  std::erase_if(incoming_orders, matches_existing_order);
}

auto RecordApplier::find_same_party_side_order(const Order& order)
    -> std::optional<GeneratedOrderData> {
  auto& context = *context_;
  auto& registry = context.take_registry();

  const auto orders = registry.find_all_by_owner(order.counterparty_id);
  auto it = std::ranges::find_if(orders, [this, &order](const auto& existing) {
    return !matched_order_ids_.contains(existing.get_order_id().value()) &&
           existing.get_order_side() == order.side;
  });

  return it != orders.end() ? std::optional{*it} : std::nullopt;
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

auto RecordApplier::cancel_not_placed_orders() -> void {
  cancel([this](const GeneratedOrderData& order) {
    return !placed_client_order_ids_.contains(order.get_order_id().value());
  });
}

auto RecordApplier::sort_messages() -> void {
  std::ranges::stable_sort(
      request_messages_,
      [](const GeneratedMessage& a, const GeneratedMessage& b) {
        static constexpr int cancel_priority = 0;
        static constexpr int new_priority = 1;
        static constexpr int modify_priority = 2;

        auto get_priority = [](const GeneratedMessage& msg) -> int {
          switch (msg.message_type) {
            case MessageType::OrderCancelRequest:
              return cancel_priority;
            case MessageType::NewOrderSingle:
              return new_priority;
            case MessageType::OrderCancelReplaceRequest:
              return modify_priority;
            default:
              return modify_priority;
          }
        };

        return get_priority(a) < get_priority(b);
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

auto RecordApplier::RecordChecker::has_valid_bid(
    const historical::Level& level) noexcept -> bool {
  return level.bid_price().has_value() && !is_empty(level.bid_quantity());
}

auto RecordApplier::RecordChecker::has_valid_offer(
    const historical::Level& level) noexcept -> bool {
  return level.offer_price().has_value() && !is_empty(level.offer_quantity());
}

auto RecordApplier::RecordChecker::is_empty(
    const std::optional<double> qty) noexcept -> bool {
  return !qty.has_value() || qty.value() <= 0.0;
}

}  // namespace simulator::generator::historical
