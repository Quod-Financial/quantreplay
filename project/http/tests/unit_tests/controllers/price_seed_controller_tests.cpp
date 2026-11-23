#include <gmock/gmock.h>

#include <memory>
#include <tl/expected.hpp>
#include <utility>

#include "data_layer/api/models/price_seed.hpp"
#include "ih/controllers/price_seed_controller.hpp"
#include "ih/data_bridge/operation_failure.hpp"
#include "ih/marshalling/json/price_seed.hpp"
#include "tests/mocks/price_seed_accessor.hpp"
#include "tests/mocks/setting_accessor.hpp"

namespace simulator::http::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct HttpPriceSeedController : public Test {
  using Code = Pistache::Http::Code;
  using PriceSeed = data_layer::PriceSeed;

  HttpPriceSeedController() {
    auto accessor = std::make_unique<NiceMock<mock::PriceSeedAccessor>>();
    seeds = accessor.get();
    controller = std::make_unique<PriceSeedController>(
        std::move(accessor),
        std::make_shared<NiceMock<mock::SettingAccessor>>());
  }

  static auto make_stored_price_seed() -> PriceSeed {
    return PriceSeed::create(PriceSeed::Patch{}
                                 .with_symbol("AAPL")
                                 .with_security_type("CS")
                                 .with_price_currency("USD")
                                 .with_mid_price(100.5)
                                 .with_last_update("2026-11-19 10:00:00"),
                             42);
  }

  NiceMock<mock::PriceSeedAccessor>* seeds = nullptr;
  std::unique_ptr<PriceSeedController> controller;
};

TEST_F(HttpPriceSeedController, ReportsDataIntegrityViolationOnInsert) {
  EXPECT_CALL(*seeds, add)
      .WillOnce(Return(
          tl::unexpected{data_bridge::Failure::DataIntegrityViolationError}));

  const auto [code, content] =
      controller->insert_price_seed(R"({"symbol":"AAPL"})");

  EXPECT_EQ(code, Code::Bad_Request);
  EXPECT_EQ(
      content,
      R"({"result":"Requested operation violates data integrity constraints"})");
}

TEST_F(HttpPriceSeedController, RepliesCreatedWithStoredPriceSeedOnInsert) {
  const auto stored = make_stored_price_seed();
  EXPECT_CALL(*seeds, add).WillOnce(Return(stored));

  const auto [code, content] =
      controller->insert_price_seed(R"({"symbol":"AAPL"})");

  EXPECT_EQ(code, Code::Created);
  EXPECT_EQ(content, json::PriceSeedMarshaller::marshall(stored));
}

TEST_F(HttpPriceSeedController, ReportsPriceSeedNotFoundOnUpdate) {
  EXPECT_CALL(*seeds, update)
      .WillOnce(Return(
          tl::unexpected{data_bridge::Failure::ResponseCardinalityError}));

  const auto [code, content] =
      controller->update_price_seed(42, R"({"midPrice":100.5})");

  EXPECT_EQ(code, Code::Not_Found);
  EXPECT_EQ(
      content,
      R"({"result":"Can not resolve a single PriceSeed by a given key"})");
}

TEST_F(HttpPriceSeedController,
       RepliesOkWithAllFieldsOfStoredPriceSeedOnUpdate) {
  const auto stored = make_stored_price_seed();
  EXPECT_CALL(*seeds, update).WillOnce(Return(stored));

  const auto [code, content] =
      controller->update_price_seed(42, R"({"midPrice":100.5})");

  EXPECT_EQ(code, Code::Ok);
  EXPECT_EQ(content, json::PriceSeedMarshaller::marshall(stored));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::http::test
