#include <gmock/gmock.h>

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/datasource_listing.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/listing_random_price_source.hpp"
#include "ih/market_data/utils.hpp"

namespace simulator::generator::mdata::test {
namespace {

using namespace testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

TEST(GeneratorMarketDataSessionParsing, ParsesBeginString) {
  const auto session = parse_fix_session("FIXT.1.1:SENDER->TARGET");

  ASSERT_TRUE(session.has_value());
  ASSERT_THAT(session->begin_string,
              Eq(protocol::fix::BeginString{"FIXT.1.1"}));
}

TEST(GeneratorMarketDataSessionParsing, ParsesSenderCompId) {
  const auto session = parse_fix_session("FIXT.1.1:SENDER->TARGET");

  ASSERT_TRUE(session.has_value());
  ASSERT_THAT(session->sender_comp_id,
              Eq(protocol::fix::SenderCompId{"SENDER"}));
}

TEST(GeneratorMarketDataSessionParsing, ParsesTargetCompId) {
  const auto session = parse_fix_session("FIXT.1.1:SENDER->TARGET");

  ASSERT_TRUE(session.has_value());
  ASSERT_THAT(session->target_comp_id,
              Eq(protocol::fix::TargetCompId{"TARGET"}));
}

TEST(GeneratorMarketDataSessionParsing,
     ParsesNoSessionQualifierWhenItIsAbsent) {
  const auto session = parse_fix_session("FIXT.1.1:SENDER->TARGET");

  ASSERT_TRUE(session.has_value());
  ASSERT_THAT(session->session_qualifier, Eq(std::nullopt));
}

TEST(GeneratorMarketDataSessionParsing, ParsesSessionQualifier) {
  const auto session = parse_fix_session("FIXT.1.1:SENDER->TARGET:QUALIFIER");

  ASSERT_TRUE(session.has_value());
  ASSERT_THAT(session->session_qualifier,
              Optional(Eq(protocol::fix::SessionQualifier{"QUALIFIER"})));
}

struct GeneratorMarketDataDatasourceRequirementBuilder : public Test {
  using Datasource = data_layer::Datasource;
  using DatasourceListing = data_layer::DatasourceListing;
  using Listing = data_layer::Listing;
  using ListingRandomPriceSource = data_layer::ListingRandomPriceSource;

  static auto make_datasource(Datasource::Patch patch,
                              std::uint64_t datasource_id = 42) -> Datasource {
    patch.with_name("datasource")
        .with_type(Datasource::Type::OrderBook)
        .with_enabled_flag(true);
    if (!patch.venue_id().has_value()) {
      patch.with_venue_id("LSE");
    }
    if (!patch.connection().has_value()) {
      patch.with_connection("FIXT.1.1:SENDER->TARGET");
    }
    if (!patch.format().has_value()) {
      patch.with_format(Datasource::Format::Fix);
    }
    return Datasource::create(std::move(patch), datasource_id);
  }

  static auto with_symbols(Datasource::Patch patch,
                           std::initializer_list<std::string> symbols)
      -> Datasource::Patch {
    for (const std::string& symbol : symbols) {
      DatasourceListing::Patch listing;
      listing.with_symbol(symbol);
      patch.with_listing(std::move(listing));
    }
    return patch;
  }

  static auto with_random_price_sources(
      Listing::Patch patch,
      std::initializer_list<std::pair<std::uint64_t, std::string>> sources)
      -> Listing::Patch {
    for (const auto& [datasource_id, symbol] : sources) {
      ListingRandomPriceSource::Patch source;
      source.with_datasource_id(datasource_id).with_symbol(symbol);
      patch.with_random_price_source(std::move(source));
    }
    return patch;
  }

  static auto make_fix_datasource() -> Datasource {
    return make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"}));
  }

