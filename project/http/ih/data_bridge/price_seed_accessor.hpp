#ifndef SIMULATOR_HTTP_IH_DATA_BRIDGE_PRICE_SEED_ACCESSOR_HPP_
#define SIMULATOR_HTTP_IH_DATA_BRIDGE_PRICE_SEED_ACCESSOR_HPP_

#include <cstdint>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/database/context.hpp"
#include "data_layer/api/models/price_seed.hpp"
#include "ih/data_bridge/operation_failure.hpp"

namespace simulator::http::data_bridge {

class PriceSeedAccessor {
 public:
  PriceSeedAccessor() = default;
  PriceSeedAccessor(const PriceSeedAccessor&) = default;
  PriceSeedAccessor(PriceSeedAccessor&&) noexcept = default;

  auto operator=(const PriceSeedAccessor&) -> PriceSeedAccessor& = default;
  auto operator=(PriceSeedAccessor&&) noexcept -> PriceSeedAccessor& = default;

  virtual ~PriceSeedAccessor() = default;

  [[nodiscard]]
  virtual auto select_single(std::uint64_t seed_id) const noexcept
      -> tl::expected<data_layer::PriceSeed, Failure> = 0;

  [[nodiscard]]
  virtual auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::PriceSeed>, Failure> = 0;

  [[nodiscard]]
  virtual auto add(data_layer::PriceSeed::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> = 0;

  [[nodiscard]]
  virtual auto update(data_layer::PriceSeed::Patch update,
                      std::uint64_t seed_id) const noexcept
      -> tl::expected<void, Failure> = 0;

  [[nodiscard]]
  virtual auto drop_single(std::uint64_t seed_id) const noexcept
      -> tl::expected<void, Failure> = 0;

  [[nodiscard]]
  virtual auto sync(std::string pqxx_source_connection) const noexcept
      -> tl::expected<void, Failure> = 0;
};

class DataLayerPriceSeedAccessor final : public PriceSeedAccessor {
 public:
  using DbContext = data_layer::database::Context;

  DataLayerPriceSeedAccessor() = delete;

  explicit DataLayerPriceSeedAccessor(DbContext context) noexcept;

  [[nodiscard]]
  auto select_single(std::uint64_t seed_id) const noexcept
      -> tl::expected<data_layer::PriceSeed, Failure> override;

  [[nodiscard]]
  auto select_all() const noexcept
      -> tl::expected<std::vector<data_layer::PriceSeed>, Failure> override;

  [[nodiscard]]
  auto add(data_layer::PriceSeed::Patch snapshot) const noexcept
      -> tl::expected<void, Failure> override;

  [[nodiscard]]
  auto update(data_layer::PriceSeed::Patch update,
              std::uint64_t seed_id) const noexcept
      -> tl::expected<void, Failure> override;

  [[nodiscard]]
  auto drop_single(std::uint64_t seed_id) const noexcept
      -> tl::expected<void, Failure> override;

  [[nodiscard]]
  auto sync(std::string pqxx_source_connection) const noexcept
      -> tl::expected<void, Failure> override;

 private:
  auto sync_seeds(const data_layer::PriceSeed& ext_seed) const -> void;

  data_layer::database::Context context_;
};

}  // namespace simulator::http::data_bridge

#endif  // SIMULATOR_HTTP_IH_DATA_BRIDGE_PRICE_SEED_ACCESSOR_HPP_
