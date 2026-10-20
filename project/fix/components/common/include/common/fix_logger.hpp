#ifndef SIMULATOR_FIX_COMMON_FIX_LOGGER_HPP_
#define SIMULATOR_FIX_COMMON_FIX_LOGGER_HPP_

#include <quickfix/Log.h>

#include <memory>

namespace simulator::fix {

[[nodiscard]] auto create_log_factory() -> std::unique_ptr<FIX::LogFactory>;

}  // namespace simulator::fix

#endif  // SIMULATOR_FIX_COMMON_INCLUDE_FIX_COMMON_FIX_LOGGER_HPP_
