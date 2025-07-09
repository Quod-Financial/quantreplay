#ifndef SIMULATOR_IDGEN_IH_CONTEXTS_EXECUTION_ID_CONTEXT_HPP_
#define SIMULATOR_IDGEN_IH_CONTEXTS_EXECUTION_ID_CONTEXT_HPP_

#include <limits>
#include <tl/expected.hpp>

#include "common/attributes.hpp"
#include "idgen/errors.hpp"
#include "idgen/execution_id.hpp"
#include "ih/sequences/numeric_sequence.hpp"

namespace simulator::trading_system::idgen {

struct ExecutionIdContext::Implementation {
 private:
  constexpr inline static std::uint64_t first = 1;
  constexpr inline static std::uint64_t last =
      std::numeric_limits<std::uint64_t>::max();

 public:
  using CounterSequence = numeric::Sequence<std::uint64_t, first, last>;
  using CounterSequenceError = numeric::SequenceError;

  Implementation() = delete;

  Implementation(OrderId target_order_id,
                 CounterSequence counter_sequence) noexcept
      : target_order_id_(target_order_id),
        counter_sequence_(counter_sequence) {}

  [[nodiscard]]
  auto target_order_id() const noexcept -> OrderId {
    return target_order_id_;
  }

  [[nodiscard]]
  auto counter_sequence() const noexcept
      -> tl::expected<CounterSequence, CounterSequenceError> {
    return counter_sequence_;
  }

  auto set_counter_sequence(CounterSequence new_sequence) noexcept -> void {
    counter_sequence_ = new_sequence;
  }

  auto set_counter_sequence_error(CounterSequenceError error) noexcept -> void {
    counter_sequence_ = tl::make_unexpected(error);
  }

 private:
  OrderId target_order_id_;
  tl::expected<CounterSequence, CounterSequenceError> counter_sequence_;
};

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_IDGEN_IH_CONTEXTS_EXECUTION_ID_CONTEXT_HPP_