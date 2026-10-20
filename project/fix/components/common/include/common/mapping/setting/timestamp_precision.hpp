#ifndef SIMULATOR_FIX_COMMON_MAPPING_SETTING_TIMESTAMP_PRECISION_HPP_
#define SIMULATOR_FIX_COMMON_MAPPING_SETTING_TIMESTAMP_PRECISION_HPP_

namespace simulator::fix {

class TimestampPrecision {
 public:
  static const TimestampPrecision Seconds;
  static const TimestampPrecision Milliseconds;
  static const TimestampPrecision Microseconds;
  static const TimestampPrecision Nanoseconds;

  constexpr explicit TimestampPrecision(int value) : value_{value} {}

  constexpr explicit operator int() const { return value_; }

  constexpr auto operator==(const TimestampPrecision& other) const
      -> bool = default;

 private:
  int value_;
};

constexpr TimestampPrecision TimestampPrecision::Seconds{0};
constexpr TimestampPrecision TimestampPrecision::Milliseconds{3};
constexpr TimestampPrecision TimestampPrecision::Microseconds{6};
constexpr TimestampPrecision TimestampPrecision::Nanoseconds{9};

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_MAPPING_SETTING_TIMESTAMP_PRECISION_HPP_
