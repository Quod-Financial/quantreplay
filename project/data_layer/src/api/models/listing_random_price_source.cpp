#include "api/models/listing_random_price_source.hpp"

#include <string>
#include <utility>

namespace simulator::data_layer {

auto ListingRandomPriceSource::create(ListingRandomPriceSource::Patch snapshot,
                                      std::uint64_t listing_id) noexcept
    -> ListingRandomPriceSource {
  ListingRandomPriceSource source;
  source.listing_id_ = listing_id;
  source.datasource_id_ = snapshot.datasource_id_;
  source.symbol_ = std::move(snapshot.symbol_);
  return source;
}

auto ListingRandomPriceSource::listing_id() const noexcept -> std::uint64_t {
  return listing_id_;
}

auto ListingRandomPriceSource::datasource_id() const noexcept -> std::uint64_t {
  return datasource_id_;
}

auto ListingRandomPriceSource::symbol() const noexcept -> const std::string& {
  return symbol_;
}

auto ListingRandomPriceSource::Patch::datasource_id() const noexcept
    -> std::uint64_t {
  return datasource_id_;
}

auto ListingRandomPriceSource::Patch::with_datasource_id(
    std::uint64_t datasource_id) noexcept -> Patch& {
  datasource_id_ = datasource_id;
  return *this;
}

auto ListingRandomPriceSource::Patch::symbol() const noexcept
    -> const std::string& {
  return symbol_;
}

auto ListingRandomPriceSource::Patch::with_symbol(std::string symbol) noexcept
    -> Patch& {
  symbol_ = std::move(symbol);
  return *this;
}

}  // namespace simulator::data_layer
