#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MOCK_EXECUTION_REPORTS_LISTENER_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MOCK_EXECUTION_REPORTS_LISTENER_HPP_

#include <vector>

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event.hpp"
#include "protocol/app/execution_report.hpp"

namespace simulator::trading_system::matching_engine {

struct MockExecutionReportsListener : public EventListener {
  std::vector<protocol::ExecutionReport> reports;

 private:
  auto on(Event event) -> void override;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MOCK_EXECUTION_REPORTS_LISTENER_HPP_
