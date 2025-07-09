#ifndef SIMULATOR_INSTRUMENTS_IH_LOOKUP_MATCH_RATE_HPP_
#define SIMULATOR_INSTRUMENTS_IH_LOOKUP_MATCH_RATE_HPP_

#include <cstdint>
#include <limits>

namespace simulator::trading_system::instrument::lookup {

class MatchRate {
 public:
  static const MatchRate NoMatch;
  static const MatchRate Unmatchable;
  static const MatchRate Match;

  MatchRate() = delete;

  constexpr explicit MatchRate(const std::uint32_t rate)
      : MatchRate(fit(rate)) {}

  [[nodiscard]]
  constexpr auto
  operator==(const MatchRate& other) const -> bool = default;

  [[nodiscard]]
  constexpr auto
  operator<=>(const MatchRate& other) const = default;

  [[nodiscard]]
  constexpr auto
  operator+(const MatchRate& other) const -> MatchRate {
    if (*this == NoMatch || other == NoMatch) {
      return NoMatch;
    }

    constexpr auto max = std::numeric_limits<std::int32_t>::max();
    const bool overflows = max - rate_ < other.rate_;
    return MatchRate(overflows ? max : rate_ + other.rate_);
  }

 private:
  constexpr static auto fit(const std::uint32_t rate) -> std::int32_t {
    constexpr auto max = std::numeric_limits<std::int32_t>::max();
    return rate < static_cast<std::uint32_t>(max)
               ? static_cast<std::int32_t>(rate)
               : max;
  }

  constexpr explicit MatchRate(const std::int32_t rate) : rate_{rate} {}

  std::int32_t rate_;
};

constexpr MatchRate MatchRate::NoMatch{-1};
constexpr MatchRate MatchRate::Unmatchable{0};
constexpr MatchRate MatchRate::Match{1};

}  // namespace simulator::trading_system::instrument::lookup

#endif  // SIMULATOR_INSTRUMENTS_IH_LOOKUP_MATCH_RATE_HPP_
