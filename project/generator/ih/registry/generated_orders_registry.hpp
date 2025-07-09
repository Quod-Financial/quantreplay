#ifndef SIMULATOR_GENERATOR_IH_REGISTRY_GENERATED_ORDERS_REGISTRY_HPP_
#define SIMULATOR_GENERATOR_IH_REGISTRY_GENERATED_ORDERS_REGISTRY_HPP_

#include <functional>
#include <optional>
#include <string_view>
#include <vector>

#include "ih/registry/generated_order_data.hpp"

namespace simulator::generator {

/**
 * An interface for a thread-safe container for per-instrument generated orders.
 * Associates each generated order with an identifier and owning counterparty.
 *
 * Each order must have a unique identifier and owner (PartyID),
 * otherwise implementation shall reject a request to save the order.
 */
class GeneratedOrdersRegistry {
 public:
  using OrderData = GeneratedOrderData;
  using Visitor = std::function<void(const OrderData&)>;
  using Predicate = std::function<bool(const OrderData&)>;

  virtual ~GeneratedOrdersRegistry() = default;

  /// Searches for an order associated with a specified owner id (PartyID)
  [[nodiscard]]
  virtual auto find_by_owner(std::string_view owner_id) const
      -> std::optional<OrderData> = 0;

  /// Searches for an order associated with a specified OrderID
  [[nodiscard]]
  virtual auto find_by_identifier(std::string_view identifier) const
      -> std::optional<OrderData> = 0;

  virtual auto add(OrderData&& new_order_data) -> bool = 0;

  /// Updates an order associated with an owner id (PartyID).
  /// OrderID-to-order association shall be updated by the implementation.
  virtual auto update_by_owner(std::string_view owner_id,
                               OrderData::Patch&& patch) -> bool = 0;

  /// Updates an order associated with an identifier.
  /// A last-known order identifier is expected by the container
  /// to resolve target order.
  /// OrderID-to-order association shall be updated by the implementation.
  virtual auto update_by_identifier(std::string_view identifier,
                                    OrderData::Patch&& patch) -> bool = 0;

  virtual auto remove_by_owner(std::string_view owner_id) -> bool = 0;

  virtual auto remove_by_identifier(std::string_view identifier) -> bool = 0;

  /// Applies provided `visitor` to each stored order.
  /// Please note: a `visitor` callback must not call any method
  /// on the same container object to avoid deadlock in the implementation.
  virtual auto for_each(const Visitor& visitor) const -> void = 0;

  /// Returns all stored orders that satisfy a given `predicate`.
  /// Please note: a `predicate` callback must not call any method
  /// on the same container object to avoid deadlock in the implementation.
  [[nodiscard]]
  virtual auto select_by(const Predicate& predicate) const
      -> std::vector<OrderData> = 0;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_REGISTRY_GENERATED_ORDERS_REGISTRY_HPP_
