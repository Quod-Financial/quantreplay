#include "ih/generation.hpp"

#include <fmt/format.h>

#include "core/common/unreachable.hpp"
#include "core/domain/attributes.hpp"
#include "ih/contexts/execution_id_context.hpp"
#include "ih/contexts/instrument_id_context.hpp"
#include "ih/contexts/market_entry_id_context.hpp"
#include "ih/contexts/order_id_context.hpp"
#include "ih/sequences/chrono_sequence.hpp"
#include "ih/sequences/numeric_sequence.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::idgen {

auto generate_identifier(
    const InstrumentIdContext::Implementation& ctx) noexcept
    -> ExpectedId<InstrumentId> {
  if (auto sequence = ctx.counter_sequence()) [[likely]] {
    return InstrumentId{sequence->current()};
  }
  return tl::make_unexpected(GenerationError::CollisionDetected);
}

auto update_counter_sequence(InstrumentIdContext::Implementation& ctx) noexcept
    -> void {
  if (auto sequence = ctx.counter_sequence()) {
    const auto incremented_sequence = increment(*sequence);
    incremented_sequence.has_value()
        ? ctx.set_sequence(*incremented_sequence)
        : ctx.set_sequence_error(numeric::SequenceError::Overflow);
  }
  // Noop, when context has a sequence in error state already.
}

auto generate_identifier(const OrderIdContext::Implementation& ctx) noexcept
    -> OrderId {
  // Compose an identifier in YYMMDDhhmmssCCCCCC format.
  // The context must be updated before generation
  // to provide a correct time component.
  constexpr std::uint64_t time_component_multiplier = 1'000'000;
  const std::uint64_t identifier =
      ctx.time_sequence().current() * time_component_multiplier +
      ctx.counter_sequence().current();
  return OrderId{identifier};
}

auto update_time_sequence(OrderIdContext::Implementation& ctx) noexcept
    -> void {
  using context_type = OrderIdContext::Implementation;

  // Update time_sequence based on the current system time
  const auto time_sequence = [&ctx]() -> context_type::TimeSequence {
    if (auto sequence = increment(ctx.time_sequence())) [[likely]] {
      return *sequence;
    }
    // The sequence is incremented using the system time, error must not occur
    core::unreachable();
  }();

  ctx.set_time_sequence(time_sequence);
}

auto update_counter_sequence(OrderIdContext::Implementation& ctx) noexcept
    -> void {
  using context_type = OrderIdContext::Implementation;

  // Increment/reset counter_sequence
  const auto counter_sequence = [&ctx]() -> context_type::CounterSequence {
    if (auto sequence = increment(ctx.counter_sequence())) {
      return *sequence;
    }
    return make_sequence<context_type::CounterSequence>();
  }();

  ctx.set_counter_sequence(counter_sequence);
}

auto generate_identifier(const ExecutionIdContext::Implementation& ctx) noexcept
    -> ExpectedId<ExecutionId> {
  if (auto sequence = ctx.counter_sequence()) [[likely]] {
    return ExecutionId{
        fmt::format("{}-{}", ctx.target_order_id(), sequence->current())};
  }
  return tl::make_unexpected(GenerationError::CollisionDetected);
}

auto update_counter_sequence(ExecutionIdContext::Implementation& ctx) noexcept
    -> void {
  if (auto sequence = ctx.counter_sequence()) {
    const auto incremented_sequence = increment(*sequence);
    incremented_sequence.has_value()
        ? ctx.set_counter_sequence(*incremented_sequence)
        : ctx.set_counter_sequence_error(numeric::SequenceError::Overflow);
  }
  // Noop, when context has a sequence in error state already.
}

auto generate_identifier(const MarketEntryIdContext::Implementation& ctx)
    -> MarketEntryId {
  return MarketEntryId{
      fmt::format("{}:{}", ctx.seed(), ctx.counter_sequence().current())};
}

auto update_counter_sequence(MarketEntryIdContext::Implementation& ctx)
    -> void {
  if (const auto incremented_sequence = increment(ctx.counter_sequence())) {
    ctx.set_counter_sequence(*incremented_sequence);
    return;
  }

  using context_type = MarketEntryIdContext::Implementation;
  using sequence_type = context_type::CounterSequence;
  ctx.set_seed(context_type::generate_seed());
  ctx.set_counter_sequence(make_sequence<sequence_type>());

  log::debug("[idgen] market entry identifier context has been reset");
}

}  // namespace simulator::trading_system::idgen