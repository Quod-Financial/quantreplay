#ifndef SIMULATOR_HTTP_IH_DATA_BRIDGE_SETTING_ACCESSOR_HPP_
#define SIMULATOR_HTTP_IH_DATA_BRIDGE_SETTING_ACCESSOR_HPP_

#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/database/context.hpp"
#include "data_layer/api/models/setting.hpp"
#include "ih/data_bridge/operation_failure.hpp"

namespace simulator::http::data_bridge {

class SettingAccessor {
 public:
  SettingAccessor() = default;
  SettingAccessor(const SettingAccessor&) = default;
  SettingAccessor(SettingAccessor&&) noexcept = default;

  auto operator=(const SettingAccessor&) -> SettingAccessor& = default;
  auto operator=(SettingAccessor&&) noexcept -> SettingAccessor& = default;

  virtual ~SettingAccessor() = default;

  [[nodiscard]]
  virtual auto select_single(const std::string& key) const noexcept
      -> tl::expected<data_layer::Setting, Failure> = 0;

  [[nodiscard]]
  virtual auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::Setting>, Failure> = 0;

  [[nodiscard]]
  virtual auto add(data_layer::Setting::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> = 0;

  [[nodiscard]]
  virtual auto update(data_layer::Setting::Patch update,
                      const std::string& key) const noexcept
      -> tl::expected<void, Failure> = 0;
};

class DataLayerSettingAccessor final : public SettingAccessor {
 public:
  using DbContext = data_layer::database::Context;

  DataLayerSettingAccessor() = delete;

  explicit DataLayerSettingAccessor(DbContext context) noexcept;

  [[nodiscard]]
  auto select_single(const std::string& key) const noexcept
      -> tl::expected<data_layer::Setting, Failure> override;

  [[nodiscard]]
  auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::Setting>, Failure> override;

  [[nodiscard]]
  auto add(data_layer::Setting::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> override;

  [[nodiscard]]
  auto update(data_layer::Setting::Patch update,
              const std::string& key) const noexcept
      -> tl::expected<void, Failure> override;

 private:
  data_layer::database::Context context_;
};

}  // namespace simulator::http::data_bridge

#endif  // SIMULATOR_HTTP_IH_DATA_BRIDGE_SETTING_ACCESSOR_HPP_
