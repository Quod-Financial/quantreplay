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

  static auto trade_in_phase(const TradingPhase::Option phase,
                             const Price price) -> Trade {
    return NewTrade()
        .with_trade_price(price)
        .with_market_phase({phase, TradingStatus::Option::Halt})
        .create();
  }

  static auto yesterday() -> core::sys_us {
    using namespace std::chrono_literals;
    return core::sys_us{core::sys_days{2025y / 1 / 1} + 12h};
  }

  static auto today() -> core::sys_us {
    using namespace std::chrono_literals;
    return core::sys_us{core::sys_days{2025y / 1 / 2} + 12h};
  }

  static auto trade_on(core::sys_us time, Price price) -> Trade {
    return NewTrade().with_trade_price(price).with_trade_time(time).create();
  }

  static auto tz_day_passed(core::sys_us time) -> TzDayPassed {
    return TzDayPassed{.sys_tick_time = time};
  }

  static auto make_update(auto&&... events) {
    return std::vector{
        OrderBookNotification{std::forward<decltype(events)>(events)}...};
  }

  static auto make_auction_cross(const TradingPhase::Option phase,
                                 const Price price,
                                 const Quantity quantity)
      -> AuctionFinalPriceUpdate {
    return AuctionFinalPriceUpdate{
        .auction_phase = TradingPhase{phase},
        .clearing_value = TradeResult{.price = price, .quantity = quantity}};
  }

  static auto make_auction_no_cross(const TradingPhase::Option phase)
      -> AuctionFinalPriceUpdate {
    return AuctionFinalPriceUpdate{.auction_phase = TradingPhase{phase},
                                   .clearing_value = std::nullopt};
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
    return EarlyPriceUpdate{
        .early_value = TradeResult{.price = price, .quantity = quantity}};
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

  static auto VolumeEntryHas(const Quantity quantity,
                             const std::optional<MarketEntryAction> action) {
    return AllOf(Field(&MarketDataEntry::quantity, Optional(Eq(quantity))),
                 Field(&MarketDataEntry::action, Eq(action)),
                 Field(&MarketDataEntry::type,
                       Eq(MdEntryType{MdEntryType::Option::TradeVolume})));
  }

  static auto make_indicative_cross(const TradingPhase::Option phase,
                                    const Price price,
                                    const Quantity volume,
                                    const Quantity size,
                                    const TradeCondition side)
      -> AuctionIndicativeUpdate {
    return AuctionIndicativeUpdate{
        .auction_phase = TradingPhase{phase},
        .price_qty =
            AuctionIndicativeUpdate::IndicativePriceQuantity{
                .price = price, .quantity = volume},
        .imbalance =
            AuctionIndicativeUpdate::Imbalance{.size = size, .side = side}};
  }

  static auto make_indicative_no_cross(const TradingPhase::Option phase)
      -> AuctionIndicativeUpdate {
    return AuctionIndicativeUpdate{
        .auction_phase = TradingPhase{phase},
        .price_qty =
            AuctionIndicativeUpdate::IndicativePriceQuantity{
                .price = std::nullopt, .quantity = Quantity{0}},
        .imbalance = std::nullopt};
  }

  static auto PriceQuantityEntryHas(
      const std::optional<Price> price,
      const std::optional<Quantity> quantity,
      const std::optional<MarketEntryAction> action,
      const MdEntryType type) {
    return AllOf(Field(&MarketDataEntry::price, Eq(price)),
                 Field(&MarketDataEntry::quantity, Eq(quantity)),
                 Field(&MarketDataEntry::action, Eq(action)),
                 Field(&MarketDataEntry::type, Eq(type)));
  }

  static auto ImbalanceEntryHas(const Quantity size,
                                const TradeCondition side,
                                const std::optional<MarketEntryAction> action) {
    return AllOf(Field(&MarketDataEntry::quantity, Optional(Eq(size))),
                 Field(&MarketDataEntry::trade_condition, Optional(Eq(side))),
                 Field(&MarketDataEntry::action, Eq(action)),
                 Field(&MarketDataEntry::type,
                       Eq(MdEntryType{MdEntryType::Option::Imbalance})));
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

TEST_F(InstrumentInfoCache, StoresStateTradeVolume) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.trade_volume = Quantity{500}})}));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::trade_volume,
                             Optional(Eq(Quantity{500})))));
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

