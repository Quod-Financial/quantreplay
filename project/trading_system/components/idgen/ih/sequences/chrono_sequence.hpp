#ifndef SIMULATOR_IDGEN_IH_SEQUENCES_CHRONO_SEQUENCE_HPP_
#define SIMULATOR_IDGEN_IH_SEQUENCES_CHRONO_SEQUENCE_HPP_

#include <cassert>
#include <chrono>
#include <cstdint>
#include <tl/expected.hpp>

#include "core/common/meta.hpp"
#include "ih/sequences/common.hpp"

namespace simulator::trading_system::idgen {

namespace chrono {

enum class SequenceFormat : std::uint8_t { Timestamp, Date };

enum class SequenceError : std::uint8_t { NegativeTick };

}  // namespace chrono

template <typename T>
concept ChronoSequence = Sequence<T> && requires(T chrono_sequence) {
  typename T::tick_type;
  requires core::ChronoDuration<typename T::tick_type>;
  requires std::same_as<typename T::counter_type, std::uint64_t>;
  requires std::
      constructible_from<T, typename T::counter_type, typename T::tick_type>;
  { T::format() } -> std::same_as<chrono::SequenceFormat>;
  { chrono_sequence.tick() } -> std::same_as<typename T::tick_type>;
};

namespace chrono {

template <SequenceFormat>
struct sequence_precision {};

template <>
struct sequence_precision<SequenceFormat::Timestamp> {
  using tick_type = std::chrono::seconds;
};

template <>
struct sequence_precision<SequenceFormat::Date> {
  using tick_type = std::chrono::days;
};

template <SequenceFormat Format>
struct Sequence {
  using counter_type = std::uint64_t;
  using tick_type = typename sequence_precision<Format>::tick_type;

  [[nodiscard]] consteval static auto format() -> SequenceFormat {
    return Format;
  }

  Sequence() = delete;

  Sequence(counter_type cached_counter, tick_type cached_tick) noexcept
      : cached_tick_(cached_tick), cached_counter_(cached_counter) {}

  [[nodiscard]] auto current() const noexcept -> counter_type {
    return cached_counter_;
  }

  [[nodiscard]] auto tick() const noexcept -> tick_type { return cached_tick_; }

