#include <gmock/gmock.h>

#include <optional>
#include <string_view>

#include "core/tools/fix_session.hpp"

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
                                       .target_comp_id = "TARGET",
                                       .session_qualifier = std::nullopt})));
}

TEST(CoreFixSessionIdParsing, ParsesSessionIdWithQualifier) {
  ASSERT_THAT(parse_fix_session_id("FIXT.1.1:SENDER->TARGET:QUALIFIER"),
              Optional(Eq(FixSessionId{.begin_string = "FIXT.1.1",
                                       .sender_comp_id = "SENDER",
                                       .target_comp_id = "TARGET",
                                       .session_qualifier = "QUALIFIER"})));
}

struct CoreFixSessionReferenceMalformedText
    : public TestWithParam<std::string_view> {};

TEST_P(CoreFixSessionReferenceMalformedText, ReturnsNothing) {
  ASSERT_THAT(parse_fix_session_reference(GetParam()), Eq(std::nullopt));
}

INSTANTIATE_TEST_SUITE_P(
    Connections,
    CoreFixSessionReferenceMalformedText,
    Values("SENDER->TARGET, /settings.cfg",
           "FIXT.1.1:SENDER->TARGET,",
           "FIXT.1.1:SENDER->TARGET,   ",
           "FIXT.1.1:SENDER->TARGET, /settings.cfg, /other.cfg"));

TEST(CoreFixSessionReferenceParsing, ParsesSessionIdWithoutSettingsFile) {
  ASSERT_THAT(parse_fix_session_reference("FIXT.1.1:SENDER->TARGET"),
              Optional(Eq(FixSessionReference{
                  .session_id = {.begin_string = "FIXT.1.1",
                                 .sender_comp_id = "SENDER",
                                 .target_comp_id = "TARGET",
                                 .session_qualifier = std::nullopt},
                  .settings_file = std::nullopt})));
}

TEST(CoreFixSessionReferenceParsing, ParsesSessionIdWithSettingsFile) {
  ASSERT_THAT(
      parse_fix_session_reference("FIXT.1.1:SENDER->TARGET,/settings.cfg"),
      Optional(Eq(FixSessionReference{
          .session_id = {.begin_string = "FIXT.1.1",
                         .sender_comp_id = "SENDER",
                         .target_comp_id = "TARGET",
                         .session_qualifier = std::nullopt},
          .settings_file = "/settings.cfg"})));
}

TEST(CoreFixSessionReferenceParsing, ParsesQualifiedSessionIdWithSettingsFile) {
  ASSERT_THAT(parse_fix_session_reference(
                  "FIXT.1.1:SENDER->TARGET:QUALIFIER, /settings.cfg"),
              Optional(Eq(FixSessionReference{
                  .session_id = {.begin_string = "FIXT.1.1",
                                 .sender_comp_id = "SENDER",
                                 .target_comp_id = "TARGET",
                                 .session_qualifier = "QUALIFIER"},
                  .settings_file = "/settings.cfg"})));
}

TEST(CoreFixSessionReferenceParsing, IgnoresWhitespaceAroundValues) {
  ASSERT_THAT(parse_fix_session_reference(
                  "  FIXT.1.1:SENDER->TARGET \t , \t /settings.cfg  "),
              Optional(Eq(FixSessionReference{
                  .session_id = {.begin_string = "FIXT.1.1",
                                 .sender_comp_id = "SENDER",
                                 .target_comp_id = "TARGET",
                                 .session_qualifier = std::nullopt},
                  .settings_file = "/settings.cfg"})));
}

}  // namespace
}  // namespace simulator::core::test