TEST_F(InstrumentInfoCache, MarksDeletedTradeVolumeOnNulloptRecover) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.trade_volume = Quantity{500}})}));
  cache.update(make_update(InstrumentInfoRecover{.info = std::nullopt}));
  settings.enable_data_type_streaming(MdEntryType::Option::TradeVolume);

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(VolumeEntryHas(Quantity{500},
                                         MarketEntryAction::Option::Delete)));
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

TEST_F(InstrumentInfoCache, HasNoUpdateWhenOnlyUnrequestedEarlyPriceChanged) {
  cache.update(
      make_update(EarlyPriceUpdate{TradeResult{Price{50}, Quantity{100}}}));
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

TEST_F(InstrumentInfoCache, OpeningAuctionCrossStoresOpeningPriceTime) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::opening_price_time,
                             Ne(std::nullopt))));
}

TEST_F(InstrumentInfoCache, OpeningAuctionNoCrossStoresOpeningPriceTime) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  cache.update(
      make_update(make_auction_no_cross(TradingPhase::Option::OpeningAuction)));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::opening_price_time,
                             Ne(std::nullopt))));
}

TEST_F(InstrumentInfoCache, RecoversOpeningPriceTime) {
  using namespace std::chrono_literals;
  constexpr auto opening_time =
      core::sys_us{core::sys_days{2025y / 12 / 31} + 9h + 30min};
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .opening_price = Price{110}, .opening_price_time = opening_time})}));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::opening_price_time,
                             Optional(Eq(opening_time)))));
}

TEST_F(InstrumentInfoCache, NulloptRecoverUpdatesOpeningPriceTime) {
  using namespace std::chrono_literals;
  constexpr auto opening_time =
      core::sys_us{core::sys_days{2025y / 12 / 31} + 9h + 30min};
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .opening_price = Price{110}, .opening_price_time = opening_time})}));
  cache.update(make_update(InstrumentInfoRecover{.info = std::nullopt}));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::opening_price_time,
                             Gt(opening_time))));
}

TEST_F(InstrumentInfoCache, ClosingAuctionCrossStoresClosingPriceTime) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{120}, Quantity{500})));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::closing_price_time,
                             Ne(std::nullopt))));
}

TEST_F(InstrumentInfoCache, ClosingAuctionNoCrossStoresClosingPriceTime) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{120}, Quantity{500})));
  cache.update(
      make_update(make_auction_no_cross(TradingPhase::Option::ClosingAuction)));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::closing_price_time,
                             Ne(std::nullopt))));
}

TEST_F(InstrumentInfoCache, RecoversClosingPriceTime) {
  using namespace std::chrono_literals;
  constexpr auto closing_time =
      core::sys_us{core::sys_days{2025y / 12 / 31} + 17h + 30min};
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .closing_price = Price{120}, .closing_price_time = closing_time})}));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::closing_price_time,
                             Optional(Eq(closing_time)))));
}

TEST_F(InstrumentInfoCache, NulloptRecoverUpdatesClosingPriceTime) {
  using namespace std::chrono_literals;
  constexpr auto closing_time =
      core::sys_us{core::sys_days{2025y / 12 / 31} + 17h + 30min};
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .closing_price = Price{120}, .closing_price_time = closing_time})}));
  cache.update(make_update(InstrumentInfoRecover{.info = std::nullopt}));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::closing_price_time,
                             Gt(closing_time))));
}

