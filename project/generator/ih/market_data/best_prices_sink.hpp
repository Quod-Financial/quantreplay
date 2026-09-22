#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_BEST_PRICES_SINK_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_BEST_PRICES_SINK_HPP_

#include "core/domain/attributes.hpp"

namespace simulator::generator::mdata {

struct BestPrices {
  std::optional<Price> bid;
  std::optional<Price> offer;
};

class BestPricesSink {
 public:
  BestPricesSink() = default;
  BestPricesSink(const BestPricesSink&) = default;
  BestPricesSink(BestPricesSink&&) noexcept = default;
  virtual ~BestPricesSink() = default;

  auto operator=(const BestPricesSink&) -> BestPricesSink& = default;
  auto operator=(BestPricesSink&&) noexcept -> BestPricesSink& = default;

  virtual auto process(const MdRequestId& request_id, const BestPrices& prices)
      -> void = 0;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_BEST_PRICES_SINK_HPP_
