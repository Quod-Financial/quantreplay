#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_QUOTING_ENGINE_QUOTE_ID_GENERATOR_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_QUOTING_ENGINE_QUOTE_ID_GENERATOR_HPP_

#include <mutex>

#include "core/domain/attributes.hpp"
#include "idgen/order_id.hpp"

namespace simulator::trading_system::quoting_engine {

class QuoteIdGenerator final {
 public:
  QuoteIdGenerator() = default;
  QuoteIdGenerator(const QuoteIdGenerator&) = delete;
  QuoteIdGenerator(QuoteIdGenerator&&) = delete;
  ~QuoteIdGenerator() = default;

  auto operator=(const QuoteIdGenerator&) -> QuoteIdGenerator& = delete;
  auto operator=(QuoteIdGenerator&&) -> QuoteIdGenerator& = delete;

  [[nodiscard]]
  auto generate() -> QuoteId;

 private:
  std::mutex mutex_;
  idgen::OrderIdContext context_{idgen::make_order_id_generation_ctx()};
};

}  // namespace simulator::trading_system::quoting_engine

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_QUOTING_ENGINE_QUOTE_ID_GENERATOR_HPP_
