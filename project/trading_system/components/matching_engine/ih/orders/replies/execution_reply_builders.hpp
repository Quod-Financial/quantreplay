#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_EXECUTION_REPLY_BUILDERS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_EXECUTION_REPLY_BUILDERS_HPP_

#include "common/attributes.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

class ExecutionReportBuilder {
 public:
  explicit ExecutionReportBuilder(protocol::Session session);

  [[nodiscard]]
  auto build() const -> protocol::ExecutionReport;

  auto for_order(const LimitOrder& order) -> ExecutionReportBuilder&;

  auto for_order(const MarketOrder& order) -> ExecutionReportBuilder&;

  auto with_execution_id(ExecutionId identifier) -> ExecutionReportBuilder&;

  auto with_execution_price(ExecutionPrice price) -> ExecutionReportBuilder&;

  auto with_executed_quantity(ExecutedQuantity quantity)
      -> ExecutionReportBuilder&;

  auto with_counterparty(std::optional<Party> counterparty)
      -> ExecutionReportBuilder&;

 private:
  protocol::ExecutionReport message_;
};

[[nodiscard]]
auto prepare_execution_report(const LimitOrder& order)
    -> ExecutionReportBuilder;

[[nodiscard]]
auto prepare_execution_report(const MarketOrder& order)
    -> ExecutionReportBuilder;

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_REPLIES_EXECUTION_REPLY_BUILDERS_HPP_