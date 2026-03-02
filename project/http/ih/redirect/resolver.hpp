#ifndef SIMULATOR_HTTP_IH_REDIRECT_RESOLVER_HPP_
#define SIMULATOR_HTTP_IH_REDIRECT_RESOLVER_HPP_

#include <string>
#include <tl/expected.hpp>

#include "ih/redirect/destination.hpp"

namespace simulator::http::redirect {

class Resolver {
 public:
  enum class Error : std::uint8_t {
    NonexistentInstance,
    ResolvingFailed,
    SelfRedirect,
    UnknownError
  };

  virtual ~Resolver() = default;

  virtual auto resolve_by_venue_id(const std::string& venue_id) const noexcept
      -> tl::expected<Destination, Error> = 0;
};

}  // namespace simulator::http::redirect

#endif  // SIMULATOR_HTTP_IH_REDIRECT_RESOLVER_HPP_
