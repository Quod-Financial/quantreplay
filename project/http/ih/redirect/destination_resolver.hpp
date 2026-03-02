#ifndef SIMULATOR_HTTP_IH_REDIRECT_DESTINATION_RESOLVER_HPP_
#define SIMULATOR_HTTP_IH_REDIRECT_DESTINATION_RESOLVER_HPP_

#include <memory>
#include <string>

#include "ih/data_bridge/venue_accessor.hpp"
#include "ih/redirect/resolver.hpp"

namespace simulator::http::redirect {

class DestinationResolver final : public Resolver {
 public:
  DestinationResolver() = delete;

  explicit DestinationResolver(
      std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
      std::string current_venue_id,
      std::uint16_t current_rest_port,
      bool use_venue_id_as_peer_host = false) noexcept;

  auto resolve_by_venue_id(const std::string& venue_id) const noexcept
      -> tl::expected<Destination, Error> override;

  static auto create(std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
                     const std::string& current_venue_id,
                     std::uint16_t current_rest_port)
      -> std::shared_ptr<DestinationResolver>;

 private:
  std::shared_ptr<data_bridge::VenueAccessor> venue_accessor_;
  std::string current_venue_id_;
  std::uint16_t current_rest_port_;
  bool use_venue_id_as_peer_host_ = false;
};

}  // namespace simulator::http::redirect

#endif  // SIMULATOR_HTTP_IH_REDIRECT_DESTINATION_RESOLVER_HPP_
