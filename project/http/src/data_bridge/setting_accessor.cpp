#include "ih/data_bridge/setting_accessor.hpp"

#include "data_layer/api/data_access_layer.hpp"
#include "log/logging.hpp"

namespace simulator::http::data_bridge {

DataLayerSettingAccessor::DataLayerSettingAccessor(DbContext context) noexcept
    : context_(std::move(context)) {}

auto DataLayerSettingAccessor::select_single(const std::string& key)
    const noexcept -> tl::expected<data_layer::Setting, Failure> {
  using Column = data_layer::Setting::Attribute;
  using data_layer::SettingCmp;

  try {
    auto pred = SettingCmp::eq(Column::Key, key);
    return data_layer::select_one_setting(context_, std::move(pred));
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting "
        "single setting by `{}' key: {}",
        key,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while selecting "
        "single setting by `{}' key",
        key);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerSettingAccessor::select_all() const noexcept
    -> tl::expected<std::vector<data_layer::Setting>, Failure> {
  try {
    return data_layer::select_all_settings(context_);
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting all "
        "settings: {}",
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while selecting all "
        "settings");
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerSettingAccessor::add(data_layer::Setting::Patch snapshot)
    const noexcept -> tl::expected<void, Failure> {
  try {
    data_layer::insert_setting(context_, std::move(snapshot));
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
        "setting: {}",
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while adding a new "
        "setting");
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerSettingAccessor::update(data_layer::Setting::Patch update,
                                      const std::string& key) const noexcept
    -> tl::expected<void, Failure> {
  using Column = data_layer::Setting::Attribute;
  using data_layer::SettingCmp;

  try {
    auto pred = SettingCmp::eq(Column::Key, key);
    auto venue = data_layer::update_one_setting(
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
        "the setting with `{}' key: {}",
        key,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while updating "
        "the setting with `{}' key",
        key);
  }

  return tl::unexpected{Failure::UnknownError};
}

}  // namespace simulator::http::data_bridge
