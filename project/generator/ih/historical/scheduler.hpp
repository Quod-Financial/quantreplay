#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_SCHEDULER_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_SCHEDULER_HPP_

#include <chrono>
#include <deque>
#include <functional>
#include <memory>

#include "ih/historical/data/provider.hpp"
#include "ih/historical/data/record.hpp"

namespace simulator::generator::historical {

class Scheduler {
 public:
  using ActionProcessor = std::function<void(historical::Action)>;

  Scheduler() = default;

  Scheduler(const Scheduler&) = default;
  auto operator=(const Scheduler&) -> Scheduler& = default;

  Scheduler(Scheduler&&) = default;
  auto operator=(Scheduler&&) -> Scheduler& = default;

  virtual ~Scheduler() = default;

  [[nodiscard]]
  virtual auto finished() const noexcept -> bool = 0;

  virtual auto initialize() noexcept -> void = 0;

  virtual auto process_next_action(const ActionProcessor& processor)
      -> void = 0;

  [[nodiscard]]
  virtual auto next_action_timeout() const -> std::chrono::microseconds = 0;
};

class ActionsScheduler final : public Scheduler {
 public:
  ActionsScheduler() = delete;

  explicit ActionsScheduler(std::unique_ptr<DataProvider> provider);

  [[nodiscard]]
  auto finished() const noexcept -> bool override;

  auto initialize() noexcept -> void override;

  auto process_next_action(const ActionProcessor& processor) -> void override;

  [[nodiscard]]
  auto next_action_timeout() const -> std::chrono::microseconds override;

 private:
  [[nodiscard]]
  auto has_data_provider() const noexcept -> bool;

  [[nodiscard]]
  auto can_pull_action() const noexcept -> bool;

  [[nodiscard]]
  auto has_pending_actions() const noexcept -> bool;

  auto pull() noexcept -> void;

  auto pull_next_action() noexcept -> void;

  std::deque<historical::Action> pending_actions_;
  std::unique_ptr<historical::DataProvider> data_provider_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_SCHEDULER_HPP_
