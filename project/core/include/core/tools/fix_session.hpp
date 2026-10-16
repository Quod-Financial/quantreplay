#ifndef SIMULATOR_CORE_TOOLS_FIX_SESSION_HPP_
#define SIMULATOR_CORE_TOOLS_FIX_SESSION_HPP_

#include <filesystem>
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

struct FixSessionReference {
  FixSessionId session_id;
  std::optional<std::filesystem::path> settings_file;

  [[nodiscard]]
  auto operator==(const FixSessionReference&) const -> bool = default;
};

[[nodiscard]]
auto parse_fix_session_id(std::string_view text) -> std::optional<FixSessionId>;

[[nodiscard]]
auto parse_fix_session_reference(std::string_view text)
    -> std::optional<FixSessionReference>;

}  // namespace simulator::core

#endif  // SIMULATOR_CORE_TOOLS_FIX_SESSION_HPP_
