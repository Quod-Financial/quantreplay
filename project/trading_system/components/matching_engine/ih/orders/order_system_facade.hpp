#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ORDER_SYSTEM_FACADE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ORDER_SYSTEM_FACADE_HPP_

#include <gsl/pointers>
#include <memory>
#include <string_view>

#include "common/instrument.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/abstractions/order_event_handler.hpp"
#include "ih/common/abstractions/order_request_processor.hpp"
#include "ih/orders/actions/order_action_handler.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/phase_handler.hpp"
#include "ih/orders/replies/reject_notifier.hpp"
#include "ih/orders/tools/order_id_generator.hpp"
#include "ih/orders/validation/order_book_side.hpp"
#include "ih/orders/validation/validator.hpp"
#include "instruments/matcher.hpp"
#include "matching_engine/configuration.hpp"
#include "protocol/app/security_status_request.hpp"

namespace simulator::trading_system::matching_engine {

class OrderSystemFacade : public OrderRequestProcessor,
                          public OrderEventHandler {
 public:
  OrderSystemFacade() = delete;
  OrderSystemFacade(const OrderSystemFacade&) = delete;
  OrderSystemFacade(OrderSystemFacade&&) = delete;
  ~OrderSystemFacade() override = default;

  auto operator=(const OrderSystemFacade&) -> OrderSystemFacade& = delete;
  auto operator=(OrderSystemFacade&&) -> OrderSystemFacade& = delete;

  auto process(const protocol::OrderPlacementRequest& request) -> void override;

  auto process(const protocol::OrderModificationRequest& request)
      -> void override;

  auto process(const protocol::OrderCancellationRequest& request)
      -> void override;

  auto process(const protocol::SecurityStatusRequest& request) -> void override;

  auto store_state(market_state::OrderBook& state) -> void override;

  auto recover_state(market_state::OrderBook state) -> void override;

  auto handle(const event::Tick& tick) -> void override;

  auto handle(const event::PhaseTransition& phase_transition) -> void override;

  auto handle_disconnection(const protocol::Session& session) -> void override;

  static auto setup(const Instrument& instrument,
                    const Configuration& configuration,
                    EventListener& listener) -> OrderSystemFacade;

 private:
  template <typename RequestType>
  auto validate(const RequestType& request) -> bool;

  auto validate(const market_state::LimitOrder& order,
                order::OrderBookSide order_book_side)
      -> std::optional<std::string_view>;

  auto recover_page(std::vector<market_state::LimitOrder> orders_state,
                    order::OrderBookSide side) -> void;

  template <typename RequestType>
  auto reject_on_halt(const RequestType& request) -> bool;

  OrderSystemFacade(
      EventListener& event_listener,
      const Instrument& instrument,
      Configuration configuration,
      std::unique_ptr<order::OrderIdGenerator> order_id_generator,
      std::unique_ptr<order::Validator> validator,
      std::unique_ptr<order::RejectNotifier> reject_notifier,
      std::unique_ptr<OrderBook> depr_order_book,
      std::unique_ptr<OrderActionHandler> depr_order_action_handler);

  Configuration configuration_;
  order::PhaseHandler phase_handler_;
  Phase::Settings halt_not_closed_phase_setting_;
  instrument::Matcher instrument_matcher_;

  std::unique_ptr<order::OrderIdGenerator> order_id_generator_;
  std::unique_ptr<order::Validator> validator_;
  std::unique_ptr<order::RejectNotifier> reject_notifier_;

  std::unique_ptr<OrderBook> depr_order_book_;
  std::unique_ptr<OrderActionHandler> depr_order_action_handler_;
  gsl::not_null<EventListener*> event_listener_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ORDER_SYSTEM_FACADE_HPP_
