#ifndef SIMULATOR_APP_IH_LOADING_HPP_
#define SIMULATOR_APP_IH_LOADING_HPP_

#include "cfg/api/cfg.hpp"
#include "ih/command_options.hpp"

namespace simulator {

auto load_configuration(const simulator::CommandOptions& options) -> void;

auto load_logger(const simulator::CommandOptions& options,
                 const simulator::cfg::LogConfiguration& config) -> void;

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_LOADING_HPP_
