#include "ih/util.hpp"

#include <fmt/format.h>

namespace simulator::log::util {

auto get_app_logger_name() -> const std::string& {
  static const std::string name = "app-logger";
  return name;
}

auto get_log_pattern() -> const std::string& {
  static const std::string pattern{
      R"({"time":"%Y-%m-%dT%T.%fZ","level":"%^%l%$","thread":%t,"file":"%s:%#","message":"%q"})"};
  return pattern;
}

auto get_default_filename() -> const std::string& {
  static const std::string name = "simulator";
  return name;
}

auto get_logging_dir(const char* (*func)()) -> std::filesystem::path {
  const auto* log_dir = func();
  return log_dir != nullptr ? std::filesystem::path(log_dir)
                            : std::filesystem::current_path();
}

auto make_log_file_path(const std::filesystem::path& directory,
                        std::string_view file_prefix,
                        std::string_view file_name) -> std::filesystem::path {
  return directory / fmt::format("{}{}.log", file_prefix, file_name);
}

auto make_level(std::string_view level) -> spdlog::level::level_enum {
  if (level == "TRACE") {
    return spdlog::level::trace;
  }
  if (level == "DEBUG") {
    return spdlog::level::debug;
  }
  if (level == "INFO") {
    return spdlog::level::info;
  }
  if (level == "WARN") {
    return spdlog::level::warn;
  }
  if (level == "ERROR") {
    return spdlog::level::err;
  }
  throw std::invalid_argument(fmt::format("unknown level: {}", level));
}

}  // namespace simulator::log::util