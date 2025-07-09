#include "log/logger.hpp"

#include <fmt/base.h>
#include <spdlog/async.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <memory>
#include <mutex>
#include <stdexcept>

#include "ih/factories/logger_factory.hpp"
#include "ih/factories/sink_factory.hpp"
#include "ih/util.hpp"
#include "log/logging.hpp"

namespace simulator::log {

namespace {

auto get_default_logger() -> spdlog::logger& {
  static std::once_flag configured;
  std::call_once(configured, [] {
    const auto logger =
        create_default_logger(create_file_sink(util::get_logging_dir()));
    spdlog::set_default_logger(logger);
  });

  if (auto* logger = spdlog::default_logger_raw()) [[likely]] {
    return *logger;
  }

  fmt::println(stderr, "[FATAL]: failed to access the default logger");
  std::abort();
}

}  // namespace

namespace details {

auto get_logger() -> spdlog::logger& {
  const auto logger = spdlog::get(util::get_app_logger_name());
  return logger ? *logger : get_default_logger();
}

}  // namespace details

auto configure_rotating_logger(const RotatingLoggerConfig& config) -> void {
  static std::once_flag configured;
  std::call_once(configured, [&] {
    const auto logger = create_app_logger(
        config.log_level,
        create_rotating_sink(config, util::get_logging_dir()));
    spdlog::register_logger(logger);
  });
}

}  // namespace simulator::log
