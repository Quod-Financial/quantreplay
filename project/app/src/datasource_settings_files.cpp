#include "ih/datasource_settings_files.hpp"

#include <filesystem>
#include <map>
#include <optional>
#include <system_error>
#include <utility>
#include <vector>

#include "core/tools/fix_session.hpp"

namespace simulator {
namespace {

// Never fails: a missing file is reported when its initiator is created.
[[nodiscard]]
auto canonical_key(const std::filesystem::path& path) -> std::filesystem::path {
  std::error_code error;
  auto key = std::filesystem::weakly_canonical(path, error);
  if (error) {
    return path.lexically_normal();
  }
  return key;
}

[[nodiscard]]
auto named_settings_file(const data_layer::Datasource& datasource)
    -> std::optional<std::filesystem::path> {
  if (datasource.format() != data_layer::Datasource::Format::Fix) {
    return std::nullopt;
  }
  const auto reference =
      core::parse_fix_session_reference(datasource.connection());
  if (!reference.has_value()) {
    return std::nullopt;
  }
  return reference->settings_file;
}

}  // namespace

auto datasource_settings_files(
    const std::vector<data_layer::Datasource>& datasources,
    const std::filesystem::path& instance_settings_file)
    -> std::vector<DatasourceSettingsFile> {
  const auto instance_key = canonical_key(instance_settings_file);
  std::map<std::filesystem::path, DatasourceSettingsFile> files_by_key;

  for (const auto& datasource : datasources) {
    const auto settings_file = named_settings_file(datasource);
    if (!settings_file.has_value()) {
      continue;
    }
    auto key = canonical_key(*settings_file);
    if (key == instance_key) {
      continue;
    }
    auto& file = files_by_key
                     .try_emplace(std::move(key),
                                  DatasourceSettingsFile{.path = *settings_file,
                                                         .datasources = {}})
                     .first->second;
    file.datasources.push_back(
        {.id = datasource.datasource_id(), .name = datasource.name()});
  }

  std::vector<DatasourceSettingsFile> files;
  files.reserve(files_by_key.size());
  for (auto& entry : files_by_key) {
    files.push_back(std::move(entry.second));
  }
  return files;
}

}  // namespace simulator
