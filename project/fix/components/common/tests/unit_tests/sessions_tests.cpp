#include <gmock/gmock.h>
#include <quickfix/Session.h>
#include <quickfix/SessionID.h>

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "common/sessions.hpp"

namespace simulator::fix::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,
// cppcoreguidelines-non-private-member-variables-in-classes)

// Neither an acceptor nor an initiator - it only repeats the two connector
// methods get_sessions calls, so their QuickFIX camelCase spelling is kept.
class StubConnector {
 public:
  auto add_session(FIX::Session& session) -> void {
    session_ids_.insert(session.getSessionID());
    sessions_.emplace(session.getSessionID(), std::addressof(session));
  }

  [[nodiscard]]
  auto getSessions() const -> const std::set<FIX::SessionID>& {
    return session_ids_;
  }

  [[nodiscard]]
  auto getSession(const FIX::SessionID& session_id) const -> FIX::Session* {
    const auto session = sessions_.find(session_id);
    return session != sessions_.end() ? session->second : nullptr;
  }

 private:
  std::set<FIX::SessionID> session_ids_;
  std::map<FIX::SessionID, FIX::Session*> sessions_;
};

class FixSessions : public Test {
 public:
  StubConnector connector;

  auto create_session(const std::string& target_comp_id) -> FIX::Session& {
    created_sessions_.push_back(std::make_unique<FIX::Session>(
        application_,
        memory_store_factory_,
        FIX::SessionID{"FIXT.1.1", "Sender", target_comp_id},
        data_dictionary_provider_,
        time_range_,
        0,
        nullptr));
    return *created_sessions_.back();
  }

 protected:
  FIX::NullApplication application_;
  FIX::MemoryStoreFactory memory_store_factory_;
  FIX::DataDictionaryProvider data_dictionary_provider_;
  FIX::TimeRange time_range_{FIX::UtcTimeOnly{0, 0, 0},
                             FIX::UtcTimeOnly{0, 0, 0}};
  std::vector<std::unique_ptr<FIX::Session>> created_sessions_;
};

TEST_F(FixSessions, ReturnsNoSessionsWhenConnectorDefinesNone) {
  ASSERT_THAT(get_sessions(connector), IsEmpty());
}

TEST_F(FixSessions, ReturnsSessionDefinedByConnector) {
  auto& session = create_session("Target");
  connector.add_session(session);

  ASSERT_THAT(get_sessions(connector), ElementsAre(std::addressof(session)));
}

TEST_F(FixSessions, ReturnsAllSessionsDefinedByConnector) {
  auto& first_session = create_session("FirstTarget");
  auto& second_session = create_session("SecondTarget");
  connector.add_session(first_session);
  connector.add_session(second_session);

  ASSERT_THAT(get_sessions(connector),
              UnorderedElementsAre(std::addressof(first_session),
                                   std::addressof(second_session)));
}

// NOLINTEND(*magic-numbers*,
// cppcoreguidelines-non-private-member-variables-in-classes)

}  // namespace
}  // namespace simulator::fix::test
