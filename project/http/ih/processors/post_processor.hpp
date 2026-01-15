#ifndef SIMULATOR_HTTP_IH_PROCESSORS_POST_PROCESSOR_HPP_
#define SIMULATOR_HTTP_IH_PROCESSORS_POST_PROCESSOR_HPP_

#include <pistache/http.h>
#include <pistache/router.h>

#include <memory>
#include <string>

#include "http/http.hpp"
#include "ih/controllers/app_controller.hpp"
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
  virtual ~PostProcessor() = default;

  virtual auto add_venue(const Pistache::Rest::Request& request,
                         Pistache::Http::ResponseWriter response) -> void = 0;

  virtual auto add_listing(const Pistache::Rest::Request& request,
                           Pistache::Http::ResponseWriter response) -> void = 0;

  virtual auto add_data_source(const Pistache::Rest::Request& request,
                               Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto add_price_seed(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto sync_price_seeds(const Pistache::Rest::Request& request,
                                Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto stop_order_gen(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto start_order_gen(const Pistache::Rest::Request& request,
                               Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto halt_phase(const Pistache::Rest::Request& request,
                          Pistache::Http::ResponseWriter response) -> void = 0;

  virtual auto resume_phase(const Pistache::Rest::Request& request,
                            Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto handle_store_request(const Pistache::Rest::Request& request,
                                    Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto handle_recover_request(const Pistache::Rest::Request& request,
                                      Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto reset_app(const Pistache::Rest::Request& request,
                         Pistache::Http::ResponseWriter response) -> void = 0;
};

class PostProcessorImpl : public PostProcessor {
 public:
  PostProcessorImpl(std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
                    std::shared_ptr<DatasourceController> datasource_controller,
                    std::shared_ptr<ListingController> listing_controller,
                    std::shared_ptr<PriceSeedController> price_seed_controller,
                    std::shared_ptr<SettingController> setting_controller,
                    std::shared_ptr<TradingController> trading_controller,
                    std::shared_ptr<VenueController> venue_controller,
                    ControlCallbacks callbacks);

  auto add_venue(const Pistache::Rest::Request& request,
                 Pistache::Http::ResponseWriter response) -> void override;

  auto add_listing(const Pistache::Rest::Request& request,
                   Pistache::Http::ResponseWriter response) -> void override;

  auto add_data_source(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response)
      -> void override;

  auto add_price_seed(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter response) -> void override;

  auto sync_price_seeds(const Pistache::Rest::Request& request,
                        Pistache::Http::ResponseWriter response)
      -> void override;

  auto stop_order_gen(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter response) -> void override;

  auto start_order_gen(const Pistache::Rest::Request& request,
                       Pistache::Http::ResponseWriter response)
      -> void override;

  auto halt_phase(const Pistache::Rest::Request& request,
                  Pistache::Http::ResponseWriter response) -> void override;

  auto resume_phase(const Pistache::Rest::Request& request,
                    Pistache::Http::ResponseWriter response) -> void override;

  auto handle_store_request(const Pistache::Rest::Request& request,
                            Pistache::Http::ResponseWriter response)
      -> void override;

  auto handle_recover_request(const Pistache::Rest::Request& request,
                              Pistache::Http::ResponseWriter response)
      -> void override;

  auto reset_app(const Pistache::Rest::Request& request,
                 Pistache::Http::ResponseWriter response) -> void override;

 private:
  static auto respond(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter& response,
                      Pistache::Http::Code code,
                      const std::string& body) -> void;

  auto redirect(const Pistache::Rest::Request& request,
                const std::string& instance_id) const -> redirect::Result;

  auto handle_generation_stop_request(const Pistache::Rest::Request& request,
                                      Pistache::Http::ResponseWriter response)
      -> void;

  auto handle_generation_start_request(const Pistache::Rest::Request& request,
                                       Pistache::Http::ResponseWriter response)
      -> void;

  std::shared_ptr<redirect::RedirectionProcessor> redirector_;

  std::shared_ptr<DatasourceController> datasource_controller_;
  std::shared_ptr<ListingController> listing_controller_;
  std::shared_ptr<PriceSeedController> price_seed_controller_;
  std::shared_ptr<SettingController> setting_controller_;
  std::shared_ptr<TradingController> trading_controller_;
  std::shared_ptr<VenueController> venue_controller_;
  AppController app_controller_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_PROCESSORS_POST_PROCESSOR_HPP_