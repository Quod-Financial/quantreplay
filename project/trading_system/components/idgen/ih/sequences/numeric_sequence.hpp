#ifndef SIMULATOR_IDGEN_IH_SEQUENCES_NUMERIC_SEQUENCE_HPP_
#define SIMULATOR_IDGEN_IH_SEQUENCES_NUMERIC_SEQUENCE_HPP_

#include <cassert>
#include <limits>
#include <tl/expected.hpp>
#include <type_traits>

#include "ih/sequences/common.hpp"

namespace simulator::trading_system::idgen {

template <typename T>
concept NumericSequence = requires(T numeric_sequence) {
  typename T::counter_type;
  requires SequenceCounter<typename T::counter_type>;
  { numeric_sequence.current() } -> SequenceCounter;
  { T::first() } -> SequenceCounter;
  { T::last() } -> SequenceCounter;
};

namespace numeric {

enum class SequenceError : std::uint8_t { Overflow };

template <typename T, T FirstV, T LastV>
struct Sequence {
  using counter_type = T;

  Sequence() = delete;

  constexpr explicit Sequence(counter_type counter) noexcept
      : counter_(counter) {}

  static_assert(
      (FirstV <= LastV) && (LastV <= std::numeric_limits<counter_type>::max()),
      "numeric::Sequence<T, FirstV, LastV> template "
      "is instantiated with bad FirstV and LastV values. "
      "FirstV <= LastV <= numeric_limits<Seq>::max() evaluates to false");

  [[nodiscard]] consteval static auto first() -> counter_type { return FirstV; }

  [[nodiscard]] consteval static auto last() -> counter_type { return LastV; }

  // Returns current counter value, which is being hold by this sequence
  [[nodiscard]] auto current() const noexcept -> counter_type {
    return counter_;
  }

 private:
  counter_type counter_;
};

}  // namespace numeric

// Create a numeric sequence with a start value as a counter value
template <typename Seq>
  requires NumericSequence<Seq>
[[nodiscard]] auto make_sequence() noexcept -> Seq {
  return Seq{Seq::first()};
}

// Increment a numeric sequence by 1
template <typename Seq>
  requires NumericSequence<Seq>
[[nodiscard]] auto increment(Seq const& sequence) noexcept
    -> tl::expected<Seq, numeric::SequenceError> {
  using counter_type = typename Seq::counter_type;

  counter_type const current = sequence.current();
  if (current < Seq::last()) {
    return Seq{static_cast<counter_type>(current + 1)};
  }
  return tl::make_unexpected(numeric::SequenceError::Overflow);
}

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_IDGEN_IH_SEQUENCES_NUMERIC_SEQUENCE_HPP_
