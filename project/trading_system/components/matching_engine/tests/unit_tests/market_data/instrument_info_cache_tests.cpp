#include <gmock/gmock.h>

#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/cache/instrument_info_cache.hpp"
#include "tools/order_book_notification_builder.hpp"

namespace simulator::trading_system::matching_engine::mdata::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct InstrumentInfoCache : Test {
  static auto make_trade(const Price price) {
    return NewTrade().with_trade_price(price).create();
  }

  static auto make_update(auto&&... events) {
    return std::vector{
        OrderBookNotification{std::forward<decltype(events)>(events)}...};
  }

  static auto make_auction_cross(const TradingPhase::Option phase,
                                 const Price price,
                                 const Quantity quantity) {
    return AuctionPricesUpdate{.auction_phase = TradingPhase{phase},
                               .clearing_price = price,
                               .clearing_quantity = quantity};
  }

  static auto make_auction_no_cross(const TradingPhase::Option phase) {
    return AuctionPricesUpdate{.auction_phase = TradingPhase{phase},
                               .clearing_price = std::nullopt,
                               .clearing_quantity = std::nullopt};
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

  static auto ClearingEntryHas(const Price price,
                               const Quantity quantity,
                               const std::optional<MarketEntryAction> action) {
    return AllOf(
        Field(&MarketDataEntry::price, Eq(price)),
        Field(&MarketDataEntry::quantity, Optional(Eq(quantity))),
        Field(&MarketDataEntry::action, Eq(action)),
        Field(&MarketDataEntry::type,
              Eq(MdEntryType{MdEntryType::Option::AuctionClearingPrice})));
  }

  static auto make_early(const Price price, const Quantity quantity) {
    return EarlyPriceUpdate{.early_price = price, .early_quantity = quantity};
  }

  static auto EarlyEntryHas(const Price price,
                            const Quantity quantity,
                            const std::optional<MarketEntryAction> action) {
    return AllOf(Field(&MarketDataEntry::price, Eq(price)),
                 Field(&MarketDataEntry::quantity, Optional(Eq(quantity))),
                 Field(&MarketDataEntry::action, Eq(action)),
                 Field(&MarketDataEntry::type,
                       Eq(MdEntryType{MdEntryType::Option::EarlyPrice})));
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

TEST_F(InstrumentInfoCache, ReportsCachedLowPriceInInitial) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(50.0), NoAction, MdEntryType::Option::LowPrice)));
}

TEST_F(InstrumentInfoCache, ReportsCachedHighPriceInInitial) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price(100.0), NoAction, MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache, ReportsCachedMidPriceInInitial) {
  cache.update(make_update(make_trade(Price(50))));
  cache.update(make_update(make_trade(Price(100))));
  settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);

  cache.compose_initial(settings, entries);

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

TEST_F(InstrumentInfoCache, PublishesOpeningPriceWhenSetAndRequested) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.opening_price = Price{110}})}));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price{110}, NoAction, MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache, OmitsOpeningPriceWhenNotRequested) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.opening_price = Price{110}})}));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCache, OmitsOpeningPriceWhenUnset) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.closing_price = Price{120}})}));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCache, PublishesClosingPriceWhenSetAndRequested) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.closing_price = Price{120}})}));
  settings.enable_data_type_streaming(MdEntryType::Option::ClosingPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price{120}, NoAction, MdEntryType::Option::ClosingPrice)));
}

TEST_F(InstrumentInfoCache, PublishesPreviousClosingPriceWhenSetAndRequested) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.previous_closing_price = Price{95}})}));
  settings.enable_data_type_streaming(
      MdEntryType::Option::PreviousClosingPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(EntryHas(
          Price{95}, NoAction, MdEntryType::Option::PreviousClosingPrice)));
}

