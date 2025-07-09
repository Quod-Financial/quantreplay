#ifndef SIMULATOR_LOG_IH_CUSTOM_SPD_FLAGS_HPP_
#define SIMULATOR_LOG_IH_CUSTOM_SPD_FLAGS_HPP_

#include <spdlog/pattern_formatter.h>

namespace simulator::log {

class EscapedPayloadFlag final : public spdlog::custom_flag_formatter {
 public:
  auto format(const spdlog::details::log_msg& msg,
              const std::tm& tm_time,
              spdlog::memory_buf_t& dest) -> void override;

  [[nodiscard]]
  auto clone() const -> std::unique_ptr<spdlog::custom_flag_formatter> override;
};

}  // namespace simulator::log

#endif  // SIMULATOR_LOG_IH_CUSTOM_SPD_FLAGS_HPP_