struct InstrumentInfoCacheOpeningPrice : InstrumentInfoCache {
  InstrumentInfoCacheOpeningPrice() {
    settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);
  }
};

TEST_F(InstrumentInfoCacheOpeningPrice, DoesNotUpdateWhenPreOpenScheduled) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = true});
  cache.update(make_update(trade_on(today(), Price{200})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCacheOpeningPrice,
       DoesNotUpdateWhenOpeningPriceTimeIsToday) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = false});
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .opening_price = Price{50}, .opening_price_time = today()})}));
  cache.update(make_update(trade_on(today(), Price{110})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price{50}, NoAction, MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCacheOpeningPrice,
       UpdatesOnFirstTradeWhenOpeningPriceTimeNotSet) {
  cache.update(make_update(trade_on(today(), Price{110})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price{110}, NoAction, MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCacheOpeningPrice,
       UpdatesWhenTradeIsFirstAfterMidnightAndOpeningPriceTimeIsYesterday) {
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .opening_price = Price{50}, .opening_price_time = yesterday()})}));
  cache.update(make_update(trade_on(today(), Price{110})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price{110}, NoAction, MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCacheOpeningPrice,
       DoesNotUpdateOnSecondTradeAfterMidnight) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = false});
  cache.update(make_update(trade_on(today(), Price{50})));
  cache.update(make_update(trade_on(today(), Price{110})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(EntryHas(
                  Price{50}, NoAction, MdEntryType::Option::OpeningPrice)));
}

struct InstrumentInfoCacheLowMidHighPrice : InstrumentInfoCache {
  InstrumentInfoCacheLowMidHighPrice() {
    settings.enable_data_type_streaming(MdEntryType::Option::LowPrice);
    settings.enable_data_type_streaming(MdEntryType::Option::MidPrice);
    settings.enable_data_type_streaming(MdEntryType::Option::HighPrice);
  }
};

TEST_F(InstrumentInfoCacheLowMidHighPrice,
       DoesNotResetToTradePriceWhenPreOpenScheduled) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = true});
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .low_price = Price{10}, .high_price = Price{20}})}));
  cache.update(make_update(trade_on(yesterday(), Price{100})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{10}, NoAction, MdEntryType::Option::LowPrice),
          EntryHas(Price{100}, NoAction, MdEntryType::Option::HighPrice),
          EntryHas(Price{55}, NoAction, MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCacheLowMidHighPrice,
       DoesNotResetToTradePriceWhenOpeningPriceTimeIsToday) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = false});
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.low_price = Price{10},
                                   .high_price = Price{20},
                                   .opening_price_time = today()})}));
  cache.update(make_update(trade_on(today(), Price{100})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{10}, NoAction, MdEntryType::Option::LowPrice),
          EntryHas(Price{100}, NoAction, MdEntryType::Option::HighPrice),
          EntryHas(Price{55}, NoAction, MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCacheLowMidHighPrice,
       ResetsToTradePriceWhenOpeningPriceTimeIsNotSet) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = false});
  cache.update(make_update(
      InstrumentInfoRecover{std::make_optional(market_state::InstrumentInfo{
          .low_price = Price{10}, .high_price = Price{20}})}));
  cache.update(make_update(trade_on(today(), Price{100})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{100}, NoAction, MdEntryType::Option::LowPrice),
          EntryHas(Price{100}, NoAction, MdEntryType::Option::HighPrice),
          EntryHas(Price{100}, NoAction, MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCacheLowMidHighPrice,
       ResetsWhenTradeIsFirstAfterMidnightAndOpeningPriceTimeIsYesterday) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = false});
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.low_price = Price{10},
                                   .high_price = Price{20},
                                   .opening_price_time = yesterday()})}));
  cache.update(make_update(trade_on(today(), Price{100})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{100}, NoAction, MdEntryType::Option::LowPrice),
          EntryHas(Price{100}, NoAction, MdEntryType::Option::HighPrice),
          EntryHas(Price{100}, NoAction, MdEntryType::Option::MidPrice)));
}

