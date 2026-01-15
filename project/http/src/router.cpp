#include "ih/router.hpp"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include "cfg/api/cfg.hpp"
#include "core/version.hpp"
#include "ih/endpoint.hpp"
#include "ih/headers/x_api_version.hpp"
#include "ih/utils/response_formatters.hpp"
#include "log/logging.hpp"

template <>
struct fmt::formatter<Pistache::Address> : fmt::formatter<std::string_view> {
  using formattable = Pistache::Address;

  auto format(const formattable& address, format_context& context) const
      -> decltype(context.out()) {
    return format_to(context.out(),
                     "{}:{}",
                     address.host(),
                     static_cast<std::uint16_t>(address.port()));
  }
};

template <>
struct fmt::formatter<Pistache::Http::Header::Raw>
    : fmt::formatter<std::string_view> {
  using formattable = Pistache::Http::Header::Raw;

  auto format(const formattable& raw, format_context& context) const
      -> decltype(context.out()) {
    return format_to(context.out(), "\"{}\"", raw.value());
  }
};

template <>
struct fmt::formatter<Pistache::Http::Request>
    : fmt::formatter<std::string_view> {
  using formattable = Pistache::Http::Request;

  auto format(const formattable& request, format_context& context) const
      -> decltype(context.out()) {
    using Pistache::Http::methodString;
    format_to(context.out(), "Request={{ ");
    format_to(context.out(), "Method={}, ", methodString(request.method()));
    format_to(context.out(), "Resource='{}', ", request.resource());
    format_to(context.out(), "Origin='{}', ", request.address());
    format_to(context.out(), "Body='{}', ", beautify_body(request.body()));
    format_to(context.out(), "Header='{}'", request.headers().rawList());
    return format_to(context.out(), " }}");
  }

 private:
  static auto beautify_body(std::string body) -> std::string {
    // That's not a fully correct approach, as we can drop spaces inside
    // JSON body attributes.
    constexpr auto remover = [](char character) -> bool {
      return character == ' ' || character == '\n';
    };

    body.erase(std::remove_if(std::begin(body), std::end(body), remover),
               std::end(body));

    return body;
  }
};

namespace database = simulator::data_layer::database;

