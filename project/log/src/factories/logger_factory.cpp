#include "ih/factories/logger_factory.hpp"

#include <spdlog/pattern_formatter.h>

#include "ih/custom_spd_flags.hpp"
#include "ih/util.hpp"

namespace simulator::log {

namespace {

auto create_formatter() -> std::unique_ptr<spdlog::pattern_formatter> {
  auto formatter = std::make_unique<spdlog::pattern_formatter>(
      spdlog::pattern_time_type::utc);
  formatter->add_flag<EscapedPayloadFlag>('q');
  formatter->set_pattern(util::get_log_pattern());
  return formatter;
}

}  // namespace

auto create_default_logger(spdlog::sink_ptr sink)
    -> std::shared_ptr<spdlog::logger> {
  auto logger = std::make_shared<spdlog::logger>("default", std::move(sink));
  logger->set_formatter(create_formatter());
  logger->set_level(spdlog::level::debug);
  logger->flush_on(spdlog::level::debug);
  return logger;
}

auto create_app_logger(std::string_view log_level, spdlog::sink_ptr sink)
    -> std::shared_ptr<spdlog::logger> {
  auto logger = std::make_shared<spdlog::logger>(util::get_app_logger_name(),
                                                 std::move(sink));
  logger->set_formatter(create_formatter());
  logger->set_level(util::make_level(log_level));
  logger->flush_on(spdlog::level::info);
  return logger;
}

}  // namespace simulator::log
