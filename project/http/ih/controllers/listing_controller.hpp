#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_LISTING_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_LISTING_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <cstdint>
#include <functional>
#include <string>
#include <utility>

#include "ih/data_bridge/listing_accessor.hpp"

namespace simulator::http {

class ListingController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  explicit ListingController(
      data_bridge::ListingAccessor& data_accessor) noexcept;

  [[nodiscard]]
  auto select_listing(const std::string& key) const -> Result;

  [[nodiscard]]
  auto select_all_listings() const -> Result;

  [[nodiscard]]
  auto insert_listing(const std::string& body) const -> Result;

  [[nodiscard]]
  auto update_listing(const std::string& key, const std::string& body) const
      -> Result;

 private:
  static auto format_error_response(data_bridge::Failure failure) -> std::string;

  std::reference_wrapper<const data_bridge::ListingAccessor> data_accessor_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_LISTING_CONTROLLER_HPP_
