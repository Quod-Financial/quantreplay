#ifndef SIMULATOR_HTTP_IH_REDIRECT_RESPONSE_HPP_
#define SIMULATOR_HTTP_IH_REDIRECT_RESPONSE_HPP_

#include <pistache/http.h>

#include <string>
#include <utility>

namespace simulator::http::redirect {

class Result {
 public:
  Result() = delete;

  explicit Result(Pistache::Http::Code code) noexcept : code_{code} {}

  auto http_code() const noexcept { return code_; }

  auto has_body_content() const noexcept -> bool { return !body_.empty(); }

  auto body_content() const noexcept -> const std::string& { return body_; }

  void set_body_content(std::string body_content) noexcept {
    body_ = std::move(body_content);
  }

 private:
  std::string body_;
  Pistache::Http::Code code_;
};

}  // namespace simulator::http::redirect

#endif  // SIMULATOR_HTTP_IH_REDIRECT_RESPONSE_HPP_
