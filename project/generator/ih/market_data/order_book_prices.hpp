#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_ORDER_BOOK_PRICES_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_ORDER_BOOK_PRICES_HPP_

#include <cstdint>

namespace simulator::generator::mdata {

enum class OrderBookPrices : std::uint8_t { Aggregated, Excluded };

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_ORDER_BOOK_PRICES_HPP_
