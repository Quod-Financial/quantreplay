#include "api/models/setting.hpp"

#include "ih/common/exceptions.hpp"

namespace simulator::data_layer {

auto Setting::create(Setting::Patch snapshot) -> Setting {
  if (!snapshot.key_.has_value()) {
    throw RequiredAttributeMissing("Setting", "Key");
  }

  Setting setting{};
  setting.key_ = std::move(*snapshot.key_);
  setting.value_ = std::move(snapshot.value_);
  return setting;
}

auto Setting::key() const noexcept -> const std::string& { return key_; }

auto Setting::value() const noexcept -> const std::optional<std::string>& {
  return value_;
}

auto Setting::Patch::key() const noexcept -> const std::optional<std::string>& {
  return key_;
}

auto Setting::Patch::with_key(std::string key) noexcept -> Patch& {
  key_ = std::move(key);
  return *this;
}

auto Setting::Patch::value() const noexcept
    -> const std::optional<std::string>& {
  return value_;
}

auto Setting::Patch::with_value(std::string value) noexcept -> Patch& {
  value_ = std::move(value);
  return *this;
}

}  // namespace simulator::data_layer
