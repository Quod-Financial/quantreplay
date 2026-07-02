#include <gmock/gmock.h>

#include "ih/utils/path.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

TEST(HttpCleansePathComponent, KeepsPlainValueUnchanged) {
  ASSERT_THAT(cleanse_path_component("Venue1"), Eq("Venue1"));
}

TEST(HttpCleansePathComponent, RemovesDots) {
  ASSERT_THAT(cleanse_path_component("a.b.c"), Eq("abc"));
}

TEST(HttpCleansePathComponent, ReplacesIllegalCharactersWithDash) {
  ASSERT_THAT(cleanse_path_component(R"(a/b\c:d*e?f"g<h>i|j)"),
              Eq("a-b-c-d-e-f-g-h-i-j"));
}

TEST(HttpCleansePathComponent, RemovesDotsBeforeReplacingIllegalCharacters) {
  ASSERT_THAT(cleanse_path_component("../etc/passwd"), Eq("-etc-passwd"));
}

TEST(HttpCleansePathComponent, ReturnsEmptyStringForEmptyInput) {
  ASSERT_THAT(cleanse_path_component(""), IsEmpty());
}

TEST(HttpMakeDictionariesFilename, ComposesNameFromVenueAndSession) {
  ASSERT_THAT(make_dictionaries_filename("Venue1", "Session1"),
              Eq("Venue1_Session1_Dictionaries.zip"));
}

TEST(HttpMakeDictionariesFilename, CleansesVenueAndSessionComponents) {
  ASSERT_THAT(make_dictionaries_filename("Ven.ue/1", "Ses:sion?2"),
              Eq("Venue-1_Ses-sion-2_Dictionaries.zip"));
}

}  // namespace
}  // namespace simulator::http::test
