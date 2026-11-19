#ifndef SIMULATOR_HTTP_IH_MARSHALLING_JSON_QUOTE_HPP_
#define SIMULATOR_HTTP_IH_MARSHALLING_JSON_QUOTE_HPP_

#include <string>
#include <string_view>

#include "protocol/admin/quote.hpp"

namespace simulator::http::json {

struct QuotePublication {
  std::string venue_id;
  protocol::QuotePublicationRequest request;
};

class QuoteUnmarshaller {
 public:
  static auto unmarshall(std::string_view json, QuotePublication& dest) -> void;
};

}  // namespace simulator::http::json

#endif  // SIMULATOR_HTTP_IH_MARSHALLING_JSON_QUOTE_HPP_
