#include "ih/utils/compression.hpp"

#include <fmt/format.h>
#include <fmt/std.h>
#include <zip.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace simulator::http {
namespace {

using ArchivePtr = std::unique_ptr<zip_t, decltype(&zip_discard)>;
using SourcePtr = std::unique_ptr<zip_source_t, decltype(&zip_source_free)>;

[[noreturn]]
auto throw_zip_error(std::string_view context, zip_error_t* error) -> void {
  throw std::runtime_error{
      fmt::format("{}: {}", context, zip_error_strerror(error))};
}

[[noreturn]]
auto throw_archive_error(std::string_view context, zip_t* archive) -> void {
  throw std::runtime_error{
      fmt::format("{}: {}", context, zip_strerror(archive))};
}

auto read_back(zip_source_t* source) -> std::string {
  if (zip_source_open(source) < 0) {
    throw_zip_error("failed to reopen in-memory zip source",
                    zip_source_error(source));
  }

  std::string archive;
  std::array<char, 8192> chunk{};
  zip_int64_t read = 0;
  while ((read = zip_source_read(source, chunk.data(), chunk.size())) > 0) {
    archive.append(chunk.data(), static_cast<std::size_t>(read));
  }

  if (read < 0) {
    zip_source_close(source);
    throw_zip_error("failed to read in-memory zip source",
                    zip_source_error(source));
  }

  zip_source_close(source);
  return archive;
}

}  // namespace

auto build_zip_entries(const std::unordered_set<std::filesystem::path>& paths)
    -> std::vector<ZipEntry> {
  namespace fs = std::filesystem;

  if (paths.empty()) {
    return {};
  }

  std::vector<fs::path> parents;
  parents.reserve(paths.size());
  for (const auto& path : paths) {
    parents.emplace_back(path.parent_path());
  }

  std::vector<fs::path> common{parents.front().begin(), parents.front().end()};
  for (std::size_t i = 1; i < parents.size(); ++i) {
    const std::vector<fs::path> components{parents[i].begin(),
                                           parents[i].end()};
    const std::size_t limit = std::min(common.size(), components.size());
    std::size_t matched = 0;
    while (matched < limit && common[matched] == components[matched]) {
      ++matched;
    }
    common.resize(matched);
  }

  fs::path base;
  for (const auto& component : common) {
    base /= component;
  }

  std::vector<ZipEntry> entries;
  entries.reserve(paths.size());
  for (const auto& path : paths) {
    const fs::path relative_dir = path.parent_path().lexically_relative(base);
    const fs::path archive_name = (relative_dir.empty() || relative_dir == ".")
                                      ? path.filename()
                                      : relative_dir / path.filename();
    entries.push_back(
        ZipEntry{.source = path,
                 .relative_path_in_archive = archive_name.generic_string()});
  }
  return entries;
}

auto compress_files_to_zip(const std::vector<ZipEntry>& entries)
    -> std::string {
  zip_error_t error{};
  zip_error_init(&error);

  // A NULL/0 buffer with a 0 "free" flag makes a growable, library-owned
  // in-memory buffer that the archive is written into on zip_close. source is
  // declared first so it is released last, after the archive is discarded.
  SourcePtr source{zip_source_buffer_create(nullptr, 0, 0, &error),
                   &zip_source_free};
  if (source == nullptr) {
    throw_zip_error("failed to create in-memory zip source", &error);
  }

  ArchivePtr archive{zip_open_from_source(source.get(), ZIP_TRUNCATE, &error),
                     &zip_discard};
  if (archive == nullptr) {
    throw_zip_error("failed to open in-memory zip archive", &error);
  }

  // zip_open_from_source took over the source reference and drops it on
  // zip_close/zip_discard; keep an extra reference so the finalized buffer
  // survives to be read back. Now there are two references: one owned by the
  // archive, one by the source owning pointer.
  zip_source_keep(source.get());

  for (const auto& entry : entries) {
    zip_source_t* file_source = zip_source_file(
        archive.get(), entry.source.string().c_str(), 0, ZIP_LENGTH_TO_END);
    if (file_source == nullptr) {
      throw_archive_error(
          fmt::format("failed to open '{}' for compression", entry.source),
          archive.get());
    }

    const zip_int64_t index =
        zip_file_add(archive.get(),
                     entry.relative_path_in_archive.c_str(),
                     file_source,
                     ZIP_FL_ENC_UTF_8);
    if (index < 0) {
      zip_source_free(file_source);
      throw_archive_error(fmt::format("failed to add '{}' to archive",
                                      entry.relative_path_in_archive),
                          archive.get());
    }

    if (zip_set_file_compression(archive.get(),
                                 static_cast<zip_uint64_t>(index),
                                 ZIP_CM_DEFLATE,
                                 0) < 0) {
      throw_archive_error(fmt::format("failed to set compression for '{}'",
                                      entry.relative_path_in_archive),
                          archive.get());
    }
  }

  // Flush the central directory into the in-memory source.
  if (zip_close(archive.get()) < 0) {
    throw_archive_error("failed to finalize in-memory zip archive",
                        archive.get());
  }
  // On success zip_close consumes the archive handle, so release it from its
  // owning pointer to avoid a double free via zip_discard.
  archive.release();

  return read_back(source.get());
}

}  // namespace simulator::http
