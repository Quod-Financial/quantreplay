#ifndef SIMULATOR_HTTP_IH_PROCESSORS_GET_PROCESSOR_HPP_
#define SIMULATOR_HTTP_IH_PROCESSORS_GET_PROCESSOR_HPP_

#include <pistache/http.h>
#include <pistache/router.h>

#include <memory>
#include <string>

#include "data_layer/api/models/venue.hpp"
#include "ih/config_provider.hpp"
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
  virtual ~GetProcessor() = default;

  virtual auto get_venue(const Pistache::Rest::Request& request,
                         Pistache::Http::ResponseWriter response) -> void = 0;

  virtual auto get_venues(const Pistache::Rest::Request& request,
                          Pistache::Http::ResponseWriter response) -> void = 0;

  virtual auto get_listing(const Pistache::Rest::Request& request,
                           Pistache::Http::ResponseWriter response) -> void = 0;

  virtual auto get_listings(const Pistache::Rest::Request& request,
                            Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto get_data_source(const Pistache::Rest::Request& request,
                               Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto get_data_sources(const Pistache::Rest::Request& request,
                                Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto get_price_seed(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto get_price_seeds(const Pistache::Rest::Request& request,
                               Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto get_venue_status(const Pistache::Rest::Request& request,
                                Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto get_all_venues_status(const Pistache::Rest::Request& request,
                                     Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto get_settings(const Pistache::Rest::Request& request,
                            Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto get_order_gen_status(const Pistache::Rest::Request& request,
                                    Pistache::Http::ResponseWriter response)
      -> void = 0;
};

class GetProcessorImpl : public GetProcessor {
 public:
  explicit GetProcessorImpl(
      std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
      std::shared_ptr<redirect::RedirectionProcessor> redirector,
      std::shared_ptr<DatasourceController> datasource_controller,
      std::shared_ptr<ListingController> listing_controller,
      std::shared_ptr<PriceSeedController> price_seed_controller,
      std::shared_ptr<SettingController> setting_controller,
      std::shared_ptr<VenueController> venue_controller,
      std::shared_ptr<ConfigProvider> config_provider);

  auto get_venue(const Pistache::Rest::Request& request,
                 Pistache::Http::ResponseWriter response) -> void override;

  auto get_venues(const Pistache::Rest::Request& request,
                  Pistache::Http::ResponseWriter response) -> void override;

  auto get_listing(const Pistache::Rest::Request& request,
                   Pistache::Http::ResponseWriter response) -> void override;

  auto get_listings(const Pistache::Rest::Request& request,
                    Pistache::Http::ResponseWriter response) -> void override;

  auto get_data_source(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response)
      -> void override;

  auto get_data_sources(const Pistache::Rest::Request& request,
                        Pistache::Http::ResponseWriter response)
      -> void override;

  auto get_price_seed(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter response) -> void override;

  auto get_price_seeds(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response)
      -> void override;

  auto get_venue_status(const Pistache::Rest::Request& request,
                        Pistache::Http::ResponseWriter response)
      -> void override;

  auto get_all_venues_status(const Pistache::Rest::Request& request,
                             Pistache::Http::ResponseWriter response)
      -> void override;

  auto get_settings(const Pistache::Rest::Request& request,
                    Pistache::Http::ResponseWriter response) -> void override;

  auto get_order_gen_status(const Pistache::Rest::Request& request,
                            Pistache::Http::ResponseWriter response)
      -> void override;

 private:
  auto get_venue_status_str(const data_layer::Venue& venue) const
      -> std::string;

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

  std::shared_ptr<data_bridge::VenueAccessor> venue_accessor_;

  std::shared_ptr<DatasourceController> datasource_controller_;
  std::shared_ptr<ListingController> listing_controller_;
  std::shared_ptr<PriceSeedController> price_seed_controller_;
  std::shared_ptr<SettingController> setting_controller_;
  std::shared_ptr<VenueController> venue_controller_;
  std::shared_ptr<ConfigProvider> config_provider_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_PROCESSORS_GET_PROCESSOR_HPP_
