#include "ih/data_bridge/listing_accessor.hpp"

#include "cfg/api/cfg.hpp"
#include "data_layer/api/data_access_layer.hpp"
#include "log/logging.hpp"

namespace simulator::http::data_bridge {

DataLayerListingAccessor::DataLayerListingAccessor(DbContext context) noexcept
    : context_(std::move(context)) {}

auto DataLayerListingAccessor::select_single(std::uint64_t listing_id)
    const noexcept -> tl::expected<data_layer::Listing, Failure> {
  using Column = data_layer::Listing::Attribute;
  using data_layer::ListingCmp;

  try {
    auto pred = ListingCmp::eq(Column::ListingId, listing_id);
    return data_layer::select_one_listing(context_, std::move(pred));
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting "
        "single listing by `{}' identifier: {}",
        listing_id,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while selecting "
        "single listing by `{}' identifier",
        listing_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerListingAccessor::select_single(const std::string& symbol)
    const noexcept -> tl::expected<data_layer::Listing, Failure> {
  using Column = data_layer::Listing::Attribute;
  using data_layer::ListingCmp;

  try {
    auto pred = ListingCmp::eq(Column::Symbol, symbol) &&
                ListingCmp::eq(Column ::VenueId, cfg::venue().name);
    return data_layer::select_one_listing(context_, std::move(pred));
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting "
        "single listing by `{}' symbol: {}",
        symbol,
        ex.what());
  } catch (...) {
    log::err(
        "Unknown error is raised by data access layer while selecting "
        "single listing by `{}' symbol",
        symbol);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerListingAccessor::select_all() const noexcept
    -> tl::expected<std::vector<data_layer::Listing>, Failure> {
  try {
    return data_layer::select_all_listings(context_);
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting all "
        "listings: {}",
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while selecting all "
        "listings");
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerListingAccessor::add(data_layer::Listing::Patch snapshot)
    const noexcept -> tl::expected<void, Failure> {
  try {
    data_layer::insert_listing(context_, std::move(snapshot));
    return {};
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const data_layer::MalformedPatch&) {
    return tl::unexpected{Failure::MalformedInput};
  } catch (const data_layer::DataIntegrityError&) {
    return tl::unexpected{Failure::DataIntegrityViolationError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while adding a new "
        "listing: {}",
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while adding a new "
        "listing");
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerListingAccessor::update(data_layer::Listing::Patch update,
                                      std::uint64_t listing_id) const noexcept
    -> tl::expected<void, Failure> {
  using Column = data_layer::Listing::Attribute;
  using data_layer::ListingCmp;

  try {
    auto pred = ListingCmp::eq(Column::ListingId, listing_id);
    data_layer::update_one_listing(
        context_, std::move(update), std::move(pred));
    return {};
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const data_layer::MalformedPatch&) {
    return tl::unexpected{Failure::MalformedInput};
  } catch (const data_layer::DataIntegrityError&) {
    return tl::unexpected{Failure::DataIntegrityViolationError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while updating "
        "the listing with `{}' identifier: {}",
        listing_id,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while updating "
        "the listing with `{}' identifier",
        listing_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerListingAccessor::update(data_layer::Listing::Patch update,
                                      const std::string& symbol) const noexcept
    -> tl::expected<void, Failure> {
  using Column = data_layer::Listing::Attribute;
  using data_layer::ListingCmp;

  try {
    auto pred = ListingCmp::eq(Column::Symbol, symbol) &&
                ListingCmp::eq(Column::VenueId, cfg::venue().name);
    data_layer::update_one_listing(
        context_, std::move(update), std::move(pred));
    return {};
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const data_layer::MalformedPatch&) {
    return tl::unexpected{Failure::MalformedInput};
  } catch (const data_layer::DataIntegrityError&) {
    return tl::unexpected{Failure::DataIntegrityViolationError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while updating "
        "the listing with `{}' symbol: {}",
        symbol,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while updating "
        "the listing with `{}' symbol",
        symbol);
  }

  return tl::unexpected{Failure::UnknownError};
}

}  // namespace simulator::http::data_bridge
