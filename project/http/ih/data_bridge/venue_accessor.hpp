#ifndef SIMULATOR_HTTP_IH_DATA_BRIDGE_VENUE_ACCESSOR_HPP_
#define SIMULATOR_HTTP_IH_DATA_BRIDGE_VENUE_ACCESSOR_HPP_

#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/database/context.hpp"
#include "data_layer/api/models/venue.hpp"
#include "ih/data_bridge/operation_failure.hpp"

namespace simulator::http::data_bridge {

class VenueAccessor {
 public:
  VenueAccessor() = default;
  VenueAccessor(const VenueAccessor&) = default;
  VenueAccessor(VenueAccessor&&) noexcept = default;

  auto operator=(const VenueAccessor&) -> VenueAccessor& = default;
  auto operator=(VenueAccessor&&) noexcept -> VenueAccessor& = default;

  virtual ~VenueAccessor() = default;

  [[nodiscard]]
  virtual auto select_single(const std::string& venue_id) const noexcept
      -> tl::expected<data_layer::Venue, Failure> = 0;

  [[nodiscard]]
  virtual auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::Venue>, Failure> = 0;

  [[nodiscard]]
  virtual auto add(data_layer::Venue::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> = 0;

  [[nodiscard]]
  virtual auto update(data_layer::Venue::Patch update,
                      const std::string& venue_id) const noexcept
      -> tl::expected<void, Failure> = 0;
};

class DataLayerVenueAccessor final : public VenueAccessor {
 public:
  using DbContext = data_layer::database::Context;

  DataLayerVenueAccessor() = delete;

  explicit DataLayerVenueAccessor(DbContext context) noexcept;

  [[nodiscard]]
  auto select_single(const std::string& venue_id) const noexcept
      -> tl::expected<data_layer::Venue, Failure> override;

  [[nodiscard]]
  auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::Venue>, Failure> override;

  [[nodiscard]]
  auto add(data_layer::Venue::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> override;

  [[nodiscard]]
  auto update(data_layer::Venue::Patch update,
              const std::string& venue_id) const noexcept
      -> tl::expected<void, Failure> override;

 private:
  data_layer::database::Context context_;
};

}  // namespace simulator::http::data_bridge

#endif  // SIMULATOR_HTTP_IH_DATA_BRIDGE_VENUE_ACCESSOR_HPP_
