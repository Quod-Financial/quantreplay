#ifndef SIMULATOR_IDGEN_IH_GENERATION_HPP_
#define SIMULATOR_IDGEN_IH_GENERATION_HPP_

#include "idgen/errors.hpp"
#include "idgen/execution_id.hpp"
#include "idgen/instrument_id.hpp"
#include "idgen/market_entry_id.hpp"
#include "idgen/order_id.hpp"

namespace simulator::trading_system::idgen {

[[nodiscard]]
auto generate_identifier(const InstrumentIdContext::Implementation&
                             ctx) noexcept -> ExpectedId<InstrumentId>;

auto update_counter_sequence(InstrumentIdContext::Implementation& ctx) noexcept
    -> void;

[[nodiscard]]
auto generate_identifier(const OrderIdContext::Implementation& ctx) noexcept
    -> OrderId;

auto update_time_sequence(OrderIdContext::Implementation& ctx) noexcept -> void;

auto update_counter_sequence(OrderIdContext::Implementation& ctx) noexcept
    -> void;

[[nodiscard]]
auto generate_identifier(const ExecutionIdContext::Implementation& ctx) noexcept
    -> ExpectedId<ExecutionId>;

auto update_counter_sequence(ExecutionIdContext::Implementation& ctx) noexcept
    -> void;

[[nodiscard]]
auto generate_identifier(const MarketEntryIdContext::Implementation& ctx)
    -> MarketEntryId;

auto update_counter_sequence(MarketEntryIdContext::Implementation& ctx) -> void;

}  // namespace simulator::trading_system::idgen

#endif  // SIMULATOR_IDGEN_IH_GENERATION_HPP_