#include <gmock/gmock.h>

#include "include/log/logger.hpp"

namespace simulator::log::test {
namespace {

TEST(LoggerFileSize, ConvertsMegabytesIntoBytes) {
  constexpr auto mb = static_cast<std::size_t>(FileSize::megabytes(5));
  ASSERT_EQ(mb, 5 * 1024 * 1024);
}

}  // namespace
}  // namespace simulator::log::test
