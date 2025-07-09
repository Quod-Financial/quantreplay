#ifndef SIMULATOR_GENERATOR_IH_TRACING_TRACE_VALUE_HPP_
#define SIMULATOR_GENERATOR_IH_TRACING_TRACE_VALUE_HPP_

#include <functional>
#include <string>
#include <string_view>
#include <type_traits>

namespace simulator::generator::trace {

template <typename T>
class TraceValue {
  using StoredType = std::decay_t<T>;

  static constexpr bool HoldsArithmetic = std::is_arithmetic_v<StoredType>;
  static constexpr bool HoldsString = std::is_same_v<StoredType, std::string>;
  static constexpr bool HoldsStringView =
      std::is_same_v<StoredType, std::string_view>;

  static_assert(
      HoldsArithmetic || HoldsString || HoldsStringView,
      "Type T is not supported by tracing component. "
      "Only std::string, std::string_view and arithmetic could be traced");

 public:
  static constexpr auto holds_string() -> bool { return HoldsString; }

  static constexpr auto holds_string_view() -> bool { return HoldsStringView; }

  static constexpr auto holds_arithmetic_type() -> bool {
    return HoldsArithmetic;
  }

  TraceValue(std::string_view key, const T& value) noexcept;

  [[nodiscard]] auto get_key() const noexcept -> std::string_view;

  [[nodiscard]] auto get_value() const noexcept -> const StoredType&;

  [[nodiscard]] auto holds_comment() const noexcept -> bool;

  [[nodiscard]] auto get_comment() const noexcept -> std::string_view;

  auto set_comment(std::string_view comment) noexcept -> void;

 private:
  std::string_view key_;
  std::string_view comment_;

  std::reference_wrapper<const StoredType> value_reference_;
};

template <typename T>
TraceValue<T>::TraceValue(std::string_view key, const T& value) noexcept
    : key_{key}, value_reference_{value} {}

template <typename T>
auto TraceValue<T>::get_key() const noexcept -> std::string_view {
  return key_;
}

template <typename T>
auto TraceValue<T>::get_value() const noexcept -> const
    typename TraceValue<T>::StoredType& {
  return value_reference_.get();
}

template <typename T>
auto TraceValue<T>::holds_comment() const noexcept -> bool {
  return !comment_.empty();
}

template <typename T>
auto TraceValue<T>::get_comment() const noexcept -> std::string_view {
  return comment_;
}

template <typename T>
auto TraceValue<T>::set_comment(std::string_view comment) noexcept -> void {
  comment_ = comment;
}

}  // namespace simulator::generator::trace

#endif  // SIMULATOR_GENERATOR_IH_TRACING_TRACE_VALUE_HPP_
