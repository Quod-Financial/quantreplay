#ifndef SIMULATOR_APP_IH_DATASOURCE_SETTINGS_FILES_HPP_
#define SIMULATOR_APP_IH_DATASOURCE_SETTINGS_FILES_HPP_

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "data_layer/api/models/datasource.hpp"

namespace simulator {

struct DatasourceIdentity {
  std::uint64_t id{0};
  std::string name;

  [[nodiscard]]
  auto operator==(const DatasourceIdentity&) const -> bool = default;
};

struct DatasourceSettingsFile {
  // As written in the connection, not canonicalised.
  std::filesystem::path path;
  std::vector<DatasourceIdentity> datasources;

  [[nodiscard]]
  auto operator==(const DatasourceSettingsFile&) const -> bool = default;
};

// One entry per distinct file a FIX datasource names, the instance's own
// file excluded.
[[nodiscard]]
auto datasource_settings_files(
    const std::vector<data_layer::Datasource>& datasources,
    const std::filesystem::path& instance_settings_file)
    -> std::vector<DatasourceSettingsFile>;

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_DATASOURCE_SETTINGS_FILES_HPP_
