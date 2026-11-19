#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_GENERATOR_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_GENERATOR_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <string>
#include <utility>

namespace simulator::http {

class GeneratorController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  virtual ~GeneratorController() = default;

  [[nodiscard]]
  virtual auto start_generation(const std::string& body) const -> Result = 0;

  [[nodiscard]]
  virtual auto stop_generation() const -> Result = 0;
};

class GeneratorControllerImpl final : public GeneratorController {
 public:
  [[nodiscard]]
  auto start_generation(const std::string& body) const -> Result override;

  [[nodiscard]]
  auto stop_generation() const -> Result override;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_GENERATOR_CONTROLLER_HPP_
