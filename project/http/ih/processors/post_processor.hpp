#ifndef SIMULATOR_HTTP_IH_PROCESSORS_POST_PROCESSOR_HPP_
#define SIMULATOR_HTTP_IH_PROCESSORS_POST_PROCESSOR_HPP_

#include <pistache/http.h>
#include <pistache/router.h>

#include <memory>
#include <string>

#include "ih/controllers/datasource_controller.hpp"
#include "ih/controllers/listing_controller.hpp"
#include "ih/controllers/price_seed_controller.hpp"
#include "ih/controllers/setting_controller.hpp"
#include "ih/controllers/trading_controller.hpp"
#include "ih/controllers/venue_controller.hpp"
#include "ih/data_bridge/venue_accessor.hpp"
#include "ih/redirect/redirection_processor.hpp"
#include "ih/redirect/result.hpp"

namespace simulator::http {

class PostProcessor {
 public:
  PostProcessor(const data_bridge::VenueAccessor& venue_accessor,
                const DatasourceController& datasource_controller,
                const ListingController& listing_controller,
                const PriceSeedController& price_seed_controller,
                const SettingController& setting_controller,
                const TradingController& trading_controller,
                const VenueController& venue_controller);

  auto add_venue(const Pistache::Rest::Request& request,
                 Pistache::Http::ResponseWriter response) -> void;

  auto add_listing(const Pistache::Rest::Request& request,
                   Pistache::Http::ResponseWriter response) -> void;

  auto add_data_source(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response) -> void;

  auto add_price_seed(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter response) -> void;

  auto handle_store_request(const Pistache::Rest::Request& request,
                            Pistache::Http::ResponseWriter response) -> void;

  auto handle_recover_request(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response) -> void;

 private:
  static auto respond(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter& response,
                      Pistache::Http::Code code,
                      const std::string& body) -> void;

  auto redirect(const Pistache::Rest::Request& request,
                const std::string& instance_id) const -> redirect::Result;

  std::shared_ptr<redirect::RedirectionProcessor> redirector_;

  std::reference_wrapper<const DatasourceController> datasource_controller_;
  std::reference_wrapper<const ListingController> listing_controller_;
  std::reference_wrapper<const PriceSeedController> price_seed_controller_;
  std::reference_wrapper<const SettingController> setting_controller_;
  std::reference_wrapper<const TradingController> trading_controller_;
  std::reference_wrapper<const VenueController> venue_controller_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_PROCESSORS_POST_PROCESSOR_HPP_