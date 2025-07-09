#include "common/phase.hpp"

#include <gmock/gmock.h>

namespace simulator::trading_system::ies {
namespace {

using namespace ::testing;

TEST(TradingSystemPhase, SetsPhase) {
  const Phase phase{TradingPhase::Option::Open,
                    TradingStatus::Option::Resume,
                    {.allow_cancels = true}};
  ASSERT_EQ(phase.phase(), TradingPhase::Option::Open);
}

TEST(TradingSystemPhase, SetsStatus) {
  const Phase phase{TradingPhase::Option::Open,
                    TradingStatus::Option::Resume,
                    {.allow_cancels = true}};
  ASSERT_EQ(phase.status(), TradingStatus::Option::Resume);
}

TEST(TradingSystemPhase, SetsHaltStatusOnClosedPhase) {
  const Phase phase{TradingPhase::Option::Closed,
                    TradingStatus::Option::Resume,
                    {.allow_cancels = true}};
  ASSERT_EQ(phase.status(), TradingStatus::Option::Halt);
}

TEST(TradingSystemPhase, DoesNotSetSettingOnClosedPhase) {
  const Phase phase{TradingPhase::Option::Closed,
                    TradingStatus::Option::Halt,
                    {.allow_cancels = false}};
  ASSERT_EQ(phase.settings(), std::nullopt);
}

TEST(TradingSystemPhase, DoesNotSetSettingOnResumeStatus) {
  const Phase phase{TradingPhase::Option::Open,
                    TradingStatus::Option::Resume,
                    {.allow_cancels = true}};
  ASSERT_EQ(phase.settings(), std::nullopt);
}

TEST(TradingSystemPhase, SetsSettingOnHaltStatus) {
  const Phase phase{TradingPhase::Option::Open,
                    TradingStatus::Option::Halt,
                    {.allow_cancels = true}};
  ASSERT_THAT(phase.settings(),
              Optional(Field(&Phase::Settings::allow_cancels, Eq(true))));
}

}  // namespace
}  // namespace simulator::trading_system::ies