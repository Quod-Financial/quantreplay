#ifndef SIMULATOR_HTTP_IH_PROCESSORS_DELETE_PROCESSOR_HPP_
#define SIMULATOR_HTTP_IH_PROCESSORS_DELETE_PROCESSOR_HPP_

#include <pistache/http.h>
#include <pistache/router.h>

#include <memory>
#include <string>

#include "ih/controllers/price_seed_controller.hpp"
#include "ih/controllers/quote_controller.hpp"
#include "ih/redirect/redirection_processor.hpp"

namespace simulator::http {

class DeleteProcessor {
 public:
  virtual ~DeleteProcessor() = default;

  virtual auto delete_price_seed(const Pistache::Rest::Request& request,
                                 Pistache::Http::ResponseWriter response)
      -> void = 0;

  virtual auto cancel_quote(const Pistache::Rest::Request& request,
                            Pistache::Http::ResponseWriter response)
      -> void = 0;
};

class DeleteProcessorImpl : public DeleteProcessor {
 public:
  DeleteProcessorImpl(
      std::shared_ptr<redirect::RedirectionProcessor> redirector,
      std::shared_ptr<PriceSeedController> price_seed_controller,
      std::shared_ptr<QuoteController> quote_controller,
      std::string venue_id) noexcept;

  auto delete_price_seed(const Pistache::Rest::Request& request,
                         Pistache::Http::ResponseWriter response)
      -> void override;

  auto cancel_quote(const Pistache::Rest::Request& request,
                    Pistache::Http::ResponseWriter response) -> void override;

 private:
  static auto respond(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter& response,
                      Pistache::Http::Code code,
                      const std::string& body) -> void;

  std::shared_ptr<redirect::RedirectionProcessor> redirector_;
  std::shared_ptr<PriceSeedController> price_seed_controller_;
  std::shared_ptr<QuoteController> quote_controller_;
  std::string venue_id_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_PROCESSORS_DELETE_PROCESSOR_HPP_
