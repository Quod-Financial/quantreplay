#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_PARSING_PARSING_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_PARSING_PARSING_HPP_

#include <cstdint>
#include <string_view>

#include "ih/historical/data/record.hpp"
#include "ih/historical/data/time.hpp"
#include "ih/historical/mapping/params.hpp"
#include "ih/historical/parsing/row.hpp"

namespace simulator::generator::historical {

auto parse_value(std::string_view data, double& result) -> bool;

auto parse_value(std::string_view data, historical::Timepoint& result) -> bool;

auto parse_value(std::string_view data, std::string& result) -> bool;

template <typename Destination>
inline auto parse_cell(const Row& row,
                       std::uint32_t cell_idx,
                       Destination& dest) -> bool {
  return cell_idx < row.columns() && parse_value(row[cell_idx], dest);
}

auto parse_bid_level_part(const Row& row,
                          Level::Builder& builder,
                          const MappingParams& mapping,
                          std::uint32_t depth) -> void;

auto parse_offer_level_part(const Row& row,
                            Level::Builder& builder,
                            const MappingParams& mapping,
                            std::uint32_t depth) -> void;

auto parse(const Row& row,
           Record::Builder& result_builder,
           const MappingParams& mapping,
           std::uint32_t depth_levels_to_parse) -> void;

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_PARSING_PARSING_HPP_
