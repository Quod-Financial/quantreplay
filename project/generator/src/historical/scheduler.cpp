#include "ih/historical/scheduler.hpp"

#include <fmt/chrono.h>

#include <cassert>
#include <chrono>
#include <exception>
#include <memory>
#include <utility>

#include "ih/historical/data/provider.hpp"
#include "ih/historical/data/record.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

ActionsScheduler::ActionsScheduler(std::unique_ptr<DataProvider> provider)
    : data_provider_{std::move(provider)} {
  log::info("historical records scheduler initialized successfully");
}

auto ActionsScheduler::finished() const noexcept -> bool {
  return !has_pending_actions() && !can_pull_action();
}

auto ActionsScheduler::initialize() noexcept -> void {
  if (has_data_provider()) {
    log::debug(
        "scheduler is asked to explicitly set a time offset "
        "for historical data provider");

    assert(data_provider_);
    data_provider_->initialize_time_offset();
  }
  if (has_pending_actions()) {
    const auto new_actions_time = std::chrono::system_clock::now();
    log::debug(
        "scheduler is resetting a base time to `{}' "
        "for previously cached actions",
        new_actions_time);

    for (auto& pending_action : pending_actions_) {
      auto upd_action =
          Action::update_time(std::move(pending_action), new_actions_time);
      pending_action = std::move(upd_action);
    }
  }
}

auto ActionsScheduler::process_next_action(const ActionProcessor& processor)
    -> void {
  if (finished()) {
    return;
  }

  pull();

  if (has_pending_actions()) {
    assert(!pending_actions_.empty());
    auto next_action = std::move(pending_actions_.front());
    pending_actions_.pop_front();
    processor(std::move(next_action));
  }

  pull();
}

auto ActionsScheduler::next_action_timeout() const
    -> std::chrono::microseconds {
  using std::chrono::duration_cast;
  using std::chrono::microseconds;
  using std::chrono::system_clock;
  using std::chrono::time_point_cast;

  constexpr microseconds immediately{0};

  if (finished() || pending_actions_.empty()) {
    return immediately;
  }

  const auto action_time = pending_actions_.front().action_time();
  const auto now = time_point_cast<historical::Duration>(system_clock::now());
  return now < action_time ? duration_cast<microseconds>(action_time - now)
                           : immediately;
}

auto ActionsScheduler::has_data_provider() const noexcept -> bool {
  return data_provider_ != nullptr;
}

auto ActionsScheduler::can_pull_action() const noexcept -> bool {
  return has_data_provider() && !data_provider_->is_empty();
}

auto ActionsScheduler::has_pending_actions() const noexcept -> bool {
  return !pending_actions_.empty();
}

auto ActionsScheduler::pull() noexcept -> void {
  while (!has_pending_actions() && can_pull_action()) {
    pull_next_action();
  }
}

auto ActionsScheduler::pull_next_action() noexcept -> void {
  if (!can_pull_action()) {
    return;
  }

  if (!data_provider_->has_time_offset()) {
    data_provider_->initialize_time_offset();
  }

  try {
    data_provider_->pull_action([this](auto action) {
      pending_actions_.emplace_back(std::move(action));
    });
  } catch (const std::exception& ex) {
    log::warn(
        "an error occurred while fetching a record from "
        "a data provider: {}",
        ex.what());
  }
}

}  // namespace simulator::generator::historical
