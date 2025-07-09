#ifndef SIMULATOR_PROJECT_DATA_LAYER_IH_COMMON_CONTEXT_RESOLVER_HPP_
#define SIMULATOR_PROJECT_DATA_LAYER_IH_COMMON_CONTEXT_RESOLVER_HPP_

#include "api/database/context.hpp"
#include "ih/pqxx/context.hpp"

namespace simulator::data_layer::database {

class ContextResolver {
  friend class database::Context::Implementation;

 public:
  ContextResolver() = default;

  ContextResolver(const ContextResolver&) = default;
  auto operator=(const ContextResolver&) -> ContextResolver& = default;

  ContextResolver(ContextResolver&&) = default;
  auto operator=(ContextResolver&&) -> ContextResolver& = default;

  virtual ~ContextResolver() = default;

 protected:
  auto resolve(const database::Context& dbms_context) -> void {
    dbms_context.accept(*this);
  }

 private:
  virtual void execute_with(const internal_pqxx::Context& pqxx_context) = 0;
};

}  // namespace simulator::data_layer::database

#endif  // SIMULATOR_PROJECT_DATA_LAYER_IH_COMMON_CONTEXT_RESOLVER_HPP_
