#ifndef SIMULATOR_GENERATOR_IH_MARKET_DATA_RECORD_FACTORY_HPP_
#define SIMULATOR_GENERATOR_IH_MARKET_DATA_RECORD_FACTORY_HPP_

#include <cstdint>

#include "ih/historical/data/record.hpp"
#include "ih/market_data/subscription_requirement.hpp"
#include "protocol/app/market_data_snapshot.hpp"

namespace simulator::generator::mdata {

[[nodiscard]]
auto make_order_book_record(const protocol::MarketDataSnapshot& snapshot,
                            const OrderDerivationRequirement& requirement,
                            std::uint64_t source_row) -> historical::Record;

}  // namespace simulator::generator::mdata

#endif  // SIMULATOR_GENERATOR_IH_MARKET_DATA_RECORD_FACTORY_HPP_
