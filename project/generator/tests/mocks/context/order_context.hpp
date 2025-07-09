#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_CONTEXT_ORDER_CONTEXT_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_CONTEXT_ORDER_CONTEXT_HPP_

#include <string>

#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/price_seed.hpp"
#include "data_layer/api/models/venue.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/context/order_generation_context.hpp"
#include "mocks/registry/generated_orders_registry.hpp"

namespace simulator::generator::mock {

class OrderInstrumentContext : public generator::OrderInstrumentContext {
 public:
  MOCK_METHOD(std::string, get_synthetic_identifier, (), (noexcept, override));

  MOCK_METHOD(const simulator::data_layer::Venue&,
              get_venue,
              (),
              (const, noexcept, override));

  MOCK_METHOD(const simulator::data_layer::Listing&,
              get_instrument,
              (),
              (const, noexcept, override));

  MOCK_METHOD(const simulator::InstrumentDescriptor&,
              get_instrument_descriptor,
              (),
              (const, noexcept, override));

  MOCK_METHOD(std::size_t, next_message_number, (), (noexcept, override));

  MOCK_METHOD(generator::GeneratedOrdersRegistry&,
              take_registry,
              (),
              (noexcept, override));
};

class OrderGenerationContext : public generator::OrderGenerationContext {
 public:
  MOCK_METHOD(std::string, get_synthetic_identifier, (), (noexcept, override));

  MOCK_METHOD(const simulator::data_layer::Venue&,
              get_venue,
              (),
              (const, noexcept, override));

  MOCK_METHOD(const simulator::data_layer::Listing&,
              get_instrument,
              (),
              (const, noexcept, override));

  MOCK_METHOD(const simulator::InstrumentDescriptor&,
              get_instrument_descriptor,
              (),
              (const, noexcept, override));

  MOCK_METHOD(std::size_t, next_message_number, (), (noexcept, override));

  MOCK_METHOD(generator::GeneratedOrdersRegistry&,
              take_registry,
              (),
              (noexcept, override));

  MOCK_METHOD(const simulator::data_layer::PriceSeed&,
              get_price_seed,
              (),
              (const, noexcept, override));

  MOCK_METHOD(MarketState, get_current_market_state, (), (const, override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_CONTEXT_ORDER_CONTEXT_HPP_
