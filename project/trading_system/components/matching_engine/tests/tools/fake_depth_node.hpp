#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_FAKE_DEPTH_NODE_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_FAKE_DEPTH_NODE_HPP_

#include <cstdint>

#include "core/common/unreachable.hpp"
#include "ih/market_data/depth/depth_level.hpp"
#include "ih/market_data/depth/depth_record.hpp"

namespace simulator::trading_system::matching_engine::mdata {

template <core::enumerators::MdEntryType Type>
class FakeDepthNode {
  enum class Category : std::uint8_t {
    Unchanged,
    Added,
    Changed,
    Removed,
    Invisible
  };

 public:
  auto level() const& -> DepthLevel {
    switch (category_) {
      case Category::Unchanged:
        return DepthLevel::make_unchanged(record_, quantity_);
      case Category::Added:
        return DepthLevel::make_added(record_, quantity_);
      case Category::Changed:
        return DepthLevel::make_changed(record_, quantity_);
      case Category::Removed:
        return DepthLevel::make_removed(record_);
      case Category::Invisible:
        return DepthLevel::make_invisible(record_);
    }
    core::unreachable();
  }

  static auto fake_id() -> const MarketEntryId& { return FakeNodeId; }

  static auto unchanged(const std::optional<Price> price,
                        const Quantity quantity) {
    return FakeDepthNode(
        DepthRecord(FakeNodeId, price, Type), quantity, Category::Unchanged);
  }

  static auto added(const std::optional<Price> price, const Quantity quantity) {
    return FakeDepthNode(
        DepthRecord(FakeNodeId, price, Type), quantity, Category::Added);
  }

  static auto changed(const std::optional<Price> price,
                      const Quantity quantity) {
    return FakeDepthNode(
        DepthRecord(FakeNodeId, price, Type), quantity, Category::Changed);
  }

  static auto removed(const std::optional<Price> price) {
    return FakeDepthNode(
        DepthRecord(FakeNodeId, price, Type), Quantity(0), Category::Removed);
  }

  static auto invisible(const std::optional<Price> price) {
    return FakeDepthNode(
        DepthRecord(FakeNodeId, price, Type), Quantity(0), Category::Invisible);
  }

 private:
  FakeDepthNode(DepthRecord record,
                const Quantity quantity,
                const Category category)
      : record_(std::move(record)), quantity_(quantity), category_(category) {}

  DepthRecord record_;
  Quantity quantity_;
  Category category_;

  static inline auto FakeNodeId = MarketEntryId("fake-node");
};

using FakeBidNode = FakeDepthNode<MdEntryType::Option::Bid>;
using FakeOfferNode = FakeDepthNode<MdEntryType::Option::Offer>;

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_FAKE_DEPTH_NODE_HPP_
