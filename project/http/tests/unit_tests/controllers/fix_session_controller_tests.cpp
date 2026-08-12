#include <gmock/gmock.h>

#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include "core/common/session_settings.hpp"
#include "core/tools/time.hpp"
#include "data_layer/api/models/fix_session.hpp"
#include "ih/controllers/fix_session_controller.hpp"
#include "protocol/app/session_connected_event.hpp"
#include "protocol/app/session_terminated_event.hpp"
#include "protocol/types/session.hpp"
#include "tests/mocks/config_provider.hpp"
#include "tests/mocks/fix_session_accessor.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

[[nodiscard]]
auto sample_last_connected_time() -> core::sys_us {
  using namespace std::chrono_literals;
  return std::chrono::sys_days{2026y / std::chrono::May / 6} + 11h + 31min +
         40s + 904900us;
}

MATCHER_P3(IsFixSessionPatch, venue_id, session_id, last_connected_time, "") {
  return ExplainMatchResult(
      AllOf(Property(
                "venue_id", &data_layer::FixSession::Patch::venue_id, venue_id),
            Property("session_id",
                     &data_layer::FixSession::Patch::session_id,
                     session_id),
            Property("last_connected_time",
                     &data_layer::FixSession::Patch::last_connected_time,
                     last_connected_time)),
      arg,
      result_listener);
}

struct HttpFixSessionController : Test {
  static auto make_default_section(std::string accept_port)
      -> core::FixSessionSettings {
    return core::FixSessionSettings{
        .heading = "DEFAULT",
        .id = std::nullopt,
        .settings = {{"SOCKETACCEPTPORT", std::move(accept_port)}}};
  }

  static auto make_config_session_without_port(std::string id)
      -> core::FixSessionSettings {
    return core::FixSessionSettings{
        .heading = "SESSION", .id = std::move(id), .settings = {}};
  }

  static auto make_config_session(std::string id, std::string accept_port)
      -> core::FixSessionSettings {
    return core::FixSessionSettings{
        .heading = "SESSION",
        .id = std::move(id),
        .settings = {{"SOCKETACCEPTPORT", std::move(accept_port)}}};
  }

  auto make_db_session(std::string session_id,
                       std::optional<core::sys_us> last_connected_time) const
      -> data_layer::FixSession {
    auto patch =
        data_layer::FixSession::Patch{}.with_venue_id(VenueId).with_session_id(
            std::move(session_id));
    if (last_connected_time.has_value()) {
      patch.with_last_connected_time(*last_connected_time);
    }
    return data_layer::FixSession::create(std::move(patch));
  }

  static auto make_connected_event(const std::string& sender_comp_id,
                                   const std::string& target_comp_id)
      -> protocol::SessionConnectedEvent {
    return protocol::SessionConnectedEvent{protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIX.4.4"},
                               protocol::fix::SenderCompId{sender_comp_id},
                               protocol::fix::TargetCompId{target_comp_id}}}};
  }

  static auto make_terminated_event(const std::string& sender_comp_id,
                                    const std::string& target_comp_id)
      -> protocol::SessionTerminatedEvent {
    return protocol::SessionTerminatedEvent{protocol::Session{
        protocol::fix::Session{protocol::fix::BeginString{"FIX.4.4"},
                               protocol::fix::SenderCompId{sender_comp_id},
                               protocol::fix::TargetCompId{target_comp_id}}}};
  }

  auto make_controller() -> FixSessionControllerImpl {
    return FixSessionControllerImpl{accessor, config_provider, HostIp};
  }

  void SetUp() override {
    ON_CALL(*config_provider, venue_id()).WillByDefault(ReturnRef(VenueId));
    ON_CALL(*config_provider, session_settings())
        .WillByDefault(ReturnRef(session_settings_));
    ON_CALL(*accessor, select_all(_)).WillByDefault([this](const auto&) {
      return tl::expected<std::vector<data_layer::FixSession>,
                          data_bridge::Failure>{db_sessions_};
    });
  }

  static constexpr std::string HostIp = "192.168.0.10";
  static constexpr std::string VenueId = "LSE";

  // using EmptyResult = tl::expected<void, data_bridge::Failure>;

  std::shared_ptr<NiceMock<mock::FixSessionAccessor>> accessor =
      std::make_shared<NiceMock<mock::FixSessionAccessor>>();
  std::shared_ptr<NiceMock<mock::ConfigProvider>> config_provider =
      std::make_shared<NiceMock<mock::ConfigProvider>>();

  std::vector<core::FixSessionSettings> session_settings_;
  std::vector<data_layer::FixSession> db_sessions_;
};

TEST_F(HttpFixSessionController,
       PopulatesSessionsFromConfigProviderOnConstruction) {
  const std::string session_id = "FIX.4.4:MDSERVER->MDCLIENT";
  session_settings_ = {make_config_session(session_id, "4500")};
  db_sessions_ = {make_db_session(session_id, sample_last_connected_time())};

  const auto controller = make_controller();

  auto sessions = controller.sessions();
  ASSERT_THAT(sessions, Contains(Key(session_id)));

  const auto& info = sessions[session_id];
  EXPECT_EQ(info.host_port, HostIp + ":4500");
  EXPECT_FALSE(info.connected);
  EXPECT_THAT(info.last_connected_time,
              Optional(Eq(sample_last_connected_time())));
}

