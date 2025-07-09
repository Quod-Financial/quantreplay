#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_EOD_ELIMINATION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_EOD_ELIMINATION_HPP_

#include <gsl/pointers>

#include "common/events.hpp"
#include "core/domain/attributes.hpp"
#include "core/tools/time.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"

namespace simulator::trading_system::matching_engine::order {

class SystemElimination : EventReporter {
 public:
  SystemElimination(EventListener& event_listener, event::Tick system_tick);

  auto operator()(OrderBook& book) const -> void;

 private:
  auto eliminate_expired(LimitOrdersContainer& orders) const -> void;

  auto is_expired(const LimitOrder& order) const -> bool;

  auto eliminate(LimitOrder& order) const -> void;

  core::sys_us current_expire_time_;
  core::local_days current_expire_date_;
  bool is_new_day_;
};

class AllOrdersElimination : EventReporter {
 public:
  explicit AllOrdersElimination(EventListener& event_listener);

  auto operator()(OrderBook& book) const -> void;

 private:
  auto eliminate(LimitOrdersContainer& orders) const -> void;

  auto eliminate(LimitOrder& order) const -> void;
};

class ClosedPhaseElimination : EventReporter {
 public:
  ClosedPhaseElimination(EventListener& event_listener,
                         core::tz_us phase_tz_start_time);

  auto operator()(OrderBook& book) const -> void;

 private:
  auto eliminate_expired(LimitOrdersContainer& orders) const -> void;

  auto is_expired(const LimitOrder& order) const -> bool;

  auto eliminate(LimitOrder& order) const -> void;

  core::local_days phase_start_date_;
};

class OnDisconnectElimination : EventReporter {
 public:
  OnDisconnectElimination(EventListener& event_listener,
                          const protocol::Session& disconnected_session);

  auto operator()(OrderBook& book) const -> void;

 private:
  auto handle_eliminated_orders(LimitOrdersContainer& orders) const -> void;

  auto should_be_eliminated(const LimitOrder& order) const -> bool;

  auto eliminate(LimitOrder& order) const -> void;

  gsl::not_null<const protocol::Session*> disconnected_session_;
};

}  // namespace simulator::trading_system::matching_engine::order

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_EOD_ELIMINATION_HPP_