TEST_F(InstrumentInfoCache,
       PublishesAuctionClearingPriceAndQuantityWhenSetAndRequested) {
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .auction_clearing_price = Price{130},
          .auction_clearing_quantity = Quantity{500}})}));
  settings.enable_data_type_streaming(
      MdEntryType::Option::AuctionClearingPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(ClearingEntryHas(Price{130}, Quantity{500}, NoAction)));
}

TEST_F(InstrumentInfoCache, ReportsOpeningPriceAsNewInComposeUpdate) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.opening_price = Price{110}})}));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price{110},
                                   MarketEntryAction::Option::New,
                                   MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache,
       ReportsAuctionClearingPriceAndQuantityAsNewInComposeUpdate) {
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .auction_clearing_price = Price{130},
          .auction_clearing_quantity = Quantity{500}})}));
  settings.enable_data_type_streaming(
      MdEntryType::Option::AuctionClearingPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(ClearingEntryHas(
                  Price{130}, Quantity{500}, MarketEntryAction::Option::New)));
}

TEST_F(InstrumentInfoCache,
       ReportsAuctionClearingPriceAndQuantityAsChangeInComposeUpdate) {
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .auction_clearing_price = Price{130},
          .auction_clearing_quantity = Quantity{500}})}));
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .auction_clearing_price = Price{140},
          .auction_clearing_quantity = Quantity{600}})}));
  settings.enable_data_type_streaming(
      MdEntryType::Option::AuctionClearingPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(ClearingEntryHas(
          Price{140}, Quantity{600}, MarketEntryAction::Option::Change)));
}

TEST_F(InstrumentInfoCache, StoresStateOpeningPriceEvenWithoutLowAndHighPrice) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.opening_price = Price{110}})}));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(AllOf(Field(&market_state::InstrumentInfo::opening_price,
                                   Optional(Eq(Price{110}))),
                             Field(&market_state::InstrumentInfo::low_price,
                                   Eq(std::nullopt)))));
}

TEST_F(InstrumentInfoCache, StoresStateAuctionClearingPriceAndQuantity) {
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .auction_clearing_price = Price{130},
          .auction_clearing_quantity = Quantity{500}})}));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(
      info,
      Optional(
          AllOf(Field(&market_state::InstrumentInfo::auction_clearing_price,
                      Optional(Eq(Price{130}))),
                Field(&market_state::InstrumentInfo::auction_clearing_quantity,
                      Optional(Eq(Quantity{500}))))));
}

TEST_F(InstrumentInfoCache, MarksDeletedOpeningPriceOnNulloptRecover) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.opening_price = Price{110}})}));
  cache.update(make_update(InstrumentInfoRecover{.info = std::nullopt}));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price{110},
                                   MarketEntryAction::Option::Delete,
                                   MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache,
       MarksDeletedAuctionClearingPriceAndQuantityOnNulloptRecover) {
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .auction_clearing_price = Price{130},
          .auction_clearing_quantity = Quantity{500}})}));
  cache.update(make_update(InstrumentInfoRecover{.info = std::nullopt}));
  settings.enable_data_type_streaming(
      MdEntryType::Option::AuctionClearingPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(ClearingEntryHas(
          Price{130}, Quantity{500}, MarketEntryAction::Option::Delete)));
}

TEST_F(InstrumentInfoCache, OpeningAuctionCrossSetsOpeningPrice) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price{110}, NoAction, MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache, OpeningAuctionCrossSetsAuctionClearingPrice) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  settings.enable_data_type_streaming(
      MdEntryType::Option::AuctionClearingPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(ClearingEntryHas(Price{110}, Quantity{500}, NoAction)));
}

TEST_F(InstrumentInfoCache,
       OpeningAuctionCrossResetsSessionHighLowToClearingPrice) {
  cache.update(make_update(
      make_trade(Price(90)),
      make_trade(Price(200)),
      make_auction_cross(
          TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice)
      .enable_data_type_streaming(MdEntryType::Option::HighPrice)
      .enable_data_type_streaming(MdEntryType::Option::MidPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{110}, NoAction, MdEntryType::Option::LowPrice),
          EntryHas(Price{110}, NoAction, MdEntryType::Option::HighPrice),
          EntryHas(Price{110}, NoAction, MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCache, ClosingAuctionCrossSetsClosingPrice) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{120}, Quantity{300})));
  settings.enable_data_type_streaming(MdEntryType::Option::ClosingPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price{120}, NoAction, MdEntryType::Option::ClosingPrice)));
}

