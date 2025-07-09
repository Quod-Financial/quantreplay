#ifndef SIMULATOR_IDGEN_IH_SEQUENCES_COMMON_HPP_
#define SIMULATOR_IDGEN_IH_SEQUENCES_COMMON_HPP_

#include <concepts>

namespace simulator::trading_system::idgen {

template <typename T>
concept SequenceCounter = std::is_unsigned_v<T> && !std::same_as<T, bool>;

template <typename T>
concept Sequence = requires(T sequence) {
  typename T::counter_type;
  requires SequenceCounter<typename T::counter_type>;
  { sequence.current() } -> SequenceCounter;
};

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_IDGEN_IH_SEQUENCES_COMMON_HPP_
