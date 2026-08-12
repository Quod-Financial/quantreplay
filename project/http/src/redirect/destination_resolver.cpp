#include "ih/redirect/destination_resolver.hpp"

#include "cfg/api/cfg.hpp"
#include "ih/formatters/redirect.hpp"
#include "log/logging.hpp"

namespace simulator::http::redirect {

DestinationResolver::DestinationResolver(
    std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
    std::string current_venue_id,
    std::uint16_t current_rest_port,
    bool use_venue_id_as_peer_host) noexcept
    : venue_accessor_{std::move(venue_accessor)},
      current_venue_id_{std::move(current_venue_id)},
      current_rest_port_{current_rest_port},
      use_venue_id_as_peer_host_{use_venue_id_as_peer_host} {}

auto DestinationResolver::resolve_by_venue_id(const std::string& venue_id)
    const noexcept -> tl::expected<Destination, Error> try {
  static constexpr std::string Localhost{"localhost"};

  auto result = venue_accessor_->select_single(venue_id);
  if (!result) {
    log::err(
        "failed to resolve destination with '{0}' VenueID - "
        "venue {0} was not found in the database",
        venue_id);
    return tl::make_unexpected(Error::NonexistentInstance);
  }

  const data_layer::Venue& venue = result.value();
  const auto destination_port = venue.rest_port();
  if (!destination_port.has_value()) {
    log::err(
        "failed to resolve destination with '{0}' VenueID - "
        "venue entry with an '{0}' identifier has no REST port specified",
        venue_id);
    return tl::make_unexpected(Error::ResolvingFailed);
  }

  std::string destination_host =
      use_venue_id_as_peer_host_ ? venue_id : Localhost;

  const bool is_loopback_host =
      destination_host == Localhost || destination_host == current_venue_id_;
  const bool is_same_port = *destination_port == current_rest_port_;
  if (is_loopback_host && is_same_port) {
    log::err(
        "blocked self-redirection attempt for '{}' VenueID - destination "
        "{}:{} points to current process on REST port {}",
        venue_id,
        destination_host,
        *destination_port,
        current_rest_port_);
    return tl::make_unexpected(Error::SelfRedirect);
  }

  Destination destination{std::move(destination_host), *destination_port};
  log::debug("successfully resolved {} for '{}'", destination, venue_id);
  return destination;
} catch (const std::exception& ex) {
  log::err(
      "failed to resolve destination with '{}' VenueID - "
      "an error occurred while resolving destination: {}",
      venue_id,
      ex.what());
  return tl::make_unexpected(Error::UnknownError);
} catch (...) {
  log::err(
      "failed to resolve destination with '{}' VenueID - "
      "unknown error occurred while resolving destination",
      venue_id);
  return tl::make_unexpected(Error::UnknownError);
}

auto DestinationResolver::create(
    std::shared_ptr<data_bridge::VenueAccessor> venue_accessor,
    const std::string& current_venue_id,
    std::uint16_t current_rest_port) -> std::shared_ptr<DestinationResolver> {
  const bool resolve_hostname_as_venue_id =
      cfg::http().peer_resolution ==
      cfg::HttpConfiguration::PeerHostResolution::VenueId;

  return std::make_shared<DestinationResolver>(std::move(venue_accessor),
                                               current_venue_id,
                                               current_rest_port,
                                               resolve_hostname_as_venue_id);
}

}  // namespace simulator::http::redirect