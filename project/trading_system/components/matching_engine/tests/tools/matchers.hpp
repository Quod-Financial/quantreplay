#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_MATCHERS_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_MATCHERS_HPP_

#include <gmock/gmock.h>

#include <string>

#include "core/domain/attributes.hpp"
#include "ih/common/events/event.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"

namespace simulator::trading_system::matching_engine {

MATCHER_P(IsOrderBookNotification, matcher, "") {
  using namespace ::testing;  // NOLINT
  return ExplainMatchResult(Field(&Event::value,
                                  VariantWith<OrderBookNotification>(Field(
                                      &OrderBookNotification::value, matcher))),
                            arg,
                            result_listener);
}

MATCHER_P(IsClientNotification, matcher, "") {
  using namespace ::testing;  // NOLINT
  return ExplainMatchResult(Field(&Event::value,
                                  VariantWith<ClientNotification>(Field(
                                      &ClientNotification::value, matcher))),
                            arg,
                            result_listener);
}

MATCHER_P4(IsOrderCancellationConfirmation,
           venue_order_id,
           order_status,
           leaving_quantity,
           client_order_id,
           "") {
  using namespace ::testing;  // NOLINT
  return ExplainMatchResult(
      VariantWith<protocol::OrderCancellationConfirmation>(AllOf(
          Field(&protocol::OrderCancellationConfirmation::venue_order_id,
                Optional(Eq(venue_order_id))),
          Field(&protocol::OrderCancellationConfirmation::order_status,
                Optional(Eq(order_status))),
          Field(&protocol::OrderCancellationConfirmation::leaving_quantity,
                Optional(Eq(leaving_quantity))),
          Field(&protocol::OrderCancellationConfirmation::execution_id,
                Ne(std::nullopt)),
          Field(&protocol::OrderCancellationConfirmation::client_order_id,
                Eq(client_order_id)))),
      arg,
      result_listener);
}

MATCHER_P(IsOrderModificationReject, reject_text, "") {
  using namespace ::testing;  // NOLINT
  return ExplainMatchResult(
      IsClientNotification(VariantWith<protocol::OrderModificationReject>(
          Field(&protocol::OrderModificationReject::reject_text,
                Optional(Eq(RejectText{std::string{reject_text}}))))),
      arg,
      result_listener);
}

MATCHER_P2(IsOrderModificationRejectWithStatus, reject_text, order_status, "") {
  using namespace ::testing;  // NOLINT
  return ExplainMatchResult(
      IsClientNotification(VariantWith<protocol::OrderModificationReject>(
          AllOf(Field(&protocol::OrderModificationReject::reject_text,
                      Optional(Eq(RejectText{std::string{reject_text}}))),
                Field(&protocol::OrderModificationReject::order_status,
                      Optional(Eq(order_status)))))),
      arg,
      result_listener);
}

MATCHER(IsClientNotificationFlush, "") {
  using namespace ::testing;  // NOLINT
  return ExplainMatchResult(
      Field(&Event::value, VariantWith<ClientNotificationFlush>(_)),
      arg,
      result_listener);
}

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_MATCHERS_HPP_
