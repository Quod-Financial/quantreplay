#ifndef SIMULATOR_TRADING_SYSTEM_IH_REPOSITORY_REPOSITORY_ACCESSOR_HPP_
#define SIMULATOR_TRADING_SYSTEM_IH_REPOSITORY_REPOSITORY_ACCESSOR_HPP_

#include <concepts>
#include <functional>
#include <memory>
#include <tl/function_ref.hpp>

#include "common/attributes.hpp"

namespace simulator::trading_system {

class TradingEngine;
class TradingEnginesRepository;

class RepositoryAccessor {
 public:
  using Action = tl::function_ref<void(TradingEngine&)>;

  RepositoryAccessor() = default;
  RepositoryAccessor(const RepositoryAccessor&) = default;
  RepositoryAccessor(RepositoryAccessor&&) noexcept = default;
  virtual ~RepositoryAccessor() = default;

  // clang-format off
  auto operator=(const RepositoryAccessor&)
      -> RepositoryAccessor& = default;
  auto operator=(RepositoryAccessor&&) noexcept
      -> RepositoryAccessor& = default;
  // clang-format on

  template <typename ActionType>
  auto unicast(InstrumentId instrument_id, ActionType&& action) const -> void {
    unicast_impl(instrument_id, std::forward<ActionType>(action));
  }

  template <typename ActionType>
  auto broadcast(ActionType&& action) const -> void {
    broadcast_impl(std::forward<ActionType>(action));
  }

  static auto create(const TradingEnginesRepository& repository)
      -> std::unique_ptr<RepositoryAccessor>;

 private:
  virtual auto unicast_impl(InstrumentId instrument_id,
                            Action action) const -> void = 0;

  virtual auto broadcast_impl(Action action) const -> void = 0;
};

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_IH_REPOSITORY_REPOSITORY_ACCESSOR_HPP_