TEST_F(InstrumentInfoCacheLowMidHighPrice,
       DoesNotResetOnSecondTradeAfterMidnight) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = false});
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.low_price = Price{10},
                                   .high_price = Price{20},
                                   .opening_price_time = yesterday()})}));
  cache.update(make_update(trade_on(today(), Price{100})));
  cache.update(make_update(trade_on(today(), Price{200})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{100}, NoAction, MdEntryType::Option::LowPrice),
          EntryHas(Price{200}, NoAction, MdEntryType::Option::HighPrice),
          EntryHas(Price{150}, NoAction, MdEntryType::Option::MidPrice)));
}

struct InstrumentInfoCacheClosingPrice : InstrumentInfoCache {
  InstrumentInfoCacheClosingPrice() {
    settings.enable_data_type_streaming(MdEntryType::Option::ClosingPrice);
    settings.enable_data_type_streaming(
        MdEntryType::Option::PreviousClosingPrice);
  }
};

TEST_F(InstrumentInfoCacheClosingPrice, DoesNotUpdateWhenPreCloseScheduled) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .closing_auction_scheduled = true});
  cache.update(make_update(trade_on(yesterday(), Price{200})));
  cache.update(make_update(trade_on(today(), Price{100})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCacheClosingPrice,
       DoesNotUpdateWhenNoLastTradeBeforeMidnightWhenFirstTradeComes) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .closing_auction_scheduled = false});
  cache.update(make_update(trade_on(today(), Price{100})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(
    InstrumentInfoCacheClosingPrice,
    UpdatesToLastTradeBeforeMidnightWhenItExistsAndFirstTradeAfterMidnightComes) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .closing_auction_scheduled = false});
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.closing_price = Price{95}})}));
  cache.update(make_update(trade_on(yesterday(), Price{200})));
  cache.update(make_update(trade_on(yesterday(), Price{150})));
  cache.update(make_update(trade_on(today(), Price{100})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{150}, NoAction, MdEntryType::Option::ClosingPrice),
          EntryHas(
              Price{95}, NoAction, MdEntryType::Option::PreviousClosingPrice)));
}

TEST_F(InstrumentInfoCacheClosingPrice,
       DoesNotUpdateWhenNoLastTradeBeforeMidnightWhenDayPasses) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .closing_auction_scheduled = false});
  cache.update(make_update(tz_day_passed(today())));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCacheClosingPrice,
       UpdatesToLastTradeBeforeMidnightWhenItExistsAndDayPasses) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .closing_auction_scheduled = false});
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.closing_price = Price{95}})}));
  cache.update(make_update(trade_on(yesterday(), Price{200})));
  cache.update(make_update(trade_on(yesterday(), Price{150})));
  cache.update(make_update(tz_day_passed(today())));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{150}, NoAction, MdEntryType::Option::ClosingPrice),
          EntryHas(
              Price{95}, NoAction, MdEntryType::Option::PreviousClosingPrice)));
}

TEST_F(
    InstrumentInfoCacheClosingPrice,
    DoesNotUpdatePreviousClosingPriceAgainWhenFirstTradeComesAfterDayPassed) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .closing_auction_scheduled = false});
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.closing_price = Price{95}})}));

  cache.update(make_update(trade_on(yesterday(), Price{150})));
  cache.update(make_update(tz_day_passed(today())));
  cache.update(make_update(trade_on(today(), Price{100})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(
          EntryHas(Price{150}, NoAction, MdEntryType::Option::ClosingPrice),
          EntryHas(
              Price{95}, NoAction, MdEntryType::Option::PreviousClosingPrice)));
}

struct InstrumentInfoCacheTradeVolume : InstrumentInfoCache {
  InstrumentInfoCacheTradeVolume() {
    settings.enable_data_type_streaming(MdEntryType::Option::TradeVolume);
  }

