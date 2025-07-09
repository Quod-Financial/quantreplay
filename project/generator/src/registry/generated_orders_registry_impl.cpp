#include "ih/registry/generated_orders_registry_impl.hpp"

#include <cassert>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <vector>

#include "ih/registry/generated_order_data.hpp"
#include "ih/registry/generated_orders_registry.hpp"

namespace simulator::generator {

auto GeneratedOrdersRegistryImpl::find_by_owner(std::string_view owner_id) const
    -> std::optional<GeneratedOrdersRegistryImpl::OrderData> {
  const std::shared_lock<decltype(mutex_)> lock{mutex_};

  if (auto stored_it = lookup_in(by_owner_assoc_, owner_id)) {
    auto it = *stored_it;
    return std::make_optional(*it);
  }

  return std::nullopt;
}

auto GeneratedOrdersRegistryImpl::find_by_identifier(
    std::string_view identifier) const
    -> std::optional<GeneratedOrdersRegistryImpl::OrderData> {
  const std::shared_lock<decltype(mutex_)> lock{mutex_};

  if (auto stored_it = lookup_in(by_identifier_assoc_, identifier)) {
    auto it = *stored_it;
    return std::make_optional(*it);
  }

  return std::nullopt;
}

auto GeneratedOrdersRegistryImpl::add(OrderData&& new_order_data) -> bool {
  const std::unique_lock<decltype(mutex_)> lock{mutex_};

  if (violates_unique_constraints(new_order_data)) {
    return false;
  }

  insert(std::move(new_order_data));
  return true;
}

auto GeneratedOrdersRegistryImpl::update_by_owner(std::string_view owner_id,
                                                  OrderData::Patch&& patch)
    -> bool {
  const std::unique_lock<decltype(mutex_)> lock{mutex_};

  auto opt_stored_it = lookup_in(by_owner_assoc_, owner_id);
  if (!opt_stored_it.has_value()) {
    return false;
  }

  update(*opt_stored_it, std::move(patch));
  return true;
}

auto GeneratedOrdersRegistryImpl::update_by_identifier(
    std::string_view identifier, OrderData::Patch&& patch) -> bool {
  const std::unique_lock<decltype(mutex_)> lock{mutex_};

  auto opt_stored_it = lookup_in(by_identifier_assoc_, identifier);
  if (!opt_stored_it.has_value()) {
    return false;
  }

  update(*opt_stored_it, std::move(patch));
  return true;
}

auto GeneratedOrdersRegistryImpl::remove_by_owner(std::string_view owner_id)
    -> bool {
  const std::unique_lock<decltype(mutex_)> lock{mutex_};

  auto opt_stored_it = lookup_in(by_owner_assoc_, owner_id);
  if (!opt_stored_it.has_value()) {
    return false;
  }

  remove(*opt_stored_it);
  return true;
}

auto GeneratedOrdersRegistryImpl::remove_by_identifier(
    std::string_view identifier) -> bool {
  const std::unique_lock<decltype(mutex_)> lock{mutex_};

  auto opt_stored_it = lookup_in(by_identifier_assoc_, identifier);
  if (!opt_stored_it.has_value()) {
    return false;
  }

  remove(*opt_stored_it);
  return true;
}

auto GeneratedOrdersRegistryImpl::for_each(const Visitor& visitor) const
    -> void {
  const std::shared_lock<decltype(mutex_)> lock{mutex_};

  for (const auto& stored_order : storage_) {
    visitor(stored_order);
  }
}

auto GeneratedOrdersRegistryImpl::select_by(const Predicate& predicate) const
    -> std::vector<GeneratedOrdersRegistryImpl::OrderData> {
  std::vector<OrderData> selected{};

  {
    const std::shared_lock<decltype(mutex_)> lock{mutex_};

    for (const auto& stored_order : storage_) {
      if (predicate(stored_order)) {
        selected.emplace_back(stored_order);
      }
    }
  }

  return selected;
}

auto GeneratedOrdersRegistryImpl::violates_unique_constraints(
    const OrderData& order_data) const -> bool {
  auto by_owner_it = by_owner_assoc_.find(order_data.get_owner_id().value());
  auto by_order_id_it =
      by_identifier_assoc_.find(order_data.get_order_id().value());

  const bool by_owner_idx_exists = by_owner_it != std::end(by_owner_assoc_);
  const bool by_id_idx_exists =
      by_order_id_it != std::end(by_identifier_assoc_);
  return by_owner_idx_exists || by_id_idx_exists;
}

auto GeneratedOrdersRegistryImpl::insert(OrderData&& order_data) -> void {
  auto& inserted = storage_.emplace_back(std::move(order_data));
  assert(!storage_.empty());
  auto inserted_it = std::prev(std::end(storage_));

  // It's critical to ensure that we create a string_views keys
  // that are pointing to strings in inserted element
  const std::string_view owner_key{inserted.get_owner_id().value()};
  auto owner_assoc = std::make_pair(owner_key, inserted_it);
  [[maybe_unused]]
  const auto owner_assoc_res = by_owner_assoc_.emplace(std::move(owner_assoc));
  assert(owner_assoc_res.second);

  const std::string_view id_key{inserted.get_order_id().value()};
  auto id_assoc = std::make_pair(id_key, inserted_it);
  [[maybe_unused]]
  const auto id_assoc_res = by_identifier_assoc_.emplace(std::move(id_assoc));
  assert(id_assoc_res.second);
}

auto GeneratedOrdersRegistryImpl::update(Storage::iterator stored_it,
                                         OrderData::Patch&& patch) -> void {
  assert(stored_it != std::end(storage_));

  // Remove existent by-owner association
  auto by_owner_assoc_it =
      by_owner_assoc_.find(stored_it->get_owner_id().value());
  assert(by_owner_assoc_it != std::end(by_owner_assoc_));
  by_owner_assoc_.erase(by_owner_assoc_it);

  // Remove existent by-id association
  auto by_id_assoc_it =
      by_identifier_assoc_.find(stored_it->get_order_id().value());
  assert(by_id_assoc_it != std::end(by_identifier_assoc_));
  by_identifier_assoc_.erase(by_id_assoc_it);

  stored_it->apply(std::move(patch));

  // Re-associate updated order data with an owner id
  const std::string_view owner_key{stored_it->get_owner_id().value()};
  auto owner_assoc = std::make_pair(owner_key, stored_it);
  [[maybe_unused]]
  auto by_owner_assoc_res = by_owner_assoc_.emplace(std::move(owner_assoc));
  assert(by_owner_assoc_res.second);

  // Re-associate updated order data with an order id
  const std::string_view id_key{stored_it->get_order_id().value()};
  auto id_assoc = std::make_pair(id_key, stored_it);
  [[maybe_unused]]
  auto by_id_assoc_res = by_identifier_assoc_.emplace(std::move(id_assoc));
  assert(by_id_assoc_res.second);
}

auto GeneratedOrdersRegistryImpl::remove(Storage::iterator stored_it) -> void {
  assert(stored_it != std::end(storage_));

  // Remove association firstly by accessing valid stored_it
  by_identifier_assoc_.erase(stored_it->get_order_id().value());
  by_owner_assoc_.erase(stored_it->get_owner_id().value());

  storage_.erase(stored_it);
}

auto GeneratedOrdersRegistryImpl::lookup_in(const HashTable& hashtable,
                                            std::string_view key)
    -> std::optional<GeneratedOrdersRegistryImpl::Storage::iterator> {
  auto target_it = hashtable.find(key);
  return target_it == std::end(hashtable)
             ? std::nullopt
             : std::make_optional(target_it->second);
}

}  // namespace simulator::generator
