#include <gtest/gtest.h>

#include <array>
#include <optional>

#include "core/domain/attributes.hpp"
#include "ih/market_data/streaming_settings.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

using namespace testing;  // NOLINT

struct StreamingSettings : Test {
  mdata::StreamingSettings settings;
};

TEST_F(StreamingSettings, DefaultSettings) {
  EXPECT_FALSE(settings.is_full_update_requested());
  EXPECT_FALSE(settings.is_top_of_book_only_requested());
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::Bid));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::Offer));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::Trade));
  EXPECT_FALSE(
      settings.is_data_type_requested(MdEntryType::Option::OpeningPrice));
  EXPECT_FALSE(
      settings.is_data_type_requested(MdEntryType::Option::ClosingPrice));
  EXPECT_FALSE(
      settings.is_data_type_requested(MdEntryType::Option::SettlementPrice));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::HighPrice));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::LowPrice));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::Imbalance));
  EXPECT_FALSE(
      settings.is_data_type_requested(MdEntryType::Option::TradeVolume));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::MidPrice));
  EXPECT_FALSE(
      settings.is_data_type_requested(MdEntryType::Option::EarlyPrice));
  EXPECT_FALSE(settings.is_data_type_requested(
      MdEntryType::Option::AuctionClearingPrice));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::MarketBid));
  EXPECT_FALSE(
      settings.is_data_type_requested(MdEntryType::Option::MarketOffer));
  EXPECT_FALSE(settings.is_data_type_requested(
      MdEntryType::Option::PreviousClosingPrice));
  EXPECT_EQ(settings.excluded_orders_owner(), std::nullopt);
}

TEST_F(StreamingSettings, EachRequestableDataTypeOwnsADistinctFlag) {
  using Option = MdEntryType::Option;

  // Every requestable entry type must map to its own flag, so enabling one
  // never marks another as requested.
  constexpr std::array requestable_types{Option::Bid,
                                         Option::Offer,
                                         Option::Trade,
                                         Option::OpeningPrice,
                                         Option::ClosingPrice,
                                         Option::SettlementPrice,
                                         Option::HighPrice,
                                         Option::LowPrice,
                                         Option::Imbalance,
                                         Option::TradeVolume,
                                         Option::MidPrice,
                                         Option::EarlyPrice,
                                         Option::AuctionClearingPrice,
                                         Option::MarketBid,
                                         Option::MarketOffer,
                                         Option::PreviousClosingPrice};

  for (const auto enabled : requestable_types) {
    mdata::StreamingSettings subject;
    subject.enable_data_type_streaming(enabled);

    for (const auto other : requestable_types) {
      EXPECT_EQ(subject.is_data_type_requested(other), other == enabled)
          << "enabling one requestable data type affected another";
    }
  }
}

TEST_F(StreamingSettings, ManageFullUpdateStreamingOption) {
  settings.enable_full_update_streaming();

  ASSERT_TRUE(settings.is_full_update_requested());
}

TEST_F(StreamingSettings, ManageTopOfBookOnlyStreamingOption) {
  settings.enable_top_of_book_only_streaming();

  ASSERT_TRUE(settings.is_top_of_book_only_requested());
}

TEST_F(StreamingSettings, FilterOrdersByOwner) {
  const PartyId owner("excluded-ord-owner");

  settings.filter_orders_by_owner(owner);

  ASSERT_EQ(settings.excluded_orders_owner(), owner);
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test