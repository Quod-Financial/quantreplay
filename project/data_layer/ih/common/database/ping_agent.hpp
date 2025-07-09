#ifndef SIMULATOR_DATA_LAYER_IH_COMMON_DATABASE_PING_AGENT_HPP_
#define SIMULATOR_DATA_LAYER_IH_COMMON_DATABASE_PING_AGENT_HPP_

#include <functional>

#include "api/database/context.hpp"
#include "ih/common/database/context_resolver.hpp"
#include "ih/pqxx/database/connector.hpp"

namespace simulator::data_layer::database {

class PingAgent final : public ContextResolver {
 public:
  using PqxxAgentStrategy = std::function<void(const internal_pqxx::Context&)>;

  static auto ping(const database::Context& context) noexcept -> bool;

  PingAgent() = delete;

  explicit PingAgent(PqxxAgentStrategy pqxx_agent) noexcept;

  auto ping_with_context(const database::Context& context) noexcept -> bool;

 private:
  auto execute_with(const internal_pqxx::Context& pqxx_context)
      -> void override;

  PqxxAgentStrategy pqxx_agent_;
};

}  // namespace simulator::data_layer::database

#endif  // SIMULATOR_DATA_LAYER_IH_COMMON_DATABASE_PING_AGENT_HPP_
