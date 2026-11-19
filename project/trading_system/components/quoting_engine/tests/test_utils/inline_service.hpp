#ifndef SIMULATOR_QUOTING_ENGINE_TESTS_TEST_UTILS_INLINE_SERVICE_HPP_
#define SIMULATOR_QUOTING_ENGINE_TESTS_TEST_UTILS_INLINE_SERVICE_HPP_

#include <functional>

#include "runtime/service.hpp"

namespace simulator::trading_system::quoting_engine::test {

struct InlineService : public runtime::Service {
  auto execute(std::function<void()> task) -> void override { task(); }
};

}  // namespace simulator::trading_system::quoting_engine::test

#endif  // SIMULATOR_QUOTING_ENGINE_TESTS_TEST_UTILS_INLINE_SERVICE_HPP_
