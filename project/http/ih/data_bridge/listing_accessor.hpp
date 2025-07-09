#ifndef SIMULATOR_HTTP_IH_DATA_BRIDGE_LISTING_ACCESSOR_HPP_
#define SIMULATOR_HTTP_IH_DATA_BRIDGE_LISTING_ACCESSOR_HPP_

#include <cstdint>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/database/context.hpp"
#include "data_layer/api/models/listing.hpp"
#include "ih/data_bridge/operation_failure.hpp"

namespace simulator::http::data_bridge {

class ListingAccessor {
 public:
  ListingAccessor() = default;
  ListingAccessor(const ListingAccessor&) = default;
  ListingAccessor(ListingAccessor&&) noexcept = default;

  auto operator=(const ListingAccessor&) -> ListingAccessor& = default;
  auto operator=(ListingAccessor&&) noexcept -> ListingAccessor& = default;

  virtual ~ListingAccessor() = default;

  [[nodiscard]]
  virtual auto select_single(std::uint64_t listing_id) const noexcept
      -> tl::expected<data_layer::Listing, Failure> = 0;

  [[nodiscard]]
  virtual auto select_single(const std::string& symbol) const noexcept
      -> tl::expected<data_layer::Listing, Failure> = 0;

  [[nodiscard]]
  virtual auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::Listing>, Failure> = 0;

  [[nodiscard]]
  virtual auto add(data_layer::Listing::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> = 0;

  [[nodiscard]]
  virtual auto update(data_layer::Listing::Patch update,
                      std::uint64_t listing_id) const noexcept
      -> tl::expected<void, Failure> = 0;

  [[nodiscard]]
  virtual auto update(data_layer::Listing::Patch update,
                      const std::string& symbol) const noexcept
      -> tl::expected<void, Failure> = 0;
};

class DataLayerListingAccessor final : public ListingAccessor {
 public:
  using DbContext = data_layer::database::Context;

  DataLayerListingAccessor() = delete;

  explicit DataLayerListingAccessor(DbContext context) noexcept;

  [[nodiscard]]
  auto select_single(std::uint64_t listing_id) const noexcept
      -> tl::expected<data_layer::Listing, Failure> override;

  [[nodiscard]]
  auto select_single(const std::string& symbol) const noexcept
      -> tl::expected<data_layer::Listing, Failure> override;

  [[nodiscard]]
  auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::Listing>, Failure> override;

  [[nodiscard]]
  auto add(data_layer::Listing::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> override;

  [[nodiscard]]
  auto update(data_layer::Listing::Patch update,
              std::uint64_t listing_id) const noexcept
      -> tl::expected<void, Failure> override;

  [[nodiscard]]
  auto update(data_layer::Listing::Patch update,
              const std::string& symbol) const noexcept
      -> tl::expected<void, Failure> override;

 private:
  data_layer::database::Context context_;
};

}  // namespace simulator::http::data_bridge

#endif  // SIMULATOR_HTTP_IH_DATA_BRIDGE_LISTING_ACCESSOR_HPP_
