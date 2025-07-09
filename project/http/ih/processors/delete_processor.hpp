#ifndef SIMULATOR_HTTP_IH_PROCESSORS_DELETE_PROCESSOR_HPP_
#define SIMULATOR_HTTP_IH_PROCESSORS_DELETE_PROCESSOR_HPP_

#include <pistache/http.h>
#include <pistache/router.h>

#include <functional>

#include "ih/controllers/price_seed_controller.hpp"

namespace simulator::http {

class DeleteProcessor {
 public:
  explicit DeleteProcessor(
      const PriceSeedController& price_seed_controller) noexcept;

  auto delete_price_seed(const Pistache::Rest::Request& request,
                         Pistache::Http::ResponseWriter response) -> void;

 private:
  static auto respond(const Pistache::Rest::Request& request,
                      Pistache::Http::ResponseWriter& response,
                      Pistache::Http::Code code,
                      const std::string& body) -> void;

  std::reference_wrapper<const PriceSeedController> price_seed_controller_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_PROCESSORS_DELETE_PROCESSOR_HPP_
