#ifndef SIMULATOR_IDGEN_IH_CONTEXTS_MARKET_ENTRY_ID_CONTEXT_HPP_
#define SIMULATOR_IDGEN_IH_CONTEXTS_MARKET_ENTRY_ID_CONTEXT_HPP_

#include <chrono>

#include "idgen/market_entry_id.hpp"
#include "ih/sequences/numeric_sequence.hpp"

namespace simulator::trading_system::idgen {

struct MarketEntryIdContext::Implementation {
 private:
  constexpr static std::uint64_t first = 1;
  constexpr static std::uint64_t last =
      std::numeric_limits<std::uint64_t>::max();

 public:
  using CounterSequence = numeric::Sequence<std::uint64_t, first, last>;

  Implementation() = delete;

  Implementation(std::uint64_t seed, CounterSequence counter_sequence) noexcept
      : seed_(seed), counter_sequence_(counter_sequence) {}

  [[nodiscard]]
  auto seed() const noexcept -> std::uint64_t {
    return seed_;
  }

  [[nodiscard]]
  auto counter_sequence() const noexcept -> CounterSequence {
    return counter_sequence_;
  }

  auto set_counter_sequence(CounterSequence new_sequence) noexcept -> void {
    counter_sequence_ = new_sequence;
  }

  auto set_seed(std::uint64_t seed) noexcept -> void { seed_ = seed; }

  static auto generate_seed() -> std::uint64_t {
    using std::chrono::seconds;
    using std::chrono::system_clock;

    return static_cast<std::uint64_t>(
        floor<seconds>(system_clock::now().time_since_epoch()).count());
  }

 private:
  std::uint64_t seed_;
  CounterSequence counter_sequence_;
};

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_IDGEN_IH_CONTEXTS_MARKET_ENTRY_ID_CONTEXT_HPP_
