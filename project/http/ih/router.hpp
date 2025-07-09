#ifndef SIMULATOR_HTTP_IH_ROUTER_HPP_
#define SIMULATOR_HTTP_IH_ROUTER_HPP_

#include <pistache/router.h>

#include "data_layer/api/database/context.hpp"
#include "ih/controllers/datasource_controller.hpp"
#include "ih/controllers/listing_controller.hpp"
#include "ih/controllers/price_seed_controller.hpp"
#include "ih/controllers/setting_controller.hpp"
#include "ih/controllers/venue_controller.hpp"
#include "ih/data_bridge/datasource_accessor.hpp"
#include "ih/data_bridge/listing_accessor.hpp"
#include "ih/data_bridge/price_seed_accessor.hpp"
#include "ih/data_bridge/setting_accessor.hpp"
#include "ih/data_bridge/venue_accessor.hpp"
#include "ih/processors/delete_processor.hpp"
#include "ih/processors/get_processor.hpp"
#include "ih/processors/post_processor.hpp"
#include "ih/processors/put_processor.hpp"

namespace simulator::http {

class Router : public Pistache::Http::Handler {
 public:
  HTTP_PROTOTYPE(Router)

  explicit Router(data_layer::database::Context database);

  auto onRequest(const Pistache::Http::Request& request,
                 Pistache::Http::ResponseWriter response) -> void override;

 private:
  auto init_generator_admin_routers() -> void;

  auto init_matching_engine_admin_routes() -> void;

  auto init_admin_routes() -> void;

  auto init_venue_routes() -> void;

  auto init_listing_routes() -> void;

  auto init_price_seed_routes() -> void;

  auto init_datasource_routes() -> void;

  auto init_setting_routes() -> void;

  Pistache::Rest::Router router_;

  data_bridge::DataLayerDatasourceAccessor datasource_accessor_;
  data_bridge::DataLayerListingAccessor listing_accessor_;
  data_bridge::DataLayerPriceSeedAccessor price_seed_accessor_;
  data_bridge::DataLayerSettingAccessor settings_accessor_;
  data_bridge::DataLayerVenueAccessor venue_accessor_;

  DatasourceController datasource_controller_;
  ListingController listing_controller_;
  PriceSeedController price_seed_controller_;
  SettingController setting_controller_;
  TradingController trading_controller_;
  VenueController venue_controller_;

  GetProcessor get_processor_;
  PostProcessor post_processor_;
  PutProcessor put_processor_;
  DeleteProcessor delete_processor_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_ROUTER_HPP_