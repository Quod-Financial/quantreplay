#ifndef SIMULATOR_LOG_IH_FACTORIES_LOGGER_FACTORY_HPP_
#define SIMULATOR_LOG_IH_FACTORIES_LOGGER_FACTORY_HPP_

#include <spdlog/logger.h>

namespace simulator::log {

auto create_default_logger(spdlog::sink_ptr sink)
    -> std::shared_ptr<spdlog::logger>;

auto create_app_logger(std::string_view log_level, spdlog::sink_ptr sink)
    -> std::shared_ptr<spdlog::logger>;

}  // namespace simulator::log

#endif  // SIMULATOR_LOG_IH_FACTORIES_LOGGER_FACTORY_HPP_
