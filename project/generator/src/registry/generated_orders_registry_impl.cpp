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

  if (auto stored_it = lookup_first_in(by_owner_assoc_, owner_id)) {
    auto it = *stored_it;
    return std::make_optional(*it);
  }

  return std::nullopt;
}

auto GeneratedOrdersRegistryImpl::find_all_by_owner(std::string_view owner_id)
    const -> std::vector<GeneratedOrdersRegistryImpl::OrderData> {
  const std::shared_lock<decltype(mutex_)> lock{mutex_};

  std::vector<OrderData> result;
  auto iterators = lookup_all_in(by_owner_assoc_, owner_id);
  result.reserve(iterators.size());

  for (const auto& it : iterators) {
    result.emplace_back(*it);
  }

  return result;
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
  auto by_order_id_it =
      by_identifier_assoc_.find(order_data.get_order_id().value());

  const bool by_id_idx_exists =
      by_order_id_it != std::end(by_identifier_assoc_);
  return by_id_idx_exists;
}

auto GeneratedOrdersRegistryImpl::insert(OrderData&& order_data) -> void {
  auto& inserted = storage_.emplace_back(std::move(order_data));
  assert(!storage_.empty());
  auto inserted_it = std::prev(std::end(storage_));

  // It's critical to ensure that we create a string_views keys
  // that are pointing to strings in inserted element
  const std::string_view owner_key{inserted.get_owner_id().value()};
  by_owner_assoc_.emplace(owner_key, inserted_it);

  const std::string_view id_key{inserted.get_order_id().value()};
  auto id_assoc = std::make_pair(id_key, inserted_it);
  [[maybe_unused]]
  const auto id_assoc_res = by_identifier_assoc_.emplace(std::move(id_assoc));
  assert(id_assoc_res.second);
}

auto GeneratedOrdersRegistryImpl::update(Storage::iterator stored_it,
                                         OrderData::Patch&& patch) -> void {
  assert(stored_it != std::end(storage_));

  remove_by_owner_association(stored_it);

  // Remove existent by-id association
  auto by_id_assoc_it =
      by_identifier_assoc_.find(stored_it->get_order_id().value());
  assert(by_id_assoc_it != std::end(by_identifier_assoc_));
  by_identifier_assoc_.erase(by_id_assoc_it);

  stored_it->apply(std::move(patch));

  const std::string_view owner_key{stored_it->get_owner_id().value()};
  by_owner_assoc_.emplace(owner_key, stored_it);

  const std::string_view id_key{stored_it->get_order_id().value()};
  auto id_assoc = std::make_pair(id_key, stored_it);
  [[maybe_unused]]
  auto by_id_assoc_res = by_identifier_assoc_.emplace(std::move(id_assoc));
  assert(by_id_assoc_res.second);
}

auto GeneratedOrdersRegistryImpl::remove(Storage::iterator stored_it) -> void {
  assert(stored_it != std::end(storage_));

  by_identifier_assoc_.erase(stored_it->get_order_id().value());

  remove_by_owner_association(stored_it);

  storage_.erase(stored_it);
}

auto GeneratedOrdersRegistryImpl::remove_by_owner_association(
    Storage::iterator stored_it) -> void {
  assert(stored_it != std::end(storage_));

  const std::string_view owner_key{stored_it->get_owner_id().value()};
  auto [owner_begin, owner_end] = by_owner_assoc_.equal_range(owner_key);
  for (auto it = owner_begin; it != owner_end; ++it) {
    if (it->second == stored_it) {
      by_owner_assoc_.erase(it);
      break;
    }
  }
}

auto GeneratedOrdersRegistryImpl::lookup_in(
    const IdentifierHashTable& hashtable, std::string_view key)
    -> std::optional<GeneratedOrdersRegistryImpl::Storage::iterator> {
  auto target_it = hashtable.find(key);
  return target_it == std::end(hashtable)
             ? std::nullopt
             : std::make_optional(target_it->second);
}

auto GeneratedOrdersRegistryImpl::lookup_first_in(
    const OwnerHashTable& hashtable, std::string_view key)
    -> std::optional<GeneratedOrdersRegistryImpl::Storage::iterator> {
  auto target_it = hashtable.find(key);
  return target_it == std::end(hashtable)
             ? std::nullopt
             : std::make_optional(target_it->second);
}

auto GeneratedOrdersRegistryImpl::lookup_all_in(const OwnerHashTable& hashtable,
                                                std::string_view key)
    -> std::vector<GeneratedOrdersRegistryImpl::Storage::iterator> {
  std::vector<Storage::iterator> result;
  auto [begin, end] = hashtable.equal_range(key);

  for (auto it = begin; it != end; ++it) {
    result.push_back(it->second);
  }

  return result;
}

}  // namespace simulator::generator
