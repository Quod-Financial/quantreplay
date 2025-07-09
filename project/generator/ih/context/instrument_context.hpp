#ifndef SIMULATOR_GENERATOR_IH_CONTEXT_INSTRUMENT_CONTEXT_HPP_
#define SIMULATOR_GENERATOR_IH_CONTEXT_INSTRUMENT_CONTEXT_HPP_

#include <string>

#include "core/domain/instrument_descriptor.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/venue.hpp"
#include "ih/registry/generated_orders_registry.hpp"

namespace simulator::generator {

class InstrumentContext {
 public:
  virtual ~InstrumentContext() = default;

  [[nodiscard]]
  virtual auto get_synthetic_identifier() noexcept -> std::string = 0;

  [[nodiscard]]
  virtual auto get_venue() const noexcept -> const data_layer::Venue& = 0;

  [[nodiscard]]
  virtual auto get_instrument() const noexcept
      -> const data_layer::Listing& = 0;

  virtual auto get_instrument_descriptor() const noexcept
      -> const InstrumentDescriptor& = 0;
};

class OrderInstrumentContext : public InstrumentContext {
 public:
  [[nodiscard]]
  virtual auto next_message_number() noexcept -> std::size_t = 0;

  [[nodiscard]]
  virtual auto take_registry() noexcept -> GeneratedOrdersRegistry& = 0;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_CONTEXT_INSTRUMENT_CONTEXT_HPP_
