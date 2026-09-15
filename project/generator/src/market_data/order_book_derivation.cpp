#include "ih/market_data/order_book_derivation.hpp"

#include <cassert>
#include <mutex>
#include <optional>
#include <utility>

#include "ih/market_data/record_factory.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "log/logging.hpp"

namespace simulator::generator::mdata {

OrderBookDerivation::OrderBookDerivation(
    std::shared_ptr<historical::Processor> processor,
    std::shared_ptr<ComponentContext> context)
    : processor_{std::move(processor)}, context_{std::move(context)} {
  assert(processor_);
  assert(context_);
}

auto OrderBookDerivation::add(const MdRequestId& request_id,
                              OrderDerivationRequirement requirement) -> void {
  const std::lock_guard lock{mutex_};

  const auto binding = bindings_.find(request_id);
  if (binding == bindings_.end()) {
    bindings_.emplace(request_id,
                      Binding{.requirement = std::move(requirement)});
    return;
  }

  OrderDerivationRequirement& bound = binding->second.requirement;
  bound.depth = merge_depth(bound.depth, requirement.depth);
  log::debug(
      "merged an order derivation requirement on `{}' into the one of the "
      "`{}' market data subscription with the {} depth, keeping the `{}' "
      "counterparty",
      bound.symbol,
      request_id,
      bound.depth,
      bound.counterparty);
}

auto OrderBookDerivation::process(const protocol::MarketDataSnapshot& snapshot)
    -> void {
  auto record = convert(snapshot);
  if (!record.has_value()) {
    return;
  }

  // Applied outside the lock, so no downstream work of the derivation runs
  // under it.
  processor_->process(*std::move(record));
}

auto OrderBookDerivation::convert(const protocol::MarketDataSnapshot& snapshot)
    -> std::optional<historical::Record> {
  if (!snapshot.request_id.has_value()) {
    log::debug(
        "ignoring a market data snapshot without a request identifier as an "
        "order book source");
    return std::nullopt;
  }

  const std::lock_guard lock{mutex_};

  const auto binding = bindings_.find(*snapshot.request_id);
  if (binding == bindings_.end()) {
    log::debug(
        "no order derivation is configured for the `{}' market data "
        "subscription, ignoring its snapshot as an order book source",
        *snapshot.request_id);
    return std::nullopt;
  }

  if (!context_->is_component_running()) {
    log::debug(
        "order generation is stopped, ignoring a snapshot of the `{}' market "
        "data subscription",
        *snapshot.request_id);
    return std::nullopt;
  }

  Binding& bound = binding->second;
  return std::make_optional(make_order_book_record(
      snapshot, bound.requirement, ++bound.converted_records));
}

}  // namespace simulator::generator::mdata
