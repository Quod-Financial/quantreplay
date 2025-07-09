#include "ih/redirect/destination_resolver.hpp"

#include "cfg/api/cfg.hpp"
#include "ih/formatters/redirect.hpp"
#include "log/logging.hpp"

namespace simulator::http::redirect {

DestinationResolver::DestinationResolver(
    const data_bridge::VenueAccessor& venue_accessor,
    bool use_venue_id_as_peer_host) noexcept
    : venue_accessor_{venue_accessor},
      use_venue_id_as_peer_host_{use_venue_id_as_peer_host} {}

auto DestinationResolver::resolve_by_venue_id(const std::string& venue_id)
    const noexcept -> Resolver::ResolvingResult try {
  auto result = venue_accessor_.get().select_single(venue_id);
  if (!result) {
    log::err(
        "failed to resolve destination with '{0}' VenueID - "
        "venue {0} was not found in the database",
        venue_id);
    return std::make_pair(std::nullopt, Status::NonexistentInstance);
  }

  const data_layer::Venue& venue = result.value();
  const auto destination_port = venue.rest_port();
  if (!destination_port.has_value()) {
    log::err(
        "failed to resolve destination with '{0}' VenueID - "
        "venue entry with an '{0}' identifier has no REST port specified",
        venue_id);
    return std::make_pair(std::nullopt, Status::ResolvingFailed);
  }

  std::string destination_host =
      use_venue_id_as_peer_host_ ? venue_id : "localhost";

  Destination destination{std::move(destination_host), *destination_port};
  log::debug("successfully resolved {} for '{}'", destination, venue_id);
  return std::make_pair(std::move(destination), Status::Success);
} catch (const std::exception& ex) {
  log::err(
      "failed to resolve destination with '{}' VenueID - "
      "an error occurred while resolving destination: {}",
      venue_id,
      ex.what());
  return std::make_pair(std::nullopt, Status::UnknownError);
} catch (...) {
  log::err(
      "failed to resolve destination with '{}' VenueID - "
      "unknown error occurred while resolving destination",
      venue_id);
  return std::make_pair(std::nullopt, Status::UnknownError);
}

auto DestinationResolver::create(
    const data_bridge::VenueAccessor& venue_accessor)
    -> std::shared_ptr<DestinationResolver> {
  const bool resolve_hostname_as_venue_id =
      cfg::http().peer_resolution ==
      cfg::HttpConfiguration::PeerHostResolution::VenueId;

  return std::make_shared<DestinationResolver>(venue_accessor,
                                               resolve_hostname_as_venue_id);
}

}  // namespace simulator::http::redirect