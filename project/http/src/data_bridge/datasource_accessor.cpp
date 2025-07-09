#include "ih/data_bridge/datasource_accessor.hpp"

#include "data_layer/api/data_access_layer.hpp"
#include "log/logging.hpp"

namespace simulator::http::data_bridge {

DataLayerDatasourceAccessor::DataLayerDatasourceAccessor(
    DbContext context) noexcept
    : context_(std::move(context)) {}

auto DataLayerDatasourceAccessor::select_single(std::uint64_t datasource_id)
    const noexcept -> tl::expected<data_layer::Datasource, FailureInfo> {
  using Column = data_layer::Datasource::Attribute;
  using data_layer::DatasourceCmp;

  try {
    auto pred = DatasourceCmp::eq(Column::DatasourceId, datasource_id);
    return data_layer::select_one_datasource(context_, std::move(pred));
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{FailureInfo{Failure::DatabaseConnectionError, {}}};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{FailureInfo{Failure::ResponseCardinalityError, {}}};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{FailureInfo{Failure::ResponseDecodingError, {}}};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting "
        "single datasource by `{}' identifier: {}",
        datasource_id,
        ex.what());
  } catch (...) {
    log::err(
        "Unknown error is raised by data access layer while selecting "
        "single datasource by `{}' identifier",
        datasource_id);
  }

  return tl::unexpected{FailureInfo{Failure::UnknownError, {}}};
}

auto DataLayerDatasourceAccessor::select_all() const noexcept
    -> tl::expected<std::vector<data_layer::Datasource>, FailureInfo> {
  try {
    return data_layer::select_all_datasources(context_);
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{FailureInfo{Failure::DatabaseConnectionError, {}}};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{FailureInfo{Failure::ResponseDecodingError, {}}};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting all "
        "datasources: {}",
        ex.what());
  } catch (...) {
    log::err(
        "Unknown error is raised by data access layer while selecting all "
        "datasources");
  }

  return tl::unexpected{FailureInfo{Failure::UnknownError, {}}};
}

auto DataLayerDatasourceAccessor::add(data_layer::Datasource::Patch snapshot)
    const noexcept -> tl::expected<void, FailureInfo> {
  try {
    data_layer::insert_datasource(context_, std::move(snapshot));
    return {};
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{FailureInfo{Failure::DatabaseConnectionError, {}}};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{FailureInfo{Failure::ResponseCardinalityError, {}}};
  } catch (const data_layer::MalformedPatch& ex) {
    return tl::unexpected{FailureInfo{Failure::MalformedInput, ex.what()}};
  } catch (const data_layer::DataIntegrityError&) {
    return tl::unexpected{
        FailureInfo{Failure::DataIntegrityViolationError, {}}};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{FailureInfo{Failure::ResponseDecodingError, {}}};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while adding a new "
        "datasource: {}",
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while adding a new "
        "datasource");
  }

  return tl::unexpected{FailureInfo{Failure::UnknownError, {}}};
}

auto DataLayerDatasourceAccessor::update(data_layer::Datasource::Patch update,
                                         std::uint64_t datasource_id)
    const noexcept -> tl::expected<void, FailureInfo> {
  using Column = data_layer::Datasource::Attribute;
  using data_layer::DatasourceCmp;

  try {
    auto pred = DatasourceCmp::eq(Column::DatasourceId, datasource_id);
    auto venue = data_layer::update_one_datasource(
        context_, std::move(update), std::move(pred));
    return {};
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{FailureInfo{Failure::DatabaseConnectionError, {}}};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{FailureInfo{Failure::ResponseCardinalityError, {}}};
  } catch (const data_layer::MalformedPatch& ex) {
    return tl::unexpected{FailureInfo{Failure::MalformedInput, ex.what()}};
  } catch (const data_layer::DataIntegrityError&) {
    return tl::unexpected{
        FailureInfo{Failure::DataIntegrityViolationError, {}}};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{FailureInfo{Failure::ResponseDecodingError, {}}};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while updating "
        "the datasource with `{}' identifier: {}",
        datasource_id,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while updating "
        "the datasource with `{}' identifier",
        datasource_id);
  }

  return tl::unexpected{FailureInfo{Failure::UnknownError, {}}};
}

}  // namespace simulator::http::data_bridge
