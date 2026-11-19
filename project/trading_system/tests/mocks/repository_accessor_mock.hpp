#ifndef SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_REPOSITORY_ACCESSOR_MOCK_HPP_
#define SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_REPOSITORY_ACCESSOR_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/repository/repository_accessor.hpp"

namespace simulator::trading_system::test {

struct RepositoryAccessorMock : public RepositoryAccessor {
  // clang-format off
  MOCK_METHOD(void, unicast_impl, (InstrumentId, tl::function_ref<void(TradingEngine&)>), (const, override));
  MOCK_METHOD(void, broadcast_impl, (tl::function_ref<void(TradingEngine&)>), (const, override));
  MOCK_METHOD(bool, unicast_order_modifying_impl, (InstrumentId, tl::function_ref<void(OrderModifyingEngine&)>), (const, override));
  MOCK_METHOD(bool, unicast_state_persisting_impl, (InstrumentId, tl::function_ref<void(StatePersistingEngine&)>), (const, override));
  MOCK_METHOD(bool, unicast_quote_publishing_impl, (InstrumentId, tl::function_ref<void(QuotePublishingEngine&)>), (const, override));
  // clang-format on
};

}  // namespace simulator::trading_system::test

#endif  // SIMULATOR_TRADING_SYSTEM_TESTS_MOCKS_REPOSITORY_ACCESSOR_MOCK_HPP_
