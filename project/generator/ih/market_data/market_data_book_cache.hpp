#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_DATA_BOOK_CACHE_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_DATA_BOOK_CACHE_HPP_

#include <cstdint>
#include <map>
#include <mutex>
#include <optional>
#include <vector>

#include "core/domain/attributes.hpp"
#include "core/domain/instrument_descriptor.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"

namespace simulator::generator::mdata {

// Keeps a copy of the book of every incremental subscription, written by the
// market data thread and by the threads that subscribe and unsubscribe.
class MarketDataBookCache {
 public:
  struct UpdateOutcome {
    std::optional<protocol::MarketDataSnapshot> snapshot;
    // Set when the update can not be applied, the updates and full refreshes
    // of the subscription are then ignored until it is tracked again.
    bool diverged = false;
  };

  // Starts keeping the book of the subscription, awaiting its full refresh.
  auto track(const MdRequestId& request_id, MarketDepth depth) -> void;

  auto reset(const MdRequestId& request_id) -> void;

  // Tells whether the full refresh is to be published, which it is not once
  // the book of its subscription has diverged.
  [[nodiscard]]
  auto process(const protocol::MarketDataSnapshot& snapshot) -> bool;

  [[nodiscard]]
  auto process(const protocol::MarketDataUpdate& update) -> UpdateOutcome;

 private:
  enum class EntryOutcome : std::uint8_t { Skipped, Applied, Unmatched };

  class Book {
   public:
    Book(InstrumentDescriptor instrument,
         const std::vector<MarketDataEntry>& entries,
         MarketDepth depth);

    [[nodiscard]]
    auto apply(const MarketDataEntry& entry) -> EntryOutcome;

    // Drops the levels the update pushed beyond the subscribed depth.
    auto trim() -> void;

    [[nodiscard]]
    auto make_snapshot(const protocol::MarketDataUpdate& update) const
        -> protocol::MarketDataSnapshot;

   private:
    struct Location {
      std::vector<MarketDataEntry>* side = nullptr;
      std::vector<MarketDataEntry>::iterator entry;
    };

    [[nodiscard]]
    auto insert(MarketDataEntry entry) -> EntryOutcome;

    [[nodiscard]]
    auto change(const MarketDataEntry& entry) -> EntryOutcome;

    [[nodiscard]]
    auto remove(const MarketDataEntry& entry) -> EntryOutcome;

    [[nodiscard]]
    auto locate(const MarketDataEntry& entry) -> std::optional<Location>;

    [[nodiscard]]
    auto side_of(const MarketDataEntry& entry) -> std::vector<MarketDataEntry>*;

    InstrumentDescriptor instrument_;
    std::vector<MarketDataEntry> bids_;
    std::vector<MarketDataEntry> offers_;
    MarketDepth depth_;
  };

  struct TrackedBook {
    MarketDepth depth{AllDepthLevels};
    // Absent while the full refresh of the subscription is awaited, and once
    // the book has diverged.
    std::optional<Book> book;
    bool diverged = false;
  };

  std::map<MdRequestId, TrackedBook> books_;
  std::mutex mutex_;
};

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_MARKET_DATA_BOOK_CACHE_HPP_
