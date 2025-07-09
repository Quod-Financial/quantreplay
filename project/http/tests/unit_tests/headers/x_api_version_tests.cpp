#include <gmock/gmock.h>

#include <string>

#include "ih/headers/x_api_version.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

TEST(XApiVersion, StoresNoValueIfHeaderIsAbsent) {
  XApiVersion header;

  header.parse(std::string{});

  ASSERT_FALSE(header.version().has_value());
}

TEST(XApiVersion, ParsesToString) {
  XApiVersion header;

  header.parse("version_42");

  ASSERT_THAT(header.version(), Optional(Eq("version_42")));
}

}  // namespace
}  // namespace simulator::http::test
