#ifndef SIMULATOR_HTTP_IH_PROCESSORS_PUT_PROCESSOR_HPP_
#define SIMULATOR_HTTP_IH_PROCESSORS_PUT_PROCESSOR_HPP_

#include <pistache/http.h>
#include <pistache/router.h>

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

class PutProcessor {
 public:
  virtual ~PutProcessor() = default;

  virtual auto update_venue(const Pistache::Rest::Request& request,
                            Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto update_listing(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto update_data_source(const Pistache::Rest::Request& request,
                                  Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto update_price_seed(const Pistache::Rest::Request& request,
                                 Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto update_settings(const Pistache::Rest::Request& request,
                               Pistache::Http::ResponseWriter response)
      -> void = 0;
};

class PutProcessorImpl : public PutProcessor {
 public:
  PutProcessorImpl(std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
                   std::shared_ptr<DatasourceController> datasource_controller,
                   std::shared_ptr<ListingController> listing_controller,
                   std::shared_ptr<PriceSeedController> price_seed_controller,
                   std::shared_ptr<SettingController> setting_controller,
                   std::shared_ptr<TradingController> trading_controller,
                   std::shared_ptr<VenueController> venue_controller);

  auto update_venue(const Pistache::Rest::Request& request,
                    Pistache::Http::ResponseWriter response) -> void override;

  auto update_listing(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter response) -> void override;

  auto update_data_source(const Pistache::Rest::Request& request,
                          Pistache::Http::ResponseWriter response)
      -> void override;

  auto update_price_seed(const Pistache::Rest::Request& request,
                         Pistache::Http::ResponseWriter response)
      -> void override;

  auto update_settings(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response)
      -> void override;

 private:
  static auto respond(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter& response,
                      Pistache::Http::Code code,
                      const std::string& body) -> void;

  auto redirect(const Pistache::Rest::Request& request,
                const std::string& instance_id) const -> redirect::Result;

  std::shared_ptr<redirect::RedirectionProcessor> redirector_;

  std::shared_ptr<DatasourceController> datasource_controller_;
  std::shared_ptr<ListingController> listing_controller_;
  std::shared_ptr<PriceSeedController> price_seed_controller_;
  std::shared_ptr<SettingController> setting_controller_;
  std::shared_ptr<TradingController> trading_controller_;
  std::shared_ptr<VenueController> venue_controller_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_PROCESSORS_PUT_PROCESSOR_HPP_
