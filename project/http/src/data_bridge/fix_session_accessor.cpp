#include "ih/data_bridge/fix_session_accessor.hpp"

#include "data_layer/api/data_access_layer.hpp"
#include "log/logging.hpp"

namespace simulator::http::data_bridge {

DataLayerFixSessionAccessor::DataLayerFixSessionAccessor(
    DbContext context) noexcept
    : context_{std::move(context)} {}

auto DataLayerFixSessionAccessor::select_all(
    const std::string& venue_id) const noexcept
    -> tl::expected<std::vector<data_layer::FixSession>, Failure> {
  using Column = data_layer::FixSession::Attribute;
  using data_layer::FixSessionCmp;

  try {
    auto pred = FixSessionCmp::eq(Column::VenueId, venue_id);
    return data_layer::select_all_fix_sessions(context_, std::move(pred));
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting all "
        "fix sessions by `{}' venue identifier: {}",
        venue_id,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while selecting all "
        "fix sessions by `{}' venue identifier",
        venue_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerFixSessionAccessor::add(data_layer::FixSession::Patch snapshot)
    const noexcept -> tl::expected<void, Failure> {
  try {
    data_layer::insert_fix_session(context_, std::move(snapshot));
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
        "fix session: {}",
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while adding a new "
        "fix session");
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerFixSessionAccessor::update(data_layer::FixSession::Patch update,
                                         const std::string& venue_id,
                                         const std::string& session_id)
    const noexcept -> tl::expected<void, Failure> {
  using Column = data_layer::FixSession::Attribute;
  using data_layer::FixSessionCmp;

  try {
    auto pred = FixSessionCmp::eq(Column::VenueId, venue_id) &&
                FixSessionCmp::eq(Column::SessionId, session_id);
    data_layer::update_one_fix_session(
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
        "the fix session with `{}' venue and `{}' session identifiers: {}",
        venue_id,
        session_id,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while updating "
        "the fix session with `{}' venue and `{}' session identifiers",
        venue_id,
        session_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerFixSessionAccessor::delete_all(
    const std::string& venue_id,
    const std::vector<std::string>& session_ids) const noexcept
    -> tl::expected<void, Failure> {
  using Column = data_layer::FixSession::Attribute;
  using data_layer::FixSessionCmp;

  try {
    auto pred = FixSessionCmp::eq(Column::VenueId, venue_id) &&
                FixSessionCmp::in(Column::SessionId, session_ids);
    data_layer::delete_all_fix_session(context_, std::move(pred));
    return {};
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while deleting "
        "fix sessions by `{}' venue identifier: {}",
        venue_id,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while deleting "
        "fix sessions by `{}' venue identifier",
        venue_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

}  // namespace simulator::http::data_bridge
