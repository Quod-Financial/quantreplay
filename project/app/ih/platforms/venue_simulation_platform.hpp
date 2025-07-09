#ifndef SIMULATOR_APP_IH_PLATFORMS_VENUE_SIMULATION_PLATFORM_HPP_
#define SIMULATOR_APP_IH_PLATFORMS_VENUE_SIMULATION_PLATFORM_HPP_

#include <memory>

#include "data_layer/api/database/context.hpp"
#include "ih/components/fix_acceptor.hpp"
#include "ih/components/generator.hpp"
#include "ih/components/http_server.hpp"
#include "ih/components/trading_engine.hpp"
#include "ih/platforms/platform.hpp"

namespace simulator {

class VenueSimulationPlatform final : public Platform {
 public:
  explicit VenueSimulationPlatform(
      const data_layer::database::Context& database);

  auto launch() -> void override;

  auto terminate() -> void override;

 private:
  std::shared_ptr<TradingEngine> trading_engine_;
  std::shared_ptr<FixAcceptor> fix_acceptor_;
  std::shared_ptr<Generator> generator_;
  std::shared_ptr<HttpServer> http_server_;
};

}  // namespace simulator

#endif  // SIMULATOR_APP_IH_PLATFORMS_VENUE_SIMULATION_PLATFORM_HPP_