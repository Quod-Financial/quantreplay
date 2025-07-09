#include <gtest/gtest.h>

#include <optional>

#include "core/domain/attributes.hpp"
#include "ih/market_data/streaming_settings.hpp"

using namespace testing;  // NOLINT

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

struct StreamingSettings : Test {
  mdata::StreamingSettings settings;
};

TEST_F(StreamingSettings, DefaultSettings) {
  EXPECT_FALSE(settings.is_full_update_requested());
  EXPECT_FALSE(settings.is_top_of_book_only_requested());
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::Bid));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::Offer));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::Trade));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::LowPrice));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::MidPrice));
  EXPECT_FALSE(settings.is_data_type_requested(MdEntryType::Option::HighPrice));
  EXPECT_EQ(settings.excluded_orders_owner(), std::nullopt);
}

TEST_F(StreamingSettings, ManageBidMarketDataType) {
  settings.enable_data_type_streaming(MdEntryType::Option::Bid);

  ASSERT_TRUE(settings.is_data_type_requested(MdEntryType::Option::Bid));
}

TEST_F(StreamingSettings, ManageOfferMarketDataType) {
  settings.enable_data_type_streaming(MdEntryType::Option::Offer);

  ASSERT_TRUE(settings.is_data_type_requested(MdEntryType::Option::Offer));
}

TEST_F(StreamingSettings, ManageTradeMarketDataType) {
  settings.enable_data_type_streaming(MdEntryType::Option::Trade);

  ASSERT_TRUE(settings.is_data_type_requested(MdEntryType::Option::Trade));
}

TEST_F(StreamingSettings, ManageLowPriceMarketDataType) {
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  ASSERT_TRUE(settings.is_data_type_requested(MdEntryType::Option::LowPrice));
}

TEST_F(StreamingSettings, ManageMidPriceMarketDataType) {
  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);

  ASSERT_TRUE(settings.is_data_type_requested(MdEntryType::Option::MidPrice));
}

TEST_F(StreamingSettings, ManageHighPriceMarketDataType) {
  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);

  ASSERT_TRUE(settings.is_data_type_requested(MdEntryType::Option::HighPrice));
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