#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_MUX_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_MUX_HPP_

#include <functional>
#include <memory>

#include "runtime/service.hpp"

namespace simulator::trading_system::runtime {

class Mux : public Service {
 public:
  class Implementation;

  [[nodiscard]]
  static auto create_chained_mux(Service& executor) -> Mux;

  explicit Mux(std::unique_ptr<Implementation> impl);

  Mux() = delete;
  Mux(const Mux&) = delete;
  Mux(Mux&&) noexcept;
  ~Mux() noexcept override;

  auto operator=(const Mux&) -> Mux& = delete;
  auto operator=(Mux&&) noexcept -> Mux&;

  auto execute(std::function<void()> task) -> void override;

 private:
  std::unique_ptr<Implementation> impl_;
};

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_RUNTIME_MUX_HPP_
