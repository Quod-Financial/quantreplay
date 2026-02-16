#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MARKET_ENTRY_ID_GENERATOR_MOCK_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MARKET_ENTRY_ID_GENERATOR_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/market_data/tools/market_entry_id_generator.hpp"

namespace simulator::trading_system::matching_engine {

class MarketEntryIdGeneratorMock : public mdata::MarketEntryIdGenerator {
 public:
  MOCK_METHOD(MarketEntryId, generate, ());

 private:
  auto operator()() -> MarketEntryId override { return generate(); }
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MARKET_ENTRY_ID_GENERATOR_MOCK_HPP_
