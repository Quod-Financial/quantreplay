#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_PATCH_FIELD_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_PATCH_FIELD_HPP_

#include <optional>

namespace simulator::data_layer {

template <typename T>
class PatchField {
 public:
  using value_type = T;

  enum class State : std::uint8_t { Unset, Null, Value };

  PatchField() = default;

  explicit constexpr PatchField(const T& value)
      : state_{State::Value}, value_{std::make_optional<T>(value)} {}

  explicit constexpr PatchField(const std::optional<T>& value)
      : state_{value.has_value() ? State::Value : State::Null}, value_{value} {}

  explicit constexpr PatchField(T&& value) noexcept
      : state_{State::Value}, value_{std::make_optional<T>(std::move(value))} {}

  explicit constexpr PatchField(std::optional<T>&& value) noexcept
      : state_{value.has_value() ? State::Value : State::Null},
        value_{std::move(value)} {}

  constexpr auto operator=(const T& value) -> PatchField& {
    state_ = State::Value;
    value_ = std::make_optional<T>(value);
    return *this;
  }

  constexpr auto operator=(const std::optional<T>& value) -> PatchField& {
    state_ = value.has_value() ? State::Value : State::Null;
    value_ = value;
    return *this;
  }

  constexpr auto operator=(T&& value) noexcept -> PatchField& {
    state_ = State::Value;
    value_ = std::make_optional<T>(std::move(value));
    return *this;
  }

  constexpr auto operator=(std::optional<T>&& value) noexcept -> PatchField& {
    state_ = value.has_value() ? State::Value : State::Null;
    value_ = std::move(value);
    return *this;
  }

  [[nodiscard]]
  constexpr auto operator*() const& noexcept -> const std::optional<T>& {
    return value_;
  }

  [[nodiscard]]
  constexpr auto operator*() && -> std::optional<T>&& {
    return std::move(value_);
  }

  constexpr explicit operator bool() const noexcept { return has_value(); }

  [[nodiscard]]
  constexpr auto state() const noexcept -> State {
    return state_;
  }

  [[nodiscard]]
  constexpr auto has_value() const noexcept -> bool {
    return state_ != State::Unset;
  }

  [[nodiscard]]
  constexpr auto value() const noexcept -> const std::optional<T>& {
    return value_;
  }

  [[nodiscard]]
  auto inner_value() const -> const T& {
    return value_.value();
  }

  [[nodiscard]]
  auto inner_value_or(const T& default_value) const -> T {
    if (has_value()) {
      return value_.value_or(default_value);
    }
    return default_value;
  }

 private:
  State state_{State::Unset};
  std::optional<T> value_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_PATCH_FIELD_HPP_
