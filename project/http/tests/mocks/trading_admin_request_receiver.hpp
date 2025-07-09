#ifndef SIMULATOR_HTTP_TESTS_MOCKS_TRADING_ADMIN_REQUEST_RECEIVER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_TRADING_ADMIN_REQUEST_RECEIVER_HPP_

#include <gmock/gmock.h>

#include "middleware/channels/trading_admin_channel.hpp"

namespace simulator::http::mock {

struct TradingAdminRequestReceiver
    : public middleware::TradingAdminRequestReceiver {
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

}  // namespace simulator::http::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_TRADING_ADMIN_REQUEST_RECEIVER_HPP_
