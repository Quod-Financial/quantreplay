#include <fmt/format.h>
#include <gmock/gmock.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "fix/generator_initiator/generator_initiator.hpp"
#include "fix/generator_initiator/lifetime.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix::test {
namespace {

using namespace ::testing;  // NOLINT

struct GeneratorInitiatorLifetime : public Test {
  constexpr static std::string_view SessionBeginString{"FIXT.1.1"};
  constexpr static std::string_view SessionSenderCompId{"MktSimulator"};

  [[nodiscard]]
  auto create_initiator(const std::string& configuration)
      -> std::optional<GeneratorInitiator> {
    return create_generator_initiator(write_config(".cfg", configuration));
  }

  auto write_config(const std::string_view suffix,
                    const std::string& configuration)
      -> std::filesystem::path {
    auto path = make_test_path(suffix);
    std::ofstream{path} << configuration;
    config_paths_.push_back(path);
    return path;
  }

  [[nodiscard]]
  auto missing_config_path() const -> std::filesystem::path {
    return make_test_path(".missing.cfg");
  }

  // Settings a quickfix session requires on top of its connection type.
  [[nodiscard]]
  auto session_requirements() const -> std::string {
    return "DefaultApplVerID=FIX.5.0SP2\n"
           "UseDataDictionary=N\n"
           "StartTime=00:00:00\n"
           "EndTime=00:00:00\n"
           "HeartBtInt=30\n"
           "SocketConnectHost=127.0.0.1\n"
           "SocketConnectPort=45001\n"
           "FileStorePath=" +
           store_path_.string() + "\n";
  }

  [[nodiscard]]
  auto initiator_configuration(const std::string_view target_comp_id) const
      -> std::string {
    return "[DEFAULT]\n"
           "ConnectionType=initiator\n" +
           session_requirements() + session_definition(target_comp_id);
  }

  [[nodiscard]]
  static auto session_definition(const std::string_view target_comp_id)
      -> std::string {
    return fmt::format(
        "\n[SESSION]\n"
        "BeginString={}\n"
        "SenderCompID={}\n"
        "TargetCompID={}\n",
        SessionBeginString,
        SessionSenderCompId,
        target_comp_id);
  }

  [[nodiscard]]
  static auto session(const std::string_view target_comp_id)
      -> protocol::fix::Session {
    return protocol::fix::Session{
        protocol::fix::BeginString{std::string{SessionBeginString}},
        protocol::fix::SenderCompId{std::string{SessionSenderCompId}},
        protocol::fix::TargetCompId{std::string{target_comp_id}}};
  }

 private:
  auto TearDown() -> void override {
    for (const auto& path : config_paths_) {
      std::filesystem::remove(path);
    }
    std::filesystem::remove_all(store_path_);
  }

  static auto make_test_path(const std::string_view suffix)
      -> std::filesystem::path {
    const auto* current_test = UnitTest::GetInstance()->current_test_info();
    assert(current_test);

    return std::filesystem::path{TempDir()} /
           (std::string{current_test->name()} + std::string{suffix});
  }

