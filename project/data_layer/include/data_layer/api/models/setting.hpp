#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_SETTING_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_SETTING_HPP_

#include <optional>
#include <string>

#include "data_layer/api/predicate/definitions.hpp"

namespace simulator::data_layer {

class Setting {
 public:
  class Patch;

  using Predicate = predicate::Expression<Setting>;

  enum class Attribute { Key, Value };

  static auto create(Patch snapshot) -> Setting;

  [[nodiscard]]
  auto key() const noexcept -> const std::string&;

  [[nodiscard]]
  auto value() const noexcept -> const std::optional<std::string>&;

 private:
  Setting() = default;

  std::optional<std::string> value_;

  std::string key_;
};

class Setting::Patch {
  friend class Setting;

 public:
  using Attribute = Setting::Attribute;

  [[nodiscard]]
  auto key() const noexcept -> const std::optional<std::string>&;
  auto with_key(std::string key) noexcept -> Patch&;

  [[nodiscard]]
  auto value() const noexcept -> const std::optional<std::string>&;
  auto with_value(std::string value) noexcept -> Patch&;

 private:
  std::optional<std::string> key_;
  std::optional<std::string> value_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_MODELS_SETTING_HPP_
