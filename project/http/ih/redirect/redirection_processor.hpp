#ifndef SIMULATOR_HTTP_IH_REDIRECT_REDIRECTION_PROCESSOR_HPP_
#define SIMULATOR_HTTP_IH_REDIRECT_REDIRECTION_PROCESSOR_HPP_

#include <pistache/http.h>

#include <memory>
#include <string>

#include "ih/data_bridge/venue_accessor.hpp"
#include "ih/redirect/redirector.hpp"
#include "ih/redirect/resolver.hpp"
#include "ih/redirect/result.hpp"

namespace simulator::http::redirect {

class RedirectionProcessor {
 public:
  RedirectionProcessor() = delete;

  explicit RedirectionProcessor(
      const data_bridge::VenueAccessor& venue_accessor);

  RedirectionProcessor(std::shared_ptr<Resolver> resolver,
                       std::shared_ptr<Redirector> redirector) noexcept;

  auto redirect_to_venue(const std::string& venue_id,
                         Pistache::Http::Method method,
                         const std::string& url) const -> Result;

  static auto create(const data_bridge::VenueAccessor& venue_accessor)
      -> std::shared_ptr<RedirectionProcessor>;

 private:
  static auto process_resolve_error(Resolver::Status status,
                                    std::string_view venue_id) -> Result;

  static auto process_redirect_error(Redirector::Status status,
                                     std::string_view venue_id) -> Result;

  std::shared_ptr<Resolver> resolver_;
  std::shared_ptr<Redirector> redirector_;
};

}  // namespace simulator::http::redirect

#endif  // SIMULATOR_HTTP_SRC_REDIRECT_REDIRECTION_PROCESSOR_HPP_
