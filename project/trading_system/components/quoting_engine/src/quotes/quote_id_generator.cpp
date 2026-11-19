#include "quoting_engine/quote_id_generator.hpp"

#include <fmt/format.h>

#include <mutex>

#include "idgen/order_id.hpp"

namespace simulator::trading_system::quoting_engine {

auto QuoteIdGenerator::generate() -> QuoteId {
  const std::lock_guard lock{mutex_};
  return QuoteId{fmt::to_string(idgen::generate_new_id(context_))};
}

}  // namespace simulator::trading_system::quoting_engine
