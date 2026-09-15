#include "fix/generator_initiator/lifetime.hpp"

#include <quickfix/Dictionary.h>
#include <quickfix/SessionID.h>
#include <quickfix/SessionSettings.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <set>
#include <utility>

#include "common/session_settings.hpp"
#include "ih/implementation.hpp"
#include "log/logging.hpp"

namespace simulator::fix {
namespace {

[[nodiscard]]
auto defines_initiator_session(const FIX::SessionSettings& settings) -> bool {
  const std::set<FIX::SessionID> sessions = settings.getSessions();
  return std::ranges::any_of(sessions, [&](const FIX::SessionID& session_id) {
    const FIX::Dictionary& session = settings.get(session_id);
    return session.has("ConnectionType") &&
           session.getString("ConnectionType") == "initiator";
  });
}

}  // namespace

auto create_generator_initiator(const std::filesystem::path& config_path)
    -> std::optional<GeneratorInitiator> {
  log::debug("creating a FIX initiator instance");

  const auto configuration = read_settings_from_file(config_path);
  if (!defines_initiator_session(configuration)) {
    log::info(
        "fix configuration defines no initiator session, "
        "a FIX initiator instance is not created");
    return std::nullopt;
  }

  std::optional<GeneratorInitiator> initiator{
      std::in_place,
      std::make_unique<GeneratorInitiator::Implementation>(configuration)};

  log::info("created a FIX initiator instance");

  return initiator;
}

auto start_generator_initiator(GeneratorInitiator& initiator) -> void {
  log::debug("starting FIX initiator");

  initiator.implementation().start_connection();

  log::info("started FIX initiator");
}

auto stop_generator_initiator(GeneratorInitiator& initiator) noexcept -> void {
  log::debug("stopping FIX initiator");

  initiator.implementation().stop_connection();

  log::info("stopped FIX initiator");
}

}  // namespace simulator::fix
