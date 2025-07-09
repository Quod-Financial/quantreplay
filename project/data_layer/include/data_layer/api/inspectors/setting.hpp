#ifndef SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_SETTING_HPP_
#define SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_SETTING_HPP_

#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

#include "data_layer/api/models/setting.hpp"

namespace simulator::data_layer {

template <typename Marshaller>
class SettingReader final {
  using Attribute = Setting::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit SettingReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const Setting& setting) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Marshaller>
class SettingPatchReader final {
  using Attribute = Setting::Attribute;

  template <typename T>
  constexpr inline static bool can_marshall_v =
      std::is_invocable_v<Marshaller,
                          Attribute,
                          std::add_const_t<std::add_lvalue_reference_t<T>>>;

 public:
  explicit SettingPatchReader(Marshaller& marshaller) noexcept
      : marshaller_(marshaller) {}

  auto read(const Setting::Patch& patch) -> void;

 private:
  std::reference_wrapper<Marshaller> marshaller_;
};

template <typename Unmarshaller>
class SettingPatchWriter final {
  using Attribute = Setting::Attribute;

  template <typename T>
  constexpr inline static bool can_unmarshall_v =
      std::is_invocable_r_v<bool,
                            Unmarshaller,
                            Attribute,
                            std::add_lvalue_reference_t<T>>;

 public:
  explicit SettingPatchWriter(Unmarshaller& unmarshaller) noexcept
      : unmarshaller_(unmarshaller) {}

  auto write(Setting::Patch& patch) -> void;

 private:
  std::reference_wrapper<Unmarshaller> unmarshaller_;
};

template <typename Marshaller>
auto SettingReader<Marshaller>::read(const Setting& setting) -> void {
  const std::string& key = setting.key();
  static_assert(can_marshall_v<decltype(key)>);
  marshaller_(Attribute::Key, key);

  if (const auto& value = setting.value()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Value, *value);
  }
}

template <typename Marshaller>
inline auto SettingPatchReader<Marshaller>::read(const Setting::Patch& patch)
    -> void {
  if (const auto& value = patch.key()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Key, *value);
  }

  if (const auto& value = patch.value()) {
    static_assert(can_marshall_v<decltype(*value)>);
    marshaller_(Attribute::Value, *value);
  }
}

template <typename Unmarshaller>
inline auto SettingPatchWriter<Unmarshaller>::write(Setting::Patch& patch)
    -> void {
  std::string key{};
  static_assert(can_unmarshall_v<decltype(key)>);
  if (unmarshaller_(Attribute::Key, key)) {
    patch.with_key(std::move(key));
  }

  std::string value{};
  static_assert(can_unmarshall_v<decltype(value)>);
  if (unmarshaller_(Attribute::Value, value)) {
    patch.with_value(std::move(value));
  }
}

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_API_COMMON_INSPECTORS_SETTING_HPP_
