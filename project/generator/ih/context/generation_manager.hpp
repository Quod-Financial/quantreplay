#ifndef SIMULATOR_GENERATOR_IH_CONTEXT_GENERATION_MANAGER_HPP_
#define SIMULATOR_GENERATOR_IH_CONTEXT_GENERATION_MANAGER_HPP_

#include <atomic>
#include <cstddef>
#include <list>
#include <memory>
#include <mutex>
#include <string>

#include "data_layer/api/models/venue.hpp"
#include "ih/context/component_context.hpp"

namespace simulator::generator {

class GenerationState {
  enum class State { Active, Suspended, Terminated };

 public:
  GenerationState() = delete;

  explicit GenerationState(bool activate_on_creation) noexcept;

  [[nodiscard]]
  auto is_running() const noexcept -> bool;

  auto set_running() noexcept -> void;

  auto set_stopped() noexcept -> void;

  [[nodiscard]]
  auto is_terminated() const noexcept -> bool;

  auto set_terminated() noexcept -> void;

 private:
  std::atomic<State> state{State::Suspended};
};

class GeneratedMessagesCounter {
 public:
  auto increment_order_messages_count() noexcept -> std::size_t;

 private:
  std::atomic<std::size_t> num_order_messages_generated{0};
};

class IdentifierGenerator {
 public:
  IdentifierGenerator();

  auto generate_identifier() noexcept -> std::string;

 private:
  std::atomic<std::size_t> next_identifier;
};

class GenerationManager : public ComponentContext {
 public:
  GenerationManager() = delete;

  explicit GenerationManager(data_layer::Venue target_venue);

  static auto create(const data_layer::Venue& target_venue)
      -> std::shared_ptr<GenerationManager>;

  [[nodiscard]]
  auto is_component_running() const noexcept -> bool override;

  [[nodiscard]]
  auto get_venue() const noexcept -> const data_layer::Venue& override;

  auto generate_identifier() noexcept -> std::string override;

  auto next_generated_order_message_number() noexcept -> std::size_t override;

  auto call_on_launch(const OnStartupCallback& callback) -> void override;

  auto launch() -> void;

  auto suspend() -> void;

  auto terminate() noexcept -> void;

 private:
  data_layer::Venue target_venue_;

  std::list<OnStartupCallback> launch_event_listeners_;
  std::mutex events_listeners_mutex_;

  GeneratedMessagesCounter messages_counter_;
  IdentifierGenerator id_generator_;
  GenerationState generation_state_;

  bool terminated_{false};
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_CONTEXT_GENERATION_MANAGER_HPP_
