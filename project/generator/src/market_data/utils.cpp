#include "ih/market_data/utils.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "core/tools/fix_session_id.hpp"
#include "ih/constants.hpp"
#include "log/logging.hpp"

namespace simulator::generator::mdata {
namespace {

[[nodiscard]]
auto used_only_for_random_prices(const data_layer::Datasource& datasource)
    -> bool {
  return datasource.random_price_only_flag().value_or(
      data_layer::Datasource::DefaultRandomPriceOnlyFlag);
}

[[nodiscard]]
auto make_datasource_requirements(const data_layer::Datasource& datasource)
    -> std::vector<SubscriptionRequirement> {
  const std::optional<protocol::fix::Session> session =
      parse_fix_session(datasource.connection());
  if (!session.has_value()) {
    log::warn(
        "the `{}' datasource (id: {}) does not specify a valid FIX session in "
        "its connection, ignoring it as a market data source",
        datasource.name(),
        datasource.datasource_id());
    return {};
  }

  const std::vector<data_layer::DatasourceListing>& datasource_listings =
      datasource.listings();
  if (datasource_listings.empty()) {
    log::warn(
        "the `{}' datasource (id: {}) does not specify any symbols, ignoring "
        "it as a market data source",
        datasource.name(),
        datasource.datasource_id());
    return {};
  }

  const bool random_price_only = used_only_for_random_prices(datasource);
  if (random_price_only) {
    log::info(
        "the `{}' datasource (id: {}) is used only as a random price source, "
        "subscribing to the top of the book of its symbols",
        datasource.name(),
        datasource.datasource_id());
  }

  const MarketDepth depth =
      random_price_only ? TopOfTheBookDepth
                        : MarketDepth{datasource.max_depth_levels().value_or(
                              data_layer::Datasource::AllDepthLevels)};

  std::vector<SubscriptionRequirement> requirements;
  requirements.reserve(datasource_listings.size());

  for (const data_layer::DatasourceListing& datasource_listing :
       datasource_listings) {
    const std::string& datasource_symbol = datasource_listing.symbol();
    if (datasource_symbol.empty()) {
      log::warn(
          "the `{}' datasource (id: {}) specifies an empty symbol, ignoring "
          "the symbol as a market data source",
          datasource.name(),
          datasource.datasource_id());
      continue;
    }

    requirements.emplace_back(
        protocol::Session{*session}, Symbol{datasource_symbol}, depth);
  }

  return requirements;
}

[[nodiscard]]
auto subscribes_to_symbol(const data_layer::Datasource& datasource,
                          const std::string& symbol) -> bool {
  return std::ranges::any_of(
      datasource.listings(),
      [&symbol](const data_layer::DatasourceListing& datasource_listing) {
        return datasource_listing.symbol() == symbol;
      });
}

[[nodiscard]]
auto make_random_price_source(
    const data_layer::Listing& listing,
    const data_layer::ListingRandomPriceSource& random_price_source,
    const std::vector<data_layer::Datasource>& datasources)
    -> std::optional<SubscriptionKey> {
  const std::uint64_t datasource_id = random_price_source.datasource_id();

  const auto datasource = std::ranges::find_if(
      datasources, [datasource_id](const data_layer::Datasource& candidate) {
        return candidate.datasource_id() == datasource_id;
      });
  if (datasource == datasources.end()) {
    log::warn(
        "the `{}' listing (id: {}) bases its random prices on the datasource "
        "(id: {}), which is not an enabled datasource of the venue, ignoring "
        "its random price source",
        listing.symbol(),
        listing.listing_id(),
        datasource_id);
    return std::nullopt;
  }

  if (datasource->format() != data_layer::Datasource::Format::Fix) {
    log::warn(
        "the `{}' listing (id: {}) bases its random prices on the `{}' "
        "datasource (id: {}), which is not of the FIX format, ignoring its "
        "random price source",
        listing.symbol(),
        listing.listing_id(),
        datasource->name(),
        datasource->datasource_id());
    return std::nullopt;
  }

  const std::optional<protocol::fix::Session> session =
      parse_fix_session(datasource->connection());
  if (!session.has_value()) {
    log::warn(
        "the `{}' datasource (id: {}), the `{}' listing (id: {}) bases its "
        "random prices on, does not specify a valid FIX session in its "
        "connection `{}', ignoring the random price source of the listing",
        datasource->name(),
        datasource->datasource_id(),
        listing.symbol(),
        listing.listing_id(),
        datasource->connection());
    return std::nullopt;
  }

  const std::string& symbol = random_price_source.symbol();
  if (symbol.empty()) {
    log::warn(
        "the `{}' listing (id: {}) does not specify the symbol to take the "
        "random prices from on the `{}' datasource (id: {}), ignoring its "
        "random price source",
        listing.symbol(),
        listing.listing_id(),
        datasource->name(),
        datasource->datasource_id());
    return std::nullopt;
  }

  if (!subscribes_to_symbol(*datasource, symbol)) {
    log::warn(
        "the `{}' listing (id: {}) bases its random prices on the `{}' symbol "
        "of the `{}' datasource (id: {}), which does not list the symbol among "
        "the symbols it subscribes to, ignoring its random price source",
        listing.symbol(),
        listing.listing_id(),
        symbol,
        datasource->name(),
        datasource->datasource_id());
    return std::nullopt;
  }

  return std::make_optional<SubscriptionKey>(protocol::Session{*session},
                                             Symbol{symbol});
}

}  // namespace

auto bases_random_prices_on_datasource(const data_layer::Listing& listing)
    -> bool {
  return !listing.random_price_sources().empty();
}

auto parse_fix_session(std::string_view connection)
    -> std::optional<protocol::fix::Session> {
  const auto session_id = core::parse_fix_session_id(connection);
  if (!session_id.has_value()) {
    return std::nullopt;
  }

  auto session = std::make_optional<protocol::fix::Session>(
      protocol::fix::BeginString{session_id->begin_string},
      protocol::fix::SenderCompId{session_id->sender_comp_id},
      protocol::fix::TargetCompId{session_id->target_comp_id});
  if (session_id->session_qualifier.has_value()) {
    session->session_qualifier =
        protocol::fix::SessionQualifier{*session_id->session_qualifier};
  }
  return session;
}

auto make_datasource_subscription_requirements(
    const std::vector<data_layer::Datasource>& datasources)
    -> std::vector<SubscriptionRequirement> {
  std::vector<SubscriptionRequirement> requirements;

  for (const data_layer::Datasource& datasource : datasources) {
    if (datasource.format() != data_layer::Datasource::Format::Fix) {
      continue;
    }

    std::vector<SubscriptionRequirement> datasource_requirements =
        make_datasource_requirements(datasource);
    requirements.insert(
        requirements.end(),
        std::make_move_iterator(datasource_requirements.begin()),
        std::make_move_iterator(datasource_requirements.end()));
  }

  return requirements;
}

auto make_order_derivation_requirements(
    const std::vector<data_layer::Datasource>& datasources)
    -> std::vector<OrderDerivationRequirement> {
  std::vector<OrderDerivationRequirement> requirements;

  for (const data_layer::Datasource& datasource : datasources) {
    if (datasource.format() != data_layer::Datasource::Format::Fix ||
        used_only_for_random_prices(datasource)) {
      continue;
    }

    const PartyId counterparty{
        fmt::format(constant::market_data::DatasourceCounterpartyPattern,
                    datasource.datasource_id())};
    for (SubscriptionRequirement& subscription :
         make_datasource_requirements(datasource)) {
      requirements.push_back(OrderDerivationRequirement{
          .session = std::move(subscription.session),
          .symbol = std::move(subscription.symbol),
          .depth = subscription.depth,
          .counterparty = counterparty});
    }
  }

  return requirements;
}

auto make_listing_random_price_sources(
    const std::vector<data_layer::Datasource>& datasources,
    const std::vector<data_layer::Listing>& listings,
    const std::unordered_set<ListingId>& random_generation_listings)
    -> std::map<ListingId, std::vector<SubscriptionKey>> {
  std::map<ListingId, std::vector<SubscriptionKey>> price_sources;

  for (const data_layer::Listing& listing : listings) {
    if (!bases_random_prices_on_datasource(listing)) {
      continue;
    }

    if (!random_generation_listings.contains(listing.listing_id())) {
      log::info(
          "no random orders are generated for the `{}' listing (id: {}), not "
          "taking the market data its random prices are based on into account",
          listing.symbol(),
          listing.listing_id());
      continue;
    }

    std::vector<SubscriptionKey> listing_sources;
    for (const data_layer::ListingRandomPriceSource& random_price_source :
         listing.random_price_sources()) {
      auto source =
          make_random_price_source(listing, random_price_source, datasources);
      if (!source.has_value()) {
        continue;
      }

      if (std::ranges::find(listing_sources, *source) !=
          listing_sources.end()) {
        log::debug(
            "the `{}' listing (id: {}) specifies the `{}' random price source "
            "symbol more than once, taking it into account once",
            listing.symbol(),
            listing.listing_id(),
            source->symbol);
        continue;
      }

      listing_sources.emplace_back(*std::move(source));
    }

    if (listing_sources.empty()) {
      continue;
    }

    price_sources.emplace(listing.listing_id(), std::move(listing_sources));
  }

  return price_sources;
}

}  // namespace simulator::generator::mdata
