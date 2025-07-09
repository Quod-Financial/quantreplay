#include <gmock/gmock.h>

#include "include/log/logging.hpp"

namespace simulator::log::test {
namespace {

TEST(LocationConverter, ConvertsFromStdSourceLocationToSpdlogSourceLoc) {
  constexpr std::source_location location = std::source_location::current();
  constexpr spdlog::source_loc spdlog_location =
      details::convert_location(location);

  ASSERT_EQ(spdlog_location.filename, location.file_name());
  ASSERT_EQ(spdlog_location.line, location.line());
  ASSERT_EQ(spdlog_location.funcname, location.function_name());
}

}  // namespace
}  // namespace simulator::log::test
