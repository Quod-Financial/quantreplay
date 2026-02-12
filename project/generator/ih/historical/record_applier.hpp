#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_RECORD_APPLIER_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_RECORD_APPLIER_HPP_

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "ih/adaptation/generated_message.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/registry/generated_orders_registry.hpp"

namespace simulator::generator::historical {

class RecordApplier {
 public:
  struct Order;
  class RecordChecker;

  using ContextPointer = std::shared_ptr<OrderInstrumentContext>;

  RecordApplier() = delete;

  static auto apply(historical::Record record, ContextPointer context) noexcept
      -> std::vector<GeneratedMessage>;

 private:
  explicit RecordApplier(ContextPointer context) noexcept;

  auto process(historical::Record record) -> void;

  auto collect_orders(historical::Record& record) -> std::vector<Order>;

  /// Applies incoming orders in two passes to minimize messages:
  /// Pass 1: Mark all exact matches (same party+side+price+qty) - no messages
  /// Pass 2: For remaining, reuse same-side orders or create new
  auto apply_orders(std::vector<Order> incoming_orders) -> void;

  auto filter_already_placed_orders(std::vector<Order>& incoming_orders)
      -> void;

  auto find_same_party_side_order(const Order& order)
      -> std::optional<GeneratedOrderData>;

  auto cancel(const GeneratedOrdersRegistry::Predicate& cancel_criteria)
      -> void;

  auto cancel_bid_part() -> void;

  auto cancel_offer_part() -> void;

  auto cancel_not_placed_orders() -> void;

  /// Sorts messages in order: Cancel → New → Modify
  auto sort_messages() -> void;

  auto next_party_id() -> std::string;

  std::vector<GeneratedMessage> request_messages_;
  std::unordered_set<std::string> placed_client_order_ids_;

  /// Tracks order IDs that have been matched/modified in the current record
  /// processing. This prevents the same existing order from being reused
  /// when the same counterparty appears multiple times in the data.
  std::unordered_set<std::string> matched_order_ids_;

  ContextPointer context_;

  std::uint64_t party_id_counter_{0};

  bool skip_bids_{false};
  bool skip_offers_{false};
};

struct RecordApplier::Order {
  Order() = delete;

  Order(double order_price,
        Side order_side,
        double order_quantity,
        std::string order_counterparty_id) noexcept;

  std::string counterparty_id;
  double price;
  double quantity;
  Side side;
};

class RecordApplier::RecordChecker {
 public:
  static auto has_valid_bid(const historical::Level& level) noexcept -> bool;

  static auto has_valid_offer(const historical::Level& level) noexcept -> bool;

 private:
  static auto is_empty(const std::optional<double> qty) noexcept -> bool;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_RECORD_APPLIER_HPP_
