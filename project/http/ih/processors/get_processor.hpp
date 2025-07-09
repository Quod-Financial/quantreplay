#ifndef SIMULATOR_HTTP_IH_PROCESSORS_GET_PROCESSOR_HPP_
#define SIMULATOR_HTTP_IH_PROCESSORS_GET_PROCESSOR_HPP_

#include <pistache/http.h>
#include <pistache/router.h>

#include <functional>
#include <memory>

#include "data_layer/api/models/venue.hpp"
#include "ih/controllers/datasource_controller.hpp"
#include "ih/controllers/listing_controller.hpp"
#include "ih/controllers/price_seed_controller.hpp"
#include "ih/controllers/setting_controller.hpp"
#include "ih/controllers/venue_controller.hpp"
#include "ih/data_bridge/venue_accessor.hpp"
#include "ih/redirect/redirection_processor.hpp"
#include "ih/redirect/result.hpp"

namespace simulator::http {

class GetProcessor {
 public:
  explicit GetProcessor(const data_bridge::VenueAccessor& venue_accessor,
                        const DatasourceController& datasource_controller,
                        const ListingController& listing_controller,
                        const PriceSeedController& price_seed_controller,
                        const SettingController& setting_controller,
                        const VenueController& venue_controller);

  auto get_venue(const Pistache::Rest::Request& request,
                 Pistache::Http::ResponseWriter response) -> void;

  auto get_venues(const Pistache::Rest::Request& request,
                  Pistache::Http::ResponseWriter response) -> void;

  auto get_listing(const Pistache::Rest::Request& request,
                   Pistache::Http::ResponseWriter response) -> void;

  auto get_listings(const Pistache::Rest::Request& request,
                    Pistache::Http::ResponseWriter response) -> void;

  auto get_data_source(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response) -> void;

  auto get_data_sources(const Pistache::Rest::Request& request,
                        Pistache::Http::ResponseWriter response) -> void;

  auto get_price_seed(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter response) -> void;

  auto get_price_seeds(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response) -> void;

  auto get_status(const Pistache::Rest::Request& request,
                  Pistache::Http::ResponseWriter response) -> void;

  auto get_venue_status(const Pistache::Rest::Request& request,
                        Pistache::Http::ResponseWriter response) -> void;

  auto get_venue_statuses(const Pistache::Rest::Request& request,
                          Pistache::Http::ResponseWriter response) -> void;

  auto get_settings(const Pistache::Rest::Request& request,
                    Pistache::Http::ResponseWriter response) -> void;

  auto get_order_gen_status(const Pistache::Rest::Request& request,
                            Pistache::Http::ResponseWriter response) -> void;

  auto get_venue_status_str(const data_layer::Venue& venue,
                            bool send_response_code,
                            bool& available) const -> std::string;

 private:
  auto handle_generation_status_request(const Pistache::Rest::Request& request,
                                        Pistache::Http::ResponseWriter response)
      -> void;

  static auto respond(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter& response,
                      Pistache::Http::Code code,
                      const std::string& body) -> void;

  auto redirect(const Pistache::Rest::Request& request,
                const std::string& instance_id) const -> redirect::Result;

  std::shared_ptr<redirect::RedirectionProcessor> redirector_;

  std::reference_wrapper<const data_bridge::VenueAccessor> venue_accessor_;

  std::reference_wrapper<const DatasourceController> datasource_controller_;
  std::reference_wrapper<const ListingController> listing_controller_;
  std::reference_wrapper<const PriceSeedController> price_seed_controller_;
  std::reference_wrapper<const SettingController> setting_controller_;
  std::reference_wrapper<const VenueController> venue_controller_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_PROCESSORS_GET_PROCESSOR_HPP_
