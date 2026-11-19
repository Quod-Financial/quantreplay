#include <gtest/gtest.h>

#include <stdexcept>

#include "core/domain/enumerators.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/market_phase.hpp"
#include "ih/marshalling/json/detail/enumeration_resolver.hpp"
#include "tests/test_utils/utils.hpp"

namespace simulator::http::json::test {
namespace {

struct HttpJsonEnumerationResolverDatasourceFormat : public ::testing::Test {
  using Format = data_layer::Datasource::Format;
};

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesEnumUndefined) {
  EXPECT_THROW((void)EnumerationResolver::resolve(
                   http::test::util::invalid_enum_value<Format>()),
               std::logic_error);
}

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesEnumCSV) {
  EXPECT_EQ(EnumerationResolver::resolve(Format::Csv), "CSV");
}

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesEnumPostgres) {
  EXPECT_EQ(EnumerationResolver::resolve(Format::Postgres), "PSQL");
}

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesEnumFIX) {
  EXPECT_EQ(EnumerationResolver::resolve(Format::Fix), "FIX");
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

TEST_F(HttpJsonEnumerationResolverDatasourceFormat, ResolvesStringFIX) {
  Format format{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("FIX", format));
  EXPECT_EQ(format, Format::Fix);
}

struct HttpJsonEnumerationResolverDatasourceType : public ::testing::Test {
  using Type = data_layer::Datasource::Type;
};

TEST_F(HttpJsonEnumerationResolverDatasourceType, ResolvesEnumUndefined) {
  EXPECT_THROW((void)EnumerationResolver::resolve(
                   http::test::util::invalid_enum_value<Type>()),
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
  EXPECT_THROW((void)EnumerationResolver::resolve(
                   http::test::util::invalid_enum_value<Phase>()),
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

struct HttpJsonEnumerationResolverListingEngineType : public ::testing::Test {
  using EngineType = data_layer::Listing::EngineType;
};

TEST_F(HttpJsonEnumerationResolverListingEngineType, ResolvesEnumUndefined) {
  EXPECT_THROW((void)EnumerationResolver::resolve(
                   http::test::util::invalid_enum_value<EngineType>()),
               std::logic_error);
}

TEST_F(HttpJsonEnumerationResolverListingEngineType, ResolvesEnumMatching) {
  EXPECT_EQ(EnumerationResolver::resolve(EngineType::Matching), "Matching");
}

TEST_F(HttpJsonEnumerationResolverListingEngineType, ResolvesEnumQuoting) {
  EXPECT_EQ(EnumerationResolver::resolve(EngineType::Quoting), "Quoting");
}

TEST_F(HttpJsonEnumerationResolverListingEngineType, ResolvesStringUndefined) {
  EngineType type{};
  EXPECT_THROW(EnumerationResolver::resolve("bad", type), std::runtime_error);
}

TEST_F(HttpJsonEnumerationResolverListingEngineType, ResolvesStringMatching) {
  EngineType type{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("Matching", type));
  EXPECT_EQ(type, EngineType::Matching);
}

TEST_F(HttpJsonEnumerationResolverListingEngineType, ResolvesStringQuoting) {
  EngineType type{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("Quoting", type));
  EXPECT_EQ(type, EngineType::Quoting);
}

struct HttpJsonEnumerationResolverSecurityType : public ::testing::Test {
  using SecurityType = core::enumerators::SecurityType;
};

TEST_F(HttpJsonEnumerationResolverSecurityType, ResolvesStringUndefined) {
  SecurityType type{};
  EXPECT_THROW(EnumerationResolver::resolve("bad", type), std::runtime_error);
}

TEST_F(HttpJsonEnumerationResolverSecurityType, ResolvesStringFxSpot) {
  SecurityType type{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("FXSPOT", type));
  EXPECT_EQ(type, SecurityType::FxSpot);
}

struct HttpJsonEnumerationResolverSecurityIdSource : public ::testing::Test {
  using SecurityIdSource = core::enumerators::SecurityIdSource;
};

TEST_F(HttpJsonEnumerationResolverSecurityIdSource, ResolvesStringUndefined) {
  SecurityIdSource source{};
  EXPECT_THROW(EnumerationResolver::resolve("ISI", source), std::runtime_error);
}

TEST_F(HttpJsonEnumerationResolverSecurityIdSource, ResolvesStringCusip) {
  SecurityIdSource source{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("CUSIP", source));
  EXPECT_EQ(source, SecurityIdSource::Cusip);
}

TEST_F(HttpJsonEnumerationResolverSecurityIdSource, ResolvesStringSedol) {
  SecurityIdSource source{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("SEDOL", source));
  EXPECT_EQ(source, SecurityIdSource::Sedol);
}

TEST_F(HttpJsonEnumerationResolverSecurityIdSource, ResolvesStringIsin) {
  SecurityIdSource source{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("ISIN", source));
  EXPECT_EQ(source, SecurityIdSource::Isin);
}

TEST_F(HttpJsonEnumerationResolverSecurityIdSource, ResolvesStringRic) {
  SecurityIdSource source{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("RIC", source));
  EXPECT_EQ(source, SecurityIdSource::Ric);
}

TEST_F(HttpJsonEnumerationResolverSecurityIdSource,
       ResolvesStringExchangeSymbol) {
  SecurityIdSource source{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("ExchangeSymbol", source));
  EXPECT_EQ(source, SecurityIdSource::ExchangeSymbol);
}

TEST_F(HttpJsonEnumerationResolverSecurityIdSource,
       ResolvesStringBloombergSymbol) {
  SecurityIdSource source{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("BloombergSymbol", source));
  EXPECT_EQ(source, SecurityIdSource::BloombergSymbol);
}

struct HttpJsonEnumerationResolverPartyRole : public ::testing::Test {
  using PartyRole = core::enumerators::PartyRole;
};

TEST_F(HttpJsonEnumerationResolverPartyRole, ResolvesStringUndefined) {
  PartyRole role{};
  EXPECT_THROW(EnumerationResolver::resolve("bad", role), std::runtime_error);
}

TEST_F(HttpJsonEnumerationResolverPartyRole, ResolvesStringExecutingFirm) {
  PartyRole role{};
  ASSERT_NO_THROW(EnumerationResolver::resolve("ExecutingFirm", role));
  EXPECT_EQ(role, PartyRole::ExecutingFirm);
}

}  // namespace
}  // namespace simulator::http::json::test
