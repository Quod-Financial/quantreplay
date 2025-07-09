#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_DATABASE_CONTEXT_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_DATABASE_CONTEXT_HPP_

#include <memory>

namespace simulator::data_layer::database {

class ContextResolver;

class Context final {
  friend class Driver;
  friend class ContextResolver;
  class Implementation;

 public:
  Context(const Context&) = default;
  auto operator=(const Context&) -> Context& = default;

  Context(Context&&) = default;
  auto operator=(Context&&) -> Context& = default;

  ~Context() = default;

 private:
  Context() = default;

  auto accept(ContextResolver& resolver) const -> void;

  std::shared_ptr<Implementation> implementation_;
};

}  // namespace simulator::data_layer::database

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_DATABASE_CONTEXT_HPP_