  static auto open_trade(const Quantity quantity) -> Trade {
    return NewTrade()
        .with_traded_quantity(quantity)
        .with_market_phase(MarketPhase::open())
        .create();
  }

  static auto open_trade_on(core::sys_us time, const Quantity quantity)
      -> Trade {
    return NewTrade()
        .with_traded_quantity(quantity)
        .with_trade_time(time)
        .with_market_phase(MarketPhase::open())
        .create();
  }

  static auto auction_trade_on(core::sys_us time,
                               const Quantity quantity,
                               const TradingPhase::Option phase) -> Trade {
    return NewTrade()
        .with_traded_quantity(quantity)
        .with_trade_time(time)
        .with_market_phase({phase, TradingStatus::Option::Halt})
        .create();
  }
};

TEST_F(InstrumentInfoCacheTradeVolume, DoesNotPublishTradeVolumeWhenNeverSet) {
  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, IsEmpty());
}

TEST_F(InstrumentInfoCacheTradeVolume, RecoversFromInstrumentInfoRecover) {
  cache.update(make_update(InstrumentInfoRecover{std::make_optional(
      market_state::InstrumentInfo{.trade_volume = Quantity{500}})}));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(VolumeEntryHas(Quantity{500}, NoAction)));
}

TEST_F(InstrumentInfoCacheTradeVolume,
       ResetsToClearingQuantityWhenOpeningAuctionCrossed) {
  cache.update(make_update(open_trade(Quantity{40})));
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(VolumeEntryHas(Quantity{500}, NoAction)));
}

TEST_F(InstrumentInfoCacheTradeVolume, DeletesWhenOpeningAuctionNotCrossed) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  cache.update(
      make_update(make_auction_no_cross(TradingPhase::Option::OpeningAuction)));

  cache.compose_update(settings, entries);

  ASSERT_THAT(entries,
              ElementsAre(VolumeEntryHas(Quantity{500},
                                         MarketEntryAction::Option::Delete)));
}

TEST_F(InstrumentInfoCacheTradeVolume, AddsTradeQuantityOnOpenPhase) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = true});
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  cache.update(make_update(open_trade(Quantity{40})));
  cache.update(make_update(open_trade(Quantity{60})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(VolumeEntryHas(Quantity{600}, NoAction)));
}

TEST_F(InstrumentInfoCacheTradeVolume,
       DoesNotAddTradeQuantityOnPreOpeningPhase) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = true});
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  cache.update(
      make_update(NewTrade()
                      .with_market_phase({TradingPhase::Option::OpeningAuction,
                                          TradingStatus::Option::Resume})
                      .with_traded_quantity(Quantity{40})
                      .create()));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(VolumeEntryHas(Quantity{500}, NoAction)));
}

TEST_F(InstrumentInfoCacheTradeVolume,
       ResetsToFirstTradeQuantityAfterMidnightWhenOpeningAuctionNotConfigured) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = false});
  cache.update(make_update(open_trade_on(yesterday(), Quantity{200})));
  cache.update(make_update(open_trade_on(today(), Quantity{80})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(VolumeEntryHas(Quantity{80}, NoAction)));
}

TEST_F(InstrumentInfoCacheTradeVolume,
       AddsClearingQuantityWhenIntradayAuctionCrossed) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::IntradayAuction, Price{130}, Quantity{600})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(VolumeEntryHas(Quantity{1100}, NoAction)));
}

TEST_F(InstrumentInfoCacheTradeVolume,
       AddsClearingQuantityWhenClosingAuctionCrossed) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{110}, Quantity{500})));
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{120}, Quantity{300})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(VolumeEntryHas(Quantity{800}, NoAction)));
}

TEST_F(InstrumentInfoCacheTradeVolume,
       DoesNotDoubleCountFirstDailyTradeCrossedByIntradayAuction) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = false});
  cache.update(make_update(auction_trade_on(
      today(), Quantity{40}, TradingPhase::Option::IntradayAuction)));
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::IntradayAuction, Price{130}, Quantity{140})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(VolumeEntryHas(Quantity{140}, NoAction)));
}

