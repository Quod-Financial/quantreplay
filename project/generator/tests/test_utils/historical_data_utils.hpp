#ifndef SIMULATOR_GENERATOR_TESTS_TEST_UTILS_HISTORICAL_DATA_UTILS_HPP_
#define SIMULATOR_GENERATOR_TESTS_TEST_UTILS_HISTORICAL_DATA_UTILS_HPP_

#include <gmock/gmock.h>

#include <chrono>
#include <cstdint>
#include <optional>

#include "ih/historical/data/record.hpp"
#include "ih/historical/data/time.hpp"

namespace simulator::generator::historical {

MATCHER_P6(LevelEq,
           bid_party,
           bid_quantity,
           bid_price,
           offer_price,
           offer_quantity,
           offer_party,
           "") {
  using namespace ::testing;

  return ExplainMatchResult(Optional(Eq(bid_party)),
                            arg.bid_counterparty(),
                            result_listener) &&
         ExplainMatchResult(Optional(DoubleEq(bid_quantity)),
                            arg.bid_quantity(),
                            result_listener) &&
         ExplainMatchResult(
             Optional(DoubleEq(bid_price)), arg.bid_price(), result_listener) &&
         ExplainMatchResult(Optional(DoubleEq(offer_price)),
                            arg.offer_price(),
                            result_listener) &&
         ExplainMatchResult(Optional(DoubleEq(offer_quantity)),
                            arg.offer_quantity(),
                            result_listener) &&
         ExplainMatchResult(Optional(Eq(offer_party)),
                            arg.offer_counterparty(),
                            result_listener);
}

constexpr auto make_time(std::uint64_t unix_nanoseconds)
    -> historical::Timepoint {
  const std::chrono::nanoseconds unix_time{unix_nanoseconds};
  return historical::Timepoint{unix_time};
}

inline auto make_level(std::optional<double> bid_px,
                       std::optional<double> bid_qty,
                       std::optional<std::string> bid_cp,
                       std::optional<double> offer_px,
                       std::optional<double> offer_qty,
                       std::optional<std::string> offer_cp)
    -> historical::Level {
  Level::Builder builder;
  if (bid_px.has_value()) {
    builder.with_bid_price(*bid_px);
  }
  if (bid_qty.has_value()) {
    builder.with_bid_quantity(*bid_qty);
  }
  if (bid_cp.has_value()) {
    builder.with_bid_counterparty(std::move(*bid_cp));
  }
  if (offer_px.has_value()) {
    builder.with_offer_price(*offer_px);
  }
  if (offer_qty.has_value()) {
    builder.with_offer_quantity(*offer_qty);
  }
  if (offer_cp.has_value()) {
    builder.with_offer_counterparty(std::move(*offer_cp));
  }
  return Level::Builder::construct(std::move(builder));
}

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_TESTS_TEST_UTILS_HISTORICAL_DATA_UTILS_HPP_
