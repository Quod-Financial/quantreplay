#ifndef SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_VALUE_GENERATOR_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_VALUE_GENERATOR_HPP_

#include <cassert>
#include <cmath>
#include <cstdint>
#include <type_traits>

namespace simulator::generator::random {

class ValueGenerator {
 protected:
  using RandomInt = std::int64_t;
  using RandomUnsignedInt = std::uint64_t;
  using RandomFloat = std::double_t;

 private:
  template <typename T>
  constexpr static bool is_signed_integer_v =
      std::is_integral_v<T> && std::is_signed_v<T>;

  template <typename T>
  constexpr static bool is_unsigned_integer_v =
      std::is_integral_v<T> && std::is_unsigned_v<T>;

  template <typename F, typename S>
  constexpr static bool both_are_signed =
      std::is_signed_v<F> && std::is_signed_v<S>;

  template <typename F, typename S>
  constexpr static bool both_are_unsigned =
      std::is_unsigned_v<F> && std::is_unsigned_v<S>;

 public:
  virtual ~ValueGenerator() = default;

  template <typename T, std::enable_if_t<is_signed_integer_v<T>>* = nullptr>
  auto generate_uniform_value(T min, T max) -> T {
    assert(min <= max);
    return get_random_uniform_value<RandomInt, T>(min, max);
  }

  template <typename T, std::enable_if_t<is_unsigned_integer_v<T>>* = nullptr>
  auto generate_uniform_value(T min, T max) -> T {
    assert(min <= max);
    return get_random_uniform_value<RandomUnsignedInt, T>(min, max);
  }

  template <typename T,
            std::enable_if_t<std::is_floating_point_v<T>>* = nullptr>
  auto generate_uniform_value(T min, T max) -> T {
    assert(min <= max);
    return get_random_uniform_value<RandomFloat, T>(min, max);
  }

 private:
  template <typename SupportedRandomType, typename TargetType>
  auto get_random_uniform_value(TargetType min, TargetType max) -> TargetType {
    static_assert(sizeof(SupportedRandomType) >= sizeof(TargetType));
    static_assert(both_are_signed<SupportedRandomType, TargetType> ||
                  both_are_unsigned<SupportedRandomType, TargetType>);

    const auto generated =
        generate_uniform(static_cast<SupportedRandomType>(min),
                         static_cast<SupportedRandomType>(max));

    return static_cast<TargetType>(generated);
  }

  virtual auto generate_uniform(RandomInt min, RandomInt max) -> RandomInt = 0;

  virtual auto generate_uniform(RandomUnsignedInt min, RandomUnsignedInt max)
      -> RandomUnsignedInt = 0;

  virtual auto generate_uniform(RandomFloat min, RandomFloat max)
      -> RandomFloat = 0;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_GENERATORS_VALUE_GENERATOR_HPP_
