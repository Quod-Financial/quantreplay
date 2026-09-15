#include <gmock/gmock.h>

#include <optional>

#include "core/tools/fix_session_id.hpp"

namespace simulator::core::test {
namespace {

using namespace testing;  // NOLINT

TEST(CoreFixSessionIdParsing, ReturnsNothingForEmptyText) {
  ASSERT_THAT(parse_fix_session_id(""), Eq(std::nullopt));
}

TEST(CoreFixSessionIdParsing, ReturnsNothingWithoutBeginString) {
  ASSERT_THAT(parse_fix_session_id("SENDER->TARGET"), Eq(std::nullopt));
}

TEST(CoreFixSessionIdParsing, ReturnsNothingWithoutCompIdSeparator) {
  ASSERT_THAT(parse_fix_session_id("FIXT.1.1:SENDER"), Eq(std::nullopt));
}

TEST(CoreFixSessionIdParsing, ReturnsNothingWithEmptyPart) {
  ASSERT_THAT(parse_fix_session_id("FIXT.1.1:->TARGET"), Eq(std::nullopt));
}

TEST(CoreFixSessionIdParsing, ReturnsNothingWithEmptySessionQualifier) {
  ASSERT_THAT(parse_fix_session_id("FIXT.1.1:SENDER->TARGET:"),
              Eq(std::nullopt));
}

TEST(CoreFixSessionIdParsing, ParsesSessionIdWithoutQualifier) {
  ASSERT_THAT(parse_fix_session_id("FIXT.1.1:SENDER->TARGET"),
              Optional(Eq(FixSessionId{.begin_string = "FIXT.1.1",
                                       .sender_comp_id = "SENDER",
                                       .target_comp_id = "TARGET"})));
}

TEST(CoreFixSessionIdParsing, ParsesSessionIdWithQualifier) {
  ASSERT_THAT(parse_fix_session_id("FIXT.1.1:SENDER->TARGET:QUALIFIER"),
              Optional(Eq(FixSessionId{.begin_string = "FIXT.1.1",
                                       .sender_comp_id = "SENDER",
                                       .target_comp_id = "TARGET",
                                       .session_qualifier = "QUALIFIER"})));
}

}  // namespace
}  // namespace simulator::core::test
