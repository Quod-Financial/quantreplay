#ifndef SIMULATOR_HTTP_IH_CONTROLLERS_LISTING_CONTROLLER_HPP_
#define SIMULATOR_HTTP_IH_CONTROLLERS_LISTING_CONTROLLER_HPP_

#include <pistache/http_defs.h>

#include <cstdint>
#include <optional>
#include <string>
#include <tl/expected.hpp>
#include <unordered_set>
#include <utility>
#include <vector>

#include "data_layer/api/models/datasource.hpp"
#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/listing_random_price_source.hpp"
#include "ih/data_bridge/datasource_accessor.hpp"
#include "ih/data_bridge/listing_accessor.hpp"

namespace simulator::http {

class ListingController {
 public:
  using Result = std::pair<Pistache::Http::Code, std::string>;

  ListingController(std::unique_ptr<data_bridge::ListingAccessor> data_accessor,
                    std::unique_ptr<data_bridge::DatasourceAccessor>
                        datasource_accessor) noexcept;

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
  using Validation = tl::expected<void, Result>;

  enum class SourcesOrigin : std::uint8_t { Request, StoredListing };

  [[nodiscard]]
  auto validate_random_price_source(
      const data_layer::Listing::Patch& patch,
      const std::optional<data_layer::Listing>& current) const -> Validation;

  [[nodiscard]]
  static auto listing_updated_symbol_does_not_reference_itself(
      const data_layer::Listing::Patch& patch,
      const std::optional<data_layer::Listing>& current) -> Validation;

  [[nodiscard]]
  static auto listing_does_not_reference_itself(
      const std::vector<data_layer::ListingRandomPriceSource::Patch>&
          random_price_sources,
      const std::optional<std::string>& listing_symbol) -> Validation;

  [[nodiscard]]
  static auto validate_random_price_symbols(
      const data_layer::Listing::Patch& patch,
      const std::optional<data_layer::Listing>& current) -> Validation;

  [[nodiscard]]
  auto validate_random_price_datasources(
      const std::unordered_set<std::uint64_t>& datasource_ids,
      const std::optional<std::string>& venue_id,
      SourcesOrigin origin) const -> Validation;

  [[nodiscard]]
  auto select_random_price_datasource(std::uint64_t datasource_id) const
      -> tl::expected<data_layer::Datasource, Result>;

  [[nodiscard]]
  static auto validate_datasource_venue(
      const data_layer::Datasource& datasource,
      const std::optional<std::string>& venue_id,
      SourcesOrigin origin) -> Validation;

  static auto format_error_response(data_bridge::Failure failure)
      -> std::string;

  std::unique_ptr<data_bridge::ListingAccessor> data_accessor_;
  std::unique_ptr<data_bridge::DatasourceAccessor> datasource_accessor_;
};

}  // namespace simulator::http

#endif  // SIMULATOR_HTTP_IH_CONTROLLERS_LISTING_CONTROLLER_HPP_
