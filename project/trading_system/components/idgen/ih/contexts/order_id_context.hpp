#ifndef SIMULATOR_IDGEN_IH_CONTEXTS_ORDER_ID_CONTEXT_HPP_
#define SIMULATOR_IDGEN_IH_CONTEXTS_ORDER_ID_CONTEXT_HPP_

#include "idgen/order_id.hpp"
#include "ih/sequences/chrono_sequence.hpp"
#include "ih/sequences/numeric_sequence.hpp"

namespace simulator::trading_system::idgen {

struct OrderIdContext::Implementation {
  using CounterSequence = numeric::Sequence<std::uint32_t, 0, 999999>;
  using TimeSequence = chrono::Sequence<chrono::SequenceFormat::Timestamp>;

  Implementation() noexcept = delete;

  Implementation(TimeSequence time_sequence,
                 CounterSequence counter_sequence) noexcept
      : time_sequence_(time_sequence), counter_sequence_(counter_sequence) {}

  [[nodiscard]]
  auto time_sequence() const& noexcept -> const TimeSequence& {
    return time_sequence_;
  }

  [[nodiscard]]
  auto counter_sequence() const& noexcept -> const CounterSequence& {
    return counter_sequence_;
  }

  auto set_time_sequence(TimeSequence new_sequence) noexcept -> void {
    time_sequence_ = new_sequence;
  }

  auto set_counter_sequence(CounterSequence new_sequence) noexcept -> void {
    counter_sequence_ = new_sequence;
  }

 private:
  TimeSequence time_sequence_;
  CounterSequence counter_sequence_;
};

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_IDGEN_IH_CONTEXTS_ORDER_ID_CONTEXT_HPP_