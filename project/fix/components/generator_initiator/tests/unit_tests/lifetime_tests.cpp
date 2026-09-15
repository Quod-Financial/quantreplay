#include <gmock/gmock.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include "fix/generator_initiator/generator_initiator.hpp"
#include "fix/generator_initiator/lifetime.hpp"

namespace simulator::fix::test {
namespace {

using namespace ::testing;  // NOLINT

struct GeneratorInitiatorLifetime : public Test {
  [[nodiscard]]
  auto create_initiator(const std::string& configuration) const
      -> std::optional<GeneratorInitiator> {
    std::ofstream{config_path} << configuration;
    return create_generator_initiator(config_path);
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
           store_path.string() + "\n";
  }

  constexpr static auto session_identity =
      "BeginString=FIXT.1.1\n"
      "SenderCompID=MktSimulator\n"
      "TargetCompID=QuodGateway\n";

 private:
  auto TearDown() -> void override {
    std::filesystem::remove(config_path);
    std::filesystem::remove_all(store_path);
  }

  static auto make_test_path(const std::string_view suffix)
      -> std::filesystem::path {
    const auto* current_test = UnitTest::GetInstance()->current_test_info();
    assert(current_test);

    return std::filesystem::path{TempDir()} /
           (std::string{current_test->name()} + std::string{suffix});
  }

  std::filesystem::path config_path = make_test_path(".cfg");
  std::filesystem::path store_path = make_test_path(".store");
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
      session_requirements() + "\n[SESSION]\n" + session_identity);

  ASSERT_THAT(initiator.has_value(), IsFalse());
}

TEST_F(GeneratorInitiatorLifetime,
       CreatesInitiatorWhenSessionDefinesConnectionType) {
  const auto initiator = create_initiator(
      "[DEFAULT]\n"
      "ConnectionType=acceptor\n" +
      session_requirements() + "\n[SESSION]\n" + session_identity +
      "ConnectionType=initiator\n");

  ASSERT_THAT(initiator.has_value(), IsTrue());
}

TEST_F(GeneratorInitiatorLifetime,
       CreatesInitiatorWhenConnectionTypeIsInherited) {
  const auto initiator = create_initiator(
      "[DEFAULT]\n"
      "ConnectionType=initiator\n" +
      session_requirements() + "\n[SESSION]\n" + session_identity);

  ASSERT_THAT(initiator.has_value(), IsTrue());
}

}  // namespace
}  // namespace simulator::fix::test
