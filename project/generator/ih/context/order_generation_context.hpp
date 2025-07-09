#ifndef SIMULATOR_GENERATOR_IH_CONTEXT_ORDER_GENERATION_CONTEXT_HPP_
#define SIMULATOR_GENERATOR_IH_CONTEXT_ORDER_GENERATION_CONTEXT_HPP_

#include "data_layer/api/models/price_seed.hpp"
#include "ih/context/instrument_context.hpp"
#include "ih/context/order_market_data_provider.hpp"

namespace simulator::generator {

class OrderGenerationContext : public OrderInstrumentContext {
 public:
  [[nodiscard]]
  virtual auto get_price_seed() const noexcept
      -> const data_layer::PriceSeed& = 0;

  [[nodiscard]]
  virtual auto get_current_market_state() const -> MarketState = 0;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_CONTEXT_ORDER_GENERATION_CONTEXT_HPP_
