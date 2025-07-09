#ifndef SIMULATOR_HTTP_IH_HEADERS_X_API_VERSION_HPP_
#define SIMULATOR_HTTP_IH_HEADERS_X_API_VERSION_HPP_

#include <pistache/http_header.h>

#include <optional>
#include <string>

namespace simulator::http {

class XApiVersion final : public Pistache::Http::Header::Header {
 public:
  NAME("X-API-Version")

  auto parse(const std::string& data) -> void override {
    if (!data.empty()) {
      version_ = data;
    }
  }

  auto write(std::ostream& ostream) const -> void override {
    if (version_) {
      ostream << version_.value();
    }
  }

  auto version() const -> std::optional<std::string> { return version_; }

 private:
  std::optional<std::string> version_;
};

}  // namespace simulator::http
#endif  // SIMULATOR_HTTP_IH_HEADERS_X_API_VERSION_HPP_