TEST_F(InstrumentInfoCacheTradeVolume,
       DoesNotDoubleCountFirstDailyTradeCrossedByClosingAuction) {
  cache.configure({.clock = core::TzClock{"Europe/Kyiv"},
                   .opening_auction_scheduled = false});
  cache.update(make_update(auction_trade_on(
      today(), Quantity{40}, TradingPhase::Option::ClosingAuction)));
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::ClosingAuction, Price{120}, Quantity{140})));

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries, ElementsAre(VolumeEntryHas(Quantity{140}, NoAction)));
}

TEST_F(InstrumentInfoCache, TracksLastOpenPhaseTradedPrice) {
  cache.update(make_update(make_trade(Price{100})));

  ASSERT_EQ(cache.last_open_phase_traded_price(), Price{100});
}

TEST_F(InstrumentInfoCache,
       KeepsLastOpenPhaseTradedPriceWhenLaterAuctionTradeArrives) {
  cache.update(make_update(make_trade(Price{100})));
  cache.update(make_update(
      trade_in_phase(TradingPhase::Option::IntradayAuction, Price{200})));

  ASSERT_EQ(cache.last_open_phase_traded_price(), Price{100});
}

TEST_F(InstrumentInfoCache,
       DoesNotTrackNonOpenPhaseTradeAsLastOpenPhaseTradedPrice) {
  cache.update(make_update(
      trade_in_phase(TradingPhase::Option::PostTrading, Price{200})));

  ASSERT_EQ(cache.last_open_phase_traded_price(), std::nullopt);
}

TEST_F(InstrumentInfoCache, StoresLastOpenPhaseTradedPrice) {
  cache.update(make_update(make_trade(Price{100})));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(
                  &market_state::InstrumentInfo::last_open_phase_traded_price,
                  Optional(Price{100}))));
}

TEST_F(InstrumentInfoCache, RecoversLastOpenPhaseTradedPrice) {
  cache.update(make_update(
      InstrumentInfoRecover{.info = market_state::InstrumentInfo{
                                .last_open_phase_traded_price = Price{100}}}));

  ASSERT_EQ(cache.last_open_phase_traded_price(), Price{100});
}

TEST_F(InstrumentInfoCache, ReturnesClosingPrice) {
  cache.update(make_update(InstrumentInfoRecover{
      .info = market_state::InstrumentInfo{.closing_price = Price{120}}}));

  ASSERT_EQ(cache.closing_price(), Price{120});
}

TEST_F(InstrumentInfoCache,
       ComposesIndicativeOpeningPriceAndImbalanceWhenCrossed) {
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::OpeningAuction,
                            Price{60.07},
                            Quantity{300},
                            Quantity{300},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice)
      .enable_data_type_streaming(MdEntryType::Option::Imbalance);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(entries,
              UnorderedElementsAre(
                  PriceQuantityEntryHas(Price{60.07},
                                        Quantity{300},
                                        NoAction,
                                        MdEntryType::Option::OpeningPrice),
                  ImbalanceEntryHas(Quantity{300},
                                    TradeCondition::Option::ImbalanceMoreBuyers,
                                    NoAction)));
}

TEST_F(InstrumentInfoCache,
       ComposesIndicativeSettlementPriceAndImbalanceWhenCrossed) {
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::IntradayAuction,
                            Price{60.08},
                            Quantity{1300},
                            Quantity{0},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  settings.enable_data_type_streaming(MdEntryType::Option::SettlementPrice)
      .enable_data_type_streaming(MdEntryType::Option::Imbalance);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(PriceQuantityEntryHas(Price{60.08},
                                        Quantity{1300},
                                        NoAction,
                                        MdEntryType::Option::SettlementPrice),
                  ImbalanceEntryHas(Quantity{0},
                                    TradeCondition::Option::ImbalanceMoreBuyers,
                                    NoAction)));
}

