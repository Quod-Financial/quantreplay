#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "data_layer/api/models/market_phase.hpp"
#include "ies/phase_record.hpp"
#include "ih/config/phase_entry_reader.hpp"

namespace simulator::trading_system::test {
namespace {

using namespace testing;  // NOLINT
using namespace std::chrono_literals;

struct TradingSystemPhaseEntryReader : public Test {
  using MarketPhase = data_layer::MarketPhase;

  static auto create_phase(MarketPhase::Patch patch) -> MarketPhase {
    return MarketPhase::create(std::move(patch), "venue_id");
  }

  Config config;
  PhaseEntryReader reader{config};

  MarketPhase::Patch patch;
};

TEST_F(TradingSystemPhaseEntryReader,
       DoesNotAddMarketPhaseIfStartTimeIsMalformed) {
  patch.with_start_time("11h").with_end_time("12:00").with_phase(
      MarketPhase::Phase::Closed);
  reader(create_phase(patch));
  ASSERT_TRUE(config.trading_phases_schedule().phase_records().empty());
}

TEST_F(TradingSystemPhaseEntryReader,
       DoesNotAddMarketPhaseIfEndTimeIsMalformed) {
  patch.with_start_time("11:00").with_end_time("12h").with_phase(
      MarketPhase::Phase::Closed);
  reader(create_phase(patch));
  ASSERT_TRUE(config.trading_phases_schedule().phase_records().empty());
}

TEST_F(TradingSystemPhaseEntryReader, DoesNotAddMarketPhaseIfPhaseIsMalformed) {
  patch.with_start_time("11:00").with_end_time("12:00").with_phase(
      static_cast<MarketPhase::Phase>(-1));
  reader(create_phase(patch));
  ASSERT_TRUE(config.trading_phases_schedule().phase_records().empty());
}

TEST_F(TradingSystemPhaseEntryReader,
       AddsMarketPhaseWithStartTimeEndTimeAndPhase) {
  patch.with_start_time("11:00").with_end_time("12:00").with_phase(
      MarketPhase::Phase::Closed);
  reader(create_phase(patch));
  ASSERT_THAT(
      config.trading_phases_schedule().phase_records(),
      ElementsAre(ies::PhaseRecord{
          .begin = 11h, .end = 12h, .phase = TradingPhase::Option::Closed}));
}

TEST_F(TradingSystemPhaseEntryReader, SetsEndRangeToZeroSecondIfNotSet) {
  patch.with_start_time("11:00").with_end_time("12:00").with_phase(
      MarketPhase::Phase::Closed);
  reader(create_phase(patch));
  ASSERT_EQ(config.trading_phases_schedule().phase_records()[0].end_range, 0s);
}

TEST_F(TradingSystemPhaseEntryReader, SetsEndRange) {
  patch.with_start_time("11:00")
      .with_end_time("12:00")
      .with_phase(MarketPhase::Phase::Closed)
      .with_end_time_range(30);
  reader(create_phase(patch));
  ASSERT_EQ(config.trading_phases_schedule().phase_records()[0].end_range, 30s);
}

TEST_F(TradingSystemPhaseEntryReader, SetsAllowCancelsToFalseIfNotSet) {
  patch.with_start_time("11:00").with_end_time("12:00").with_phase(
      MarketPhase::Phase::Closed);
  reader(create_phase(patch));
  ASSERT_FALSE(config.trading_phases_schedule()
                   .phase_records()[0]
                   .allow_cancels_on_halt);
}

TEST_F(TradingSystemPhaseEntryReader, SetsAllowCancels) {
  patch.with_start_time("11:00")
      .with_end_time("12:00")
      .with_phase(MarketPhase::Phase::Closed)
      .with_allow_cancels(true);
  reader(create_phase(patch));
  ASSERT_TRUE(config.trading_phases_schedule()
                  .phase_records()[0]
                  .allow_cancels_on_halt);
}

TEST_F(TradingSystemPhaseEntryReader,
       DoesNotAddMarketPhaseIfStartTimeIsEqualToEndTime) {
  patch.with_start_time("11:00").with_end_time("11:00").with_phase(
      MarketPhase::Phase::Closed);
  reader(create_phase(patch));
  ASSERT_TRUE(config.trading_phases_schedule().phase_records().empty());
}

TEST_F(TradingSystemPhaseEntryReader,
       DoesNotAddMarketPhaseIfStartTimeIsGreaterThanEndTime) {
  patch.with_start_time("12:00").with_end_time("11:00").with_phase(
      MarketPhase::Phase::Closed);
  reader(create_phase(patch));
  ASSERT_TRUE(config.trading_phases_schedule().phase_records().empty());
}

TEST_F(TradingSystemPhaseEntryReader,
       DoesNotAddMarketPhaseIfStartTimeIsGreaterThanHoursInADay) {
  patch.with_start_time("25:00").with_end_time("48:00").with_phase(
      MarketPhase::Phase::Closed);
  reader(create_phase(patch));
  ASSERT_TRUE(config.trading_phases_schedule().phase_records().empty());
}

TEST_F(TradingSystemPhaseEntryReader,
       DoesNotAddMarketPhaseIfEndTimeIsGreaterThanHoursInADay) {
  patch.with_start_time("10:00").with_end_time("48:00").with_phase(
      MarketPhase::Phase::Closed);
  reader(create_phase(patch));
  ASSERT_TRUE(config.trading_phases_schedule().phase_records().empty());
}

}  // namespace
}  // namespace simulator::trading_system::test