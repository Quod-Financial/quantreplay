#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <stdexcept>

#include "ih/command_options.hpp"

namespace simulator::test {
namespace {

using namespace testing;  // NOLINT

struct CommandOptionsParser : public Test {
  CommandOptions options;
};

TEST_F(CommandOptionsParser, ParsesHelpRequestOption) {
  std::array<const char*, 1> args = {"-h"};

  options.parse_from_cli_arguments(args.size(), args.data());

  ASSERT_THAT(options.print_help_message_and_exit(), IsTrue());
}

TEST_F(CommandOptionsParser, ParsesWideHelpRequestOption) {
  std::array<const char*, 1> args = {"--help"};

  options.parse_from_cli_arguments(args.size(), args.data());

  ASSERT_THAT(options.print_help_message_and_exit(), IsTrue());
}

TEST_F(CommandOptionsParser, ParsesVersionRequestOption) {
  std::array<const char*, 1> args = {"-v"};

  options.parse_from_cli_arguments(args.size(), args.data());

  ASSERT_THAT(options.print_version_and_exit(), IsTrue());
}

TEST_F(CommandOptionsParser, ParsesWideVersionRequestOption) {
  std::array<const char*, 1> args = {"--version"};

  options.parse_from_cli_arguments(args.size(), args.data());

  ASSERT_THAT(options.print_version_and_exit(), IsTrue());
}

TEST_F(CommandOptionsParser, ParsesInstancePrefixOption) {
  std::array<const char*, 2> args = {"--pf", "DEV."};

  options.parse_from_cli_arguments(args.size(), args.data());

  ASSERT_THAT(options.get_instance_prefix(), Optional(Eq("DEV.")));
}

TEST_F(CommandOptionsParser, ReportsInstancePrefixOptionWithoutValue) {
  std::array<const char*, 1> args = {"--pf"};

  ASSERT_THROW(options.parse_from_cli_arguments(args.size(), args.data()),
               std::runtime_error);
}

TEST_F(CommandOptionsParser, ParsesInstanceIdOption) {
  std::array<const char*, 2> args = {"--id", "SIM-INTERNAL"};

  options.parse_from_cli_arguments(args.size(), args.data());

  ASSERT_THAT(options.get_instance_id(), Optional(Eq("SIM-INTERNAL")));
}

TEST_F(CommandOptionsParser, ReportsInstanceIdOptionWithoutValue) {
  std::array<const char*, 1> args = {"--id"};

  ASSERT_THROW(options.parse_from_cli_arguments(args.size(), args.data()),
               std::runtime_error);
}

TEST_F(CommandOptionsParser, ParsesConfigurationPathOption) {
  std::array<const char*, 2> args = {"-f", "/config/path"};

  options.parse_from_cli_arguments(args.size(), args.data());

  ASSERT_THAT(options.get_config_file_path(), Optional(Eq("/config/path")));
}

TEST_F(CommandOptionsParser, ReportsConfigurationPathOptionWithoutValue) {
  std::array<const char*, 1> args = {"-f"};

  ASSERT_THROW(options.parse_from_cli_arguments(args.size(), args.data()),
               std::runtime_error);
}

TEST_F(CommandOptionsParser, IgnoresUnknownOption) {
  std::array<const char*, 1> args = {"--unknown"};

  ASSERT_NO_THROW(options.parse_from_cli_arguments(args.size(), args.data()));
}

TEST_F(CommandOptionsParser, CreatesHelpMessage) {
  constexpr std::string_view help_message =
      "Usage: market-simulator [OPTION] [VALUE] ...\n"
      "\n"
      "  -h|--help - print help message and exit\n"
      "  -v|--version - print version and exit\n"
      "  --pf [VALUE] - specify instance prefix (required)\n"
      "  --id [VALUE] - specify instance identifier (required)\n"
      "  -f [VALUE] - specify path to XML configuration file (required)";

  ASSERT_THAT(CommandOptions::get_help_message(), Eq(help_message));
}

}  // namespace
}  // namespace simulator::test