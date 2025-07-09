#ifndef SIMULATOR_LOG_IH_FACTORIES_SINK_FACTORY_HPP_
#define SIMULATOR_LOG_IH_FACTORIES_SINK_FACTORY_HPP_

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "ih/util.hpp"
#include "logger.hpp"

namespace simulator::log {

inline auto create_file_sink(const std::filesystem::path& log_dir)
    -> std::shared_ptr<spdlog::sinks::basic_file_sink_mt> {
  return std::make_shared<spdlog::sinks::basic_file_sink_mt>(
      util::make_log_file_path(log_dir, "", util::get_default_filename()),
      true);
}

inline auto create_rotating_sink(const RotatingLoggerConfig& config,
                                 const std::filesystem::path& log_dir)
    -> std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> {
  return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      util::make_log_file_path(
          log_dir, config.log_file_prefix, config.log_file_name),
      static_cast<std::size_t>(config.log_file_size),
      config.log_files_count);
}

}  // namespace simulator::log

#endif  // SIMULATOR_LOG_IH_FACTORIES_SINK_FACTORY_HPP_