TEST_F(InstrumentInfoCache,
       ClosingAuctionCrossCarriesOldClosingPriceIntoPreviousClosingPrice) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{120}, Quantity{300})));
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{125}, Quantity{350})));
  settings.enable_data_type_streaming(MdEntryType::Option::ClosingPrice)
      .enable_data_type_streaming(MdEntryType::Option::PreviousClosingPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{125}, NoAction, MdEntryType::Option::ClosingPrice),
          EntryHas(Price{120},
                   NoAction,
                   MdEntryType::Option::PreviousClosingPrice)));
}

TEST_F(InstrumentInfoCache,
       ClosingAuctionCrossLeavesPreviousClosingUnsetWhenNoPriorClosingPrice) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{120}, Quantity{300})));
  settings.enable_data_type_streaming(
      MdEntryType::Option::PreviousClosingPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCache, IntradayAuctionCrossSetsAuctionClearingPriceOnly) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::IntradayAuction, Price{130}, Quantity{600})));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice)
      .enable_data_type_streaming(MdEntryType::Option::ClosingPrice)
      .enable_data_type_streaming(MdEntryType::Option::AuctionClearingPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(ClearingEntryHas(Price{130}, Quantity{600}, NoAction)));
}

TEST_F(InstrumentInfoCache, OpeningAuctionNoCrossClearsOpeningPrice) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  cache.update(
      make_update(make_auction_no_cross(TradingPhase::Option::OpeningAuction)));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price{110},
                                   MarketEntryAction::Option::Delete,
                                   MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache,
       OpeningAuctionNoCrossLeavesSessionHighLowUntouched) {
  cache.update(make_update(make_trade(Price(90)), make_trade(Price(200))));
  cache.update(
      make_update(make_auction_no_cross(TradingPhase::Option::OpeningAuction)));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice)
      .enable_data_type_streaming(MdEntryType::Option::HighPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{90}, NoAction, MdEntryType::Option::LowPrice),
          EntryHas(Price{200}, NoAction, MdEntryType::Option::HighPrice)));
}

TEST_F(InstrumentInfoCache, OpeningAuctionNoCrossClearsAuctionClearingPrice) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::IntradayAuction, Price{130}, Quantity{600})));
  cache.update(
      make_update(make_auction_no_cross(TradingPhase::Option::OpeningAuction)));
  settings.enable_data_type_streaming(
      MdEntryType::Option::AuctionClearingPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(ClearingEntryHas(
          Price{130}, Quantity{600}, MarketEntryAction::Option::Delete)));
}

TEST_F(InstrumentInfoCache,
       ClosingAuctionNoCrossClearsClosingPriceAndCarriesItIntoPreviousClosing) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{120}, Quantity{300})));
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{125}, Quantity{350})));
  cache.update(
      make_update(make_auction_no_cross(TradingPhase::Option::ClosingAuction)));
  settings.enable_data_type_streaming(MdEntryType::Option::ClosingPrice)
      .enable_data_type_streaming(MdEntryType::Option::PreviousClosingPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(Price{125},
                                   MarketEntryAction::Option::Delete,
                                   MdEntryType::Option::ClosingPrice),
                          EntryHas(Price{125},
                                   MarketEntryAction::Option::Change,
                                   MdEntryType::Option::PreviousClosingPrice)));
}

