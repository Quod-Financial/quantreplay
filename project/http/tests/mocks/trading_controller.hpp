#ifndef SIMULATOR_HTTP_TESTS_MOCKS_TRADING_CONTROLLER_HPP_
#define SIMULATOR_HTTP_TESTS_MOCKS_TRADING_CONTROLLER_HPP_

#include <gmock/gmock.h>

#include "ih/controllers/trading_controller.hpp"

namespace simulator::http::test::mock {

class TradingController : public http::TradingController {
 public:
  MOCK_METHOD(Result, halt, (const std::string& body), (const, override));

  MOCK_METHOD(Result, resume, (), (const, override));

  MOCK_METHOD(Result, store_market_state, (), (const, override));

  MOCK_METHOD(Result, recover_market_state, (), (const, override));
};

}  // namespace simulator::http::test::mock

#endif  // SIMULATOR_HTTP_TESTS_MOCKS_TRADING_CONTROLLER_HPP_
