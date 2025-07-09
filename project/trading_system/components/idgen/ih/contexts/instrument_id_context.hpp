#ifndef SIMULATOR_IDGEN_IH_CONTEXTS_INSTRUMENT_ID_CONTEXT_HPP_
#define SIMULATOR_IDGEN_IH_CONTEXTS_INSTRUMENT_ID_CONTEXT_HPP_

#include <limits>
#include <tl/expected.hpp>

#include "idgen/instrument_id.hpp"
#include "ih/sequences/numeric_sequence.hpp"

namespace simulator::trading_system::idgen {

struct InstrumentIdContext::Implementation {
 private:
  constexpr inline static std::uint64_t first = 1;
  constexpr inline static std::uint64_t last =
      std::numeric_limits<std::uint64_t>::max();

 public:
  using SequenceType = numeric::Sequence<std::uint64_t, first, last>;
  using SequenceError = numeric::SequenceError;

  Implementation() = delete;

  explicit Implementation(SequenceType sequence) noexcept
      : counter_sequence_(sequence) {}

  [[nodiscard]]
  auto counter_sequence() const noexcept
      -> tl::expected<SequenceType, SequenceError> {
    return counter_sequence_;
  }

  auto set_sequence(SequenceType new_sequence) noexcept -> void {
    counter_sequence_ = new_sequence;
  }

  auto set_sequence_error(SequenceError error) noexcept -> void {
    counter_sequence_ = tl::make_unexpected(error);
  }

 private:
  tl::expected<SequenceType, SequenceError> counter_sequence_;
};

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_IDGEN_IH_CONTEXTS_INSTRUMENT_ID_CONTEXT_HPP_