TEST_F(InstrumentInfoCache, IntradayAuctionNoCrossClearsAuctionClearingPrice) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::IntradayAuction, Price{130}, Quantity{600})));
  cache.update(make_update(
      make_auction_no_cross(TradingPhase::Option::IntradayAuction)));
  settings.enable_data_type_streaming(
      MdEntryType::Option::AuctionClearingPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(ClearingEntryHas(
          Price{130}, Quantity{600}, MarketEntryAction::Option::Delete)));
}

TEST_F(InstrumentInfoCache, PublishesEarlyPriceAndQuantityWhenRequested) {
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  settings.enable_data_type_streaming(MdEntryType::Option::EarlyPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EarlyEntryHas(Price{105}, Quantity{100}, NoAction)));
}

TEST_F(InstrumentInfoCache, OmitsEarlyPriceWhenNotRequested) {
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCache, OmitsEarlyPriceWhenUnset) {
  settings.enable_data_type_streaming(MdEntryType::Option::EarlyPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCache, ReportsEarlyPriceAsNewInComposeUpdate) {
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  settings.enable_data_type_streaming(MdEntryType::Option::EarlyPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EarlyEntryHas(
                  Price{105}, Quantity{100}, MarketEntryAction::Option::New)));
}

TEST_F(InstrumentInfoCache, ReportsEarlyPriceAsChangeInComposeUpdate) {
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  cache.update(make_update(make_early(Price{110}, Quantity{120})));
  settings.enable_data_type_streaming(MdEntryType::Option::EarlyPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(EarlyEntryHas(
          Price{110}, Quantity{120}, MarketEntryAction::Option::Change)));
}

TEST_F(InstrumentInfoCache,
       RepublishesUnchangedEarlyPriceAsChangeInComposeUpdate) {
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  settings.enable_data_type_streaming(MdEntryType::Option::EarlyPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(EarlyEntryHas(
          Price{105}, Quantity{100}, MarketEntryAction::Option::Change)));
}

TEST_F(InstrumentInfoCache, ClearsEarlyPriceOnEmptyUpdate) {
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  cache.update(make_update(EarlyPriceUpdate{}));
  settings.enable_data_type_streaming(MdEntryType::Option::EarlyPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(EarlyEntryHas(
          Price{105}, Quantity{100}, MarketEntryAction::Option::Delete)));
}

TEST_F(InstrumentInfoCache, DoesNotStoreEarlyPriceBecauseItIsTransient) {
  cache.update(make_update(make_early(Price{105}, Quantity{100})));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_EQ(info, std::nullopt);
}

TEST_F(InstrumentInfoCache, HasNoUpdateWhenNothingChanged) {
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  EXPECT_FALSE(cache.has_update(settings));
}

TEST_F(InstrumentInfoCache, HasNoUpdateWhenOnlyUnrequestedPricesChanged) {
  cache.update(make_update(make_trade(Price{50})));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  EXPECT_FALSE(cache.has_update(settings));
}

TEST_F(InstrumentInfoCache, HasNoUpdateWhenBatchLeftPricesUnchanged) {
  cache.update(make_update(make_trade(Price{50})));
  cache.update(make_update(make_trade(Price{50})));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  EXPECT_FALSE(cache.has_update(settings));
}

TEST_F(InstrumentInfoCache, HasUpdateWhenRequestedPriceChanged) {
  cache.update(make_update(make_trade(Price{50})));
  settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);

  EXPECT_TRUE(cache.has_update(settings));
}

TEST_F(InstrumentInfoCache, HasUpdateWhenRequestedPriceCleared) {
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  cache.update(make_update(EarlyPriceUpdate{}));
  settings.enable_data_type_streaming(MdEntryType::Option::EarlyPrice);

  EXPECT_TRUE(cache.has_update(settings));
}

TEST_F(InstrumentInfoCache, HasUpdateOnRepeatedEarlyPriceRepublication) {
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  cache.update(make_update(make_early(Price{105}, Quantity{100})));
  settings.enable_data_type_streaming(MdEntryType::Option::EarlyPrice);

  EXPECT_TRUE(cache.has_update(settings));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test

// NOLINTEND(*magic-numbers*)
