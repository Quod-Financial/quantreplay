#include "ih/common/database/ping_agent.hpp"

#include "api/exceptions/exceptions.hpp"
#include "ih/pqxx/context.hpp"
#include "ih/pqxx/database/connector.hpp"
#include "log/logging.hpp"

namespace simulator::data_layer::database {

auto PingAgent::ping(const database::Context& context) noexcept -> bool {
  const static auto pqxx_agent =
      [](const internal_pqxx::Context& pqxx_context) {
        internal_pqxx::Connector::ping(pqxx_context);
      };

  return PingAgent{pqxx_agent}.ping_with_context(context);
}

PingAgent::PingAgent(PqxxAgentStrategy pqxx_agent) noexcept
    : pqxx_agent_(std::move(pqxx_agent)) {}

auto PingAgent::ping_with_context(const database::Context& context) noexcept
    -> bool {
  try {
    // Resolve the context to call one of `execute_with` methods
    // with concrete DBMS context to enrich the correct strategy
    ContextResolver::resolve(context);
    log::debug("ping reached the database");
    return true;
  } catch (const data_layer::ConnectionFailure& exception) {
    log::warn("failed to ping the database - {}", exception.what());
  } catch (const std::exception& exception) {
    log::err("failed to ping the database, error occurred: {}",
                        exception.what());
  } catch (...) {
    log::err("failed to ping the database, unknown error occurred");
  }

  return false;
}

auto PingAgent::execute_with(const internal_pqxx::Context& pqxx_context)
    -> void {
  log::debug("pinging database with the pqxx (postgres) driver");
  pqxx_agent_(pqxx_context);
}

}  // namespace simulator::data_layer::database
