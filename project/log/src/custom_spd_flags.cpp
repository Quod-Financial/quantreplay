#include "ih/custom_spd_flags.hpp"

#include <iostream>

namespace simulator::log {

auto EscapedPayloadFlag::format(const spdlog::details::log_msg& msg,
                                const std::tm& tm_time,
                                spdlog::memory_buf_t& dest) -> void {
  // Debug fmt format. The string is quoted and special characters escaped:
  // a message with "quotation marks" -> "a message with \"quotation marks\""
  const auto quoted_escaped_payload = fmt::format("{:?}", msg.payload);
  // Append without quotation marks at the beginning and end of the string that
  // fmt adds.
  dest.append(
      quoted_escaped_payload.data() + 1,
      quoted_escaped_payload.data() + quoted_escaped_payload.size() - 1);
}

auto EscapedPayloadFlag::clone() const
    -> std::unique_ptr<spdlog::custom_flag_formatter> {
  return spdlog::details::make_unique<EscapedPayloadFlag>();
}

}  // namespace simulator::log