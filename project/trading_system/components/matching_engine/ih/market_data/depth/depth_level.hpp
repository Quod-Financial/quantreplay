#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_LEVEL_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_LEVEL_HPP_

#include <gsl/pointers>

#include "core/domain/attributes.hpp"
#include "ih/market_data/depth/depth_record.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class DepthLevel {
  enum class Status : std::uint8_t {
    Unchanged,
    Invisible,
    Added,
    Changed,
    Removed
  };

 public:
  auto is_invisible() const -> bool { return status_ == Status::Invisible; }

  auto is_unchanged() const -> bool { return status_ == Status::Unchanged; }

  auto is_added() const -> bool { return status_ == Status::Added; }

  auto is_changed() const -> bool { return status_ == Status::Changed; }

  auto is_removed() const -> bool { return status_ == Status::Removed; }

  auto id() const -> const MarketEntryId& { return record_->id(); }

  auto price() const -> std::optional<Price> { return record_->price(); }

  auto type() const -> MdEntryType { return record_->type(); }

  auto record() const -> const DepthRecord& { return *record_; }

  auto quantity() const -> Quantity { return quantity_; }

  static auto make_invisible(const DepthRecord& record) {
    return DepthLevel(record, Quantity(0), Status::Invisible);
  }

  static auto make_added(const DepthRecord& record, const Quantity quantity) {
    return DepthLevel(record, quantity, Status::Added);
  }

  static auto make_changed(const DepthRecord& record, const Quantity quantity) {
    return DepthLevel(record, quantity, Status::Changed);
  }

  static auto make_removed(const DepthRecord& record) {
    return DepthLevel(record, Quantity(0), Status::Removed);
  }

  static auto make_unchanged(const DepthRecord& record,
                             const Quantity quantity) {
    return DepthLevel(record, quantity, Status::Unchanged);
  }

 private:
  DepthLevel(const DepthRecord& record,
             const Quantity quantity,
             const Status status)
      : record_(&record), quantity_(quantity), status_(status) {}

  gsl::not_null<const DepthRecord*> record_;
  Quantity quantity_;
  Status status_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_LEVEL_HPP_
