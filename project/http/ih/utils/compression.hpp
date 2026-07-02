#ifndef SIMULATOR_HTTP_IH_UTILS_COMPRESSION_HPP_
#define SIMULATOR_HTTP_IH_UTILS_COMPRESSION_HPP_

#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

namespace simulator::http {

struct ZipEntry {
  std::filesystem::path source;
  std::string relative_path_in_archive;

  auto operator==(const ZipEntry&) const -> bool = default;
};

// For all path, deletes the common directory path and stores only the relative
// ones to be stored in the archive.
// It allows to compress files with the same name but in different directories.
[[nodiscard]]
auto build_zip_entries(const std::unordered_set<std::filesystem::path>& paths)
    -> std::vector<ZipEntry>;

// Builds a zip archive entirely in memory from the given entries and returns
// its raw bytes.
[[nodiscard]]
auto compress_files_to_zip(const std::vector<ZipEntry>& entries) -> std::string;

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_UTILS_COMPRESSION_HPP_
