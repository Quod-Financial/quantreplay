#ifndef SIMULATOR_MIDDLEWARE_TESTS_MOCKS_TRADING_PHASE_ADMIN_RECEIVER_MOCK_HPP_
#define SIMULATOR_MIDDLEWARE_TESTS_MOCKS_TRADING_PHASE_ADMIN_RECEIVER_MOCK_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/trading_admin_channel.hpp"

namespace simulator::middleware::test {

struct TradingAdminRequestReceiverMock : public TradingAdminRequestReceiver {
  MOCK_METHOD(void,
              process,
              (const protocol::HaltPhaseRequest& request,
               protocol::HaltPhaseReply& reply),
              (override));

  MOCK_METHOD(void,
              process,
              (const protocol::ResumePhaseRequest& request,
               protocol::ResumePhaseReply& reply),
              (override));

  MOCK_METHOD(void,
              process,
              (const protocol::StoreMarketStateRequest& request,
               protocol::StoreMarketStateReply& reply),
              (override));

  MOCK_METHOD(void,
              process,
              (const protocol::RecoverMarketStateRequest& request,
               protocol::RecoverMarketStateReply& reply),
              (override));
};

}  // namespace simulator::middleware::test

#endif  // SIMULATOR_MIDDLEWARE_TESTS_MOCKS_TRADING_PHASE_ADMIN_RECEIVER_MOCK_HPP_