  static auto make_session(std::string target_comp_id) -> protocol::Session {
    return protocol::Session{protocol::fix::Session{
        protocol::fix::BeginString{"FIXT.1.1"},
        protocol::fix::SenderCompId{"SENDER"},
        protocol::fix::TargetCompId{std::move(target_comp_id)}}};
  }

  static auto make_listing(Listing::Patch patch, std::uint64_t listing_id = 1)
      -> Listing {
    if (!patch.venue_id().has_value()) {
      patch.with_venue_id("LSE");
    }
    return Listing::create(std::move(patch), listing_id);
  }
};

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       IgnoresNonFixDatasource) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_format(Datasource::Format::Csv));

  ASSERT_THAT(make_datasource_subscription_requirements({datasource}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       BuildsRequirementForDatasourceUsedOnlyForRandomPrices) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_random_price_only_flag(true));

  ASSERT_THAT(make_datasource_subscription_requirements({datasource}),
              SizeIs(1));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       BuildsRequirementForDatasourceNotUsedOnlyForRandomPrices) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_random_price_only_flag(false));

  ASSERT_THAT(make_datasource_subscription_requirements({datasource}),
              SizeIs(1));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       TakesTopOfBookDepthWhenDatasourceIsUsedOnlyForRandomPrices) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_random_price_only_flag(true)
                          .with_max_depth_levels(5));

  const auto requirements =
      make_datasource_subscription_requirements({datasource});

  ASSERT_THAT(requirements, SizeIs(1));
  ASSERT_THAT(requirements.front().depth, Eq(TopOfTheBookDepth));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       IgnoresUnparsableConnection) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_connection("host:12000"));

  ASSERT_THAT(make_datasource_subscription_requirements({datasource}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       IgnoresDatasourceWithoutListings) {
  const auto datasource = make_datasource(Datasource::Patch{});

  ASSERT_THAT(make_datasource_subscription_requirements({datasource}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       IgnoresDatasourceWithEmptySymbol) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {""}));

  ASSERT_THAT(make_datasource_subscription_requirements({datasource}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       TakesSymbolFromDatasourceListing) {
  const auto requirements =
      make_datasource_subscription_requirements({make_fix_datasource()});

  ASSERT_THAT(requirements, SizeIs(1));
  ASSERT_THAT(requirements.front().symbol, Eq(Symbol{"AAPL"}));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       TakesSessionFromConnection) {
  const auto requirements =
      make_datasource_subscription_requirements({make_fix_datasource()});

  ASSERT_THAT(requirements, SizeIs(1));
  ASSERT_THAT(requirements.front().session,
              Eq(protocol::Session{protocol::fix::Session{
                  protocol::fix::BeginString{"FIXT.1.1"},
                  protocol::fix::SenderCompId{"SENDER"},
                  protocol::fix::TargetCompId{"TARGET"}}}));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       TakesDepthFromMaxDepthLevels) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_random_price_only_flag(false)
                          .with_max_depth_levels(5));

  const auto requirements =
      make_datasource_subscription_requirements({datasource});

  ASSERT_THAT(requirements, SizeIs(1));
  ASSERT_THAT(requirements.front().depth, Eq(MarketDepth{5}));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       BuildsRequirementForEverySymbol) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL", "MSFT"}));

  const auto requirements =
      make_datasource_subscription_requirements({datasource});

  ASSERT_THAT(requirements, SizeIs(2));
  EXPECT_THAT(requirements.front().symbol, Eq(Symbol{"AAPL"}));
  EXPECT_THAT(requirements.back().symbol, Eq(Symbol{"MSFT"}));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder, IgnoresEmptySymbol) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"", "AAPL"}));

  const auto requirements =
      make_datasource_subscription_requirements({datasource});

  ASSERT_THAT(requirements, SizeIs(1));
  EXPECT_THAT(requirements.front().symbol, Eq(Symbol{"AAPL"}));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       UsesAllDepthLevelsWhenMaxDepthLevelsIsAbsent) {
  const auto requirements =
      make_datasource_subscription_requirements({make_fix_datasource()});

  ASSERT_THAT(requirements, SizeIs(1));
  ASSERT_THAT(requirements.front().depth, Eq(AllDepthLevels));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       BuildsRequirementsForEveryDatasource) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"}));
  const auto other_datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"MSFT"})
                          .with_connection("FIXT.1.1:SENDER->OTHER"));

  const auto requirements =
      make_datasource_subscription_requirements({datasource, other_datasource});

  ASSERT_THAT(requirements, SizeIs(2));
  EXPECT_THAT(requirements.front().symbol, Eq(Symbol{"AAPL"}));
  EXPECT_THAT(requirements.front().session, Eq(make_session("TARGET")));
  EXPECT_THAT(requirements.back().symbol, Eq(Symbol{"MSFT"}));
  EXPECT_THAT(requirements.back().session, Eq(make_session("OTHER")));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       BuildsRequirementPerDatasourceForSameSymbolOnSameConnection) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_random_price_only_flag(true));
  const auto other_datasource = make_datasource(
      with_symbols(Datasource::Patch{}, {"AAPL"}).with_max_depth_levels(5));

  const auto requirements =
      make_datasource_subscription_requirements({datasource, other_datasource});

  ASSERT_THAT(requirements, SizeIs(2));
  EXPECT_THAT(requirements.front().depth, Eq(TopOfTheBookDepth));
  EXPECT_THAT(requirements.back().depth, Eq(MarketDepth{5}));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       BuildsRequirementsOnlyForFixDatasources) {
  const auto csv_datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"MSFT"})
                          .with_format(Datasource::Format::Csv));

  const auto requirements = make_datasource_subscription_requirements(
      {csv_datasource, make_fix_datasource()});

  ASSERT_THAT(requirements, SizeIs(1));
  EXPECT_THAT(requirements.front().symbol, Eq(Symbol{"AAPL"}));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       SelectsNoOrderDerivationRequirementForRandomPriceOnlyDatasource) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_random_price_only_flag(true));

  ASSERT_THAT(make_order_derivation_requirements({datasource}), IsEmpty());
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       SelectsNoOrderDerivationRequirementForNonFixDatasource) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_format(Datasource::Format::Csv)
                          .with_random_price_only_flag(false));

  ASSERT_THAT(make_order_derivation_requirements({datasource}), IsEmpty());
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       SelectsOrderDerivationRequirementsOfOrderDerivingDatasources) {
  const auto price_only_datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"})
                          .with_random_price_only_flag(true));
  const auto deriving_datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"MSFT"}));

  const auto requirements = make_order_derivation_requirements(
      {price_only_datasource, deriving_datasource});

  ASSERT_THAT(requirements, SizeIs(1));
  EXPECT_THAT(requirements.front().symbol, Eq(Symbol{"MSFT"}));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       TakesOrderDerivationDepthFromMaxDepthLevels) {
  const auto datasource = make_datasource(
      with_symbols(Datasource::Patch{}, {"AAPL"}).with_max_depth_levels(5));

  const auto requirements = make_order_derivation_requirements({datasource});

  ASSERT_THAT(requirements, SizeIs(1));
  ASSERT_THAT(requirements.front().depth, Eq(MarketDepth{5}));
}

