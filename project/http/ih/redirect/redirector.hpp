#ifndef SIMULATOR_HTTP_IH_REDIRECT_REDIRECTOR_HPP_
#define SIMULATOR_HTTP_IH_REDIRECT_REDIRECTOR_HPP_

#include <optional>
#include <utility>

#include "ih/redirect/request.hpp"
#include "ih/redirect/result.hpp"

namespace simulator::http::redirect {

class Redirector {
 public:
  enum class Status : std::uint8_t { Success, ConnectionFailed, UnknownError };

  virtual ~Redirector() = default;

  using RedirectionResult = std::pair<std::optional<Result>, Status>;

  virtual auto redirect(const Request& request) const noexcept
      -> RedirectionResult = 0;
};

}  // namespace simulator::http::redirect

#endif  // SIMULATOR_HTTP_IH_REDIRECT_REDIRECTOR_HPP_
