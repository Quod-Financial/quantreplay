#include <gmock/gmock.h>

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "core/common/session_settings.hpp"
#include "ih/config_provider.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

using Path = std::filesystem::path;

class HttpCollectSessionDictionaries : public Test {
 protected:
  [[nodiscard]]
  static auto make_session(std::string_view id,
                           std::map<std::string, std::string> settings)
      -> core::FixSessionSettings {
    return core::FixSessionSettings{.heading = "SESSION",
                                    .id = std::string{id},
                                    .settings = std::move(settings)};
  }

  [[nodiscard]]
  static auto make_default(std::map<std::string, std::string> settings)
      -> core::FixSessionSettings {
    return core::FixSessionSettings{.heading = "DEFAULT",
                                    .id = std::nullopt,
                                    .settings = std::move(settings)};
  }

  static constexpr std::string_view SessionId{
      "BeginString:Sender->Target:Qualifier"};
  static constexpr std::string_view OtherSessionId{"Other:Other->Other:Other"};
};

TEST_F(HttpCollectSessionDictionaries, ReturnsNulloptWhenNoSessionMatchesId) {
  const std::vector sessions{
      make_session(OtherSessionId, {{"DATADICTIONARY", "other.xml"}})};

  ASSERT_FALSE(collect_session_dictionaries(sessions, SessionId).has_value());
}

TEST_F(HttpCollectSessionDictionaries,
       ReturnsDictionaryPathsOfMatchingSession) {
  const std::vector sessions{make_session(
      SessionId,
      {{"DATADICTIONARY", "session.xml"}, {"BEGINSTRING", "FIX.4.4"}})};

  ASSERT_THAT(collect_session_dictionaries(sessions, SessionId),
              Optional(UnorderedElementsAre(Path{"session.xml"})));
}

TEST_F(HttpCollectSessionDictionaries, IgnoresNonDictionaryKeys) {
  const std::vector sessions{make_session(
      SessionId, {{"BEGINSTRING", "FIX.4.4"}, {"SOCKETACCEPTPORT", "9001"}})};

  ASSERT_THAT(collect_session_dictionaries(sessions, SessionId),
              Optional(IsEmpty()));
}

TEST_F(HttpCollectSessionDictionaries,
       CollectsTransportAndAppDataDictionaryKeys) {
  const std::vector sessions{
      make_session(SessionId,
                   {{"DATADICTIONARY", "data.xml"},
                    {"TRANSPORTDATADICTIONARY", "tt.xml"},
                    {"APPDATADICTIONARY", "app.xml"},
                    {"APPDATADICTIONARY.FIX.4.2", "app42.xml"},
                    {"BEGINSTRING", "FIX.4.4"}})};

  ASSERT_THAT(collect_session_dictionaries(sessions, SessionId),
              Optional(UnorderedElementsAre(Path{"data.xml"},
                                            Path{"tt.xml"},
                                            Path{"app.xml"},
                                            Path{"app42.xml"})));
}

TEST_F(HttpCollectSessionDictionaries, OverlaysDefaultSectionForAbsentKeys) {
  const std::vector sessions{
      make_default({{"TRANSPORTDATADICTIONARY", "default.xml"}}),
      make_session(SessionId, {{"DATADICTIONARY", "session.xml"}})};

  ASSERT_THAT(
      collect_session_dictionaries(sessions, SessionId),
      Optional(UnorderedElementsAre(Path{"session.xml"}, Path{"default.xml"})));
}

TEST_F(HttpCollectSessionDictionaries,
       PrefersSessionValueOverDefaultForSameKey) {
  const std::vector sessions{
      make_default({{"DATADICTIONARY", "default.xml"}}),
      make_session(SessionId, {{"DATADICTIONARY", "session.xml"}})};

  ASSERT_THAT(collect_session_dictionaries(sessions, SessionId),
              Optional(UnorderedElementsAre(Path{"session.xml"})));
}

TEST_F(HttpCollectSessionDictionaries, WorksWithoutDefaultSection) {
  const std::vector sessions{
      make_session(SessionId, {{"DATADICTIONARY", "session.xml"}})};

  ASSERT_THAT(collect_session_dictionaries(sessions, SessionId),
              Optional(UnorderedElementsAre(Path{"session.xml"})));
}

}  // namespace
}  // namespace simulator::http::test
