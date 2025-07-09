#ifndef SIMULATOR_DATA_LAYER_IH_COMMON_DRIVER_HPP_
#define SIMULATOR_DATA_LAYER_IH_COMMON_DRIVER_HPP_

#include "api/database/context.hpp"
#include "cfg/api/cfg.hpp"
#include "ih/common/database/context_resolver.hpp"
#include "ih/pqxx/context.hpp"

namespace simulator::data_layer::database {

class Driver final {
 public:
  [[nodiscard]]
  static auto configure(const cfg::DbConfiguration& cfg)
      -> database::Context;

  [[nodiscard]]
  static auto make_database_context(internal_pqxx::Context pqxx)
      -> database::Context;

 private:
  Driver() = default;

  [[nodiscard]]
  static auto setup_pqxx_backend(const cfg::DbConfiguration& cfg)
      -> database::Context;
};

}  // namespace simulator::data_layer::database

#endif  // SIMULATOR_DATA_LAYER_IH_COMMON_DRIVER_HPP_
