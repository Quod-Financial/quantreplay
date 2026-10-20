#ifndef SIMULATOR_GENERATOR_TESTS_MOCKS_MARKET_DATA_BEST_PRICES_SINK_HPP_
#define SIMULATOR_GENERATOR_TESTS_MOCKS_MARKET_DATA_BEST_PRICES_SINK_HPP_

#include <gmock/gmock.h>

#include "core/domain/attributes.hpp"
#include "ih/market_data/best_prices_sink.hpp"

namespace simulator::generator::mock {

class BestPricesSink : public mdata::BestPricesSink {
 public:
  MOCK_METHOD(void,
              process,
              (const MdRequestId& request_id, const mdata::BestPrices& prices),
              (override));
};

}  // namespace simulator::generator::mock

#endif  // SIMULATOR_GENERATOR_TESTS_MOCKS_MARKET_DATA_BEST_PRICES_SINK_HPP_
