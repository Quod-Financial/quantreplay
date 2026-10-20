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

  [[nodiscard]]
  static auto make_initiator_session(
      std::string_view id, std::map<std::string, std::string> settings)
      -> core::FixSessionSettings {
    settings.emplace("CONNECTIONTYPE", "initiator");
    return make_session(id, std::move(settings));
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

TEST_F(HttpCollectSessionDictionaries,
       ReturnsNulloptWhenMatchingSessionIsInitiator) {
  const std::vector sessions{
      make_initiator_session(SessionId, {{"DATADICTIONARY", "session.xml"}})};

  ASSERT_FALSE(collect_session_dictionaries(sessions, SessionId).has_value());
}

TEST_F(HttpCollectSessionDictionaries,
       ReturnsNulloptWhenSessionInheritsInitiatorConnectionTypeFromDefault) {
  const std::vector sessions{
      make_default({{"CONNECTIONTYPE", "initiator"}}),
      make_session(SessionId, {{"DATADICTIONARY", "session.xml"}})};

  ASSERT_FALSE(collect_session_dictionaries(sessions, SessionId).has_value());
}

TEST_F(HttpCollectSessionDictionaries,
       ReturnsDictionaryPathsWhenSessionOverridesInitiatorDefaultWithAcceptor) {
  const std::vector sessions{make_default({{"CONNECTIONTYPE", "initiator"}}),
                             make_session(SessionId,
                                          {{"CONNECTIONTYPE", "acceptor"},
                                           {"DATADICTIONARY", "session.xml"}})};

  ASSERT_THAT(collect_session_dictionaries(sessions, SessionId),
              Optional(UnorderedElementsAre(Path{"session.xml"})));
}

class HttpHasAcceptorSession : public HttpCollectSessionDictionaries {};

TEST_F(HttpHasAcceptorSession, ReturnsFalseWhenNoSessionMatchesId) {
  const std::vector sessions{make_session(OtherSessionId, {})};

  ASSERT_FALSE(has_acceptor_session(sessions, SessionId));
}

TEST_F(HttpHasAcceptorSession, ReturnsTrueWhenMatchingSessionIsAcceptor) {
  const std::vector sessions{
      make_session(SessionId, {{"CONNECTIONTYPE", "acceptor"}})};

  ASSERT_TRUE(has_acceptor_session(sessions, SessionId));
}

TEST_F(HttpHasAcceptorSession, ReturnsFalseWhenMatchingSessionIsInitiator) {
  const std::vector sessions{make_initiator_session(SessionId, {})};

  ASSERT_FALSE(has_acceptor_session(sessions, SessionId));
}

TEST_F(HttpHasAcceptorSession,
       ReturnsFalseWhenSessionInheritsInitiatorConnectionTypeFromDefault) {
  const std::vector sessions{make_default({{"CONNECTIONTYPE", "initiator"}}),
                             make_session(SessionId, {})};

  ASSERT_FALSE(has_acceptor_session(sessions, SessionId));
}

TEST_F(HttpHasAcceptorSession,
       ReturnsTrueWhenSessionOverridesInitiatorDefaultWithAcceptor) {
  const std::vector sessions{
      make_default({{"CONNECTIONTYPE", "initiator"}}),
      make_session(SessionId, {{"CONNECTIONTYPE", "acceptor"}})};

  ASSERT_TRUE(has_acceptor_session(sessions, SessionId));
}

}  // namespace
}  // namespace simulator::http::test
