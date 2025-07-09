#include "ih/loading.hpp"

#include <fmt/format.h>

#include <stdexcept>

#include "cfg/api/cfg.hpp"
#include "log/logger.hpp"

namespace simulator {

auto load_configuration(const simulator::CommandOptions& options) -> void {
  const auto& cfg_path = options.get_config_file_path();
  if (!cfg_path.has_value()) {
    throw std::runtime_error("configuration file path is not specified");
  }

  try {
    cfg::init(*cfg_path);
  } catch (const std::exception& exception) {
    throw std::runtime_error(
        fmt::format("failed to load configuration, an error occurred: {}",
                    exception.what()));
  } catch (...) {
    throw std::runtime_error(
        "failed to load configuration, unknown error occurred");
  }
}

auto load_logger(const simulator::CommandOptions& options,
                 const cfg::LogConfiguration& config) -> void {
  const auto& instance_id = options.get_instance_id();
  const auto& instance_prefix = options.get_instance_prefix();

  if (!instance_id.has_value()) {
    throw std::runtime_error("instance identifier is not defined");
  }
  if (!instance_prefix.has_value()) {
    throw std::runtime_error("instance prefix is not defined");
  }

  try {
    log::configure_rotating_logger(
        {.log_file_prefix = *instance_prefix,
         .log_file_name = *instance_id,
         .log_level = config.level,
         .log_file_size = log::FileSize::megabytes(config.max_size),
         .log_files_count = static_cast<std::uint32_t>(config.max_files)});
  } catch (const std::exception& exception) {
    throw std::runtime_error(
        fmt::format("failed to initialize logging, an error occurred: {}",
                    exception.what()));
  } catch (...) {
    throw std::runtime_error(
        "failed to initialize logging, unknown error occurred");
  }
}

}  // namespace simulator