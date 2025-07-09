#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_STREAMING_SETTINGS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_STREAMING_SETTINGS_HPP_

#include <bitset>
#include <cstdint>
#include <optional>

#include "core/domain/attributes.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class StreamingSettings {
 public:
  auto is_data_type_requested(const MdEntryType type) const -> bool {
    if (const auto flag = to_option_flag(type); flag < flags_count) {
      return flags_[flag];
    }
    return false;
  }

  auto is_full_update_requested() const -> bool {
    return flags_[compose_full_update_flag];
  }

  auto is_top_of_book_only_requested() const -> bool {
    return flags_[report_top_of_book_only_flag];
  }

  auto excluded_orders_owner() const& -> const std::optional<PartyId>& {
    return owner_filtering_id_;
  }

  auto enable_data_type_streaming(const MdEntryType type)
      -> StreamingSettings& {
    if (const auto flag = to_option_flag(type); flag < flags_count) {
      flags_[flag] = true;
    }
    return *this;
  }

  auto enable_full_update_streaming() -> StreamingSettings& {
    flags_[compose_full_update_flag] = true;
    return *this;
  }

  auto enable_top_of_book_only_streaming() -> StreamingSettings& {
    flags_[report_top_of_book_only_flag] = true;
    return *this;
  }

  auto filter_orders_by_owner(PartyId owner) -> StreamingSettings& {
    owner_filtering_id_ = std::move(owner);
    return *this;
  }

 private:
  enum OptionFlag : std::uint8_t {
    compose_full_update_flag,
    report_top_of_book_only_flag,
    stream_bid_data_flag,
    stream_offer_data_flag,
    stream_trades_data_flag,
    stream_low_price_flag,
    stream_high_price_flag,
    stream_mid_price_flag,
    flags_count
  };

  constexpr static auto to_option_flag(const MdEntryType type) -> OptionFlag {
    switch (static_cast<MdEntryType::Option>(type)) {
      case MdEntryType::Option::Bid:
        return stream_bid_data_flag;
      case MdEntryType::Option::Offer:
        return stream_offer_data_flag;
      case MdEntryType::Option::Trade:
        return stream_trades_data_flag;
      case MdEntryType::Option::LowPrice:
        return stream_low_price_flag;
      case MdEntryType::Option::MidPrice:
        return stream_mid_price_flag;
      case MdEntryType::Option::HighPrice:
        return stream_high_price_flag;
    }
    return flags_count;
  }

  std::optional<PartyId> owner_filtering_id_;
  std::bitset<flags_count> flags_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_STREAMING_SETTINGS_HPP_
