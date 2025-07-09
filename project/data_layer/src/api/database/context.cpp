#include "api/database/context.hpp"

#include <cassert>
#include <variant>

#include "ih/common/database/context_resolver.hpp"
#include "ih/common/database/driver.hpp"
#include "ih/pqxx/context.hpp"

namespace simulator::data_layer::database {

class Context::Implementation {
  using GenericDmbsContext = std::variant<internal_pqxx::Context>;

 public:
  Implementation() = delete;

  explicit Implementation(internal_pqxx::Context pqxx_context) noexcept;

  static auto create(internal_pqxx::Context pqxx) -> database::Context;

  auto accept(database::ContextResolver& resolver) -> void;

 private:
  GenericDmbsContext dbms_context_;
};

Context::Implementation::Implementation(
    internal_pqxx::Context pqxx_context) noexcept
    : dbms_context_(std::move(pqxx_context)) {}

auto Context::Implementation::create(internal_pqxx::Context pqxx_context)
    -> database::Context {
  database::Context dbms_context{};
  auto impl = std::make_shared<Implementation>(std::move(pqxx_context));
  dbms_context.implementation_ = std::move(impl);
  return dbms_context;
}

auto Context::Implementation::accept(database::ContextResolver& resolver)
    -> void {
  std::visit(
      [&resolver](const auto& concrete_context) {
        resolver.execute_with(concrete_context);
      },
      dbms_context_);
}

}  // namespace simulator::data_layer::database

namespace simulator::data_layer::database {

void Context::accept(database::ContextResolver& resolver) const {
  assert(implementation_);
  implementation_->accept(resolver);
}

}  // namespace simulator::data_layer::database

namespace simulator::data_layer::database {

auto Driver::configure(const cfg::DbConfiguration& cfg) -> database::Context {
  // For the time being, only PostgreSQL is supported by the project
  return Driver::setup_pqxx_backend(cfg);
}

auto Driver::make_database_context(internal_pqxx::Context pqxx)
    -> database::Context {
  return Context::Implementation::create(std::move(pqxx));
}

auto Driver::setup_pqxx_backend(const cfg::DbConfiguration& cfg)
    -> database::Context {
  internal_pqxx::Context pqxx_context{};
  pqxx_context.configure(cfg);
  return Driver::make_database_context(std::move(pqxx_context));
}

}  // namespace simulator::data_layer::database