#include "api/models/datasource_listing.hpp"

#include <string>
#include <utility>

namespace simulator::data_layer {

auto DatasourceListing::create(DatasourceListing::Patch snapshot,
                               std::uint64_t datasource_id) noexcept
    -> DatasourceListing {
  DatasourceListing listing;
  listing.symbol_ = std::move(snapshot.symbol_);
  listing.datasource_id_ = datasource_id;
  return listing;
}

auto DatasourceListing::datasource_id() const noexcept -> std::uint64_t {
  return datasource_id_;
}

auto DatasourceListing::symbol() const noexcept -> const std::string& {
  return symbol_;
}

auto DatasourceListing::Patch::symbol() const noexcept -> const std::string& {
  return symbol_;
}

auto DatasourceListing::Patch::with_symbol(std::string symbol) noexcept
    -> Patch& {
  symbol_ = std::move(symbol);
  return *this;
}

}  // namespace simulator::data_layer
