#include <fmt/format.h>
#include <gmock/gmock.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <utility>

#include "core/domain/attributes.hpp"
#include "ih/market_data/best_prices_sink.hpp"
#include "ih/market_data/historical_random_price_source_registry.hpp"
#include "ih/market_data/price_cache.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "mocks/context/component_context.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct GeneratorHistoricalRandomPriceSourceRegistry : public Test {
  static constexpr DatasourceId SourceDatasourceId = 42;
  static constexpr DatasourceId OtherDatasourceId = 43;

  std::shared_ptr<NiceMock<mock::ComponentContext>> context =
      std::make_shared<NiceMock<mock::ComponentContext>>();
  std::shared_ptr<PriceCache> price_cache = std::make_shared<PriceCache>();
  HistoricalRandomPriceSourceRegistry registry{price_cache, context};

  GeneratorHistoricalRandomPriceSourceRegistry() {
    ON_CALL(*context, generate_identifier()).WillByDefault([this] {
      return fmt::format("MD-{}", ++generated_identifiers_);
    });
  }

  [[nodiscard]]
  static auto make_key(Symbol symbol = Symbol{"AAPL.OQ"},
                       DatasourceId datasource_id = SourceDatasourceId)
      -> RandomPriceSourceKey {
    return RandomPriceSourceKey{.datasource_id = datasource_id,
                                .symbol = std::move(symbol)};
  }

  auto publish_bid_price(const MdRequestId& request_id, double price) -> void {
    price_cache->process(
        request_id, BestPrices{.bid = Price{price}, .offer = std::nullopt});
  }

  [[nodiscard]]
  auto cached_bid_price(const MdRequestId& request_id) const
      -> std::optional<double> {
    return price_cache->market_state(request_id, {}).best_bid_price;
  }

 private:
  std::size_t generated_identifiers_ = 0;
};

TEST_F(GeneratorHistoricalRandomPriceSourceRegistry,
       ReturnsGeneratedIdentifierAsRequestId) {
  EXPECT_CALL(*context, generate_identifier()).WillOnce(Return("MD-42"));

  ASSERT_THAT(registry.add(make_key()), Eq(MdRequestId{"MD-42"}));
}

TEST_F(GeneratorHistoricalRandomPriceSourceRegistry,
       RegistersRequestIdInPriceCache) {
  const MdRequestId request_id = registry.add(make_key());

  publish_bid_price(request_id, 100.);

  ASSERT_THAT(cached_bid_price(request_id), Optional(DoubleEq(100.)));
}

TEST_F(GeneratorHistoricalRandomPriceSourceRegistry,
       ReturnsSameRequestIdForRepeatedSource) {
  const MdRequestId request_id = registry.add(make_key());

  ASSERT_THAT(registry.add(make_key()), Eq(request_id));
}

TEST_F(GeneratorHistoricalRandomPriceSourceRegistry,
       ReturnsDistinctRequestIdsForSymbolsOfSameDatasource) {
  const MdRequestId request_id = registry.add(make_key(Symbol{"AAPL.OQ"}));

  ASSERT_THAT(registry.add(make_key(Symbol{"MSFT.OQ"})), Ne(request_id));
}

TEST_F(GeneratorHistoricalRandomPriceSourceRegistry,
       ReturnsDistinctRequestIdsForSameSymbolOfDifferentDatasources) {
  const MdRequestId request_id = registry.add(make_key());

  ASSERT_THAT(registry.add(make_key(Symbol{"AAPL.OQ"}, OtherDatasourceId)),
              Ne(request_id));
}

TEST_F(GeneratorHistoricalRandomPriceSourceRegistry,
       ReportsNoSymbolsOfUnknownDatasource) {
  registry.add(make_key());

  ASSERT_THAT(registry.symbols_of(OtherDatasourceId), IsEmpty());
}

TEST_F(GeneratorHistoricalRandomPriceSourceRegistry,
       ReportsSymbolsOfRequestedDatasource) {
  const MdRequestId request_id = registry.add(make_key());
  const MdRequestId other_request_id =
      registry.add(make_key(Symbol{"MSFT.OQ"}));
  registry.add(make_key(Symbol{"AAPL.N"}, OtherDatasourceId));

  ASSERT_THAT(registry.symbols_of(SourceDatasourceId),
              UnorderedElementsAre(Pair("AAPL.OQ", Eq(request_id)),
                                   Pair("MSFT.OQ", Eq(other_request_id))));
}

TEST_F(GeneratorHistoricalRandomPriceSourceRegistry,
       ClearsCachedPricesOfEverySourceOnReset) {
  const MdRequestId request_id = registry.add(make_key());
  const MdRequestId other_request_id =
      registry.add(make_key(Symbol{"AAPL.N"}, OtherDatasourceId));
  publish_bid_price(request_id, 100.);
  publish_bid_price(other_request_id, 101.);

  registry.reset();

  EXPECT_THAT(cached_bid_price(request_id), Eq(std::nullopt));
  EXPECT_THAT(cached_bid_price(other_request_id), Eq(std::nullopt));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
