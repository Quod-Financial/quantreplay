#ifndef SIMULATOR_HTTP_IH_REDIRECT_REQUEST_REDIRECTOR_HPP_
#define SIMULATOR_HTTP_IH_REDIRECT_REQUEST_REDIRECTOR_HPP_

#include <memory>

#include "ih/redirect/redirector.hpp"

namespace simulator::http::redirect {

class RequestRedirector final : public Redirector {
 public:
  auto redirect(const Request& request) const noexcept
      -> RedirectionResult override;

  static auto create() -> std::shared_ptr<RequestRedirector>;
};

}  // namespace simulator::http::redirect

#endif  // SIMULATOR_HTTP_IH_REDIRECT_REQUEST_REDIRECTOR_HPP_
