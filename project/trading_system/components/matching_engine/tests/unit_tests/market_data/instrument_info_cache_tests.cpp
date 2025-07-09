#include <gmock/gmock.h>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/cache/instrument_info_cache.hpp"
#include "tools/order_book_notification_builder.hpp"

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

namespace simulator::trading_system::matching_engine::mdata {
namespace {

struct InstrumentInfoCache : Test {
  static auto make_trade(const Price price) {
    return NewTrade().with_trade_price(price).create();
  }

  static auto make_update(auto&&... events) {
    return std::vector{
        OrderBookNotification{std::forward<decltype(events)>(events)}...};
  }

  constexpr static auto NoAction = std::nullopt;

  StreamingSettings settings;
  std::vector<MarketDataEntry> entries;
  mdata::InstrumentInfoCache cache;

  static auto EntryHas(const Price price,
                       const std::optional<MarketEntryAction> action,
                       const MdEntryType type) {
    return AllOf(Field(&MarketDataEntry::price, Eq(price)),
                 Field(&MarketDataEntry::action, Eq(action)),
                 Field(&MarketDataEntry::type, Eq(type)));
  }
};

TEST_F(InstrumentInfoCache, InitialEmptyByDefault) {
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice)
      .enable_data_type_streaming(MdEntryType::Option::HighPrice)
      .enable_data_type_streaming(MdEntryType::Option::MidPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCache, CachesLowPriceAfterSingleTrade) {
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(100.0), NoAction, MdEntryType::Option::LowPrice)));
}

TEST_F(InstrumentInfoCache, CachesLowPriceAfterMultipleTrades) {
  cache.update(make_update(make_trade(Price(100)), make_trade(Price(50))));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(50.0), NoAction, MdEntryType::Option::LowPrice)));
}

TEST_F(InstrumentInfoCache, CachesHighPriceAfterSingleTrade) {
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(100.0), NoAction, MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache, CachesHighPriceAfterMultipleTrades) {
  cache.update(make_update(make_trade(Price(100)), make_trade(Price(200))));
  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(200.0), NoAction, MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache, CachesMidPriceAfterSingleTrade) {
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(100.0), NoAction, MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCache, CachesMidPriceAfterMultipleTrades) {
  cache.update(make_update(make_trade(Price(100)), make_trade(Price(200))));
  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(150.0), NoAction, MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCache, AddsLowPriceWhenInitiallyCached) {
  cache.update(make_update(make_trade(Price(50))));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(50.0),
                                   MarketEntryAction::Option::New,
                                   MdEntryType::Option::LowPrice)));
}

TEST_F(InstrumentInfoCache, ChangesLowPriceWhenCachedValueUpdated) {
  cache.update(make_update(make_trade(Price(50)), make_trade(Price(100))));
  cache.update(make_update(make_trade(Price(45)), make_trade(Price(105))));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(45),
                                   MarketEntryAction::Option::Change,
                                   MdEntryType::Option::LowPrice)));
}

TEST_F(InstrumentInfoCache, DoesNotAffectLowPriceWhenNotUpdated) {
  cache.update(make_update(make_trade(Price(50)), make_trade(Price(100))));
  cache.update(make_update(make_trade(Price(50))));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCache, AddsHighPriceWhenInitiallyCached) {
  cache.update(make_update(make_trade(Price(50))));
  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(50.0),
                                   MarketEntryAction::Option::New,
                                   MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache, ChangesHighPriceWhenCachedValueUpdated) {
  cache.update(make_update(make_trade(Price(50)), make_trade(Price(100))));
  cache.update(make_update(make_trade(Price(45)), make_trade(Price(105))));
  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(105),
                                   MarketEntryAction::Option::Change,
                                   MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache, DoesNotAffectHighPriceWhenNotUpdated) {
  cache.update(make_update(make_trade(Price(50)), make_trade(Price(100))));
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCache, AddsMidPriceWhenInitiallyCached) {
  cache.update(make_update(make_trade(Price(50))));
  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(50.0),
                                   MarketEntryAction::Option::New,
                                   MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCache, ChangesMidPriceWhenCachedValueUpdated) {
  cache.update(make_update(make_trade(Price(50)), make_trade(Price(100))));
  cache.update(make_update(make_trade(Price(45)), make_trade(Price(105))));
  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(75),
                                   MarketEntryAction::Option::Change,
                                   MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCache, DoesNotAffectMidPriceWhenNotUpdated) {
  cache.update(make_update(make_trade(Price(50)), make_trade(Price(100))));
  cache.update(make_update(make_trade(Price(50))));
  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCache, ReportsLowPriceWhenInitiallyCachedInFullUpdate) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice)
      .enable_full_update_streaming();

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(50.0), NoAction, MdEntryType::Option::LowPrice)));
}

TEST_F(InstrumentInfoCache, ReportsHighPriceWhenInitiallyCachedInFullUpdate) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice)
      .enable_full_update_streaming();

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(100.0), NoAction, MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache, ReportsMidPriceWhenInitiallyCachedInFullUpdate) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice)
      .enable_full_update_streaming();

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(75.0), NoAction, MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCache, StoresNullStateWhenCacheIsEmpty) {
  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_EQ(info, std::nullopt);
}

TEST_F(InstrumentInfoCache, StoresStateLowPriceWhenInitiallyCached) {
  cache.update(make_update(make_trade(Price(100)), make_trade(Price(50))));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(
      info,
      Optional(Field(&market_state::InstrumentInfo::low_price, Price{50})));
}

