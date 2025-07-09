#include <gmock/gmock.h>

#include "common/market_state/json/instrument_info.hpp"
#include "test_utils/matchers.hpp"

namespace simulator::trading_system::market_state::test {
namespace {

using namespace ::testing;

TEST(TradingSystemCommonInstrumentInfoFmt, FormatsToString) {
  const InstrumentInfo instrument_info{.low_price = Price{100.1},
                                       .high_price = Price{300.3}};
  ASSERT_EQ(fmt::to_string(instrument_info),
            R"({ "low_price": 100.1, "high_price": 300.3 })");
}

struct TradingSystemCommonInstrumentInfo : public ::testing::Test {
  rapidjson::Document doc;
  rapidjson::Value value;
};

TEST_F(TradingSystemCommonInstrumentInfo, ReadsFromJson) {
  value.SetObject();
  value.AddMember("low_price", 100.1, doc.GetAllocator());
  value.AddMember("high_price", 300.3, doc.GetAllocator());

  const auto instrument_info =
      core::json::Type<InstrumentInfo>::read_json_value(value);

  ASSERT_EQ(instrument_info.low_price, Price{100.1});
  ASSERT_EQ(instrument_info.high_price, Price{300.3});
}

TEST_F(TradingSystemCommonInstrumentInfo, WritesToJson) {
  using namespace simulator::trading_system::test;

  const InstrumentInfo instrument_info{.low_price = Price{100.1},
                                       .high_price = Price{300.3}};

  core::json::Type<InstrumentInfo>::write_json_value(
      value, doc.GetAllocator(), instrument_info);

  ASSERT_THAT(value, HasDouble("low_price", 100.1));
  ASSERT_THAT(value, HasDouble("high_price", 300.3));
}

}  // namespace
}  // namespace simulator::trading_system::market_state::test
