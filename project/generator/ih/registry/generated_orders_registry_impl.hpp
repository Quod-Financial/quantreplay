#ifndef SIMULATOR_GENERATOR_IH_REGISTRY_GENERATED_ORDERS_REGISTRY_IMPL_HPP_
#define SIMULATOR_GENERATOR_IH_REGISTRY_GENERATED_ORDERS_REGISTRY_IMPL_HPP_

#include <list>
#include <optional>
#include <shared_mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "ih/registry/generated_order_data.hpp"
#include "ih/registry/generated_orders_registry.hpp"

namespace simulator::generator {

class GeneratedOrdersRegistryImpl : public GeneratedOrdersRegistry {
 private:
  using Storage = std::list<OrderData>;

  // An orders registry must guarantee that keys in hash-tables pointing
  // to the stored object's string members.
  // Using unordered_multimap to allow multiple orders per owner
  using OwnerHashTable =
      std::unordered_multimap<std::string_view,  // A string key referenced to a
                                                 // stored object
                              Storage::iterator  // An iterator to a stored
                                                 // object
                              >;

  // Order ID must remain unique
  using IdentifierHashTable =
      std::unordered_map<std::string_view,  // A string key referenced to a
                                            // stored object
                         Storage::iterator  // An iterator to a stored object
                         >;

 public:
  auto find_by_owner(std::string_view owner_id) const
      -> std::optional<OrderData> override;

  auto find_all_by_owner(std::string_view owner_id) const
      -> std::vector<OrderData> override;

  auto find_by_identifier(std::string_view identifier) const
      -> std::optional<OrderData> override;

  auto add(OrderData&& new_order_data) -> bool override;

  auto update_by_identifier(std::string_view identifier,
                            OrderData::Patch&& patch) -> bool override;

  auto remove_by_identifier(std::string_view identifier) -> bool override;

  auto for_each(const Visitor& visitor) const -> void override;

  auto select_by(const Predicate& predicate) const
      -> std::vector<OrderData> override;

 private:
  [[nodiscard]]
  auto violates_unique_constraints(const OrderData& order_data) const -> bool;

  auto insert(OrderData&& order_data) -> void;

  auto update(Storage::iterator stored_it, OrderData::Patch&& patch) -> void;

  auto remove(Storage::iterator stored_it) -> void;

  auto remove_by_owner_association(Storage::iterator stored_it) -> void;

  [[nodiscard]]
  static auto lookup_in(const IdentifierHashTable& hashtable,
                        std::string_view key)
      -> std::optional<Storage::iterator>;

  [[nodiscard]]
  static auto lookup_first_in(const OwnerHashTable& hashtable,
                              std::string_view key)
      -> std::optional<Storage::iterator>;

  [[nodiscard]]
  static auto lookup_all_in(const OwnerHashTable& hashtable,
                            std::string_view key)
      -> std::vector<Storage::iterator>;

  OwnerHashTable by_owner_assoc_;
  IdentifierHashTable by_identifier_assoc_;

  mutable std::shared_mutex mutex_;

  Storage storage_;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_REGISTRY_GENERATED_ORDERS_REGISTRY_IMPL_HPP_
