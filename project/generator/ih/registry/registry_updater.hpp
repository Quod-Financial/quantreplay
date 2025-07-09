#ifndef SIMULATOR_GENERATOR_IH_REGISTRY_REGISTRY_UPDATER_HPP_
#define SIMULATOR_GENERATOR_IH_REGISTRY_REGISTRY_UPDATER_HPP_

#include <functional>

#include "ih/adaptation/generated_message.hpp"
#include "ih/registry/generated_orders_registry.hpp"

namespace simulator::generator {

class OrderRegistryUpdater {
 public:
  explicit OrderRegistryUpdater(GeneratedOrdersRegistry& registry) noexcept;

  static auto update(GeneratedOrdersRegistry& registry,
                     const GeneratedMessage& message) -> void;

  auto update(const GeneratedMessage& message) -> void;

 private:
  auto handle_new_order(const GeneratedMessage& message) -> void;

  auto handle_modification(const GeneratedMessage& message) -> void;

  auto handle_cancellation(const GeneratedMessage& message) -> void;

  auto handle_execution(const GeneratedMessage& message) -> void;

  auto registry() noexcept -> GeneratedOrdersRegistry&;

  [[nodiscard]]
  static auto contains_resting_order(const GeneratedMessage& message) -> bool;

  static auto validate_new_order(const GeneratedMessage& message) -> void;

  static auto validate_modification(const GeneratedMessage& message) -> void;

  static auto validate_cancellation(const GeneratedMessage& message) -> void;

  static auto validate_execution(const GeneratedMessage& message) -> void;

  std::reference_wrapper<GeneratedOrdersRegistry> registry_ref_;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_REGISTRY_REGISTRY_UPDATER_HPP_
