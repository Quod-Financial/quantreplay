#include <fmt/format.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string_view>

#include "core/version.hpp"
#include "ih/application.hpp"
#include "ih/command_options.hpp"
#include "ih/loading.hpp"
#include "ih/loop.hpp"

namespace simulator {
namespace {

auto parse_options(CommandOptions& options,
                   int argc,
                   const char** argv) noexcept -> void try {
  options.parse_from_cli_arguments(argc, argv);
} catch (const std::exception& exception) {
  fmt::println(stderr, "failed to process arguments: {}", exception.what());
  fmt::println(stderr, "{}", simulator::CommandOptions::get_help_message());
  std::exit(EXIT_FAILURE);
} catch (...) {
  fmt::println(stderr, "failed to process arguments: unknown error occurred");
  fmt::println(stderr, "{}", simulator::CommandOptions::get_help_message());
  std::exit(EXIT_FAILURE);
}

auto get_stop_signal_name(int signal_code) noexcept -> std::string_view {
  switch (signal_code) {
    case SIGINT:
      return "SIGINT";
    case SIGQUIT:
      return "SIGQUIT";
    case SIGTERM:
      return "SIGTERM";
    default:
      return "unknown";
  }
}

auto terminate_application([[maybe_unused]] int signal_code) -> void {
  Loop::release_main_thread();
}

auto set_termination_signal_handler(int signal_code) -> void {
  if (SIG_ERR == std::signal(signal_code, terminate_application)) {
    throw std::runtime_error(
        fmt::format("failed to set termination handler on '{}' signal",
                    get_stop_signal_name(signal_code)));
  }
}

auto run_application(const simulator::CommandOptions& options) noexcept
    -> void try {
  load_configuration(options);
  load_logger(options, simulator::cfg::log());

  // constructor launches application, destructor - terminates
  Application app = Application::start();

  set_termination_signal_handler(SIGINT);
  set_termination_signal_handler(SIGQUIT);
  set_termination_signal_handler(SIGTERM);

  Loop::suspend_main_thread();

} catch (const std::exception& exception) {
  fmt::println(stderr, "failed to run application: {}", exception.what());
  std::exit(EXIT_FAILURE);
} catch (...) {
  fmt::println(stderr, "failed to run application: unknown error occurred");
  std::exit(EXIT_FAILURE);
}

}  // namespace
}  // namespace simulator

auto main(int argc, const char** argv) -> int {
  simulator::CommandOptions options;
  simulator::parse_options(options, argc, argv);

  if (options.print_help_message_and_exit()) {
    fmt::println(stdout, "{}", simulator::CommandOptions::get_help_message());
    std::exit(EXIT_SUCCESS);
  }

  if (options.print_version_and_exit()) {
    fmt::println(stdout, "{}", simulator::core::version());
    std::exit(EXIT_SUCCESS);
  }

  simulator::run_application(options);

  return EXIT_SUCCESS;
}