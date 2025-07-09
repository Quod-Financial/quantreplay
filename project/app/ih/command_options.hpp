#ifndef SIMULATOR_APP_IH_COMMAND_OPTIONS_HPP_
#define SIMULATOR_APP_IH_COMMAND_OPTIONS_HPP_

#include <optional>
#include <string>

namespace simulator {

class CommandOptions {
 public:
  CommandOptions() = default;

  auto parse_from_cli_arguments(int argc, const char** argv) -> void;

  auto print_help_message_and_exit() const noexcept -> bool;

  auto print_version_and_exit() const noexcept -> bool;

  auto get_instance_prefix() const noexcept
      -> const std::optional<std::string>&;

  auto get_instance_id() const noexcept -> const std::optional<std::string>&;

  auto get_config_file_path() const noexcept
      -> const std::optional<std::string>&;

  static auto get_help_message() -> std::string;

 private:
  std::optional<std::string> instance_prefix_;
  std::optional<std::string> instance_id_;
  std::optional<std::string> config_file_path_;
  bool version_requested_ = false;
  bool help_requested_ = false;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_COMMAND_OPTIONS_HPP_