#ifndef SIMULATOR_LOG_IH_UTIL_HPP_
#define SIMULATOR_LOG_IH_UTIL_HPP_

#include <spdlog/common.h>

#include <filesystem>
#include <string>

namespace simulator::log::util {

auto get_app_logger_name() -> const std::string&;

auto get_log_pattern() -> const std::string&;

auto get_default_filename() -> const std::string&;

inline auto env_logging_dir = []() -> const char* {
  return std::getenv("LOG_DIR");
};

auto get_logging_dir(const char* (*func)() = env_logging_dir)
    -> std::filesystem::path;

auto make_log_file_path(const std::filesystem::path& directory,
                        std::string_view file_prefix,
                        std::string_view file_name) -> std::filesystem::path;

auto make_level(std::string_view level) -> spdlog::level::level_enum;

}  // namespace simulator::log::util

#endif  // SIMULATOR_LOG_IH_UTIL_HPP_
