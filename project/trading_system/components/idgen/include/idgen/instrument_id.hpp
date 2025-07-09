#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_INSTRUMENT_ID_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_INSTRUMENT_ID_HPP_

#include <memory>

#include "common/attributes.hpp"
#include "idgen/errors.hpp"

namespace simulator::trading_system::idgen {

struct InstrumentIdContext {
  struct Implementation;

  explicit InstrumentIdContext(std::unique_ptr<Implementation> impl) noexcept;

  InstrumentIdContext(const InstrumentIdContext&) = delete;
  InstrumentIdContext(InstrumentIdContext&&) noexcept = default;
  ~InstrumentIdContext() noexcept;

  auto operator=(const InstrumentIdContext&) -> InstrumentIdContext& = delete;
  auto operator=(InstrumentIdContext&&) noexcept
      -> InstrumentIdContext& = default;

  [[nodiscard]]
  auto implementation() & noexcept -> Implementation&;

 private:
  std::unique_ptr<Implementation> impl_;
};

// Initialize new InstrumentId generation context.
// May throw std::bad_alloc if it was not possible to allocate an implementation
// for a new context
// NOTE: requires allocation, if you want to reset an existing context,
//       use `reset` function
[[nodiscard]]
auto make_instrument_id_generation_ctx() -> InstrumentIdContext;

// Reset passed instrument generation context to its default state
auto reset_generation_ctx(InstrumentIdContext& ctx) noexcept -> void;

// Generate new instrument identifier with passed generation context
// GenerationError::CollisionDetected is returned when a new generated
// identifier would conflict with previously generated id
[[nodiscard]]
auto generate_new_id(InstrumentIdContext& ctx) noexcept
    -> ExpectedId<InstrumentId>;

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_IDGEN_INSTRUMENT_ID_HPP_