TEST_F(HttpFixSessionController,
       UsesDefaultSectionAcceptPortWhenSessionHasNoPort) {
  const std::string session_id = "FIX.4.4:MDSERVER->MDCLIENT";
  session_settings_ = {make_default_section("4400"),
                       make_config_session_without_port(session_id)};
  db_sessions_ = {make_db_session(session_id, std::nullopt)};

  const auto controller = make_controller();

  auto sessions = controller.sessions();
  ASSERT_THAT(sessions, Contains(Key(session_id)));
  EXPECT_EQ(sessions[session_id].host_port, HostIp + ":4400");
}

TEST_F(HttpFixSessionController,
       PrefersSessionAcceptPortOverDefaultSectionPort) {
  const std::string session_id = "FIX.4.4:MDSERVER->MDCLIENT";
  session_settings_ = {make_default_section("4400"),
                       make_config_session(session_id, "4500")};
  db_sessions_ = {make_db_session(session_id, std::nullopt)};

  const auto controller = make_controller();

  auto sessions = controller.sessions();
  ASSERT_THAT(sessions, Contains(Key(session_id)));
  EXPECT_EQ(sessions[session_id].host_port, HostIp + ":4500");
}

TEST_F(HttpFixSessionController,
       UsesHostIpOnlyWhenNeitherSessionNorDefaultDefinePort) {
  const std::string session_id = "FIX.4.4:MDSERVER->MDCLIENT";
  session_settings_ = {make_config_session_without_port(session_id)};
  db_sessions_ = {make_db_session(session_id, std::nullopt)};

  const auto controller = make_controller();

  auto sessions = controller.sessions();
  ASSERT_THAT(sessions, Contains(Key(session_id)));
  EXPECT_EQ(sessions[session_id].host_port, HostIp);
}

TEST_F(HttpFixSessionController,
       DeletesFixSessionRowsAbsentFromConfigOnConstruction) {
  session_settings_ = {
      make_config_session("FIX.4.4:MDSERVER->MDCLIENT", "4500")};
  db_sessions_ = {
      make_db_session("FIX.4.4:MDSERVER->MDCLIENT", std::nullopt),
      make_db_session("FIX.4.4:OLDSERVER->OLDCLIENT", std::nullopt)};

  EXPECT_CALL(
      *accessor,
      delete_all(Eq(VenueId), ElementsAre("FIX.4.4:OLDSERVER->OLDCLIENT")))
      .Times(1);

  const auto controller = make_controller();
}

TEST_F(HttpFixSessionController,
       InsertsSessionsAbsentFromFixSessionTableOnConstruction) {
  const std::string session_id1 = "FIX.4.4:MDSERVER1->MDCLIENT1";
  const std::string session_id2 = "FIX.4.4:MDSERVER2->MDCLIENT2";
  session_settings_ = {make_config_session(session_id1, "4500"),
                       make_config_session(session_id2, "4501")};
  db_sessions_ = {};

  ON_CALL(*accessor, select_all(_))
      .WillByDefault(Return(tl::expected<std::vector<data_layer::FixSession>,
                                         data_bridge::Failure>{db_sessions_}));

  EXPECT_CALL(*accessor,
              add(IsFixSessionPatch(
                  Optional(VenueId), Optional(session_id1), Eq(std::nullopt))))
      .Times(1);
  EXPECT_CALL(*accessor,
              add(IsFixSessionPatch(
                  Optional(VenueId), Optional(session_id2), Eq(std::nullopt))))
      .Times(1);

  const auto controller = make_controller();
}

TEST_F(HttpFixSessionController, MarksConnectedOnSessionConnectedEvent) {
  const std::string session_id = "FIX.4.4:MDSERVER->MDCLIENT";
  session_settings_ = {make_config_session(session_id, "4500")};
  db_sessions_ = {make_db_session(session_id, std::nullopt)};
  auto controller = make_controller();

  controller.handle(make_connected_event("MDSERVER", "MDCLIENT"));

  EXPECT_TRUE(controller.sessions().at(session_id).connected);
}

TEST_F(HttpFixSessionController,
       UpdatesLastConnectedTimeOnSessionConnectedEvent) {
  const std::string session_id = "FIX.4.4:MDSERVER->MDCLIENT";
  session_settings_ = {make_config_session(session_id, "4500")};
  db_sessions_ = {make_db_session(session_id, std::nullopt)};
  auto controller = make_controller();

  controller.handle(make_connected_event("MDSERVER", "MDCLIENT"));

  ASSERT_NE(controller.sessions().at(session_id).last_connected_time,
            std::nullopt);
}

TEST_F(HttpFixSessionController,
       PersistsLastConnectedTimeInDatabaseOnSessionConnectedEvent) {
  const std::string session_id = "FIX.4.4:MDSERVER->MDCLIENT";
  session_settings_ = {make_config_session(session_id, "4500")};
  db_sessions_ = {make_db_session(session_id, std::nullopt)};
  auto controller = make_controller();

  EXPECT_CALL(
      *accessor,
      update(Property(&data_layer::FixSession::Patch::last_connected_time,
                      Ne(std::nullopt)),
             Eq(VenueId),
             Eq(session_id)))
      .Times(1);

  controller.handle(make_connected_event("MDSERVER", "MDCLIENT"));
}

TEST_F(HttpFixSessionController,
       MarksSessionDisconnectedOnSessionTerminatedEvent) {
  const std::string session_id = "FIX.4.4:MDSERVER->MDCLIENT";
  session_settings_ = {make_config_session(session_id, "4500")};
  db_sessions_ = {make_db_session(session_id, std::nullopt)};
  auto controller = make_controller();
  controller.handle(make_connected_event("MDSERVER", "MDCLIENT"));

  controller.handle(make_terminated_event("MDSERVER", "MDCLIENT"));

  EXPECT_FALSE(controller.sessions().at(session_id).connected);
}

}  // namespace
}  // namespace simulator::http::test