  std::vector<std::filesystem::path> config_paths_;
  std::filesystem::path store_path_ = make_test_path(".store");
};

TEST_F(GeneratorInitiatorLifetime, DoesNotCreateInitiatorWithoutAnySession) {
  const auto initiator = create_initiator(
      "[DEFAULT]\n"
      "ConnectionType=initiator\n" +
      session_requirements());

  ASSERT_THAT(initiator.has_value(), IsFalse());
}

TEST_F(GeneratorInitiatorLifetime,
       DoesNotCreateInitiatorWhenEverySessionIsAcceptor) {
  const auto initiator = create_initiator(
      "[DEFAULT]\n"
      "ConnectionType=acceptor\n" +
      session_requirements() + session_definition("QuodGateway"));

  ASSERT_THAT(initiator.has_value(), IsFalse());
}

TEST_F(GeneratorInitiatorLifetime,
       CreatesInitiatorWhenSessionDefinesConnectionType) {
  const auto initiator = create_initiator(
      "[DEFAULT]\n"
      "ConnectionType=acceptor\n" +
      session_requirements() + session_definition("QuodGateway") +
      "ConnectionType=initiator\n");

  ASSERT_THAT(initiator.has_value(), IsTrue());
}

TEST_F(GeneratorInitiatorLifetime,
       CreatesInitiatorWhenConnectionTypeIsInherited) {
  const auto initiator = create_initiator(
      "[DEFAULT]\n"
      "ConnectionType=initiator\n" +
      session_requirements() + session_definition("QuodGateway"));

  ASSERT_THAT(initiator.has_value(), IsTrue());
}

TEST_F(GeneratorInitiatorLifetime,
       DoesNotCreateInitiatorWhenConfigurationFileIsMissing) {
  const auto initiator = create_generator_initiator(missing_config_path());

  ASSERT_THAT(initiator.has_value(), IsFalse());
}

TEST_F(GeneratorInitiatorLifetime,
       DoesNotCreateInitiatorWhenQuickfixRejectsSession) {
  const auto initiator = create_initiator(
      initiator_configuration("QuodGateway") + "HeartBtInt=0\n");

  ASSERT_THAT(initiator.has_value(), IsFalse());
}

TEST_F(GeneratorInitiatorLifetime,
       DoesNotCreateInitiatorForSessionAlreadyInUse) {
  const auto live = create_generator_initiator(
      write_config(".first.cfg", initiator_configuration("QuodGateway")));
  ASSERT_THAT(live.has_value(), IsTrue());

  const auto duplicate = create_generator_initiator(
      write_config(".second.cfg", initiator_configuration("QuodGateway")));

  ASSERT_THAT(duplicate.has_value(), IsFalse());
}

TEST_F(GeneratorInitiatorLifetime,
       CreatesSecondInitiatorForDifferentSession) {
  const auto live = create_generator_initiator(
      write_config(".first.cfg", initiator_configuration("QuodGateway")));
  ASSERT_THAT(live.has_value(), IsTrue());

  const auto another = create_generator_initiator(
      write_config(".second.cfg", initiator_configuration("OtherGateway")));

  ASSERT_THAT(another.has_value(), IsTrue());
}

TEST_F(GeneratorInitiatorLifetime,
       CreatesInitiatorAgainAfterPreviousOneIsDestroyed) {
  const auto config_path =
      write_config(".cfg", initiator_configuration("QuodGateway"));
  ASSERT_THAT(create_generator_initiator(config_path).has_value(), IsTrue());

  const auto recreated = create_generator_initiator(config_path);

  ASSERT_THAT(recreated.has_value(), IsTrue());
}

TEST_F(GeneratorInitiatorLifetime,
       CreatesInitiatorForSessionOfRejectedConfiguration) {
  const auto rejected = create_generator_initiator(
      write_config(".rejected.cfg",
                   initiator_configuration("FirstGateway") +
                       session_definition("SecondGateway") + "HeartBtInt=0\n"));
  ASSERT_THAT(rejected.has_value(), IsFalse());

  const auto initiator = create_generator_initiator(
      write_config(".cfg", initiator_configuration("FirstGateway")));

  ASSERT_THAT(initiator.has_value(), IsTrue());
}

TEST_F(GeneratorInitiatorLifetime,
       DoesNotReportAcceptorSessionOfConfiguration) {
  const auto initiator = create_initiator(
      initiator_configuration("QuodGateway") + session_definition("Client") +
      "ConnectionType=acceptor\n");
  ASSERT_THAT(initiator.has_value(), IsTrue());

  ASSERT_THAT(generator_initiator_sessions(*initiator),
              ElementsAre(session("QuodGateway")));
}

TEST_F(GeneratorInitiatorLifetime,
       ReportsEveryInitiatorSessionOfConfiguration) {
  const auto initiator =
      create_initiator(initiator_configuration("QuodGateway") +
                       session_definition("OtherGateway"));
  ASSERT_THAT(initiator.has_value(), IsTrue());

  ASSERT_THAT(
      generator_initiator_sessions(*initiator),
      UnorderedElementsAre(session("QuodGateway"), session("OtherGateway")));
}

}  // namespace
}  // namespace simulator::fix::test
