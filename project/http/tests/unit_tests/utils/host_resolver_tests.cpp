#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "ih/utils/host_resolver.hpp"

namespace simulator::http::test {
namespace {

using ::testing::MatchesRegex;

TEST(HttpResolveHostIp, ReturnsIpV4FormattedAddress) {
  ASSERT_THAT(resolve_host_ip(),
              MatchesRegex(R"(([0-9]{1,3}\.){3}[0-9]{1,3})"));
}

}  // namespace
}  // namespace simulator::http::test
