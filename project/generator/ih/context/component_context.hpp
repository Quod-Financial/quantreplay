#ifndef SIMULATOR_GENERATOR_IH_CONTEXT_COMPONENT_CONTEXT_HPP_
#define SIMULATOR_GENERATOR_IH_CONTEXT_COMPONENT_CONTEXT_HPP_

#include <cstddef>
#include <functional>

#include "data_layer/api/models/venue.hpp"

namespace simulator::generator {

class ComponentContext {
 public:
  using OnStartupCallback = std::function<void()>;

  virtual ~ComponentContext() = default;

  [[nodiscard]]
  virtual auto is_component_running() const noexcept -> bool = 0;

  [[nodiscard]]
  virtual auto get_venue() const noexcept -> const data_layer::Venue& = 0;

  virtual auto generate_identifier() noexcept -> std::string = 0;

  virtual auto next_generated_order_message_number() noexcept
      -> std::size_t = 0;

  virtual auto call_on_launch(const OnStartupCallback& callback) -> void = 0;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_CONTEXT_COMPONENT_CONTEXT_HPP_
