#include "fix/generator_initiator/lifetime.hpp"

#include <quickfix/Dictionary.h>
#include <quickfix/Session.h>
#include <quickfix/SessionID.h>
#include <quickfix/SessionSettings.h>

#include <algorithm>
#include <exception>
#include <iterator>
#include <memory>
#include <optional>
#include <set>
#include <utility>
#include <vector>

#include "common/session_settings.hpp"
#include "ih/implementation.hpp"
#include "log/logging.hpp"
#include "protocol/types/session.hpp"

namespace simulator::fix {
namespace {

[[nodiscard]]
auto load_settings(const std::filesystem::path& config_path)
    -> std::optional<FIX::SessionSettings> {
  try {
    return read_settings_from_file(config_path);
  } catch (const std::exception&) {
    return std::nullopt;
  }
}

[[nodiscard]]
auto initiator_sessions(const FIX::SessionSettings& settings)
    -> std::vector<FIX::SessionID> {
  const std::set<FIX::SessionID> sessions = settings.getSessions();
  std::vector<FIX::SessionID> initiators;
  std::ranges::copy_if(
      sessions,
      std::back_inserter(initiators),
      [&](const FIX::SessionID& session_id) {
        const FIX::Dictionary& session = settings.get(session_id);
        return session.has("ConnectionType") &&
               session.getString("ConnectionType") == "initiator";
      });
  return initiators;
}

[[nodiscard]]
auto existing_sessions(const std::vector<FIX::SessionID>& sessions)
    -> std::vector<FIX::SessionID> {
  std::vector<FIX::SessionID> existing;
  std::ranges::copy_if(sessions,
                       std::back_inserter(existing),
                       &FIX::Session::doesSessionExist);
  return existing;
}

[[nodiscard]]
auto create_implementation(const FIX::SessionSettings& settings)
    -> std::unique_ptr<GeneratorInitiator::Implementation> {
  try {
    return std::make_unique<GeneratorInitiator::Implementation>(settings);
  } catch (const std::exception&) {
    return nullptr;
  }
}

}  // namespace

auto create_generator_initiator(const std::filesystem::path& config_path)
    -> std::optional<GeneratorInitiator> {
  log::debug("creating a FIX initiator instance from `{}'",
             config_path.string());

  const auto configuration = load_settings(config_path);
  if (!configuration.has_value()) {
    log::err(
        "failed to load the fix configuration `{}', "
        "a FIX initiator instance is not created",
        config_path.string());
    return std::nullopt;
  }

  const auto sessions = initiator_sessions(*configuration);
  if (sessions.empty()) {
    log::info(
        "fix configuration `{}' defines no initiator session, "
        "a FIX initiator instance is not created",
        config_path.string());
    return std::nullopt;
  }

  // QuickFIX silently ignores a session whose id is already registered.
  const auto duplicates = existing_sessions(sessions);
  for (const FIX::SessionID& session_id : duplicates) {
    log::err(
        "fix configuration `{}' defines the '{}' session, which is already "
        "defined by another fix configuration, "
        "a FIX initiator instance is not created",
        config_path.string(),
        session_id.toString());
  }
  if (!duplicates.empty()) {
    return std::nullopt;
  }

  auto implementation = create_implementation(*configuration);
  if (implementation == nullptr) {
    log::err(
        "quickfix rejected the fix configuration `{}', "
        "a FIX initiator instance is not created",
        config_path.string());
    return std::nullopt;
  }

  log::info("created a FIX initiator instance from `{}'",
            config_path.string());

  return std::make_optional<GeneratorInitiator>(std::move(implementation));
}

auto generator_initiator_sessions(const GeneratorInitiator& initiator)
    -> std::vector<protocol::fix::Session> {
  return initiator.implementation().sessions();
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
