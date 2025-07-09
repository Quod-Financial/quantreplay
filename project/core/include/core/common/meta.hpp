#ifndef SIMULATOR_CORE_COMMON_META_HPP_
#define SIMULATOR_CORE_COMMON_META_HPP_

#include <chrono>
#include <concepts>
#include <functional>
#include <optional>
#include <type_traits>
#include <vector>

namespace simulator::core {

template <typename>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template <typename>
struct is_vector : std::false_type {};

template <typename T, typename Allocator>
struct is_vector<std::vector<T, Allocator>> : std::true_type {};

template <typename>
struct is_chrono_duration : std::false_type {};

template <typename Dur, typename Rep>
struct is_chrono_duration<std::chrono::duration<Dur, Rep>> : std::true_type {};

template <typename T>
constexpr inline bool is_optional_v = is_optional<T>::value;

template <typename T>
constexpr inline bool is_vector_v = is_vector<T>::value;

template <typename T>
constexpr inline bool is_chrono_duration_v = is_chrono_duration<T>::value;

// Still waiting for CWG2518/P2593R1 to use static_assert(false, ...) directly,
// see https://en.cppreference.com/w/cpp/language/static_assert
template <typename...>
constexpr inline bool always_false_v = false;

template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

template <typename T>
concept Enumerable = std::is_enum_v<T>;

template <typename T>
concept HasValueType = requires { typename T::value_type; };

template <typename T>
concept Optional = is_optional_v<T>;

template <typename T>
concept Vector = is_vector_v<T>;

template <typename T>
concept ChronoDuration = is_chrono_duration_v<T>;

template <typename T>
concept Hashable = requires(T value) {
  { std::hash<T>{}(value) } -> std::convertible_to<std::size_t>;
};

// A concept of an invocable type that accept no arguments
// and does not return any result.
template <typename T>
concept NullaryVoidInvocable = std::invocable<T> && requires(T callable) {
  { std::invoke(callable) } -> std::same_as<void>;
};

template <typename T>
  requires HasValueType<T>
using value_type_t = typename T::value_type;

// Casts given enum value to its underlying type.
template <typename T>
  requires std::is_enum_v<T>
[[nodiscard]]
constexpr auto underlying_cast(T value) noexcept -> std::underlying_type_t<T> {
  return static_cast<std::underlying_type_t<T>>(value);
}

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_COMMON_META_HPP_