TEST_F(GeneratorMarketDataDatasourceRequirementBuilder,
       NamesOrderDerivationCounterpartyAfterDatasourceId) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL"}), 7);

  const auto requirements = make_order_derivation_requirements({datasource});

  ASSERT_THAT(requirements, SizeIs(1));
  EXPECT_THAT(requirements.front().counterparty, Eq(PartyId{"DS7"}));
}

struct GeneratorMarketDataListingPriceSourceBuilder
    : public GeneratorMarketDataDatasourceRequirementBuilder {
  static constexpr std::uint64_t DatasourceId = 42;
  static constexpr std::uint64_t OtherDatasourceId = 43;
  static constexpr ListingId PricedListingId = 1;
  static constexpr ListingId OtherPricedListingId = 2;

  // Selects every listing for random order generation.
  [[nodiscard]]
  static auto make_all_listing_price_sources(
      const std::vector<Datasource>& datasources,
      const std::vector<Listing>& listings)
      -> std::map<ListingId, std::vector<SubscriptionKey>> {
    std::unordered_set<ListingId> random_generation_listings;
    for (const Listing& listing : listings) {
      random_generation_listings.emplace(listing.listing_id());
    }
    return make_listing_random_price_sources(
        datasources, listings, random_generation_listings);
  }

  static auto make_datasource_priced_listing(Listing::Patch patch,
                                             std::uint64_t listing_id = 1)
      -> Listing {
    if (!patch.symbol().has_value()) {
      patch.with_symbol("AAPL");
    }
    if (!patch.random_price_sources().has_value()) {
      patch = with_random_price_sources(std::move(patch),
                                        {{DatasourceId, "AAPL.OQ"}});
    }
    return make_listing(std::move(patch), listing_id);
  }

  static auto make_datasource_priced_listing() -> Listing {
    return make_datasource_priced_listing(Listing::Patch{});
  }

  static auto make_price_source_datasource() -> Datasource {
    return make_datasource(with_symbols(Datasource::Patch{}, {"AAPL.OQ"})
                               .with_random_price_only_flag(true));
  }
};

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       IgnoresListingNotGeneratingRandomOrders) {
  ASSERT_THAT(
      make_listing_random_price_sources({make_price_source_datasource()},
                                        {make_datasource_priced_listing()},
                                        {}),
      IsEmpty());
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       IgnoresListingWithoutRandomPriceSources) {
  const auto listing = make_datasource_priced_listing(
      Listing::Patch{}.without_random_price_sources());

  ASSERT_THAT(make_all_listing_price_sources({make_price_source_datasource()},
                                             {listing}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder, IgnoresUnknownDatasource) {
  const auto listing = make_datasource_priced_listing(with_random_price_sources(
      Listing::Patch{}, {{OtherDatasourceId, "AAPL.OQ"}}));

  ASSERT_THAT(make_all_listing_price_sources({make_price_source_datasource()},
                                             {listing}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder, IgnoresNonFixDatasource) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL.OQ"})
                          .with_format(Datasource::Format::Csv));

  ASSERT_THAT(make_all_listing_price_sources(
                  {datasource}, {make_datasource_priced_listing()}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       IgnoresUnparsableConnection) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL.OQ"})
                          .with_connection("host:12000"));

  ASSERT_THAT(make_all_listing_price_sources(
                  {datasource}, {make_datasource_priced_listing()}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder, IgnoresEmptySymbol) {
  const auto listing = make_datasource_priced_listing(
      with_random_price_sources(Listing::Patch{}, {{DatasourceId, ""}}));

  ASSERT_THAT(make_all_listing_price_sources({make_price_source_datasource()},
                                             {listing}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       IgnoresSymbolTheDatasourceDoesNotSubscribeTo) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"MSFT.OQ"}));

  ASSERT_THAT(make_all_listing_price_sources(
                  {datasource}, {make_datasource_priced_listing()}),
              IsEmpty());
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       KeysPriceSourcesByListingId) {
  const auto price_sources = make_all_listing_price_sources(
      {make_price_source_datasource()}, {make_datasource_priced_listing()});

  ASSERT_THAT(price_sources, UnorderedElementsAre(Pair(PricedListingId, _)));
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       TakesSymbolFromRandomPriceSource) {
  const auto price_sources = make_all_listing_price_sources(
      {make_price_source_datasource()}, {make_datasource_priced_listing()});

  ASSERT_THAT(price_sources, SizeIs(1));
  ASSERT_THAT(
      price_sources.begin()->second,
      ElementsAre(Field(&SubscriptionKey::symbol, Eq(Symbol{"AAPL.OQ"}))));
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       TakesSessionFromDatasourceConnection) {
  const auto price_sources = make_all_listing_price_sources(
      {make_price_source_datasource()}, {make_datasource_priced_listing()});

  ASSERT_THAT(price_sources, SizeIs(1));
  ASSERT_THAT(price_sources.begin()->second,
              ElementsAre(Field(&SubscriptionKey::session,
                                Eq(make_session("TARGET")))));
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       TakesEveryRandomPriceSourceOfListing) {
  const auto other_datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL.N"})
                          .with_connection("FIXT.1.1:SENDER->OTHER"),
                      OtherDatasourceId);
  const auto listing = make_datasource_priced_listing(with_random_price_sources(
      Listing::Patch{},
      {{DatasourceId, "AAPL.OQ"}, {OtherDatasourceId, "AAPL.N"}}));

  const auto price_sources = make_all_listing_price_sources(
      {make_price_source_datasource(), other_datasource}, {listing});

  ASSERT_THAT(price_sources, SizeIs(1));
  EXPECT_THAT(
      price_sources.begin()->second,
      ElementsAre(
          AllOf(Field(&SubscriptionKey::symbol, Eq(Symbol{"AAPL.OQ"})),
                Field(&SubscriptionKey::session, Eq(make_session("TARGET")))),
          AllOf(Field(&SubscriptionKey::symbol, Eq(Symbol{"AAPL.N"})),
                Field(&SubscriptionKey::session, Eq(make_session("OTHER"))))));
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       TakesSourceOnceWhenDatasourcesShareSessionAndSymbol) {
  const auto other_datasource = make_datasource(
      with_symbols(Datasource::Patch{}, {"AAPL.OQ"}), OtherDatasourceId);
  const auto listing = make_datasource_priced_listing(with_random_price_sources(
      Listing::Patch{},
      {{DatasourceId, "AAPL.OQ"}, {OtherDatasourceId, "AAPL.OQ"}}));

  const auto price_sources = make_all_listing_price_sources(
      {make_price_source_datasource(), other_datasource}, {listing});

  ASSERT_THAT(price_sources, SizeIs(1));
  ASSERT_THAT(price_sources.begin()->second, SizeIs(1));
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       TakesValidSourcesOfListingWithAnInvalidSource) {
  const auto listing = make_datasource_priced_listing(with_random_price_sources(
      Listing::Patch{},
      {{OtherDatasourceId, "AAPL.N"}, {DatasourceId, "AAPL.OQ"}}));

  const auto price_sources = make_all_listing_price_sources(
      {make_price_source_datasource()}, {listing});

  ASSERT_THAT(price_sources, SizeIs(1));
  ASSERT_THAT(
      price_sources.begin()->second,
      ElementsAre(Field(&SubscriptionKey::symbol, Eq(Symbol{"AAPL.OQ"}))));
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       BuildsPriceSourcesForEveryListing) {
  const auto other_listing = make_datasource_priced_listing(
      Listing::Patch{}.with_symbol("MSFT"), OtherPricedListingId);

  const auto price_sources = make_all_listing_price_sources(
      {make_price_source_datasource()},
      {make_datasource_priced_listing(), other_listing});

  ASSERT_THAT(price_sources,
              UnorderedElementsAre(Pair(PricedListingId, _),
                                   Pair(OtherPricedListingId, _)));
}

TEST_F(GeneratorMarketDataListingPriceSourceBuilder,
       BuildsDatasourceRequirementsAndListingPriceSources) {
  const auto datasource =
      make_datasource(with_symbols(Datasource::Patch{}, {"AAPL.OQ"})
                          .with_random_price_only_flag(false));

  EXPECT_THAT(make_datasource_subscription_requirements({datasource}),
              SizeIs(1));
  EXPECT_THAT(make_all_listing_price_sources(
                  {datasource}, {make_datasource_priced_listing()}),
              SizeIs(1));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::generator::mdata::test
