#ifndef SIMULATOR_HTTP_IH_REDIRECT_DESTINATION_HPP_
#define SIMULATOR_HTTP_IH_REDIRECT_DESTINATION_HPP_

#include <string>
#include <utility>

namespace simulator::http::redirect {

class Destination {
 public:
  Destination() = delete;

  Destination(std::string host, int port) noexcept
      : host_{std::move(host)}, port_{port} {}

  auto host() const noexcept -> const std::string& { return host_; }

  auto port() const noexcept -> int { return port_; }

 private:
  std::string host_;
  int port_;
};

}  // namespace simulator::http::redirect

#endif  // SIMULATOR_HTTP_IH_REDIRECT_DESTINATION_HPP_
