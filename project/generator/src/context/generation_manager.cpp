#include "ih/context/generation_manager.hpp"

#include <fmt/format.h>

#include <chrono>
#include <list>

#include "ih/constants.hpp"
#include "log/logging.hpp"

namespace simulator::generator {

GenerationState::GenerationState(bool activate_on_creation) noexcept
    : state{activate_on_creation ? State::Active : State::Suspended} {}

auto GenerationState::is_running() const noexcept -> bool {
  return state.load() == State::Active;
}

auto GenerationState::set_running() noexcept -> void {
  if (!is_terminated()) {
    state = State::Active;
  }
}

auto GenerationState::set_stopped() noexcept -> void {
  if (!is_terminated()) {
    state = State::Suspended;
  }
}

auto GenerationState::is_terminated() const noexcept -> bool {
  return state.load() == State::Terminated;
}

auto GenerationState::set_terminated() noexcept -> void {
  state = State::Terminated;
}

auto GeneratedMessagesCounter::increment_order_messages_count() noexcept
    -> std::size_t {
  return ++num_order_messages_generated;
}

IdentifierGenerator::IdentifierGenerator()
    : next_identifier{static_cast<std::size_t>(
          std::chrono::system_clock::now().time_since_epoch().count())} {}

auto IdentifierGenerator::generate_identifier() noexcept -> std::string {
  return fmt::format("SIM-{}", next_identifier++);
}

GenerationManager::GenerationManager(data_layer::Venue target_venue)
    : target_venue_{std::move(target_venue)},
      generation_state_{target_venue_.order_on_startup_flag().value_or(
          constant::DefaultVenueOrderOnStartup)} {}

auto GenerationManager::create(const data_layer::Venue& target_venue)
    -> std::shared_ptr<GenerationManager> {
  return std::make_shared<GenerationManager>(target_venue);
}

auto GenerationManager::is_component_running() const noexcept -> bool {
  return generation_state_.is_running();
}

auto GenerationManager::get_venue() const noexcept -> const data_layer::Venue& {
  return target_venue_;
}

auto GenerationManager::generate_identifier() noexcept -> std::string {
  return id_generator_.generate_identifier();
}

auto GenerationManager::next_generated_order_message_number() noexcept
    -> std::size_t {
  return messages_counter_.increment_order_messages_count();
}

auto GenerationManager::call_on_launch(const OnStartupCallback& callback)
    -> void {
  if (!generation_state_.is_terminated()) {
    [[maybe_unused]] const std::unique_lock<std::mutex> lock{
        events_listeners_mutex_};

    launch_event_listeners_.emplace_back(callback);
  }
}

auto GenerationManager::launch() -> void {
  if (generation_state_.is_running()) {
    return;
  }

  if (generation_state_.is_terminated()) {
    log::warn(
        "unable to launch generation, as it has been terminated previously");
    return;
  }

  generation_state_.set_running();

  std::list<OnStartupCallback> pending_callbacks{};
  {
    [[maybe_unused]] const std::unique_lock<std::mutex> lock{
        events_listeners_mutex_};

    pending_callbacks = std::move(launch_event_listeners_);
    launch_event_listeners_.clear();
  }
  for (const auto& on_launch_callback : pending_callbacks) {
    on_launch_callback();
  }
}

auto GenerationManager::suspend() -> void { generation_state_.set_stopped(); }

auto GenerationManager::terminate() noexcept -> void {
  generation_state_.set_terminated();
}

}  // namespace simulator::generator