TEST_F(InstrumentInfoCache,
       ComposesIndicativeClosingPriceAndImbalanceWhenCrossed) {
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::ClosingAuction,
                            Price{60.08},
                            Quantity{1300},
                            Quantity{0},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  settings.enable_data_type_streaming(MdEntryType::Option::ClosingPrice)
      .enable_data_type_streaming(MdEntryType::Option::Imbalance);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(PriceQuantityEntryHas(Price{60.08},
                                        Quantity{1300},
                                        NoAction,
                                        MdEntryType::Option::ClosingPrice),
                  ImbalanceEntryHas(Quantity{0},
                                    TradeCondition::Option::ImbalanceMoreBuyers,
                                    NoAction)));
}

TEST_F(InstrumentInfoCache,
       IndicativeOpeningPriceReplacesPersistedOpeningPriceDuringCall) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{100}, Quantity{10})));
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::OpeningAuction,
                            Price{105},
                            Quantity{20},
                            Quantity{5},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(PriceQuantityEntryHas(Price{105},
                                        Quantity{20},
                                        NoAction,
                                        MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache, DeletesImbalanceWhenAuctionCallEnds) {
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::OpeningAuction,
                            Price{60.07},
                            Quantity{300},
                            Quantity{300},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  cache.update(make_update(AuctionIndicativeUpdate{}));
  settings.enable_data_type_streaming(MdEntryType::Option::Imbalance);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(ImbalanceEntryHas(Quantity{300},
                                    TradeCondition::Option::ImbalanceMoreBuyers,
                                    MarketEntryAction::Option::Delete)));
}

TEST_F(InstrumentInfoCache, DeletesSettlementPriceWhenAuctionCallEnds) {
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::IntradayAuction,
                            Price{60.08},
                            Quantity{1300},
                            Quantity{0},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  cache.update(make_update(AuctionIndicativeUpdate{}));
  settings.enable_data_type_streaming(MdEntryType::Option::SettlementPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(PriceQuantityEntryHas(Price{60.08},
                                        Quantity{1300},
                                        MarketEntryAction::Option::Delete,
                                        MdEntryType::Option::SettlementPrice)));
}

TEST_F(InstrumentInfoCache, ChangesIndicativeToFinalOpeningPriceAtUncross) {
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::OpeningAuction,
                            Price{60.07},
                            Quantity{300},
                            Quantity{300},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  cache.update(make_update(
      AuctionIndicativeUpdate{},
      make_auction_cross(
          TradingPhase::Option::OpeningAuction, Price{30.07}, Quantity{200})));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(PriceQuantityEntryHas(Price{30.07},
                                        std::nullopt,
                                        MarketEntryAction::Option::Change,
                                        MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache, DeletesOpeningPriceWhenAuctionEndsUncrossed) {
  cache.update(make_update(
      make_indicative_no_cross(TradingPhase::Option::OpeningAuction)));
  cache.update(
      make_update(AuctionIndicativeUpdate{},
                  make_auction_no_cross(TradingPhase::Option::OpeningAuction)));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(PriceQuantityEntryHas(std::nullopt,
                                        Quantity{0},
                                        MarketEntryAction::Option::Delete,
                                        MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache, ComposesOpeningPriceChangedDuringAuctionCall) {
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::OpeningAuction,
                            Price{60.07},
                            Quantity{300},
                            Quantity{300},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  cache.update(make_update(InstrumentInfoRecover{
      .info = market_state::InstrumentInfo{.opening_price = Price{100}}}));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(PriceQuantityEntryHas(Price{100},
                                        std::nullopt,
                                        MarketEntryAction::Option::Change,
                                        MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache, ComposesRecoveredOpeningPriceWithoutQuantity) {
  cache.update(make_update(InstrumentInfoRecover{
      .info = market_state::InstrumentInfo{.opening_price = Price{100}}}));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(PriceQuantityEntryHas(Price{100},
                                        std::nullopt,
                                        NoAction,
                                        MdEntryType::Option::OpeningPrice)));
}

TEST_F(InstrumentInfoCache,
       StoresSettledOpeningPriceWhilePublishingIndicative) {
  cache.update(make_update(make_auction_cross(
      TradingPhase::Option::OpeningAuction, Price{100}, Quantity{10})));
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::OpeningAuction,
                            Price{105},
                            Quantity{20},
                            Quantity{5},
                            TradeCondition::Option::ImbalanceMoreBuyers)));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::opening_price,
                             Price{100})));
}

