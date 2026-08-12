#ifndef SIMULATOR_HTTP_IH_HEADERS_CONTENT_DISPOSITION_ATTACHMENT_HPP_
#define SIMULATOR_HTTP_IH_HEADERS_CONTENT_DISPOSITION_ATTACHMENT_HPP_

#include <pistache/http_header.h>

#include <ostream>
#include <string>
#include <utility>

namespace simulator::http {

class ContentDispositionAttachment final
    : public Pistache::Http::Header::Header {
 public:
  NAME("Content-Disposition")

  ContentDispositionAttachment() = default;

  explicit ContentDispositionAttachment(std::string filename)
      : filename_{std::move(filename)} {}

  auto write(std::ostream& ostream) const -> void override {
    ostream << "attachment; filename=\"" << filename_ << '"';
  }

 private:
  std::string filename_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_HEADERS_CONTENT_DISPOSITION_ATTACHMENT_HPP_
