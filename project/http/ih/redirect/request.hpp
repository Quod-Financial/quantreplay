#ifndef SIMULATOR_HTTP_IH_REDIRECT_REQUEST_HPP_
#define SIMULATOR_HTTP_IH_REDIRECT_REQUEST_HPP_

#include <pistache/http.h>

#include <string>
#include <utility>

#include "ih/redirect/destination.hpp"

namespace simulator::http::redirect {

class Request {
 public:
  Request() = delete;

  Request(Destination destination,
          Pistache::Http::Method method,
          std::string url) noexcept
      : request_destination_{std::move(destination)},
        request_url_{std::move(url)},
        request_method_{method} {}

  auto destination() const noexcept -> const Destination& {
    return request_destination_;
  }

  auto url() const noexcept -> const std::string& { return request_url_; }

  auto method() const noexcept -> Pistache::Http::Method {
    return request_method_;
  }

 private:
  Destination request_destination_;
  std::string request_url_;
  Pistache::Http::Method request_method_;
};

}  // namespace simulator::http::redirect

#endif  // SIMULATOR_HTTP_IH_REDIRECT_REQUEST_HPP_
