#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_TRAITS_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_TRAITS_HPP_

#include <cstdint>
#include <type_traits>

namespace simulator::http::json {
namespace detail::traits {

template <typename T, typename Enabled = void>
struct is_marshallable_int_impl : std::false_type {};

template <typename T>
struct is_marshallable_int_impl<
    T,
    std::enable_if_t<std::is_same_v<T, std::int16_t> ||
                     std::is_same_v<T, std::int32_t> ||
                     std::is_same_v<T, std::int64_t>>> : std::true_type {};

template <typename T>
struct is_marshallable_int
    : is_marshallable_int_impl<std::remove_cv_t<std::remove_reference_t<T>>> {};

template <typename T, typename Enabled = void>
struct is_marshallable_uint_impl : std::false_type {};

template <typename T>
struct is_marshallable_uint_impl<
    T,
    std::enable_if_t<std::is_same_v<T, std::uint16_t> ||
                     std::is_same_v<T, std::uint32_t> ||
                     std::is_same_v<T, std::uint64_t>>> : std::true_type {};

template <typename T>
struct is_marshallable_uint
    : is_marshallable_uint_impl<std::remove_cv_t<std::remove_reference_t<T>>> {
};

}  // namespace detail::traits

template <typename T>
constexpr inline bool is_marshallable_int_v =
    detail::traits::is_marshallable_int<T>::value;

template <typename T>
constexpr inline bool is_marshallable_uint_v =
    detail::traits::is_marshallable_uint<T>::value;

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_DETAIL_TRAITS_HPP_
