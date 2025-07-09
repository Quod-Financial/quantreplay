#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_SERIALIZER_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_SERIALIZER_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/state_persistence/serializer.hpp"

namespace simulator::trading_system::test {

struct SerializerMock : public simulator::trading_system::Serializer {
  MOCK_METHOD(
      bool,
      serialize,
      (const simulator::trading_system::market_state::Snapshot& snapshot,
       std::ostream& os),
      (const, override));
  MOCK_METHOD((tl::expected<simulator::trading_system::market_state::Snapshot,
                            std::string>),
              deserialize,
              (std::istream & is),
              (const, override));
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_SERIALIZER_MOCK_HPP_
