#include "ih/data_bridge/venue_accessor.hpp"

#include "data_layer/api/data_access_layer.hpp"
#include "log/logging.hpp"

namespace simulator::http::data_bridge {

DataLayerVenueAccessor::DataLayerVenueAccessor(DbContext context) noexcept
    : context_(std::move(context)) {}

auto DataLayerVenueAccessor::select_single(const std::string& venue_id)
    const noexcept -> tl::expected<data_layer::Venue, Failure> {
  using Column = data_layer::Venue::Attribute;
  using data_layer::VenueCmp;

  try {
    auto pred = VenueCmp::eq(Column::VenueId, venue_id);
    return data_layer::select_one_venue(context_, std::move(pred));
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting "
        "single venue by `{}' identifier: {}",
        venue_id,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while selecting "
        "single venue by `{}' identifier",
        venue_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerVenueAccessor::select_all() const noexcept
    -> tl::expected<std::vector<data_layer::Venue>, Failure> {
  try {
    return data_layer::select_all_venues(context_);
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting all "
        "venues: {}",
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while selecting all "
        "venues");
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerVenueAccessor::add(data_layer::Venue::Patch snapshot)
    const noexcept -> tl::expected<void, Failure> {
  try {
    data_layer::insert_venue(context_, std::move(snapshot));
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
        "venue: {}",
        ex.what());
  } catch (...) {
    log::err(
        "Unknown error is raised by data access layer while adding a new "
        "venue");
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerVenueAccessor::update(data_layer::Venue::Patch update,
                                    const std::string& venue_id) const noexcept
    -> tl::expected<void, Failure> {
  using Column = data_layer::Venue::Attribute;
  using data_layer::VenueCmp;

  try {
    auto pred = VenueCmp::eq(Column ::VenueId, venue_id);
    auto venue = data_layer::update_one_venue(
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
        "the venue with `{}' identifier: {}",
        venue_id,
        ex.what());
  } catch (...) {
    log::err(
        "Unknown error is raised by data access layer while updating "
        "the venue with `{}' identifier",
        venue_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

}  // namespace simulator::http::data_bridge
