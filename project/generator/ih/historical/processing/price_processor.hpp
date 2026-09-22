#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_PRICE_PROCESSOR_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_PRICE_PROCESSOR_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include "core/domain/attributes.hpp"
#include "ih/historical/data/record.hpp"
#include "ih/historical/processing/action_sink.hpp"
#include "ih/market_data/best_prices_sink.hpp"

namespace simulator::generator::historical {

class PriceProcessor final : public historical::ActionSink {
 public:
  using Symbols = std::unordered_map<std::string, MdRequestId>;

  PriceProcessor() = delete;

  PriceProcessor(std::shared_ptr<mdata::BestPricesSink> sink, Symbols symbols);

  auto process(historical::Action action) -> void override;

 private:
  auto publish_top_of_book(const historical::Record& record) -> void;

  Symbols symbols_;
  std::shared_ptr<mdata::BestPricesSink> sink_;
};

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_PROCESSING_PRICE_PROCESSOR_HPP_
