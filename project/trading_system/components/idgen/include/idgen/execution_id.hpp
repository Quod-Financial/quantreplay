#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_EXECUTION_ID_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_EXECUTION_ID_HPP_

#include <memory>

#include "common/attributes.hpp"
#include "core/domain/attributes.hpp"
#include "idgen/errors.hpp"

namespace simulator::trading_system::idgen {

struct ExecutionIdContext {
  struct Implementation;

  explicit ExecutionIdContext(std::unique_ptr<Implementation> impl) noexcept;

  ExecutionIdContext(const ExecutionIdContext&) = delete;
  ExecutionIdContext(ExecutionIdContext&&) noexcept;

  auto operator=(const ExecutionIdContext&) -> ExecutionIdContext& = delete;
  auto operator=(ExecutionIdContext&&) noexcept
      -> ExecutionIdContext& = default;

  ~ExecutionIdContext() noexcept;

  [[nodiscard]]
  auto implementation() & noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

// Initialize new ExecutionId generation context for the given order identifier.
[[nodiscard]]
auto make_execution_id_generation_ctx(OrderId exec_order_id)
    -> ExecutionIdContext;

// Generate new execution identifier.
// It's guaranteed that a unique execution identifier is generated no more than
// 'max(std::uint64_t) - 1' times per a given order identifier.
// After each unique identifier is generated, the function returns an error,
// and the context is no more usable.
[[nodiscard]]
auto generate_new_id(ExecutionIdContext& ctx) noexcept
    -> ExpectedId<ExecutionId>;

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_EXECUTION_ID_HPP_