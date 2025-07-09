#include <memory>
#include <utility>

#include "idgen/instrument_id.hpp"
#include "ih/contexts/execution_id_context.hpp"
#include "ih/contexts/instrument_id_context.hpp"
#include "ih/contexts/market_entry_id_context.hpp"
#include "ih/contexts/order_id_context.hpp"
#include "ih/generation.hpp"
#include "ih/sequences/chrono_sequence.hpp"
#include "ih/sequences/numeric_sequence.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::idgen {
namespace {

template <typename SequenceType>
auto make_time_sequence() -> SequenceType {
  if (auto sequence = make_sequence<SequenceType>()) [[likely]] {
    return *sequence;
  }
  throw std::runtime_error("failed to create time sequence");
};

}  // namespace

auto make_instrument_id_generation_ctx() -> InstrumentIdContext {
  using implementation_type = InstrumentIdContext::Implementation;
  using sequence_type = typename implementation_type::SequenceType;

  const auto sequence = make_sequence<sequence_type>();
  auto implementation = [&]() -> std::unique_ptr<implementation_type> {
    try {
      return std::make_unique<implementation_type>(sequence);
    } catch (std::bad_alloc const&) {
      log::warn(
          "failed to allocate memory for a listing id generation context... "
          "this may indicate a problem with memory consumption or "
          "possible undefined behaviour in the program");
    }
    throw std::runtime_error(
        "failed to create instrument id generation context");
  }();

  log::debug("created new listing id generation context");
  return InstrumentIdContext{std::move(implementation)};
}

auto reset_generation_ctx(InstrumentIdContext& ctx) noexcept -> void {
  using implementation_type = typename InstrumentIdContext::Implementation;
  using sequence_type = typename implementation_type::SequenceType;

  implementation_type& context = ctx.implementation();
  context.set_sequence(make_sequence<sequence_type>());

  log::debug("listing id generation context state was reset");
}

auto generate_new_id(InstrumentIdContext& ctx) noexcept
    -> ExpectedId<InstrumentId> {
  InstrumentIdContext::Implementation& context = ctx.implementation();
  const auto new_identifier = generate_identifier(context);
  update_counter_sequence(context);
  return new_identifier;
}

auto make_market_entry_id_generation_ctx() -> MarketEntryIdContext {
  using implementation_type = MarketEntryIdContext::Implementation;
  using sequence_type = implementation_type::CounterSequence;

  const auto sequence = make_sequence<sequence_type>();
  auto implementation = [&]() -> std::unique_ptr<implementation_type> {
    try {
      return std::make_unique<implementation_type>(
          implementation_type::generate_seed(), sequence);
    } catch (std::bad_alloc const&) {
      log::warn(
          "failed to allocate memory for a market entry id generation "
          "context... this may indicate a problem with memory consumption or "
          "possible undefined behaviour in the program");
    }
    throw std::runtime_error(
        "failed to create market entry id generation context");
  }();

  log::debug("created new market entry id generation context");
  return MarketEntryIdContext{std::move(implementation)};
}

auto generate_new_id(MarketEntryIdContext& ctx) -> MarketEntryId {
  MarketEntryIdContext::Implementation& context = ctx.implementation();
  const auto new_identifier = generate_identifier(context);
  update_counter_sequence(context);
  return new_identifier;
}

auto make_order_id_generation_ctx() -> OrderIdContext {
  using implementation_type = OrderIdContext::Implementation;
  using time_sequence_type = implementation_type::TimeSequence;
  using counter_sequence_type = implementation_type::CounterSequence;

  auto implementation = []() -> std::unique_ptr<implementation_type> {
    try {
      return std::make_unique<implementation_type>(
          make_time_sequence<time_sequence_type>(),
          make_sequence<counter_sequence_type>());
    } catch (const std::bad_alloc&) {
      log::warn(
          "failed to allocate memory for an order id generation "
          "context... this may indicate a problem with memory consumption or "
          "possible undefined behaviour in the program");
    } catch (const std::exception& exception) {
      log::err("failed to create order id generation context: {}",
               exception.what());
    } catch (...) {
      log::err(
          "failed to create order id generation context: "
          "unknown error occurred");
    }
    throw std::runtime_error("failed to create order id generation context");
  }();

  log::debug("[idgen]: created new order id generation context");
  return OrderIdContext{std::move(implementation)};
}

auto generate_new_id(OrderIdContext& ctx) noexcept -> OrderId {
  OrderIdContext::Implementation& context = ctx.implementation();

  // Update time sequence before generation to provide a correct time component.
  update_time_sequence(context);
  const OrderId identifier = generate_identifier(context);
  update_counter_sequence(context);

  return identifier;
}

auto make_execution_id_generation_ctx(OrderId exec_order_id)
    -> ExecutionIdContext {
  using implementation_type = ExecutionIdContext::Implementation;
  using counter_sequence_type = implementation_type::CounterSequence;

  auto implementation = [&]() -> std::unique_ptr<implementation_type> {
    try {
      return std::make_unique<implementation_type>(
          exec_order_id, make_sequence<counter_sequence_type>());
    } catch (const std::bad_alloc&) {
      log::warn(
          "failed to allocate memory for an execution id generation "
          "context... this may indicate a problem with memory consumption or "
          "possible undefined behaviour in the program");
    } catch (const std::exception& exception) {
      log::err(
          "failed to create execution id generation context for '{}' "
          "order identifier: {}",
          exec_order_id,
          exception.what());
    } catch (...) {
      log::err(
          "failed to create execution id generation context: for '{}' "
          "order identifier: unknown error occurred",
          exec_order_id);
    }
    throw std::runtime_error(
        "failed to create execution id generation context");
  }();

  log::debug(
      "created new execution id generation context for '{}' order identifier",
      exec_order_id);
  return ExecutionIdContext{std::move(implementation)};
}

auto generate_new_id(ExecutionIdContext& ctx) noexcept
    -> ExpectedId<ExecutionId> {
  ExecutionIdContext::Implementation& context = ctx.implementation();
  auto new_identifier = generate_identifier(context);
  update_counter_sequence(context);
  return new_identifier;
}

}  // namespace simulator::trading_system::idgen