namespace simulator::http {

namespace {
auto check_api_version(const Pistache::Http::Request& request) -> bool {
  if (!cfg::http().check_api_version) {
    return true;
  }

  const auto api_version = request.headers().tryGet<XApiVersion>();
  if (!api_version) {
    return true;
  }

  if (const auto version = api_version->version(); version) {
    return *version == core::major_version();
  }
  return false;
}
}  // namespace

Router::Router(std::shared_ptr<GetProcessor> get_processor,
               std::shared_ptr<PostProcessor> post_processor,
               std::shared_ptr<PutProcessor> put_processor,
               std::shared_ptr<DeleteProcessor> delete_processor)
    : get_processor_{std::move(get_processor)},
      post_processor_{std::move(post_processor)},
      put_processor_{std::move(put_processor)},
      delete_processor_{std::move(delete_processor)} {
  init_generator_admin_routers();
  init_matching_engine_admin_routes();
  init_admin_routes();
  init_venue_routes();
  init_listing_routes();
  init_price_seed_routes();
  init_datasource_routes();
  init_setting_routes();
}

auto Router::onRequest(const Pistache::Http::Request& request,
                       Pistache::Http::ResponseWriter response) -> void {
  log::info("received request: {}", request);
  if (check_api_version(request)) {
    router_.route(request, std::move(response));
  } else {
    static const auto error_message =
        fmt::format("API version mismatch. API version '{}' is supported.",
                    core::major_version());
    log::warn("received request was rejected due to API version mismatch");
    response.send(Pistache::Http::Code::Precondition_Failed,
                  format_result_response(error_message));
  }
}

auto Router::init_generator_admin_routers() -> void {
  Pistache::Rest::Routes::Get(
      router_,
      endpoint::GenStatus,
      Pistache::Rest::Routes::bind(&GetProcessor::get_order_gen_status,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::GenStart,
      Pistache::Rest::Routes::bind(&PostProcessor::start_order_gen,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::GenStop,
      Pistache::Rest::Routes::bind(&PostProcessor::stop_order_gen,
                                   post_processor_.get()));
}

auto Router::init_matching_engine_admin_routes() -> void {
  Pistache::Rest::Routes::Post(
      router_,
      endpoint::Store,
      Pistache::Rest::Routes::bind(&PostProcessor::handle_store_request,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::StoreById,
      Pistache::Rest::Routes::bind(&PostProcessor::handle_store_request,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::Recover,
      Pistache::Rest::Routes::bind(&PostProcessor::handle_recover_request,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::RecoverById,
      Pistache::Rest::Routes::bind(&PostProcessor::handle_recover_request,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::Halt,
      Pistache::Rest::Routes::bind(&PostProcessor::halt_phase,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::Resume,
      Pistache::Rest::Routes::bind(&PostProcessor::resume_phase,
                                   post_processor_.get()));
}

auto Router::init_admin_routes() -> void {
  Pistache::Rest::Routes::Get(
      router_,
      endpoint::Status,
      Pistache::Rest::Routes::bind(&GetProcessor::get_status,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Get(
      router_,
      endpoint::VenueStatusByVenueId,
      Pistache::Rest::Routes::bind(&GetProcessor::get_venue_status,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Get(
      router_,
      endpoint::VenueStatus,
      Pistache::Rest::Routes::bind(&GetProcessor::get_venue_statuses,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::Reset,
      Pistache::Rest::Routes::bind(&PostProcessor::reset_app,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::ResetById,
      Pistache::Rest::Routes::bind(&PostProcessor::reset_app,
                                   post_processor_.get()));
}

auto Router::init_venue_routes() -> void {
  Pistache::Rest::Routes::Get(
      router_,
      endpoint::VenuesById,
      Pistache::Rest::Routes::bind(&GetProcessor::get_venue,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Get(
      router_,
      endpoint::Venues,
      Pistache::Rest::Routes::bind(&GetProcessor::get_venues,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::Venues,
      Pistache::Rest::Routes::bind(&PostProcessor::add_venue,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Put(
      router_,
      endpoint::VenuesById,
      Pistache::Rest::Routes::bind(&PutProcessor::update_venue,
                                   put_processor_.get()));
}

auto Router::init_listing_routes() -> void {
  Pistache::Rest::Routes::Get(
      router_,
      endpoint::ListingsBySymbol,
      Pistache::Rest::Routes::bind(&GetProcessor::get_listing,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Get(
      router_,
      endpoint::Listings,
      Pistache::Rest::Routes::bind(&GetProcessor::get_listings,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::Listings,
      Pistache::Rest::Routes::bind(&PostProcessor::add_listing,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Put(
      router_,
      endpoint::ListingsBySymbol,
      Pistache::Rest::Routes::bind(&PutProcessor::update_listing,
                                   put_processor_.get()));
}

auto Router::init_price_seed_routes() -> void {
  Pistache::Rest::Routes::Get(
      router_,
      endpoint::PriceSeedsById,
      Pistache::Rest::Routes::bind(&GetProcessor::get_price_seed,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Get(
      router_,
      endpoint::PriceSeeds,
      Pistache::Rest::Routes::bind(&GetProcessor::get_price_seeds,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::PriceSeeds,
      Pistache::Rest::Routes::bind(&PostProcessor::add_price_seed,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Put(
      router_,
      endpoint::PriceSeedsById,
      Pistache::Rest::Routes::bind(&PutProcessor::update_price_seed,
                                   put_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::SyncPriceSeeds,
      Pistache::Rest::Routes::bind(&PostProcessor::sync_price_seeds,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Delete(
      router_,
      endpoint::PriceSeedsById,
      Pistache::Rest::Routes::bind(&DeleteProcessor::delete_price_seed,
                                   delete_processor_.get()));
}

auto Router::init_datasource_routes() -> void {
  Pistache::Rest::Routes::Get(
      router_,
      endpoint::DataSourcesById,
      Pistache::Rest::Routes::bind(&GetProcessor::get_data_source,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Get(
      router_,
      endpoint::DataSources,
      Pistache::Rest::Routes::bind(&GetProcessor::get_data_sources,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Post(
      router_,
      endpoint::DataSources,
      Pistache::Rest::Routes::bind(&PostProcessor::add_data_source,
                                   post_processor_.get()));

  Pistache::Rest::Routes::Put(
      router_,
      endpoint::DataSourcesById,
      Pistache::Rest::Routes::bind(&PutProcessor::update_data_source,
                                   put_processor_.get()));
}

auto Router::init_setting_routes() -> void {
  Pistache::Rest::Routes::Get(
      router_,
      endpoint::Settings,
      Pistache::Rest::Routes::bind(&GetProcessor::get_settings,
                                   get_processor_.get()));

  Pistache::Rest::Routes::Put(
      router_,
      endpoint::Settings,
      Pistache::Rest::Routes::bind(&PutProcessor::update_settings,
                                   put_processor_.get()));
}

}  // namespace simulator::http