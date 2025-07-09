#include <gtest/gtest.h>

#include <functional>

#include "api/exceptions/exceptions.hpp"
#include "ih/common/database/driver.hpp"
#include "ih/common/database/ping_agent.hpp"
#include "ih/pqxx/context.hpp"

namespace simulator::data_layer::database::test {
namespace {

class DataLayer_Database_PingAgent : public ::testing::Test {
 public:
  template <typename Callable>
  static auto make_pqxx_agent(Callable callable)
      -> PingAgent::PqxxAgentStrategy {
    return [callable]([[maybe_unused]] const internal_pqxx::Context& context) {
      callable();
    };
  }

  static auto make_noop_pqxx_agent() -> PingAgent::PqxxAgentStrategy {
    return make_pqxx_agent([] {});
  }
};

TEST_F(DataLayer_Database_PingAgent, Ping_Pqxx_ConnectionErrorOccurs) {
  const auto context = Driver::make_database_context(internal_pqxx::Context{});
  auto pqxx_agent =
      make_pqxx_agent([] { throw ConnectionFailure{"test failure"}; });

  PingAgent agent{pqxx_agent};
  EXPECT_FALSE(agent.ping_with_context(context));
}

TEST_F(DataLayer_Database_PingAgent, Ping_Pqxx_GenericExceptionThrown) {
  const auto context = Driver::make_database_context(internal_pqxx::Context{});
  auto pqxx_agent =
      make_pqxx_agent([] { throw std::logic_error{"test failure"}; });
  PingAgent agent{pqxx_agent};

  EXPECT_FALSE(agent.ping_with_context(context));
}

TEST_F(DataLayer_Database_PingAgent, Ping_Pqxx_NonExecptionValueThrown) {
  const auto context = Driver::make_database_context(internal_pqxx::Context{});
  auto pqxx_agent = make_pqxx_agent([] { throw 42; });
  PingAgent agent{pqxx_agent};

  EXPECT_FALSE(agent.ping_with_context(context));
}

TEST_F(DataLayer_Database_PingAgent, Ping_Pqxx_NoExceptionThrown) {
  const auto context = Driver::make_database_context(internal_pqxx::Context{});
  auto pqxx_agent = make_noop_pqxx_agent();
  PingAgent agent{std::move(pqxx_agent)};

  EXPECT_TRUE(agent.ping_with_context(context));
}

}  // namespace
}  // namespace simulator::data_layer::database::test