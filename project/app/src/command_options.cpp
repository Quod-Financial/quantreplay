#include "ih/command_options.hpp"

#include <fmt/format.h>

#include <cassert>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace simulator {
namespace {

struct Option {
  std::optional<std::string_view> wide_notation{};
  std::optional<std::string_view> short_notation{};
  std::optional<std::string_view> description{};

  constexpr auto operator==(std::string_view notation) const noexcept -> bool {
    return wide_notation == notation || short_notation == notation;
  }
};

constexpr Option HelpOption{.wide_notation = "--help",
                            .short_notation = "-h",
                            .description = "print help message and exit"};

constexpr Option VersionOption{.wide_notation = "--version",
                               .short_notation = "-v",
                               .description = "print version and exit"};

constexpr Option InstancePrefixOption{
    .wide_notation = "--pf",
    .description = "specify instance prefix (required)"};

constexpr Option InstanceIdOption{
    .wide_notation = "--id",
    .description = "specify instance identifier (required)"};

constexpr Option ConfigurationPathOption{
    .short_notation = "-f",
    .description = "specify path to XML configuration file (required)"};

auto get_option_value(const char** option_iter, const char** end)
    -> std::string_view {
  assert(option_iter < end);

  const char** value_iter = std::next(option_iter);
  if (value_iter != end) {
    return *value_iter;
  }

  throw std::runtime_error(
      fmt::format("'{}' option is given without a value", *option_iter));
}

auto format_option_identifier(Option option) noexcept -> std::string {
  assert(option.wide_notation.has_value() || option.short_notation.has_value());

  if (option.wide_notation.has_value() && option.short_notation.has_value()) {
    return fmt::format("{}|{}", *option.short_notation, *option.wide_notation);
  }

  return option.wide_notation.has_value()
             ? std::string{option.wide_notation.value()}
             : std::string{option.short_notation.value()};
}

auto format_option_description(Option option) noexcept -> std::string {
  return option.description.has_value() ? std::string{*option.description}
                                        : "?";
}

}  // namespace

auto CommandOptions::parse_from_cli_arguments(int argc, const char** argv)
    -> void {
  const char** begin = argv;
  const char** end = std::next(argv, argc);

  for (const char** current_option = begin; current_option != end;) {
    if (*current_option == HelpOption) {
      help_requested_ = true;
      std::advance(current_option, 1);
    } else if (*current_option == VersionOption) {
      version_requested_ = true;
      std::advance(current_option, 1);
    } else if (*current_option == InstancePrefixOption) {
      instance_prefix_ = get_option_value(current_option, end);
      std::advance(current_option, 2);
    } else if (*current_option == InstanceIdOption) {
      instance_id_ = get_option_value(current_option, end);
      std::advance(current_option, 2);
    } else if (*current_option == ConfigurationPathOption) {
      config_file_path_ = get_option_value(current_option, end);
      std::advance(current_option, 2);
    } else {
      // skip uninteresting option
      std::advance(current_option, 1);
    }
  }
}

auto CommandOptions::print_help_message_and_exit() const noexcept -> bool {
  return help_requested_;
}

auto CommandOptions::print_version_and_exit() const noexcept -> bool {
  return version_requested_;
}

auto CommandOptions::get_instance_prefix() const noexcept
    -> const std::optional<std::string>& {
  return instance_prefix_;
}

auto CommandOptions::get_instance_id() const noexcept
    -> const std::optional<std::string>& {
  return instance_id_;
}

auto CommandOptions::get_config_file_path() const noexcept
    -> const std::optional<std::string>& {
  return config_file_path_;
}

auto CommandOptions::get_help_message() -> std::string {
  return fmt::format(
      "Usage: market-simulator [OPTION] [VALUE] ...\n"
      "\n"
      "  {} - {}\n"
      "  {} - {}\n"
      "  {} [VALUE] - {}\n"
      "  {} [VALUE] - {}\n"
      "  {} [VALUE] - {}",
      format_option_identifier(HelpOption),
      format_option_description(HelpOption),
      format_option_identifier(VersionOption),
      format_option_description(VersionOption),
      format_option_identifier(InstancePrefixOption),
      format_option_description(InstancePrefixOption),
      format_option_identifier(InstanceIdOption),
      format_option_description(InstanceIdOption),
      format_option_identifier(ConfigurationPathOption),
      format_option_description(ConfigurationPathOption));
}

}  // namespace simulator