#ifndef SIMULATOR_HTTP_IH_ROUTER_HPP_
#define SIMULATOR_HTTP_IH_ROUTER_HPP_

#include <pistache/router.h>

#include "ih/processors/delete_processor.hpp"
#include "ih/processors/get_processor.hpp"
#include "ih/processors/head_processor.hpp"
#include "ih/processors/post_processor.hpp"
#include "ih/processors/put_processor.hpp"

namespace simulator::http {

class Router : public Pistache::Http::Handler {
 public:
  HTTP_PROTOTYPE(Router)

  explicit Router(std::shared_ptr<GetProcessor> get_processor,
                  std::shared_ptr<HeadProcessor> head_processor,
                  std::shared_ptr<PostProcessor> post_processor,
                  std::shared_ptr<PutProcessor> put_processor,
                  std::shared_ptr<DeleteProcessor> delete_processor);

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

  std::shared_ptr<GetProcessor> get_processor_;
  std::shared_ptr<HeadProcessor> head_processor_;
  std::shared_ptr<PostProcessor> post_processor_;
  std::shared_ptr<PutProcessor> put_processor_;
  std::shared_ptr<DeleteProcessor> delete_processor_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_ROUTER_HPP_