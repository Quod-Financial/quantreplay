#include "ih/data_bridge/price_seed_accessor.hpp"

#include "data_layer/api/data_access_layer.hpp"
#include "log/logging.hpp"

namespace simulator::http::data_bridge {

DataLayerPriceSeedAccessor::DataLayerPriceSeedAccessor(
    data_layer::database::Context context) noexcept
    : context_(std::move(context)) {}

auto DataLayerPriceSeedAccessor::select_single(std::uint64_t seed_id)
    const noexcept -> tl::expected<data_layer::PriceSeed, Failure> {
  using Column = data_layer::PriceSeed::Attribute;
  using data_layer::PriceSeedCmp;

  try {
    auto pred = PriceSeedCmp::eq(Column::PriceSeedId, seed_id);
    return data_layer::select_one_price_seed(context_, std::move(pred));
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting "
        "single price seed by `{}' identifier: {}",
        seed_id,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while selecting "
        "single price seed by `{}' identifier",
        seed_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerPriceSeedAccessor::select_all() const noexcept
    -> tl::expected<std::vector<data_layer::PriceSeed>, Failure> {
  try {
    return data_layer::select_all_price_seeds(context_);
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::DataDecodingError&) {
    return tl::unexpected{Failure::ResponseDecodingError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while selecting all "
        "price seeds: {}",
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while selecting all "
        "price seeds");
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerPriceSeedAccessor::add(data_layer::PriceSeed::Patch snapshot)
    const noexcept -> tl::expected<void, Failure> {
  try {
    data_layer::insert_price_seed(context_, std::move(snapshot));
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
        "price seed: {}",
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while adding a new "
        "price seed");
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerPriceSeedAccessor::update(data_layer::PriceSeed::Patch update,
                                        std::uint64_t seed_id) const noexcept
    -> tl::expected<void, Failure> {
  using Column = data_layer::PriceSeed::Attribute;
  using data_layer::PriceSeedCmp;

  try {
    auto pred = PriceSeedCmp::eq(Column::PriceSeedId, seed_id);
    data_layer::update_one_price_seed(
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
        "the price seed with `{}' identifier: {}",
        seed_id,
        ex.what());
  } catch (...) {
    log::err(
        "Unknown error is raised by data access layer while updating "
        "the price seed with `{}' identifier",
        seed_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerPriceSeedAccessor::drop_single(
    std::uint64_t seed_id) const noexcept -> tl::expected<void, Failure> {
  using Column = data_layer::PriceSeed::Attribute;
  using data_layer::PriceSeedCmp;

  try {
    auto pred = PriceSeedCmp::eq(Column::PriceSeedId, seed_id);
    data_layer::delete_one_price_seed(context_, std::move(pred));
    return {};
  } catch (const data_layer::ConnectionFailure&) {
    return tl::unexpected{Failure::DatabaseConnectionError};
  } catch (const data_layer::CardinalityViolationError&) {
    return tl::unexpected{Failure::ResponseCardinalityError};
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised unexpected exception while deleting "
        "single price seed by `{}' identifier: {}",
        seed_id,
        ex.what());
  } catch (...) {
    log::err(
        "unknown error is raised by data access layer while deleting "
        "single price seed by `{}' identifier",
        seed_id);
  }

  return tl::unexpected{Failure::UnknownError};
}

auto DataLayerPriceSeedAccessor::sync(std::string pqxx_source_connection)
    const noexcept -> tl::expected<void, Failure> {
  const data_layer::database::Context ext_database =
      data_layer::database::create_pqxx_connection(
          std::move(pqxx_source_connection));

  std::vector<data_layer::PriceSeed> external_seeds;
  try {
    external_seeds = data_layer::select_all_price_seeds(ext_database);
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised exception while loading price seeds from "
        "external price seed sync database: {}",
        ex.what());
    return tl::unexpected{Failure::UnknownError};
  } catch (...) {
    log::err(
        "data access layer raised unknown exception while  "
        "loading price seeds from external price seed sync database");
    return tl::unexpected{Failure::UnknownError};
  }

  try {
    for (const auto& ext_price_seed : external_seeds) {
      sync_seeds(ext_price_seed);
    }
  } catch (const std::exception& ex) {
    log::warn(
        "data access layer raised exception while syncing price seeds "
        "with a downloaded external price seed: {}",
        ex.what());
    return tl::unexpected{Failure::UnknownError};
  } catch (...) {
    log::err(
        "data access layer raised unknown exception while "
        "syncing price seeds  with a downloaded external price seed");
    return tl::unexpected{Failure::UnknownError};
  }

  return {};
}

auto DataLayerPriceSeedAccessor::sync_seeds(
    const data_layer::PriceSeed& ext_seed) const -> void {
  const std::optional<std::string>& symbol = ext_seed.symbol();
  if (!symbol.has_value()) {
    log::info(
        "the price seed record without a symbol found in the external "
        "price seed table with an identifier `{}', record can not be synced",
        ext_seed.price_seed_id());
    return;
  }

  data_layer::PriceSeed::Patch patch{};
  if (const auto mid_price = ext_seed.mid_price()) {
    patch.with_mid_price(*mid_price);
  }
  if (const auto bid_price = ext_seed.bid_price()) {
    patch.with_bid_price(*bid_price);
  }
  if (const auto offer_price = ext_seed.offer_price()) {
    patch.with_offer_price(*offer_price);
  }
  if (const auto& seed_update_time = ext_seed.last_update()) {
    patch.with_last_update(*seed_update_time);
  }

  using data_layer::PriceSeedCmp;
  using Column = data_layer::PriceSeed::Attribute;
  auto pred = PriceSeedCmp::eq(Column::Symbol, *symbol);

  const std::vector<data_layer::PriceSeed> updated =
      data_layer::update_all_price_seeds(context_, patch, pred);
  if (!updated.empty()) {
    // Price seeds with such symbol are updated in the db, nothing to do...
    return;
  }

  log::info(
      "the price seed with `{}' symbol is not present in the local "
      "price seed table, but exists in the external one, inserting "
      "it into the local table",
      *symbol);

  patch.with_symbol(*symbol);
  if (const auto& value = ext_seed.security_type()) {
    patch.with_security_type(*value);
  }
  if (const auto& value = ext_seed.price_currency()) {
    patch.with_price_currency(*value);
  }
  if (const auto& value = ext_seed.security_id()) {
    patch.with_security_id(*value);
  }
  if (const auto& value = ext_seed.security_id_source()) {
    patch.with_security_id_source(*value);
  }
  if (const auto& value = ext_seed.instrument_symbol()) {
    patch.with_instrument_symbol(*value);
  }

  data_layer::insert_price_seed(context_, std::move(patch));
}

}  // namespace simulator::http::data_bridge
