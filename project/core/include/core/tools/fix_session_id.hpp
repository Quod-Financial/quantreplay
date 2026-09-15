#ifndef SIMULATOR_CORE_TOOLS_FIX_SESSION_ID_HPP_
#define SIMULATOR_CORE_TOOLS_FIX_SESSION_ID_HPP_

#include <optional>
#include <string>
#include <string_view>

namespace simulator::core {

struct FixSessionId {
  std::string begin_string;
  std::string sender_comp_id;
  std::string target_comp_id;
  std::optional<std::string> session_qualifier;

  [[nodiscard]]
  auto operator==(const FixSessionId&) const -> bool = default;
};

[[nodiscard]]
auto parse_fix_session_id(std::string_view text)
    -> std::optional<FixSessionId>;

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_TOOLS_FIX_SESSION_ID_HPP_
