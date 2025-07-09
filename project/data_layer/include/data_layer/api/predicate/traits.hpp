#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_TRAITS_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_TRAITS_HPP_

#include <tuple>
#include <type_traits>
#include <variant>

namespace simulator::data_layer::predicate {
namespace detail {

template <typename... Tuples>
struct tuple_cat;

template <typename Tuple>
struct tuple_cat<Tuple> {
  using type = Tuple;
};

template <typename... Tuples>
using tuple_cat_t = typename tuple_cat<Tuples...>::type;

template <typename... Ts, typename... Us, typename... Tuples>
struct tuple_cat<std::tuple<Ts...>, std::tuple<Us...>, Tuples...> {
  using type = tuple_cat_t<std::tuple<Ts..., Us...>, Tuples...>;
};

template <typename... Tuples>
struct aggregated_tuple {
  using type = decltype(std::tuple_cat(std::declval<Tuples>()...));
};

template <typename T, typename Tuple>
struct is_in_tuple_impl;

template <typename T>
struct is_in_tuple_impl<T, std::tuple<>> : std::false_type {};

template <typename T, typename... Types>
struct is_in_tuple_impl<T, std::tuple<T, Types...>> : std::true_type {};

template <typename T, typename U, typename... Types>
struct is_in_tuple_impl<T, std::tuple<U, Types...>>
    : is_in_tuple_impl<T, std::tuple<Types...>> {};

template <typename T, typename Tuple>
struct is_in_tuple {
 private:
  using DecayedT = std::remove_cv_t<std::remove_reference_t<T>>;

 public:
  constexpr inline static bool value = is_in_tuple_impl<DecayedT, Tuple>::value;
};

template <typename... Types>
struct contains_duplicates;

template <>
struct contains_duplicates<> : std::false_type {};

template <typename T, typename... Types>
struct contains_duplicates<T, Types...>
    : std::integral_constant<
          bool,
          std::is_same_v<std::tuple<T>, std::tuple<Types...>> ||
              contains_duplicates<Types...>::value> {};

template <typename Tuple>
struct variant_from_tuple;

template <typename... Ts>
struct variant_from_tuple<std::tuple<Ts...>> {
  static_assert(
      !contains_duplicates<Ts...>::value,
      "Duplicated types fround in the tuple, can not create variant type");

  using type = std::variant<Ts...>;
};

template <typename T, typename = void>
struct is_signed_int : std::false_type {};

template <typename T>
struct is_signed_int<
    T,
    std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>>>
    : std::true_type {};

template <typename T, typename = void>
struct is_unsigned_int : std::false_type {};

template <typename T>
struct is_unsigned_int<
    T,
    std::enable_if_t<
        std::is_integral_v<T> && std::is_unsigned_v<T> &&
        !std::is_same_v<bool, std::remove_cv_t<std::remove_reference_t<T>>>>>
    : std::true_type {};

template <typename T, typename = void>
struct is_less_greater_comparable : std::false_type {};

template <typename T>
struct is_less_greater_comparable<
    T,
    std::enable_if_t<is_signed_int<T>::value || is_unsigned_int<T>::value ||
                     std::is_floating_point_v<T>>> : std::true_type {};

}  // namespace detail

template <typename... Tuples>
using aggregated_tuple_t = typename detail::aggregated_tuple<Tuples...>::type;

template <typename Tuple>
using variant_from_tuple_t = typename detail::variant_from_tuple<Tuple>::type;

template <typename T, typename Tuple>
constexpr inline bool is_in_tuple_v = detail::is_in_tuple<T, Tuple>::value;

template <typename T>
constexpr inline bool is_signed_int_v = detail::is_signed_int<T>::value;

template <typename T>
constexpr inline bool is_unsigned_int_v = detail::is_unsigned_int<T>::value;

template <typename T>
constexpr inline bool is_less_greater_comparable_v =
    detail::is_less_greater_comparable<T>::value;

template <typename Model>
struct ModelTraits;

}  // namespace simulator::data_layer::predicate

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_PREDICATE_TRAITS_HPP_