TEST_F(InstrumentInfoCache, StoresStateLowPriceWhenCachedValueNotUpdated) {
  cache.update(make_update(make_trade(Price(100)), make_trade(Price(50))));
  cache.update(make_update(make_trade(Price(50))));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(
      info,
      Optional(Field(&market_state::InstrumentInfo::low_price, Price{50})));
}

TEST_F(InstrumentInfoCache, StoresStateHighPriceWhenInitiallyCached) {
  cache.update(make_update(make_trade(Price(100)), make_trade(Price(50))));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(
      info,
      Optional(Field(&market_state::InstrumentInfo::high_price, Price{100})));
}

TEST_F(InstrumentInfoCache, StoresStateHighPriceWhenCachedValueNotUpdated) {
  cache.update(make_update(make_trade(Price(100)), make_trade(Price(50))));
  cache.update(make_update(make_trade(Price(100))));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(
      info,
      Optional(Field(&market_state::InstrumentInfo::high_price, Price{100})));
}

TEST_F(InstrumentInfoCache,
       InstrumentInfoRecoverDeletesOldLowPriceFromComposeInitial) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(InstrumentInfoRecover{}));

  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);
  cache.compose_initial(settings, entries);

  ASSERT_TRUE(entries.empty());
}

TEST_F(InstrumentInfoCache,
       InstrumentInfoRecoverDeletesOldMidPriceFromComposeInitial) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(InstrumentInfoRecover{}));

  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);
  cache.compose_initial(settings, entries);

  ASSERT_TRUE(entries.empty());
}

TEST_F(InstrumentInfoCache,
       InstrumentInfoRecoverDeletesOldHighPriceFromComposeInitial) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(InstrumentInfoRecover{}));

  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);
  cache.compose_initial(settings, entries);

  ASSERT_TRUE(entries.empty());
}

TEST_F(InstrumentInfoCache, RecoversLowPriceInComposeInitial) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{75}, Price{100}})};
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);
  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(75), NoAction, MdEntryType::Option::LowPrice)));
}

TEST_F(InstrumentInfoCache, RecoversMidPriceInComposeInitial) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{80}, Price{100}})};
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);
  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(90), NoAction, MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCache, RecoversHighPriceInComposeInitial) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{75}, Price{100}})};
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);
  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(100), NoAction, MdEntryType::Option::HighPrice)));
}

TEST_F(
    InstrumentInfoCache,
    RecoverMarksDeletedOldLowPriceInComposeUpdateWhenInstrumentInfoIsNullopt) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(InstrumentInfoRecover{.info = std::nullopt}));

  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(50),
                                   MarketEntryAction::Option::Delete,
                                   MdEntryType::Option::LowPrice)));
}

TEST_F(
    InstrumentInfoCache,
    RecoverMarksDeletedOldMidPriceInComposeUpdateWhenInstrumentInfoIsNullopt) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(InstrumentInfoRecover{.info = std::nullopt}));

  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(50),
                                   MarketEntryAction::Option::Delete,
                                   MdEntryType::Option::MidPrice)));
}

TEST_F(
    InstrumentInfoCache,
    RecoverMarksDeletedOldHighPriceInComposeUpdateWhenInstrumentInfoIsNullopt) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(InstrumentInfoRecover{.info = std::nullopt}));

  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(50),
                                   MarketEntryAction::Option::Delete,
                                   MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache,
       RecoversLowPriceInComposeUpdateAsNewIfPreviousWasNotSet) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{75}, Price{100}})};
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(75),
                                   MarketEntryAction::Option::New,
                                   MdEntryType::Option::LowPrice)));
}

TEST_F(InstrumentInfoCache, RecoversLowPriceInComposeUpdateAsChange) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{75}, Price{100}})};
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(75),
                                   MarketEntryAction::Option::Change,
                                   MdEntryType::Option::LowPrice)));
}

TEST_F(InstrumentInfoCache,
       RecoversNotSetLowPriceInComposeUpdateIfItIsEqualToPrevious) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{50}, Price{100}})};
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);
  cache.compose_update(settings, entries);

  ASSERT_TRUE(entries.empty());
}

TEST_F(InstrumentInfoCache,
       RecoversMidPriceInComposeUpdateAsNewIfPreviousWasNotSet) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{80}, Price{100}})};
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(90),
                                   MarketEntryAction::Option::New,
                                   MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCache, RecoversMidPriceInComposeUpdateAsChange) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{80}, Price{100}})};
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(90),
                                   MarketEntryAction::Option::Change,
                                   MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCache,
       RecoversNotSetMidPriceInComposeUpdateIfItIsEqualToPrevious) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{80}, Price{100}})};
  cache.update(make_update(make_trade(Price(70)), make_trade(Price(110))));
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);
  cache.compose_update(settings, entries);

  ASSERT_TRUE(entries.empty());
}

TEST_F(InstrumentInfoCache,
       RecoversHighPriceInComposeUpdateAsNewIfPreviousWasNotSet) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{75}, Price{100}})};
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(100),
                                   MarketEntryAction::Option::New,
                                   MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache, RecoversHighPriceInComposeUpdateAsChange) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{75}, Price{100}})};
  cache.update(make_update(make_trade(Price(120))));
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);
  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price(100),
                                   MarketEntryAction::Option::Change,
                                   MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache,
       RecoversNotSetHighPriceInComposeUpdateIfItIsEqualToPrevious) {
  InstrumentInfoRecover recover{
      std::make_optional(market_state::InstrumentInfo{Price{50}, Price{100}})};
  cache.update(make_update(make_trade(Price(100))));
  cache.update(make_update(std::move(recover)));

  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);
  cache.compose_update(settings, entries);

  ASSERT_TRUE(entries.empty());
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata

// NOLINTEND(*magic-numbers*)
