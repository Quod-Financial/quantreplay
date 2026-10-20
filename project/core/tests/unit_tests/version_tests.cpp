#include <gmock/gmock.h>

#include <algorithm>
#include <string>
#include <string_view>

#include "core/version.hpp"

namespace simulator::core::test {
namespace {

using namespace testing;  // NOLINT

// git log --format=%ci renders the revision time as "2026-03-19 12:03:45 +0000"
constexpr std::string_view RevisionTimeRegex{
    R"([0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2} [+-][0-9]{4})"};

// the major version is a branch name or a tag number, so it may itself contain
// dots and underscores and has to be stripped before the revision is examined
[[nodiscard]] auto revision() -> std::string {
  const std::string full{version()};
  const std::string prefix = std::string{major_version()} + '.';
  return full.starts_with(prefix) ? full.substr(prefix.size()) : full;
}

TEST(CoreVersion, StartsWithMajorVersionFollowedByDot) {
  ASSERT_THAT(std::string{version()},
              StartsWith(std::string{major_version()} + '.'));
}

TEST(CoreVersion, SeparatesRevisionComponentsWithSingleUnderscore) {
  const std::string rev = revision();

  ASSERT_THAT(std::ranges::count(rev, '_'), Eq(1));
}

TEST(CoreVersion, ReportsUnixTimestampAsFirstRevisionComponent) {
  ASSERT_THAT(revision(), MatchesRegex(R"(^[0-9]+_.*)"));
}

TEST(CoreVersion, ReportsAbbreviatedCommitHashAsSecondRevisionComponent) {
  ASSERT_THAT(revision(), MatchesRegex(R"(.*_[0-9a-f]{8}\(.*)"));
}

TEST(CoreVersion, EnclosesRevisionDateTimeWithUtcOffsetInParentheses) {
  const std::string pattern =
      R"(.*\()" + std::string{RevisionTimeRegex} + R"(\)$)";

  ASSERT_THAT(revision(), MatchesRegex(pattern));
}

}  // namespace
}  // namespace simulator::core::test
