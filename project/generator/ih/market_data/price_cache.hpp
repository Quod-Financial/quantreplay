#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_PRICE_CACHE_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_PRICE_CACHE_HPP_

#include <cstdint>
#include <map>
#include <optional>
#include <shared_mutex>
#include <span>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/market_state.hpp"
#include "protocol/app/market_data_snapshot.hpp"

namespace simulator::generator::mdata {

// Keeps the top of the book of every market data subscription the generator
// has made, identified by the subscription request identifier.
// Written by the threads that deliver market data and read by the order
// generation threads.
class PriceCache {
 public:
  auto add(const MdRequestId& request_id) -> void;

  auto process(const protocol::MarketDataSnapshot& snapshot) -> void;

  auto reset(const MdRequestId& request_id) -> void;

  [[nodiscard]]
  auto market_state(const MdRequestId& order_book,
                    std::span<const MdRequestId> external) const -> MarketState;

 private:
  class TopOfBook {
   public:
    auto replace(const std::vector<MarketDataEntry>& entries) -> void;

    auto clear() -> void;

    [[nodiscard]]
    auto state() const -> MarketState;

   private:
    std::optional<Price> best_bid_price_;
    std::optional<Price> best_offer_price_;
    std::uint32_t bid_depth_levels_ = 0;
    std::uint32_t offer_depth_levels_ = 0;
  };

  // Requires the caller to lock the mutex.
  [[nodiscard]]
  auto find_to_modify(const std::optional<MdRequestId>& request_id)
      -> TopOfBook*;

  std::map<MdRequestId, TopOfBook> books_;
  mutable std::shared_mutex mutex_;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_PRICE_CACHE_HPP_