TEST_F(InstrumentInfoCache, KeepsOpeningPriceTimeWhilePublishingIndicative) {
  cache.update(make_update(InstrumentInfoRecover{
      .info = market_state::InstrumentInfo{
          .opening_price = Price{100}, .opening_price_time = yesterday()}}));
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::OpeningAuction,
                            Price{105},
                            Quantity{20},
                            Quantity{5},
                            TradeCondition::Option::ImbalanceMoreBuyers)));

  std::optional<market_state::InstrumentInfo> info;
  cache.store_state(info);

  ASSERT_THAT(info,
              Optional(Field(&market_state::InstrumentInfo::opening_price_time,
                             yesterday())));
}

TEST_F(InstrumentInfoCache,
       ReturnsSettledClosingPriceWhilePublishingIndicative) {
  cache.update(make_update(InstrumentInfoRecover{
      .info = market_state::InstrumentInfo{.closing_price = Price{120}}}));
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::ClosingAuction,
                            Price{130},
                            Quantity{20},
                            Quantity{5},
                            TradeCondition::Option::ImbalanceMoreBuyers)));

  ASSERT_EQ(cache.closing_price(), Price{120});
}

TEST_F(InstrumentInfoCache, RollsSettledClosingPriceIntoPreviousClosingPrice) {
  cache.update(make_update(InstrumentInfoRecover{
      .info = market_state::InstrumentInfo{.closing_price = Price{120}}}));
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::ClosingAuction,
                            Price{130},
                            Quantity{20},
                            Quantity{5},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  cache.update(make_update(
      AuctionIndicativeUpdate{},
      make_auction_cross(
          TradingPhase::Option::ClosingAuction, Price{140}, Quantity{10})));
  settings.enable_data_type_streaming(
      MdEntryType::Option::PreviousClosingPrice);

  cache.compose_initial(settings, entries);

  ASSERT_THAT(
      entries,
      ElementsAre(EntryHas(
          Price{120}, NoAction, MdEntryType::Option::PreviousClosingPrice)));
}

TEST_F(InstrumentInfoCache, MarksIndicativeChangedOnSubsequentUpdate) {
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::OpeningAuction,
                            Price{60.07},
                            Quantity{300},
                            Quantity{300},
                            TradeCondition::Option::ImbalanceMoreBuyers)));
  cache.update(make_update(
      make_indicative_cross(TradingPhase::Option::OpeningAuction,
                            Price{60.08},
                            Quantity{600},
                            Quantity{200},
                            TradeCondition::Option::ImbalanceMoreSellers)));
  settings.enable_data_type_streaming(MdEntryType::Option::OpeningPrice)
      .enable_data_type_streaming(MdEntryType::Option::Imbalance);

  cache.compose_update(settings, entries);

  ASSERT_THAT(
      entries,
      UnorderedElementsAre(
          PriceQuantityEntryHas(Price{60.08},
                                Quantity{600},
                                MarketEntryAction::Option::Change,
                                MdEntryType::Option::OpeningPrice),
          ImbalanceEntryHas(Quantity{200},
                            TradeCondition::Option::ImbalanceMoreSellers,
                            MarketEntryAction::Option::Change)));
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::mdata::test

// NOLINTEND(*magic-numbers*)
