#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_RECORD_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_RECORD_HPP_

#include <utility>

#include "core/domain/attributes.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class DepthRecord {
 public:
  DepthRecord(MarketEntryId identifier,
              const std::optional<Price> price,
              const MdEntryType type)
      : id_(std::move(identifier)), price_(price), type_(type) {}

  auto id() const -> const MarketEntryId& { return id_; }

  auto price() const -> std::optional<Price> { return price_; }

  auto type() const -> MdEntryType { return type_; }

 private:
  MarketEntryId id_;
  std::optional<Price> price_;
  MdEntryType type_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  //  SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_DEPTH_DEPTH_RECORD_HPP_
