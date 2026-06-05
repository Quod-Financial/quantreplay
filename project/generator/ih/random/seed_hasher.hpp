#ifndef SIMULATOR_GENERATOR_IH_RANDOM_SEED_HASHER_HPP_
#define SIMULATOR_GENERATOR_IH_RANDOM_SEED_HASHER_HPP_

#include <cstdint>
#include <string_view>

#include "data_layer/api/models/listing.hpp"

namespace simulator::generator::random {

class ListingSeedHasher {
 public:
  explicit ListingSeedHasher(const data_layer::Listing& listing);

  [[nodiscard]] auto with_seed(std::string_view user_seed) const
      -> std::uint64_t;

 private:
  std::uint64_t listing_hash_;
};

}  // namespace simulator::generator::random

#endif  // SIMULATOR_GENERATOR_IH_RANDOM_SEED_HASHER_HPP_
