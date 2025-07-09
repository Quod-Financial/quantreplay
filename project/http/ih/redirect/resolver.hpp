#ifndef SIMULATOR_HTTP_IH_REDIRECT_RESOLVER_HPP_
#define SIMULATOR_HTTP_IH_REDIRECT_RESOLVER_HPP_

#include <optional>
#include <string>

#include "ih/redirect/destination.hpp"

namespace simulator::http::redirect {

class Resolver {
 public:
  enum class Status : std::uint8_t {
    Success,
    NonexistentInstance,
    ResolvingFailed,
    UnknownError
  };

  using ResolvingResult = std::pair<std::optional<Destination>, Status>;

  virtual ~Resolver() = default;

  virtual auto resolve_by_venue_id(const std::string& venue_id) const noexcept
      -> ResolvingResult = 0;
};

}  // namespace simulator::http::redirect

#endif  // SIMULATOR_HTTP_IH_REDIRECT_RESOLVER_HPP_
