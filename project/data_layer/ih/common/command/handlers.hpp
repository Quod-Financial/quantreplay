#ifndef SIMULATOR_DATA_LAYER_IH_COMMON_COMMAND_HANDLERS_HPP_
#define SIMULATOR_DATA_LAYER_IH_COMMON_COMMAND_HANDLERS_HPP_

#include <functional>
#include <type_traits>

#include "api/database/context.hpp"
#include "api/models/datasource.hpp"
#include "api/models/listing.hpp"
#include "api/models/price_seed.hpp"
#include "api/models/venue.hpp"
#include "ih/common/database/context_resolver.hpp"
#include "ih/pqxx/context.hpp"
#include "ih/pqxx/dao/datasource_dao.hpp"
#include "ih/pqxx/dao/listing_dao.hpp"
#include "ih/pqxx/dao/price_seed_dao.hpp"
#include "ih/pqxx/dao/setting_dao.hpp"
#include "ih/pqxx/dao/venue_dao.hpp"

namespace simulator::data_layer {

template <typename Command>
class DatasourceCommandHandler final : public database::ContextResolver {
 public:
  using CommandType = Command;
  using ModelType = typename CommandType::ModelType;

  static_assert(std::is_same_v<ModelType, data_layer::Datasource>,
                "Datasource command handler is instantiated with a command, "
                "which is not designed to run with Datasource model");

  static auto handle(CommandType& cmd, const database::Context& context)
      -> void {
    // Redirects control to `execute_with` method with a proper context type
    DatasourceCommandHandler{cmd}.resolve(context);
  }

 private:
  explicit DatasourceCommandHandler(CommandType& command) noexcept
      : command_(command) {}

  auto execute_with(const internal_pqxx::Context& pqxx_context)
      -> void override {
    internal_pqxx::DatasourceDao::setup_with(pqxx_context).execute(command_);
  }

  std::reference_wrapper<CommandType> command_;
};

template <typename Command>
class ListingCommandHandler final : public database::ContextResolver {
 public:
  using CommandType = Command;
  using ModelType = typename CommandType::ModelType;

  static_assert(std::is_same_v<ModelType, data_layer::Listing>,
                "Listing command handler is instantiated with a command, "
                "which is not designed to run with Listing model");

  static auto handle(CommandType& cmd, const database::Context& context)
      -> void {
    // Redirects control to `execute_with` method with a proper context type
    ListingCommandHandler{cmd}.resolve(context);
  }

 private:
  explicit ListingCommandHandler(CommandType& command) noexcept
      : command_(command) {}

  auto execute_with(const internal_pqxx::Context& pqxx_context)
      -> void override {
    internal_pqxx::ListingDao::setup_with(pqxx_context).execute(command_);
  }

  std::reference_wrapper<CommandType> command_;
};

template <typename Command>
class PriceSeedCommandHandler final : public database::ContextResolver {
 public:
  using CommandType = Command;
  using ModelType = typename CommandType::ModelType;

  static_assert(std::is_same_v<ModelType, data_layer::PriceSeed>,
                "Price seed command handler is instantiated with a command, "
                "which is not designed to run with PriceSeed model");

  static auto handle(CommandType& cmd, const database::Context& context)
      -> void {
    // Redirects control to `execute_with` method with a proper context type
    PriceSeedCommandHandler{cmd}.resolve(context);
  }

 private:
  explicit PriceSeedCommandHandler(CommandType& command) noexcept
      : command_(command) {}

  auto execute_with(const internal_pqxx::Context& pqxx_context)
      -> void override {
    internal_pqxx::PriceSeedDao::setup_with(pqxx_context).execute(command_);
  }

  std::reference_wrapper<CommandType> command_;
};

template <typename Command>
class SettingCommandHandler final : public database::ContextResolver {
 public:
  using CommandType = Command;
  using ModelType = typename CommandType::ModelType;

  static_assert(std::is_same_v<ModelType, data_layer::Setting>,
                "Setting command handler is instantiated with a command, "
                "which is not designed to run with Setting model");

  static auto handle(CommandType& cmd, const database::Context& context)
      -> void {
    // Redirects control to `execute_with` method with a proper context type
    SettingCommandHandler{cmd}.resolve(context);
  }

 private:
  explicit SettingCommandHandler(CommandType& command) noexcept
      : command_(command) {}

  auto execute_with(const internal_pqxx::Context& pqxx_context)
      -> void override {
    internal_pqxx::SettingDao::setup_with(pqxx_context).execute(command_);
  }

  std::reference_wrapper<CommandType> command_;
};

template <typename Command>
class VenueCommandHandler final : public database::ContextResolver {
 public:
  using CommandType = Command;
  using ModelType = typename CommandType::ModelType;

  static_assert(std::is_same_v<ModelType, data_layer::Venue>,
                "Venue command handler is instantiated with a command, "
                "which is not designed to run with Venue model");

  static auto handle(CommandType& cmd, const database::Context& context)
      -> void {
    // Redirects control to `execute_with` method with a proper context type
    VenueCommandHandler{cmd}.resolve(context);
  }

 private:
  explicit VenueCommandHandler(CommandType& command) noexcept
      : command_(command) {}

  auto execute_with(const internal_pqxx::Context& pqxx_context)
      -> void override {
    internal_pqxx::VenueDao::setup_with(pqxx_context).execute(command_);
  }

  std::reference_wrapper<CommandType> command_;
};

}  // namespace simulator::data_layer

#endif  // SIMULATOR_DATA_LAYER_IH_COMMON_COMMAND_HANDLERS_HPP_
