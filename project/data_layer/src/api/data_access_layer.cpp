#include "api/data_access_layer.hpp"

#include <string>
#include <utility>

#include "api/database/context.hpp"
#include "cfg/api/cfg.hpp"
#include "ih/common/command/commands.hpp"
#include "ih/common/command/handlers.hpp"
#include "ih/common/database/driver.hpp"
#include "ih/common/database/ping_agent.hpp"
#include "ih/pqxx/context.hpp"

namespace simulator::data_layer::database {

auto setup(const cfg::DbConfiguration& cfg) -> database::Context {
  return Driver::configure(cfg);
}

auto create_pqxx_connection(std::string connection_string)
    -> database::Context {
  internal_pqxx::Context pqxx_context{std::move(connection_string)};
  return Driver::make_database_context(std::move(pqxx_context));
}

auto ping(const database::Context& context) noexcept -> bool {
  return PingAgent::ping(context);
}

}  // namespace simulator::data_layer::database

namespace simulator::data_layer {

auto insert_datasource(const database::Context& context,
                       Datasource::Patch initial_patch) -> Datasource {
  using Command = data_layer::command::Insert<Datasource>;
  auto command = Command::create(std::move(initial_patch));
  DatasourceCommandHandler<Command>::handle(command, context);

  return Datasource{std::move(command.result())};
}

auto select_one_datasource(const database::Context& context,
                           Datasource::Predicate predicate) -> Datasource {
  using Command = data_layer::command::SelectOne<Datasource>;
  auto command = Command::create(std::move(predicate));
  DatasourceCommandHandler<Command>::handle(command, context);

  return Datasource{std::move(command.result())};
}

auto select_all_datasources(const database::Context& context,
                            std::optional<Datasource::Predicate> predicate)
    -> std::vector<Datasource> {
  using Command = data_layer::command::SelectAll<Datasource>;
  auto command = Command::create(std::move(predicate));
  DatasourceCommandHandler<Command>::handle(command, context);

  return std::vector<Datasource>{std::move(command.result())};
}

auto update_one_datasource(const database::Context& context,
                           Datasource::Patch update,
                           Datasource::Predicate predicate) -> Datasource {
  using Command = data_layer::command::UpdateOne<Datasource>;
  auto command = Command::create(std::move(update), std::move(predicate));
  DatasourceCommandHandler<Command>::handle(command, context);

  return Datasource{std::move(command.result())};
}

auto insert_listing(const database::Context& context,
                    Listing::Patch initial_patch) -> Listing {
  using Command = data_layer::command::Insert<Listing>;
  auto command = Command::create(std::move(initial_patch));
  ListingCommandHandler<Command>::handle(command, context);

  return Listing{std::move(command.result())};
}

auto select_one_listing(const database::Context& context,
                        Listing::Predicate predicate) -> Listing {
  using Command = data_layer::command::SelectOne<Listing>;
  auto command = Command::create(std::move(predicate));
  ListingCommandHandler<Command>::handle(command, context);

  return Listing{std::move(command.result())};
}

auto select_all_listings(const database::Context& context,
                         std::optional<Listing::Predicate> predicate)
    -> std::vector<Listing> {
  using Command = data_layer::command::SelectAll<Listing>;
  auto command = Command::create(std::move(predicate));
  ListingCommandHandler<Command>::handle(command, context);

  return std::vector<Listing>{std::move(command.result())};
}

auto update_one_listing(const database::Context& context,
                        Listing::Patch update,
                        Listing::Predicate predicate) -> Listing {
  using Command = data_layer::command::UpdateOne<Listing>;
  auto command = Command::create(std::move(update), std::move(predicate));
  ListingCommandHandler<Command>::handle(command, context);

  return Listing{std::move(command.result())};
}

auto insert_price_seed(const database::Context& context,
                       PriceSeed::Patch initial_patch) -> PriceSeed {
  using Command = data_layer::command::Insert<PriceSeed>;
  auto command = Command::create(std::move(initial_patch));
  PriceSeedCommandHandler<Command>::handle(command, context);

  return PriceSeed{std::move(command.result())};
}

auto select_one_price_seed(const database::Context& context,
                           PriceSeed::Predicate predicate) -> PriceSeed {
  using Command = data_layer::command::SelectOne<PriceSeed>;
  auto command = Command::create(std::move(predicate));
  PriceSeedCommandHandler<Command>::handle(command, context);

  return PriceSeed{std::move(command.result())};
}

auto select_all_price_seeds(const database::Context& context,
                            std::optional<PriceSeed::Predicate> predicate)
    -> std::vector<PriceSeed> {
  using Command = data_layer::command::SelectAll<PriceSeed>;
  auto command = Command::create(std::move(predicate));
  PriceSeedCommandHandler<Command>::handle(command, context);

  return std::vector<PriceSeed>{std::move(command.result())};
}

auto update_one_price_seed(const database::Context& context,
                           PriceSeed::Patch update,
                           PriceSeed::Predicate predicate) -> PriceSeed {
  using Command = data_layer::command::UpdateOne<PriceSeed>;
  auto command = Command::create(std::move(update), std::move(predicate));
  PriceSeedCommandHandler<Command>::handle(command, context);

  return PriceSeed{std::move(command.result())};
}

auto update_all_price_seeds(const database::Context& context,
                            PriceSeed::Patch update,
                            std::optional<PriceSeed::Predicate> predicate)
    -> std::vector<PriceSeed> {
  using Command = data_layer::command::UpdateAll<PriceSeed>;
  auto command = Command::create(std::move(update), std::move(predicate));
  PriceSeedCommandHandler<Command>::handle(command, context);

  return std::vector<PriceSeed>{std::move(command.result())};
}

auto delete_one_price_seed(const database::Context& context,
                           PriceSeed::Predicate predicate) -> void {
  using Command = data_layer::command::DeleteOne<PriceSeed>;
  auto command = Command::create(std::move(predicate));
  PriceSeedCommandHandler<Command>::handle(command, context);
}

auto delete_all_price_seeds(const database::Context& context,
                            std::optional<PriceSeed::Predicate> predicate)
    -> void {
  using Command = data_layer::command::DeleteAll<PriceSeed>;
  auto command = Command::create(std::move(predicate));
  PriceSeedCommandHandler<Command>::handle(command, context);
}

auto insert_setting(const database::Context& context,
                    Setting::Patch initial_patch) -> data_layer::Setting {
  using Command = data_layer::command::Insert<Setting>;
  auto command = Command::create(std::move(initial_patch));
  SettingCommandHandler<Command>::handle(command, context);

  return Setting{std::move(command.result())};
}

auto select_one_setting(const database::Context& context,
                        Setting::Predicate predicate) -> data_layer::Setting {
  using Command = data_layer::command::SelectOne<Setting>;
  auto command = Command::create(std::move(predicate));
  SettingCommandHandler<Command>::handle(command, context);

  return Setting{std::move(command.result())};
}

auto select_all_settings(const database::Context& context,
                         std::optional<Setting::Predicate> predicate)
    -> std::vector<data_layer::Setting> {
  using Command = data_layer::command::SelectAll<Setting>;
  auto command = Command::create(std::move(predicate));
  SettingCommandHandler<Command>::handle(command, context);

  return std::vector<Setting>{std::move(command.result())};
}

auto update_one_setting(const database::Context& context,
                        Setting::Patch update,
                        Setting::Predicate predicate) -> data_layer::Setting {
  using Command = data_layer::command::UpdateOne<Setting>;
  auto command = Command::create(std::move(update), std::move(predicate));
  SettingCommandHandler<Command>::handle(command, context);

  return Setting{std::move(command.result())};
}

auto insert_venue(const database::Context& context, Venue::Patch initial_patch)
    -> Venue {
  using Command = data_layer::command::Insert<Venue>;
  auto command = Command::create(std::move(initial_patch));
  VenueCommandHandler<Command>::handle(command, context);

  return Venue{std::move(command.result())};
}

auto select_one_venue(const database::Context& context,
                      Venue::Predicate predicate) -> Venue {
  using Command = data_layer::command::SelectOne<Venue>;
  auto command = Command::create(std::move(predicate));
  VenueCommandHandler<Command>::handle(command, context);

  return Venue{std::move(command.result())};
}

auto select_all_venues(const database::Context& context,
                       std::optional<Venue::Predicate> predicate)
    -> std::vector<Venue> {
  using Command = data_layer::command::SelectAll<Venue>;
  auto command = Command::create(std::move(predicate));
  VenueCommandHandler<Command>::handle(command, context);

  return std::vector<Venue>{std::move(command.result())};
}

auto update_one_venue(const database::Context& context,
                      Venue::Patch update,
                      Venue::Predicate predicate) -> Venue {
  using Command = data_layer::command::UpdateOne<Venue>;
  auto command = Command::create(std::move(update), std::move(predicate));
  VenueCommandHandler<Command>::handle(command, context);

  return Venue{std::move(command.result())};
}

auto select_simulated_venue(const database::Context& database)
    -> data_layer::Venue {
  return select_one_venue(
      database, VenueCmp::eq(Venue::Attribute::VenueId, cfg::venue().name));
}

}  // namespace simulator::data_layer