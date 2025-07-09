#ifndef SIMULATOR_RUNTIME_IH_MUX_IMPL_HPP_
#define SIMULATOR_RUNTIME_IH_MUX_IMPL_HPP_

#include <functional>
#include <gsl/pointers>

#include "runtime/mux.hpp"
#include "runtime/service.hpp"

namespace simulator::trading_system::runtime {

class Mux::Implementation {
 public:
  explicit Implementation(Service& executor) noexcept : executor_(&executor) {}

  Implementation() = delete;
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) noexcept = delete;
  virtual ~Implementation() noexcept = default;

  auto operator=(const Implementation&) -> Implementation& = delete;
  auto operator=(Implementation&&) noexcept -> Implementation& = delete;

  virtual auto post(std::function<void()> task) -> void = 0;

 protected:
  gsl::not_null<Service*> executor_;
};

}  // namespace simulator::trading_system::runtime

#endif  // SIMULATOR_RUNTIME_IH_MUX_IMPL_HPP_
