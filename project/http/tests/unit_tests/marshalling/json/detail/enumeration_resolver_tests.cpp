#include <gtest/gtest.h>

#include <stdexcept>

#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/market_phase.hpp"
#include "data_layer/api/models/venue.hpp"
#include "ih/marshalling/json/detail/enumeration_resolver.hpp"

namespace simulator::http::json::test {
namespace {

struct HttpJsonEnumerationResolverDatasourceFormat : public ::testing::Test {
  using Format = data_layer::Datasource::Format;
};

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesEnumUndefined) {
  EXPECT_THROW((void)EnumerationResolver::resolve(static_cast<Format>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesEnumCSV) {
  EXPECT_EQ(EnumerationResolver::resolve(Format::Csv), "CSV");
}

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesEnumPostgres) {
  EXPECT_EQ(EnumerationResolver::resolve(Format::Postgres), "PSQL");
}

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesStringUndefined) {
  Format format{};
  EXPECT_THROW(EnumerationResolver::resolve("bad", format), std::runtime_error);
}

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesStringCSV) {
  Format format{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("CSV", format));
  EXPECT_EQ(format, Format::Csv);
}

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesStringPostgres) {
  Format format{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("PSQL", format));
  EXPECT_EQ(format, Format::Postgres);
}

struct HttpJsonEnumerationResolverDatasourceType : public ::testing::Test {
  using Type = data_layer::Datasource::Type;
};

TEST_F(HttpJsonEnumerationResolverDatasourceType, ResolvesEnumUndefined) {
  EXPECT_THROW((void)EnumerationResolver::resolve(static_cast<Type>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonEnumerationResolverDatasourceType, ResolvesEnumOrderBook) {
  EXPECT_EQ(EnumerationResolver::resolve(Type::OrderBook), "OrderBook");
}

TEST_F(HttpJsonEnumerationResolverDatasourceType, ResolvesStringUndefined) {
  Type type{};
  EXPECT_THROW(EnumerationResolver::resolve("bad", type), std::runtime_error);
}

TEST_F(HttpJsonEnumerationResolverDatasourceType, ResolvesStringOrderBook) {
  Type type{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("OrderBook", type));
  EXPECT_EQ(type, Type::OrderBook);
}

struct HttpJsonEnumerationResolverMarketPhaseType : public ::testing::Test {
  using Phase = data_layer::MarketPhase::Phase;
};

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesEnumUndefined) {
  EXPECT_THROW((void)EnumerationResolver::resolve(static_cast<Phase>(-1)),
               std::logic_error);
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesEnumOpen) {
  EXPECT_EQ(EnumerationResolver::resolve(Phase::Open), "Open");
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesEnumClosed) {
  EXPECT_EQ(EnumerationResolver::resolve(Phase::Closed), "Closed");
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesEnumOpeningAuction) {
  EXPECT_EQ(EnumerationResolver::resolve(Phase::OpeningAuction), "PreOpen");
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesEnumClosingAuction) {
  EXPECT_EQ(EnumerationResolver::resolve(Phase::ClosingAuction), "PreClose");
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType,
       ResolvesEnumIntradayAuction) {
  EXPECT_EQ(EnumerationResolver::resolve(Phase::IntradayAuction), "Auction");
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesEnumHalted) {
  EXPECT_EQ(EnumerationResolver::resolve(Phase::Halted), "Halted");
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesStringUndefined) {
  Phase phase{};
  EXPECT_THROW(EnumerationResolver::resolve("bad", phase), std::runtime_error);
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesStringOpen) {
  Phase phase{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("Open", phase));
  EXPECT_EQ(phase, Phase::Open);
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesStringClosed) {
  Phase phase{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("Closed", phase));
  EXPECT_EQ(phase, Phase::Closed);
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType,
       ResolvesStringOpeningAuction) {
  Phase phase{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("PreOpen", phase));
  EXPECT_EQ(phase, Phase::OpeningAuction);
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType,
       ResolvesStringClosingAuction) {
  Phase phase{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("PreClose", phase));
  EXPECT_EQ(phase, Phase::ClosingAuction);
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType,
       ResolvesStringIntradayAuction) {
  Phase phase{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("Auction", phase));
  EXPECT_EQ(phase, Phase::IntradayAuction);
}

TEST_F(HttpJsonEnumerationResolverMarketPhaseType, ResolvesStringHalted) {
  Phase phase{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("Halted", phase));
  EXPECT_EQ(phase, Phase::Halted);
}

struct HttJsonEnumerationResolverVenueEngineType : public ::testing::Test {
  using EngineType = data_layer::Venue::EngineType;
};

TEST_F(HttJsonEnumerationResolverVenueEngineType, ResolvesEnumUndefined) {
  EXPECT_THROW((void)EnumerationResolver::resolve(static_cast<EngineType>(-1)),
               std::logic_error);
}

TEST_F(HttJsonEnumerationResolverVenueEngineType, ResolvesEnumMatching) {
  EXPECT_EQ(EnumerationResolver::resolve(EngineType::Matching), "Matching");
}

TEST_F(HttJsonEnumerationResolverVenueEngineType, ResolvesEnumQuoting) {
  EXPECT_EQ(EnumerationResolver::resolve(EngineType::Quoting), "Quoting");
}

TEST_F(HttJsonEnumerationResolverVenueEngineType, ResolvesStringUndefined) {
  EngineType type{};
  EXPECT_THROW(EnumerationResolver::resolve("bad", type), std::runtime_error);
}

TEST_F(HttJsonEnumerationResolverVenueEngineType, ResolvesStringMatching) {
  EngineType type{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("Matching", type));
  EXPECT_EQ(type, EngineType::Matching);
}

TEST_F(HttJsonEnumerationResolverVenueEngineType, ResolvesStringQuoting) {
  EngineType type{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("Quoting", type));
  EXPECT_EQ(type, EngineType::Quoting);
}

}  // namespace
}  // namespace simulator::http::json::test