 private:
  tick_type cached_tick_;
  counter_type cached_counter_;
};

}  // namespace chrono

namespace detail {

// Error code to indicate that a given negative tick numer
// can not be composed into date/timestamp integer
constexpr inline auto ErrNegative = std::numeric_limits<std::uint64_t>::max();

// Compose YYMMDD integer from given UNIX tick value
// An ErrCompose is returned when given tick value is negative
template <typename Duration>
  requires core::ChronoDuration<Duration>
[[nodiscard]] constexpr auto compose_date_int(Duration tick) noexcept
    -> std::uint64_t {
  using std::chrono::floor;
  using std::chrono::sys_days;
  using std::chrono::year_month_day;

  if (tick < Duration{}) {
    return ErrNegative;
  }

  const year_month_day date{sys_days{floor<std::chrono::days>(tick)}};
  assert(date.ok());

  const auto year = static_cast<std::uint32_t>(static_cast<int>(date.year()));
  const auto month = static_cast<std::uint32_t>(date.month());
  const auto day = static_cast<std::uint32_t>(date.day());

  // NOLINTBEGIN(*-magic-numbers)
  std::uint64_t composed = year % 100;

  composed *= 100;
  composed += month;

  composed *= 100;
  composed += day;
  // NOLINTEND(*-magic-numbers)

  return composed;
}

// Compose YYMMDDhhmmss integer from given UNIX tick value
// An ErrCompose is returned when given tick value is negative
template <typename Duration>
  requires core::ChronoDuration<Duration>
[[nodiscard]] constexpr auto compose_timestamp_int(Duration tick) noexcept
    -> std::uint64_t {
  using std::chrono::days;
  using std::chrono::floor;
  using std::chrono::hh_mm_ss;
  using std::chrono::seconds;
  using std::chrono::year_month_day;

  std::uint64_t composed_timestamp = compose_date_int(tick);
  if (composed_timestamp == ErrNegative) {
    return ErrNegative;
  }

  // It is expected that check on negative tick is done by compose_date_integer
  assert(tick >= Duration{});

  const hh_mm_ss<seconds> daytime{floor<seconds>(tick - floor<days>(tick))};
  assert(!daytime.is_negative());  // tick is positive -> daytime is positive

  const auto hour = static_cast<std::uint32_t>(daytime.hours().count());
  const auto minute = static_cast<std::uint32_t>(daytime.minutes().count());
  const auto second = static_cast<std::uint32_t>(daytime.seconds().count());

  // NOLINTBEGIN(*-magic-numbers)
  composed_timestamp *= 100;
  composed_timestamp += hour;

  composed_timestamp *= 100;
  composed_timestamp += minute;

  composed_timestamp *= 100;
  composed_timestamp += second;
  // NOLINTEND(*-magic-numbers)

  return composed_timestamp;
}

// Composes given tick value into integer according to a given precision
// ErrCompose value is returned when tick value is negative
template <chrono::SequenceFormat Fmt, typename Duration>
  requires core::ChronoDuration<Duration>
[[nodiscard]] auto compose_chrono_int(Duration tick) noexcept -> std::uint64_t {
  const std::uint64_t composed = [tick] {
    if constexpr (Fmt == chrono::SequenceFormat::Timestamp) {
      return detail::compose_timestamp_int(tick);
    } else if constexpr (Fmt == chrono::SequenceFormat::Date) {
      return detail::compose_date_int(tick);
    }
  }();
  return composed;
}

template <typename Tick>
  requires core::ChronoDuration<Tick>
[[nodiscard]] auto current_tick() noexcept -> Tick {
  using std::chrono::system_clock;
  return std::chrono::floor<Tick>(system_clock::now().time_since_epoch());
}

}  // namespace detail

// Create a chrono sequence with a specified tick value
// chrono::SequenceError::NegativeTick is returned
// when given current_tick value is negative
template <typename Seq, typename Tick>
  requires ChronoSequence<Seq> && core::ChronoDuration<Tick>
[[nodiscard]] auto make_sequence(Tick current_tick) noexcept
    -> tl::expected<Seq, chrono::SequenceError> {
  using tick_type = typename Seq::tick_type;
  constexpr chrono::SequenceFormat format = Seq::format();

  const tick_type tick = std::chrono::floor<tick_type>(current_tick);
  const std::uint64_t counter = detail::compose_chrono_int<format>(tick);
  if (counter != detail::ErrNegative) {
    return Seq{counter, tick};
  }

  // Chrono integer composition may fail when a negative current_tick is given
  return tl::make_unexpected(chrono::SequenceError::NegativeTick);
}

// Create a chrono sequence with a current (realtime) tick value
// chrono::SequenceError::NegativeTick is returned
// when current tick value is negative
template <typename Seq>
  requires ChronoSequence<Seq>
[[nodiscard]] auto make_sequence() noexcept
    -> tl::expected<Seq, chrono::SequenceError> {
  return make_sequence<Seq>(detail::current_tick<typename Seq::tick_type>());
}

// Update a chrono sequence according to a current_tick value
// chrono::SequenceError::NegativeTick is returned
// when current tick value is negative and is greater than a cached tick value
template <typename Seq, typename Tick>
  requires ChronoSequence<Seq> && core::ChronoDuration<Tick>
[[nodiscard]] auto increment(const Seq& sequence, Tick current_tick) noexcept
    -> tl::expected<Seq, chrono::SequenceError> {
  if (sequence.tick() < current_tick) {
    return make_sequence<Seq>(current_tick);
  }
  return sequence;
}

// Update a chrono sequence according to a current (realtime) tick value
// chrono::SequenceError::NegativeTick is returned
// when current tick value is negative and is greater than a cached tick value
template <typename Seq>
  requires ChronoSequence<Seq>
[[nodiscard]] auto increment(const Seq& sequence) noexcept
    -> tl::expected<Seq, chrono::SequenceError> {
  return increment(sequence, detail::current_tick<typename Seq::tick_type>());
}

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_IDGEN_IH_SEQUENCES_CHRONO_SEQUENCE_HPP_
